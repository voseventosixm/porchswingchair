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

static void* update_smart_func(void* param);

static pthread_t sml_thread_func;
static cmn_smart_buffer* sml_buffer;
static const char sml_location[128] = "/home/root/";

// interface function
void smartlog_intialize(void)
{
    DO_SKIP();
    sml_buffer = (cmn_smart_buffer*)shmem_get(sizeof(cmn_smart_buffer));

    sml_initialize();

    smart_tgt_create(sml_buffer);
}

void smartlog_load_data(char* dev_path)
{
    DO_SKIP();
    sml_add_device(dev_path);
}

// internal function
bool get_device_name(const char* devpath, char* devname)
{
    cmn_phys_token token_list[] = { {"mmcblk",   0,   7, NUMBER},
                                  //{"sd"    , 'a', 'z', CHARACTER},
                                  //{"sg"    ,   0,   7, NUMBER},
                                  };

    uint8_t i, token_count = sizeof(token_list) / sizeof(cmn_phys_token);

    for (i = 0; i < token_count; ++ i) {
        cmn_phys_token* item = &token_list[i];

        if (strstr(devpath, item->token) != NULL) {
            char key;
            for (key = item->sequence_start; key <= item->sequence_end; ++ key) {
                // Make physical name
                if(NUMBER == item->enum_type)
                    sprintf(devname, "%s%u", item->token, key);
                else if(CHARACTER == item->enum_type)
                    sprintf(devname, "%s%c", item->token, key);
                else break;
    
                // Search physical name in the path
                if (strstr(devpath, devname) != NULL) {
                    return true;
                }
            }
        }
    }

    devname[0] = '\0';
    return false;
}

// function
void sml_reset_data(cmn_smart_data* dataptr)
{
    sml_reset_currlog(dataptr);
    sml_reset_fulllog(dataptr);
}

void sml_reset_currlog(cmn_smart_data* dataptr)
{
    cmn_raw_smart* rawptr = &dataptr->currlog.raw_attr;
    memset(rawptr, 0, sizeof(cmn_raw_smart));
    init_attribute_id(rawptr);
    rawptr->endurance.raw_low = ATTR_NAND_ENDURANCE;

    dataptr->currlog.raw_counter = 0;

    memcpy( &dataptr->currlog.raw_buffer[0],
            &dataptr->currlog.raw_attr,
            sizeof(dataptr->currlog.raw_buffer[0]));
}

void sml_reset_fulllog(cmn_smart_data* dataptr)
{
    // method set index
    dataptr->fulllog_counter = 0;
    cmn_smart_fulllog* logptr    = &dataptr->fulllog;
    cmn_smart_fulllog* bufptr = &dataptr->fulllog_buffer[0];

    uint16_t i;
    for(i = 0; i < MAX_LOG_COUNT; ++ i) {
        logptr->entry_list[i].index      = 0;
        logptr->entry_list[i].time_stamp = 0;

        bufptr->entry_list[i].index      = 0;
        bufptr->entry_list[i].time_stamp = 0;
    }

    logptr->fulllog_time  = 0;
    logptr->current_entry = MAX_LOG_COUNT - 1;

    bufptr->fulllog_time  = 0;
    bufptr->current_entry = MAX_LOG_COUNT - 1;
}

void sml_reset_device(cmn_smart_device* devptr)
{
    devptr->device_path[0]    = '\0';
    devptr->physical_path[0]  = '\0';

    devptr->currlog_file[0]   = '\0';
    devptr->currlog_backup[0] = '\0';

    devptr->fulllog_backup[0] = '\0';
    devptr->fulllog_file[0]   = '\0';

    devptr->smart_pool_idx = MAX_DEVICE_COUNT;

    sml_reset_config(&devptr->smart_config);
}

void sml_reset_config(cmn_smart_config* confptr)
{
    confptr->sampling_rate = DEFAULT_SMART_LOG_SAMPLING_RATE;
}

void sml_sample_attribute(cmn_smart_device* devptr, uint16_t samrate, bool startup)
{
    unsigned char buf[512];
    int rawfd;
    int tmpval;
    int ret;

    cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
    if(NULL == dataptr) return;

    if(true == startup) {
        samrate = 0;
        dataptr->currlog.raw_counter = 0;
    }

    // update smart attribute
    cmn_raw_smart* rawptr = &dataptr->currlog.raw_attr;

    // up date smart only effect at start up
    if(true == startup) {
        rawptr->power_count.raw_low += 1;
    }

    // Power on hour
    rawptr->acc.curr_power += samrate;
    if (rawptr->acc.curr_power >= 60) {
        rawptr->power_hour.raw_low += (rawptr->acc.curr_power / 60);
        rawptr->acc.curr_power %= 60;
    }

    // temperature
    sml_sample_temperature(dataptr);// The function smart_device_manager_add also update temp for initiate

    // read more attribute from e-mmc chip
    rawfd = open(devptr->physical_path, O_RDWR | O_RSYNC);
    if (rawfd >= 0) {
        ret = read_extcsd(rawfd, buf);
        if (0 == ret) {
            // GetValue from eMMC Register
            rawptr->prog_sector.raw_low = GET_U32(buf, 302);
            rawptr->life_typea.raw_low  = GET_U08(buf, 268);
            rawptr->life_typeb.raw_low  = GET_U08(buf, 269);
            rawptr->extcsd_ver.raw_low  = GET_U08(buf, 192);

            tmpval = (rawptr->life_typea.raw_low % 11) * 10; rawptr->life_left.raw_low = (tmpval) ? (110 - tmpval) : 0;
            tmpval = (rawptr->life_typeb.raw_low % 11) * 10; rawptr->spare_block.raw_low = (tmpval) ? (110 - tmpval) : 0;

            ret = read_erasecount(rawfd, buf);
            if (0 == ret) {
                // GetValue from VendorCommand: Value = 0x26E901EB; Buffer = [0] 26 [1] E9 [2] 01 [3] EB
                rawptr->max_erase.raw_low = VC_GET_U32(buf, 4);
                rawptr->ave_erase.raw_low = VC_GET_U32(buf, 8);

                rawptr->max_mlc.raw_low = VC_GET_U32(buf,  4);
                rawptr->ave_mlc.raw_low = VC_GET_U32(buf,  8);
                rawptr->max_slc.raw_low = VC_GET_U32(buf, 12);
                rawptr->ave_slc.raw_low = VC_GET_U32(buf, 16);
            }
        }

        close(rawfd);
    }

    // copy current smart to active buffer
    uint8_t next_index = (dataptr->currlog.raw_counter + 1) % MAX_BUFFER_COUNT;
    cmn_raw_smart *nextptr = &dataptr->currlog.raw_buffer[next_index];

    uint16_t attr_count = (sizeof(cmn_raw_smart) - sizeof(cmn_raw_acc)) / sizeof(cmn_raw_attr);
    cmn_raw_attr* srcptr = (cmn_raw_attr*)&dataptr->currlog.raw_attr;
    cmn_raw_attr* dstptr = (cmn_raw_attr*)nextptr;

    // copy raw attribute
    uint16_t  i;
    for (i = 0; i < attr_count; i ++)
    {
        do {
            memcpy(dstptr, srcptr, sizeof(cmn_raw_attr));
        } while (0 != memcmp(srcptr, dstptr, sizeof(cmn_raw_attr)));
        ++ dstptr;
        ++ srcptr;
    }

    // copy accum
    do {
        memcpy(dstptr, srcptr, sizeof(cmn_raw_acc));
    } while (0 != memcmp(srcptr, dstptr, sizeof(cmn_raw_acc)));

    ++ dataptr->currlog.raw_counter;
}

void sml_initialize()
{
    uint8_t i;
    for (i = 0; i < MAX_DEVICE_COUNT; ++ i) {
        sml_reset_device(&sml_buffer->device_list[i]);
    }

    sml_buffer->allocated_pool_count   = 0;
    sml_buffer->device_count = 0;
    sml_buffer->currlog_time = 0;
}

void sml_add_device(char* devpath)
{
    uint16_t len;
    uint8_t i;
    char devname[32];
    cmn_smart_device* devptr;

    len = strlen(devpath);

    if(0 == len) return;
    if(len >= MAX_DEVICE_PATH) return;

    if(sml_buffer->device_count >= MAX_DEVICE_COUNT) return;

    // add new device
    devptr = &sml_buffer->device_list[sml_buffer->device_count];

    sml_reset_device(devptr);

    if (false == get_device_name(devpath, devname)) return;

    // assign path
    sprintf(devptr->device_path, "%s", devpath);
    sprintf(devptr->physical_path, "/dev/%s", devname);

    // assign path to smart file
    sprintf(devptr->currlog_file, "%s%s_smart.bin", sml_location, devname);
    sprintf(devptr->currlog_backup, "%s%s_smart.bak", sml_location, devname);

    sprintf(devptr->fulllog_file, "%s%s_log.bin", sml_location, devname);
    sprintf(devptr->fulllog_backup, "%s%s_log.bak", sml_location, devname);

    sprintf(devptr->config_file, "%s%s_cfg.bin", sml_location, devname);
    sprintf(devptr->config_backup, "%s%s_cfg.bak", sml_location, devname);

    // search existed Smart device by smart file path
    for (i = 0; i < sml_buffer->device_count; ++ i)
    {
        if(0 == strcmp(devptr->currlog_file, sml_buffer->device_list[i].currlog_file))
        {
            devptr->smart_pool_idx = sml_buffer->device_list[i].smart_pool_idx;
            break;
        }
    }

    // no existed device
    if (i == sml_buffer->device_count)
    {
        // allocate a buffer for new device
        devptr->smart_pool_idx = sml_buffer->allocated_pool_count;
        ++ sml_buffer->allocated_pool_count;

        // load device config
        sml_load_config(devptr);

        // load current smart
        sml_load_currlog(devptr);

        // save file to keep some att effected by power cycle
        sml_save_currlog(devptr);

        // load smart log
        sml_load_fulllog(devptr);
    }

    // debug
    char str_buf[128];
    sprintf(str_buf, "Add Index %u", sml_buffer->device_count);
    sml_dump_device(devptr, str_buf);
    // end debug

    ++ sml_buffer->device_count;

    if(1 == sml_buffer->device_count) {
        pthread_create(&sml_thread_func, NULL, update_smart_func, NULL);
    }
}

void sml_save_all(void)
{
    uint8_t i;
    for (i = 0; i < sml_buffer->device_count; ++ i) {
        sml_save_device(&sml_buffer->device_list[i]);
    }
}

void sml_save_device(const cmn_smart_device* devptr)
{
    sml_save_currlog(devptr);
    sml_save_fulllog(devptr);
}

void sml_save_fulllog(const cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;

    dataptr = get_smart_data(devptr->smart_pool_idx);
    if(NULL == dataptr) return;

    cmn_smart_fulllog* flogptr = get_device_fulllog(dataptr, NULL);
    save_file((char*)flogptr,
              sizeof(cmn_smart_fulllog),
              devptr->fulllog_file,
              devptr->fulllog_backup);
}

void sml_load_fulllog(cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;
    dataptr = get_smart_data(devptr->smart_pool_idx);

    if(false == load_file((char*)&dataptr->fulllog,
                          sizeof(cmn_smart_fulllog),
                          devptr->fulllog_file,
                          devptr->fulllog_backup))
    {
        sml_reset_fulllog(dataptr);
    } else {
        update_device_fulllog(dataptr, true);
    }
}

void sml_save_currlog(const cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;
    dataptr = get_smart_data(devptr->smart_pool_idx);

    if(NULL == dataptr) return;

    cmn_raw_smart* clogptr = get_device_currlog(dataptr, NULL);
    save_file((char*)clogptr,
              sizeof(cmn_raw_smart),
              devptr->currlog_file,
              devptr->currlog_backup);
}

void sml_load_currlog(cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;

    dataptr = get_smart_data(devptr->smart_pool_idx);
    if(false == load_file((char*)&dataptr->currlog.raw_attr,
                          sizeof(cmn_raw_smart),
                          devptr->currlog_file,
                          devptr->currlog_backup))
    {
        sml_reset_currlog(dataptr);
    }

    sml_sample_attribute(devptr, 0, true);
}

void sml_load_config(cmn_smart_device* devptr)
{
    if(false == load_file((char*)&devptr->smart_config,
                          sizeof(devptr->smart_config),
                          devptr->config_file,
                          devptr->config_backup))
    {
        sml_reset_config(&devptr->smart_config);
    }
}

void update_raw_smart_data(void)
{
    uint8_t i;

    for (i = 0; i < sml_buffer->device_count; ++ i) {
        cmn_smart_device* devptr = &sml_buffer->device_list[i];
        cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
        if(NULL == dataptr) continue;

        sml_sample_attribute(devptr, TIMER_READ_INTERNAL_SMART, false);
    }
}

void* update_smart_func(void* param)
{
    uint16_t att_idx;
    cmn_smart_device* devptr;
    cmn_smart_data* dataptr;
    cmn_smart_attr *attrptr;
    cmn_raw_smart  *rawptr;

    uint8_t internal_count = 0;

    while (1) {

        sleep(60); // fix this number to create timer 1 minute
        if(0 == sml_buffer->device_count) continue;

        // Sample more attribute
        ++ internal_count;
        if (TIMER_READ_INTERNAL_SMART <= internal_count) {
            internal_count = 0;
            update_raw_smart_data();
        }

        ++ sml_buffer->currlog_time;

        // Check timer save data to file
        uint8_t n_dev;
        for (n_dev = 0; n_dev < sml_buffer->device_count; ++ n_dev) {
            devptr  = &sml_buffer->device_list[n_dev];
            dataptr = get_smart_data(devptr->smart_pool_idx);
            if(NULL == dataptr) continue;

            // Save current smart
            if (TIMER_BACKUP_SMART <= sml_buffer->currlog_time) {
                sml_save_currlog(devptr);
            }

            ++ dataptr->fulllog.fulllog_time;
            if (dataptr->fulllog.fulllog_time >= devptr->smart_config.sampling_rate) {
                uint32_t lastSequence = dataptr->fulllog.entry_list[dataptr->fulllog.current_entry].index;

                uint16_t log_index = (dataptr->fulllog.current_entry + 1) % MAX_LOG_COUNT;

                // copy current smart to log
                memset(&dataptr->fulllog.entry_list[log_index], 0, sizeof(dataptr->fulllog.entry_list[0]));

                // header
                dataptr->fulllog.entry_list[log_index].time_stamp = dataptr->fulllog.fulllog_time;
                dataptr->fulllog.entry_list[log_index].index      = lastSequence + 1;

                att_idx = 0;
                attrptr = &dataptr->fulllog.entry_list[log_index].attr_list[0];
                rawptr = &dataptr->currlog.raw_attr;

                #define MAP_ITEM(name,index,code) \
                    attrptr[att_idx].attr_id   = rawptr->name.attr_id; \
                    attrptr[att_idx].value     = rawptr->name.raw_low; \
                    attrptr[att_idx].raw_value = rawptr->name.raw_low; \
                    ++ att_idx;

                #include "smart_attr.h"
                #undef MAP_ITEM

                dataptr->fulllog.fulllog_time = 0;
                dataptr->fulllog.current_entry = log_index;

                update_device_fulllog(dataptr, false);

                sml_save_fulllog(devptr);
            }
        } // end of for

        if (TIMER_BACKUP_SMART <= sml_buffer->currlog_time) {
            sml_buffer->currlog_time = 0;
        }
    } // end of while
}

void update_device_fulllog(cmn_smart_data* dataptr, bool start_up)
{
    uint32_t next_idx;
    cmn_smart_fulllog* flogptr;

    if(true == start_up) {
        dataptr->fulllog_counter = 0;
    }

    next_idx = ((dataptr->fulllog_counter + 1) % MAX_BUFFER_COUNT);
    flogptr = &dataptr->fulllog_buffer[next_idx];
    memcpy(flogptr, &dataptr->fulllog, sizeof(cmn_smart_fulllog));

    // update next buffer
    ++ dataptr->fulllog_counter;
}

void sml_sample_temperature(cmn_smart_data* dataptr)
{
    dataptr->currlog.raw_attr.temperature.raw_low = 25;// Read temperature sensor.
}

void init_attribute_id(cmn_raw_smart* rawptr)
{
    #define MAP_ITEM(name,index,code) rawptr->name.attr_id = code;
    #include "smart_attr.h"
    #undef MAP_ITEM
}

bool load_file(char* buffer, uint32_t size, const char* load, const char* backup)
{
    FILE *fptr;

    // load file
    fptr = fopen(load, "rb+");

    // if file load from backup
    if (NULL == fptr) fptr = fopen(backup, "rb+");

    // return fail if still fail on opening backup file
    if (NULL == fptr) return false;

    bool rc;

    rc = (1 == fread(buffer, size, 1, fptr));
    fclose(fptr);

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

int read_erasecount(int fd, unsigned char* buf)
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

void sml_dump_buffer(void* buffer, uint32_t size)
{
    FILE *dump_file;
    dump_file = fopen("/home/root/dump.txt", "ab");
    if (NULL != dump_file) {
        fwrite(buffer, size, 1, dump_file);
        fclose(dump_file);
   }
}

void sml_dump_device(const cmn_smart_device* devptr, char* header)
{
    FILE *fptr;
    char str_buf[128];
    cmn_raw_attr* rawptr;
    cmn_smart_data* dataptr;

    sprintf(str_buf, "%s.txt", devptr->currlog_file);
    fptr = fopen(str_buf, "ab");

    if (NULL != fptr) {
        fprintf(fptr, "\n\n-----");
        if(NULL != header) fprintf(fptr, " %s ", header);
        fprintf(fptr, "-----");

        fprintf(fptr, "\n  + Path: %s", devptr->device_path);
        fprintf(fptr, "\n  + File: %s", devptr->currlog_file);
        fprintf(fptr, "\n  + Physical: %s", devptr->physical_path);

        // Print current SMART
        uint16_t attr_count = (sizeof(cmn_raw_smart)  - sizeof(cmn_raw_acc))/ sizeof(cmn_raw_attr);

        fprintf(fptr, "\n***** Current SMART total : %u", attr_count);

        dataptr = get_smart_data(devptr->smart_pool_idx);
        rawptr = (cmn_raw_attr*)&dataptr->currlog.raw_attr;

        uint16_t  i;
        for (i = 0; i < attr_count; i ++)
        {
            fprintf(fptr, "\n[%u]: id[%u] = %u", i, rawptr->attr_id, rawptr->raw_low);
            ++ rawptr;
        }

        fprintf(fptr, "\naccum: power = %u -read = %u -written = %u",
                dataptr->currlog.raw_attr.acc.curr_power,
                dataptr->currlog.raw_attr.acc.curr_read,
                dataptr->currlog.raw_attr.acc.curr_written);

        // Smart log
        fprintf(fptr,
                "\n***** SMART Log - Index %u - log size %u bytes",
                dataptr->fulllog.current_entry,
                sizeof(dataptr->fulllog));

        fclose(fptr);
    }
}
