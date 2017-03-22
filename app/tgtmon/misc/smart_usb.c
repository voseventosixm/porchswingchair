#include <linux/kernel.h>
#include <linux/slab.h>
#include "linux/smart_type.h"
#include "smart_common.h"
#include "smart_usb.h"

static cmn_smart_buffer* gp_dev_mngr;

// debug
static const char g_smart_file_folder[64] = "/home/root/";

void set_smart_id(cmn_raw_smart* smart_data)
{
    smart_data->power_hour.attr_id    = 9;
    smart_data->power_count.attr_id   = 12;
    smart_data->temperature.attr_id   = 194;
    smart_data->lba_written.attr_id = 241;
    smart_data->lba_read.attr_id    = 242;

    smart_data->max_erase.attr_id = 165;
    smart_data->ave_erase.attr_id = 167;
    smart_data->endurance.attr_id = 168;

    // Optional attributes
    smart_data->life_left.attr_id   = 248;
    smart_data->spare_block.attr_id = 249;
    smart_data->ecc_error.attr_id   = 187;
    smart_data->prog_fail.attr_id   = 181;
    smart_data->erase_fail.attr_id  = 182;

    // Debug attributes
    smart_data->last_error.attr_id  = 30;
    smart_data->prog_sector.attr_id = 31;
    smart_data->life_typea.attr_id  = 32;
    smart_data->life_typeb.attr_id  = 33;
    smart_data->extcsd_ver.attr_id  = 34;
    smart_data->ave_mlc.attr_id     = 35;
    smart_data->max_mlc.attr_id     = 36;
    smart_data->ave_slc.attr_id     = 37;
    smart_data->max_slc.attr_id     = 38;
}

static void smart_dev_reset(cmn_smart_device* ptr_smart_device)
{
    ptr_smart_device->device_path[0]    = '\0';
    ptr_smart_device->physical_path[0]  = '\0';

    ptr_smart_device->currlog_file[0]   = '\0';
    ptr_smart_device->currlog_backup[0] = '\0';

    ptr_smart_device->fulllog_backup[0] = '\0';
    ptr_smart_device->fulllog_file[0]   = '\0';

    ptr_smart_device->smart_pool_idx    = MAX_DEVICE_COUNT;
}

static void smart_dev_mngr_initialize(void)
{
    uint8_t n_dev;
    for (n_dev = 0; n_dev < MAX_DEVICE_COUNT; ++ n_dev) {
        smart_dev_reset(&gp_dev_mngr->device_list[n_dev]);
    }

    gp_dev_mngr->allocated_pool_count = 0;
    gp_dev_mngr->device_count         = 0;
    gp_dev_mngr->currlog_time         = 0;
}

static void smart_data_reset_currlog(cmn_smart_data* ptr_smart_data)
{
    cmn_smart_currlog* p_current = &ptr_smart_data->currlog;
    memset(p_current, 0, sizeof(cmn_smart_currlog));
    p_current->raw_attr.endurance.raw_low   = ATTR_NAND_ENDURANCE;
    p_current->raw_attr.temperature.raw_low = 25;

    set_smart_id(&p_current->raw_attr);
    p_current->raw_counter = 0;
    memcpy(&p_current->raw_buffer[0], &p_current->raw_attr, sizeof(p_current->raw_buffer[0]));
}

static void smart_data_reset_fulllog(cmn_smart_data* ptr_smart_data)
{
    // method set index
    cmn_smart_fulllog* p_log    = &ptr_smart_data->fulllog;
    cmn_smart_fulllog* p_active = &ptr_smart_data->fulllog_buffer[0];
    uint16_t nEntry;

    ptr_smart_data->fulllog_counter = 0;

    for(nEntry = 0; nEntry < MAX_LOG_COUNT; ++ nEntry) {
        p_log->entry_list[nEntry].index      = 0;
        p_log->entry_list[nEntry].time_stamp = 0;

        p_active->entry_list[nEntry].index      = 0;
        p_active->entry_list[nEntry].time_stamp = 0;
    }

    p_log->current_entry = MAX_LOG_COUNT - 1;
    p_log->fulllog_time  = 0;

    p_active->current_entry = MAX_LOG_COUNT - 1;
    p_active->fulllog_time  = 0;
}

static void make_sample(void)
{
    const char physicalDev[] = "mmcblk1";
    cmn_smart_device *ptr_smart_dev;
    cmn_smart_data* ptr_smart_data;

    ptr_smart_dev = &gp_dev_mngr->device_list[gp_dev_mngr->device_count];

    sprintf(ptr_smart_dev->device_path, "/dev/mmcblk1p1");
    sprintf(ptr_smart_dev->currlog_file, "%s%s_smart.bin", g_smart_file_folder, physicalDev);
    sprintf(ptr_smart_dev->currlog_backup, "%s%s_smart.bak", g_smart_file_folder, physicalDev);

    sprintf(ptr_smart_dev->fulllog_file, "%s%s_log.bin", g_smart_file_folder, physicalDev);
    sprintf(ptr_smart_dev->fulllog_backup, "%s%s_log.bak", g_smart_file_folder, physicalDev);

    ptr_smart_dev->smart_pool_idx = gp_dev_mngr->allocated_pool_count;
    ++ gp_dev_mngr->allocated_pool_count;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);

    smart_data_reset_currlog(ptr_smart_data);
    smart_data_reset_fulllog(ptr_smart_data);

    ptr_smart_dev->smart_config.sampling_rate = DEFAULT_SMART_LOG_SAMPLING_RATE;

    ++ gp_dev_mngr->device_count;
}

void debug_init(void)
{
    gp_dev_mngr = kmalloc(sizeof(cmn_smart_buffer), GFP_KERNEL);

    smart_dev_mngr_initialize();
    make_sample();
}

void dbg_update_currlog(char* dev_path)
{
    cmn_smart_device* p_smart_dev;
    cmn_smart_data* ptr_smart_data;
    uint32_t next_active_idx;
    cmn_raw_smart *ptr_next_buf;
    uint16_t total_att;
    cmn_raw_attr* ptr_att_src;
    cmn_raw_attr* ptr_att_dst;
    uint16_t  n_att;

    printk(KERN_DEBUG "\n  + dbg_update_currlog(%s)", dev_path);

    p_smart_dev = get_smart_device(dev_path);

    if(NULL == p_smart_dev) return;

    ptr_smart_data = get_smart_data(p_smart_dev->smart_pool_idx);
    if(NULL == ptr_smart_data) return;

    // copy current smart to active buffer
    next_active_idx = (ptr_smart_data->currlog.raw_counter + 1) % MAX_BUFFER_COUNT;
    ptr_next_buf = &ptr_smart_data->currlog.raw_buffer[next_active_idx];

    total_att = (sizeof(cmn_raw_smart) - sizeof(cmn_raw_acc)) / sizeof(cmn_raw_attr);
    ptr_att_src = (cmn_raw_attr*)&ptr_smart_data->currlog.raw_attr;
    ptr_att_dst = (cmn_raw_attr*)ptr_next_buf;

    for (n_att = 0; n_att < total_att; n_att ++)
    {
        do {
            memcpy(ptr_att_dst, ptr_att_src, sizeof(cmn_raw_attr));
        } while (0 != memcmp(ptr_att_src, ptr_att_dst, sizeof(cmn_raw_attr)));
        ++ ptr_att_dst;
        ++ ptr_att_src;
    }

    ++ ptr_smart_data->currlog.raw_counter;
}

// end debug

void smart_usb_create(void)
{
    // debug_init();
    gp_dev_mngr = shmem_ptr;
    init_smart_buffer(gp_dev_mngr);
}

/*
static void smart_usb_count_total_lba(const struct fsg_common *common, int reply)
{
    char path_buf[FILE_PATH_BUF + 1];
    char *path;
    cmn_smart_device* ptr_smart_dev;
    cmn_smart_data*   ptr_smart_data;
    cmn_raw_smart *p_cur_raw_att;
    u32 length;

    //if(reply < 0) return;

    if(0 == common->data_size_from_cmnd) return;

    path = d_path(&common->curlun->filp->f_path, path_buf, FILE_PATH_BUF - 1);

    ptr_smart_dev = get_smart_device(path);
    if (NULL == ptr_smart_dev) return;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if (NULL == ptr_smart_data) return;

    p_cur_raw_att = &ptr_smart_data->currlog.raw_attr;
    //length = (min((u32)reply, common->data_size_from_cmnd));
    length = common->data_size_from_cmnd;

    switch (common->cmnd[0]) {
    case INQUIRY:
    case MODE_SELECT:
    case MODE_SELECT_10:
    case MODE_SENSE:
    case MODE_SENSE_10:
    case ALLOW_MEDIUM_REMOVAL:
        break;

    case READ_6:
    case READ_10:
    case READ_12:
        p_cur_raw_att->acc.curr_read += length;
        break;

    case READ_CAPACITY:
    case READ_HEADER:
    case READ_TOC:
    case READ_FORMAT_CAPACITIES:
    case REQUEST_SENSE:
    case START_STOP:
    case SYNCHRONIZE_CACHE:
    case TEST_UNIT_READY:
        break;

    case VERIFY:
        p_cur_raw_att->acc.curr_read    += length;
        p_cur_raw_att->acc.curr_written += length;
        break;

    case WRITE_6:
    case WRITE_10:
    case WRITE_12:
        p_cur_raw_att->acc.curr_written += length;
        break;

    // process ata pass through
    case ATA_PASS_THROUGH_16:
        switch (common->cmnd[14]) {
            case ATA_READ_DMA:
                p_cur_raw_att->acc.curr_read += length;
                break;

            case ATA_WRITE_DMA:
                p_cur_raw_att->acc.curr_written += length;
                break;

            default:
                break;
        }
        break;

    case FORMAT_UNIT:
    case RELEASE:
    case RESERVE:
    case SEND_DIAGNOSTIC:

    default:
        break;
    }

    if (p_cur_raw_att->acc.curr_read >= READ_WRITE_SCALE_IN_BYTES) {
        p_cur_raw_att->lba_read.raw_low += (p_cur_raw_att->acc.curr_read / READ_WRITE_SCALE_IN_BYTES);
        p_cur_raw_att->acc.curr_read %= READ_WRITE_SCALE_IN_BYTES;
    }

    if (p_cur_raw_att->acc.curr_written >= READ_WRITE_SCALE_IN_BYTES) {
        p_cur_raw_att->lba_written.raw_low += (p_cur_raw_att->acc.curr_written / READ_WRITE_SCALE_IN_BYTES);
        p_cur_raw_att->acc.curr_written %= READ_WRITE_SCALE_IN_BYTES;
    }
}
*/
