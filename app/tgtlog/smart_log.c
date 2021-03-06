#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/mmc/ioctl.h>

#include "mmc.h"
#include "smart_common.h"
#include "smart_tgt.h"
#include "scsi_cmnd.h"
#include "tgtd.h"

#include "shmem_util.h"

#include "smart_log.h"

static const char g_smart_file_folder[128] = "/home/root/";

static cmn_smart_buffer* gp_dev_mngr;
static pthread_t g_log_smart_thread;

// interface function
void smart_intialize(void)
{
    DO_SKIP();
    gp_dev_mngr = (cmn_smart_buffer*)shmem_get(sizeof(cmn_smart_buffer));

    smart_dev_mngr_initialize();

    smart_tgt_create(gp_dev_mngr);
}

void smart_load_data(char* dev_path)
{
    DO_SKIP();
    smart_dev_mngr_add(dev_path);
}

// internal function
bool search_token_physical_device(const char* dev_path, char* dev_name)
{
    cmn_phys_token token_list[] = { {"mmcblk",   0,   7, NUMBER},
                                  //{"sd"    , 'a', 'z', CHARACTER},
                                  //{"sg"    ,   0,   7, NUMBER},
                                  };

    uint8_t n_item, num_item = sizeof(token_list) / sizeof(cmn_phys_token);

    for (n_item = 0; n_item < num_item; ++ n_item) {
        cmn_phys_token* pItem = &token_list[n_item];

        if (strstr(dev_path, pItem->token) != NULL) {
            char nDev;
            for (nDev = pItem->sequence_start; nDev <= pItem->sequence_end; ++ nDev) {
                // Make physical name
                if(NUMBER == pItem->enum_type)
                    sprintf(dev_name, "%s%u", pItem->token, nDev);
                else if(CHARACTER == pItem->enum_type)
                    sprintf(dev_name, "%s%c", pItem->token, nDev);
                else break;
    
                // Search physical name in the path
                if (strstr(dev_path, dev_name) != NULL) {
                    return true;
                }
            }
        }
    }

    dev_name[0] = '\0';
    return false;
}

// function
void smart_data_set_default(cmn_smart_data* ptr_smart_data)
{
    smart_data_reset_currlog(ptr_smart_data);
    smart_data_reset_fulllog(ptr_smart_data);
}

void smart_data_reset_currlog(cmn_smart_data* ptr_smart_data)
{
    cmn_raw_smart* p_raw_attr = &ptr_smart_data->currlog.raw_attr;
    memset(p_raw_attr, 0, sizeof(cmn_raw_smart));
    set_smart_id(p_raw_attr);
    p_raw_attr->endurance.raw_low = ATTR_NAND_ENDURANCE;

    ptr_smart_data->currlog.raw_counter = 0;

    memcpy( &ptr_smart_data->currlog.raw_buffer[0],
            &ptr_smart_data->currlog.raw_attr,
            sizeof(ptr_smart_data->currlog.raw_buffer[0]));
}

void smart_data_reset_fulllog(cmn_smart_data* ptr_smart_data)
{
    /*
    // method memset
    cmn_smart_fulllog* p_log = &ptr_smart_data->fulllog;
    memset(p_log, 0, sizeof(cmn_smart_fulllog));
    p_log->sampling_rate = DEFAULT_SMART_LOG_SAMPLING_RATE;
    p_log->current_entry = MAX_LOG_COUNT - 1;
    p_log->fulllog_time  = 0;

    // at default no log is added so set all attribute ID is 0
    ptr_smart_data->fulllog_counter = 0;
    p_log = &ptr_smart_data->fulllog_buffer[0];
    memset(p_log, 0, sizeof(cmn_smart_fulllog));
    */

    // method set index
    ptr_smart_data->fulllog_counter = 0;
    cmn_smart_fulllog* p_log    = &ptr_smart_data->fulllog;
    cmn_smart_fulllog* p_active = &ptr_smart_data->fulllog_buffer[0];
    uint16_t nEntry;
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

void smart_dev_reset(cmn_smart_device* ptr_smart_device)
{
    ptr_smart_device->device_path[0]    = '\0';
    ptr_smart_device->physical_path[0]  = '\0';

    ptr_smart_device->currlog_file[0]   = '\0';
    ptr_smart_device->currlog_backup[0] = '\0';

    ptr_smart_device->fulllog_backup[0] = '\0';
    ptr_smart_device->fulllog_file[0]   = '\0';

    ptr_smart_device->smart_pool_idx = MAX_DEVICE_COUNT;

    smart_dev_reset_config(&ptr_smart_device->smart_config);
}

void smart_dev_reset_config(cmn_smart_config* ptr_config)
{
    ptr_config->sampling_rate = DEFAULT_SMART_LOG_SAMPLING_RATE;
}

void smart_dev_sampling_raw_smart(cmn_smart_device* ptr_smart_device, uint16_t sampling_rate, bool start_up)
{
    unsigned char buf[512];
    int rawfd;
    int tmpval;
    int ret;
    uint8_t next_active_idx;

    cmn_smart_data* ptr_smart_data = get_smart_data(ptr_smart_device->smart_pool_idx);
    if(NULL == ptr_smart_data) return;

    if(true == start_up) {
        sampling_rate = 0;
        ptr_smart_data->currlog.raw_counter = 0;
    }

    // update smart attribute
    cmn_raw_smart* p_raw_att = &ptr_smart_data->currlog.raw_attr;

    // up date smart only effect at start up
    if(true == start_up) {
        p_raw_att->power_count.raw_low += 1;
    }

    // Power on hour
    p_raw_att->acc.curr_power += sampling_rate;
    if (p_raw_att->acc.curr_power >= 60) {
        p_raw_att->power_hour.raw_low += (p_raw_att->acc.curr_power / 60);
        p_raw_att->acc.curr_power %= 60;
    }

    // temperature
    smart_att_read_temperature(ptr_smart_data);// The function smart_device_manager_add also update temp for initiate

    // read more attribute from e-mmc chip
    rawfd = open(ptr_smart_device->physical_path, O_RDWR | O_RSYNC);
    if (rawfd >= 0) {
        ret = read_extcsd(rawfd, buf);
        if (0 == ret) {
            // GetValue from eMMC Register
            p_raw_att->prog_sector.raw_low = GET_U32(buf, 302);
            p_raw_att->life_typea.raw_low  = GET_U08(buf, 268);
            p_raw_att->life_typeb.raw_low  = GET_U08(buf, 269);
            p_raw_att->extcsd_ver.raw_low  = GET_U08(buf, 192);

            tmpval = (p_raw_att->life_typea.raw_low % 11) * 10; p_raw_att->life_left.raw_low = (tmpval) ? (110 - tmpval) : 0;
            tmpval = (p_raw_att->life_typeb.raw_low % 11) * 10; p_raw_att->spare_block.raw_low = (tmpval) ? (110 - tmpval) : 0;

            ret = read_erasecnt(rawfd, buf);
            if (0 == ret) {
                // GetValue from VendorCommand: Value = 0x26E901EB; Buffer = [0] 26 [1] E9 [2] 01 [3] EB
                p_raw_att->max_erase.raw_low = VC_GET_U32(buf, 4);
                p_raw_att->ave_erase.raw_low = VC_GET_U32(buf, 8);

                p_raw_att->max_mlc.raw_low = VC_GET_U32(buf,  4);
                p_raw_att->ave_mlc.raw_low = VC_GET_U32(buf,  8);
                p_raw_att->max_slc.raw_low = VC_GET_U32(buf, 12);
                p_raw_att->ave_slc.raw_low = VC_GET_U32(buf, 16);
            }
        }

        close(rawfd);
    }

    // copy current smart to active buffer
    next_active_idx = (ptr_smart_data->currlog.raw_counter + 1) % MAX_BUFFER_COUNT;
    cmn_raw_smart *ptr_next_buf = &ptr_smart_data->currlog.raw_buffer[next_active_idx];

    uint16_t total_att = (sizeof(cmn_raw_smart) - sizeof(cmn_raw_acc)) / sizeof(cmn_raw_attr);
    cmn_raw_attr* ptr_att_src = (cmn_raw_attr*)&ptr_smart_data->currlog.raw_attr;
    cmn_raw_attr* ptr_att_dst = (cmn_raw_attr*)ptr_next_buf;

    // copy raw attribute
    uint16_t  n_att;
    for (n_att = 0; n_att < total_att; n_att ++)
    {
        do {
            memcpy(ptr_att_dst, ptr_att_src, sizeof(cmn_raw_attr));
        } while (0 != memcmp(ptr_att_src, ptr_att_dst, sizeof(cmn_raw_attr)));
        ++ ptr_att_dst;
        ++ ptr_att_src;
    }

    // copy accum
    do {
        memcpy(ptr_att_dst, ptr_att_src, sizeof(cmn_raw_acc));
    } while (0 != memcmp(ptr_att_src, ptr_att_dst, sizeof(cmn_raw_acc)));

    ++ ptr_smart_data->currlog.raw_counter;
}

void smart_dev_mngr_initialize()
{
    uint8_t n_dev;
    for (n_dev = 0; n_dev < MAX_DEVICE_COUNT; ++ n_dev) {
        smart_dev_reset(&gp_dev_mngr->device_list[n_dev]);
    }

    gp_dev_mngr->allocated_pool_count   = 0;
    gp_dev_mngr->device_count = 0;
    gp_dev_mngr->currlog_time = 0;
}

void smart_dev_mngr_add(char* dev_path)
{
    uint16_t len;
    uint8_t nDev;
    char physicalDev[32];
    cmn_smart_device* ptr_smart_dev;

    len = strlen(dev_path);

    if(0 == len) return;
    if(len >= MAX_DEVICE_PATH) return;

    if(gp_dev_mngr->device_count >= MAX_DEVICE_COUNT) return;

    // add new device
    ptr_smart_dev = &gp_dev_mngr->device_list[gp_dev_mngr->device_count];

    smart_dev_reset(ptr_smart_dev);

    if (false == search_token_physical_device(dev_path, physicalDev)) return;
    // assign path
    sprintf(ptr_smart_dev->device_path, "%s", dev_path);
    sprintf(ptr_smart_dev->physical_path, "/dev/%s", physicalDev);

    // assign path to smart file
    sprintf(ptr_smart_dev->currlog_file, "%s%s_smart.bin", g_smart_file_folder, physicalDev);
    sprintf(ptr_smart_dev->currlog_backup, "%s%s_smart.bak", g_smart_file_folder, physicalDev);

    sprintf(ptr_smart_dev->fulllog_file, "%s%s_log.bin", g_smart_file_folder, physicalDev);
    sprintf(ptr_smart_dev->fulllog_backup, "%s%s_log.bak", g_smart_file_folder, physicalDev);

    sprintf(ptr_smart_dev->config_file, "%s%s_cfg.bin", g_smart_file_folder, physicalDev);
    sprintf(ptr_smart_dev->config_backup, "%s%s_cfg.bak", g_smart_file_folder, physicalDev);

    // search existed Smart device by smart file path
    for (nDev = 0; nDev < gp_dev_mngr->device_count; ++ nDev) {
        if(0 == strcmp(ptr_smart_dev->currlog_file, gp_dev_mngr->device_list[nDev].currlog_file)) {
            ptr_smart_dev->smart_pool_idx = gp_dev_mngr->device_list[nDev].smart_pool_idx;
            break;
        }
    }

    // no existed device
    if (nDev == gp_dev_mngr->device_count) {
        // allocate a buffer for new device
        ptr_smart_dev->smart_pool_idx = gp_dev_mngr->allocated_pool_count;
        ++ gp_dev_mngr->allocated_pool_count;

        // load device config
        smart_dev_mngr_load_config(ptr_smart_dev);

        // load current smart
        smart_dev_mngr_load_currlog(ptr_smart_dev);

        // save file to keep some att effected by power cycle
        smart_dev_mngr_save_currlog(ptr_smart_dev);

        // load smart log
        smart_dev_mngr_load_fulllog(ptr_smart_dev);
    }

    // debug
    char str_buf[128];
    sprintf(str_buf, "Add Index %u", gp_dev_mngr->device_count);
    debug_dump_smart_device(ptr_smart_dev, str_buf);
    // end debug

    ++ gp_dev_mngr->device_count;

    if(1 == gp_dev_mngr->device_count) {
        pthread_create(&g_log_smart_thread, NULL, thread_update_smart_att, NULL);
    }
}

void smart_dev_mngr_save_all(void)
{
    uint8_t nDev;
    for (nDev = 0; nDev < gp_dev_mngr->device_count; ++ nDev) {
        smart_dev_mngr_save(&gp_dev_mngr->device_list[nDev]);
    }
}

void smart_dev_mngr_save(const cmn_smart_device* ptr_smart_dev)
{
    smart_dev_mngr_save_currlog(ptr_smart_dev);
    smart_dev_mngr_save_fulllog(ptr_smart_dev);
}

void smart_dev_mngr_save_fulllog(const cmn_smart_device* ptr_smart_dev)
{
    cmn_smart_data* ptr_smart_data;
    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);

    if(NULL == ptr_smart_data) return;

    cmn_smart_fulllog* p_fulllog = get_device_fulllog(ptr_smart_data, NULL);
    save_file((char*)p_fulllog,
              sizeof(cmn_smart_fulllog),
              ptr_smart_dev->fulllog_file,
              ptr_smart_dev->fulllog_backup);
}

void smart_dev_mngr_load_fulllog(cmn_smart_device* ptr_smart_dev)
{
    cmn_smart_data* ptr_smart_data;
    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);

    if(false == load_file((char*)&ptr_smart_data->fulllog,
                          sizeof(cmn_smart_fulllog),
                          ptr_smart_dev->fulllog_file,
                          ptr_smart_dev->fulllog_backup))
    {
        smart_data_reset_fulllog(ptr_smart_data);
    } else {
        update_device_fulllog(ptr_smart_data, true);
    }
}

void smart_dev_mngr_save_currlog(const cmn_smart_device* ptr_smart_dev)
{
    cmn_smart_data* ptr_smart_data;
    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);

    if(NULL == ptr_smart_data) return;

    cmn_raw_smart* p_currlog = get_device_currlog(ptr_smart_data, NULL);
    save_file((char*)p_currlog,
              sizeof(cmn_raw_smart),
              ptr_smart_dev->currlog_file,
              ptr_smart_dev->currlog_backup);
}

void smart_dev_mngr_load_currlog(cmn_smart_device* ptr_smart_dev)
{
    cmn_smart_data* ptr_smart_data;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if(false == load_file((char*)&ptr_smart_data->currlog.raw_attr,
                          sizeof(cmn_raw_smart),
                          ptr_smart_dev->currlog_file,
                          ptr_smart_dev->currlog_backup))
    {
        smart_data_reset_currlog(ptr_smart_data);
    }

    smart_dev_sampling_raw_smart(ptr_smart_dev, 0, true);
}

void smart_dev_mngr_load_config(cmn_smart_device* ptr_smart_dev)
{
    if(false == load_file((char*)&ptr_smart_dev->smart_config,
                          sizeof(ptr_smart_dev->smart_config),
                          ptr_smart_dev->config_file,
                          ptr_smart_dev->config_backup))
    {
        smart_dev_reset_config(&ptr_smart_dev->smart_config);
    }
}

void update_raw_smart_data(void)
{
    uint8_t n_dev;

    for (n_dev = 0; n_dev < gp_dev_mngr->device_count; ++ n_dev) {
        cmn_smart_data* ptr_smart_data = get_smart_data(gp_dev_mngr->device_list[n_dev].smart_pool_idx);
        if(NULL == ptr_smart_data) continue;

        smart_dev_sampling_raw_smart(&gp_dev_mngr->device_list[n_dev], TIMER_READ_INTERNAL_SMART, false);
    }
}

void* thread_update_smart_att(void* params)
{
    uint8_t internal_smart_tick_count;
    cmn_smart_device* ptr_smart_dev;
    cmn_smart_data* ptr_smart_data;
    uint16_t att_idx;
    cmn_smart_attr *p_log_attr;
    cmn_raw_smart  *p_raw_attr;

    internal_smart_tick_count = 0;
    while (1) {

        sleep(60); // fix this number to create timer 1 minute
        if(0 == gp_dev_mngr->device_count) continue;

        // Sample more attribute
        ++ internal_smart_tick_count;
        if (TIMER_READ_INTERNAL_SMART <= internal_smart_tick_count) {
            internal_smart_tick_count = 0;
            update_raw_smart_data();
        }

        ++ gp_dev_mngr->currlog_time;

        // Check timer save data to file
        uint8_t n_dev;
        for (n_dev = 0; n_dev < gp_dev_mngr->device_count; ++ n_dev) {
            ptr_smart_dev  = &gp_dev_mngr->device_list[n_dev];
            ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
            if(NULL == ptr_smart_data) continue;

            // Save current smart
            if (TIMER_BACKUP_SMART <= gp_dev_mngr->currlog_time) {
                smart_dev_mngr_save_currlog(ptr_smart_dev);
            }

            ++ ptr_smart_data->fulllog.fulllog_time;
            if (ptr_smart_data->fulllog.fulllog_time >= ptr_smart_dev->smart_config.sampling_rate) {
                uint32_t lastSequence = ptr_smart_data->fulllog.entry_list[ptr_smart_data->fulllog.current_entry].index;

                uint16_t log_index = (ptr_smart_data->fulllog.current_entry + 1) % MAX_LOG_COUNT;

                // copy current smart to log
                memset(&ptr_smart_data->fulllog.entry_list[log_index], 0, sizeof(ptr_smart_data->fulllog.entry_list[0]));

                // header
                ptr_smart_data->fulllog.entry_list[log_index].time_stamp = ptr_smart_data->fulllog.fulllog_time;
                ptr_smart_data->fulllog.entry_list[log_index].index      = lastSequence + 1;

                att_idx = 0;
                p_log_attr = &ptr_smart_data->fulllog.entry_list[log_index].attr_list[0];
                p_raw_attr = &ptr_smart_data->currlog.raw_attr;

                #define MAP_ITEM(name,index,code) \
                    p_log_attr[att_idx].attr_id   = p_raw_attr->name.attr_id; \
                    p_log_attr[att_idx].value     = p_raw_attr->name.raw_low; \
                    p_log_attr[att_idx].raw_value = p_raw_attr->name.raw_low; \
                    ++ att_idx;

                #include "smart_attr.h"
                #undef MAP_ITEM

                ptr_smart_data->fulllog.fulllog_time = 0;
                ptr_smart_data->fulllog.current_entry = log_index;

                update_device_fulllog(ptr_smart_data, false);

                smart_dev_mngr_save_fulllog(ptr_smart_dev);
            }
        } // end of for

        if (TIMER_BACKUP_SMART <= gp_dev_mngr->currlog_time) {
            gp_dev_mngr->currlog_time = 0;
        }
    } // end of while
}

void update_device_fulllog(cmn_smart_data* ptr_smart_data, bool start_up)
{
    uint32_t next_idx;
    cmn_smart_fulllog* ptr_next_smart_log_buf;

    if(true == start_up) {
        ptr_smart_data->fulllog_counter = 0;
    }

    next_idx               = ((ptr_smart_data->fulllog_counter + 1) % MAX_BUFFER_COUNT);
    ptr_next_smart_log_buf = &ptr_smart_data->fulllog_buffer[next_idx];
    memcpy(ptr_next_smart_log_buf, &ptr_smart_data->fulllog, sizeof(cmn_smart_fulllog));

    // update next buffer
    ++ ptr_smart_data->fulllog_counter;
}

void smart_att_read_temperature(cmn_smart_data* ptr_smart_data)
{
    ptr_smart_data->currlog.raw_attr.temperature.raw_low = 25;// Read temperature sensor.
}

void set_smart_id(cmn_raw_smart* smart_data)
{
    #define MAP_ITEM(name,index,code) smart_data->name.attr_id = code;
    #include "smart_attr.h"
    #undef MAP_ITEM
}

bool load_file(char* buffer, uint32_t size, const char* load, const char* backup)
{
    FILE *ptr_file;

    // load file
    ptr_file = fopen(load, "rb+");

    // if file load from backup
    if (NULL == ptr_file) ptr_file = fopen(backup, "rb+");

    // return fail if still fail on opening backup file
    if (NULL == ptr_file) return false;

    bool rc;

    rc = (1 == fread(buffer, size, 1, ptr_file));
    fclose(ptr_file);

    return rc;
}

int send_mmc_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buffer)
{
    struct mmc_ioc_cmd idata;
    memset(&idata, 0, sizeof(idata));

    idata.arg = arg;
    idata.opcode = cmd;
    idata.write_flag = wflag;

    idata.blksz = 512;
    idata.blocks = 1;
    idata.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

    mmc_ioc_cmd_set_data(idata, buffer);

    return ioctl(fd, MMC_IOC_CMD, &idata);
}

int read_extcsd(int fd, unsigned char* buf)
{
    return send_mmc_cmd(fd, MMC_SEND_EXT_CSD, 0, 0, buf);
}

int read_erasecnt(int fd, unsigned char* buf)
{
    int ret = 0;
    unsigned char cmd[512];
    memset(cmd, 0, sizeof(__u8) * 512);

    cmd[0] = 0x07; // subcode
    cmd[4] = 0x26; // passwd for toshiba emmc
    cmd[5] = 0xE9;
    cmd[6] = 0x01;
    cmd[7] = 0xEB;

    ret = send_mmc_cmd(fd, 56, 0, 1, cmd); // arg = 0. write = true
    if (0 != ret) return ret;

    ret = send_mmc_cmd(fd, 56, 1, 0, buf); // arg = 1. write = false
    if (0 != ret) return ret;

    return 0;
}

// debug function
//void debug_log_cmd(struct scsi_cmd *cmd)
//{
//    FILE *dbgfd;
//    uint32_t len_in  = scsi_get_in_length(cmd);
//    uint32_t len_out = scsi_get_out_length(cmd);
//
//    dbgfd = fopen("/home/root/log.txt", "a");
//    if (NULL != dbgfd) {
//            fprintf(dbgfd, "\nDev: %s - DevID: %llu", cmd->dev->path, cmd->dev_id);
//            //fprintf(dbgfd, " - c_target: %s", cmd->c_target->name);
//            fprintf(dbgfd, " - bst: %s - Cmd: 0x%X - XferIn: %u - XferOut: %u", cmd->dev->bst->bs_name, cmd->scb[0], len_in, len_out);
//
//            fclose(dbgfd);
//    }
//}

void debug_dump_log(void* buffer, uint32_t size)
{
    FILE *dump_file;
    dump_file = fopen("/home/root/dump.txt", "ab");
    if (NULL != dump_file) {
        fwrite(buffer, size, 1, dump_file);
        fclose(dump_file);
   }
}

void debug_dump_smart_device(const cmn_smart_device* ptr_smart_device, char* header)
{
    FILE *dump_file;
    char str_buf[128];
    cmn_raw_attr* ptr_att;
    cmn_smart_data* ptr_smart_data;

    sprintf(str_buf, "%s.txt", ptr_smart_device->currlog_file);
    dump_file = fopen(str_buf, "ab");

    if (NULL != dump_file) {
        fprintf(dump_file, "\n\n-----");
        if(NULL != header) fprintf(dump_file, " %s ", header);
        fprintf(dump_file, "-----");

        fprintf(dump_file, "\n  + Path: %s", ptr_smart_device->device_path);
        fprintf(dump_file, "\n  + File: %s", ptr_smart_device->currlog_file);
        fprintf(dump_file, "\n  + Physical: %s", ptr_smart_device->physical_path);

        // Print current SMART
        uint16_t total_att = (sizeof(cmn_raw_smart)  - sizeof(cmn_raw_acc))/ sizeof(cmn_raw_attr);

        fprintf(dump_file, "\n***** Current SMART total : %u", total_att);

        ptr_smart_data = get_smart_data(ptr_smart_device->smart_pool_idx);
        ptr_att = (cmn_raw_attr*)&ptr_smart_data->currlog.raw_attr;

        uint16_t  n_att;
        for (n_att = 0; n_att < total_att; n_att ++)
        {
            fprintf(dump_file, "\n[%u]: id[%u] = %u", n_att, ptr_att->attr_id, ptr_att->raw_low);
            ++ ptr_att;
        }

        fprintf(dump_file, "\naccum: power = %u -read = %u -written = %u",
                ptr_smart_data->currlog.raw_attr.acc.curr_power,
                ptr_smart_data->currlog.raw_attr.acc.curr_read,
                ptr_smart_data->currlog.raw_attr.acc.curr_written);

        // Smart log
        fprintf(dump_file,
                "\n***** SMART Log - Index %u - log size %u bytes",
                ptr_smart_data->fulllog.current_entry,
                sizeof(ptr_smart_data->fulllog));

        fclose(dump_file);
    }
}

