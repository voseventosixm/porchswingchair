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

// smartlog data
static pthread_t sml_handler;
static cmn_smart_buffer* sml_buffer;
static const char* sml_location = "/home/root/";

// smartlog thread-func
static void* sml_thread_function(void* param);

// reset
static void sml_reset_buffer(void);
static void sml_reset_data(cmn_smart_data* dataptr);
static void sml_reset_fulllog(cmn_smart_data* dataptr);
static void sml_reset_currlog(cmn_smart_data* dataptr);

static void sml_reset_device(cmn_smart_device* devptr);
static void sml_reset_config(cmn_smart_config* confptr);
static void sml_reset_fulllog_object(cmn_smart_fulllog* logptr);

// load/save smartlog
static void sml_add_device(char* devpath);
static void sml_load_config(cmn_smart_device* devptr);
static void sml_load_currlog(cmn_smart_device* devptr);
static void sml_load_fulllog(cmn_smart_device* devptr);

static void sml_save_all(void);
static void sml_save_device(const cmn_smart_device* devptr);
static void sml_save_currlog(const cmn_smart_device* devptr);
static void sml_save_fulllog(const cmn_smart_device* devptr);

// sampling smart attributes
static void sml_update_rawsmart(void);
static void sml_sample_temperature(cmn_smart_data* dataptr);
static void sml_sample_attribute(cmn_smart_device* devptr, uint16_t samrate, bool startup);

// utilities
static bool sml_load_file(char* buffer, uint32_t size, const char* load, const char* backup);
static void sml_init_attribute(cmn_raw_smart* rawptr);
static bool sml_get_device_name(const char* devpath, char* devname);
static void sml_update_fulllog(cmn_smart_data* dataptr, bool startup);

// mmc ioctl
static int mmc_read_extcsd(int fd, unsigned char* buf);
static int mmc_read_erasecount(int fd, unsigned char* buf);
static int mmc_send_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buff);

// debug
static void dbg_dump_buffer(void* buffer, uint32_t size);
static void dbg_dump_device(const cmn_smart_device* devptr, char* header);

// --------------------------------------------------------
// smatlog interface
// --------------------------------------------------------

void smartlog_intialize(void)
{
    DO_SKIP();
    sml_buffer = (cmn_smart_buffer*)shmem_get(sizeof(cmn_smart_buffer));

    sml_reset_buffer();

    smart_tgt_create(sml_buffer);
}

void smartlog_load_data(char* dev_path)
{
    DO_SKIP();
    sml_add_device(dev_path);
}

// --------------------------------------------------------
// reset functions
// --------------------------------------------------------

void sml_reset_buffer()
{
    uint8_t i;
    for (i = 0; i < MAX_DEVICE_COUNT; ++ i)
    {
        sml_reset_device(&sml_buffer->device_list[i]);
    }

    sml_buffer->currlog_time = 0;
    sml_buffer->device_count = 0;
    sml_buffer->allocated_pool_count   = 0;
}

void sml_reset_data(cmn_smart_data* dataptr)
{
    sml_reset_currlog(dataptr);
    sml_reset_fulllog(dataptr);
}

void sml_reset_currlog(cmn_smart_data* dataptr)
{
    cmn_smart_currlog* logptr = &dataptr->currlog;

    cmn_raw_smart* rawptr = &logptr->raw_attr;
    memset(rawptr, 0, sizeof(*rawptr));

    sml_init_attribute(rawptr);
    rawptr->endurance.raw_low = ATTR_NAND_ENDURANCE;

    logptr->raw_counter = 0;
    cmn_raw_smart* bufptr = &logptr->raw_buffer[0];

    memcpy(bufptr, rawptr, sizeof(*rawptr));
}

void sml_reset_fulllog_object(cmn_smart_fulllog *logptr)
{
    logptr->fulllog_time  = 0;
    logptr->current_entry = MAX_LOG_COUNT - 1;

    uint16_t i;
    for(i = 0; i < MAX_LOG_COUNT; ++ i)
    {
        cmn_smart_entry* logentry = &logptr->entry_list[i];

        logentry->index      = 0;
        logentry->time_stamp = 0;
    }
}

void sml_reset_fulllog(cmn_smart_data* dataptr)
{
    dataptr->fulllog_counter = 0;

    sml_reset_fulllog_object(&dataptr->fulllog);
    sml_reset_fulllog_object(&dataptr->fulllog_buffer[0]);
}

void sml_reset_config(cmn_smart_config* confptr)
{
    confptr->sampling_rate = DEFAULT_SMART_LOG_SAMPLING_RATE;
}

void sml_reset_device(cmn_smart_device* devptr)
{
    devptr->device_path[0]    = '\0';
    devptr->physical_path[0]  = '\0';

    devptr->currlog_file[0]   = '\0';
    devptr->currlog_backup[0] = '\0';

    devptr->fulllog_file[0]   = '\0';
    devptr->fulllog_backup[0] = '\0';

    devptr->smart_pool_idx = MAX_DEVICE_COUNT;

    sml_reset_config(&devptr->smart_config);
}

// --------------------------------------------------------
// load functions
// --------------------------------------------------------

void sml_add_device(char* devpath)
{
    uint8_t i;
    char devname[32];
    cmn_smart_device* devptr;
    uint16_t len = strlen(devpath);

    if((0 == len) || (len >= MAX_DEVICE_PATH)) return;
    if(sml_buffer->device_count >= MAX_DEVICE_COUNT) return;

    // add new device
    devptr = &sml_buffer->device_list[sml_buffer->device_count];

    sml_reset_device(devptr);

    if (false == sml_get_device_name(devpath, devname)) return;

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
    dbg_dump_device(devptr, str_buf);
    // end debug

    ++ sml_buffer->device_count;

    if(1 == sml_buffer->device_count)
    {
        pthread_create(&sml_handler, NULL, sml_thread_function, NULL);
    }
}

void sml_load_config(cmn_smart_device* devptr)
{
    if(false == sml_load_file((char*)&devptr->smart_config,
                          sizeof(devptr->smart_config),
                          devptr->config_file,
                          devptr->config_backup))
    {
        sml_reset_config(&devptr->smart_config);
    }
}

void sml_load_currlog(cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;

    dataptr = get_smart_data(devptr->smart_pool_idx);
    if(false == sml_load_file((char*)&dataptr->currlog.raw_attr,
                          sizeof(cmn_raw_smart),
                          devptr->currlog_file,
                          devptr->currlog_backup))
    {
        sml_reset_currlog(dataptr);
    }

    sml_sample_attribute(devptr, 0, true);
}

void sml_load_fulllog(cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr;
    dataptr = get_smart_data(devptr->smart_pool_idx);

    if(false == sml_load_file((char*)&dataptr->fulllog,
                          sizeof(cmn_smart_fulllog),
                          devptr->fulllog_file,
                          devptr->fulllog_backup))
    {
        sml_reset_fulllog(dataptr);
    } else {
        sml_update_fulllog(dataptr, true);
    }
}

// --------------------------------------------------------
// save functions
// --------------------------------------------------------

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

void sml_save_currlog(const cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
    if(NULL == dataptr) return;

    cmn_raw_smart* clogptr = get_device_currlog(dataptr, NULL);
    save_file((char*)clogptr,
              sizeof(cmn_raw_smart),
              devptr->currlog_file,
              devptr->currlog_backup);
}

void sml_save_fulllog(const cmn_smart_device* devptr)
{
    cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
    if(NULL == dataptr) return;

    cmn_smart_fulllog* flogptr = get_device_fulllog(dataptr, NULL);
    save_file((char*)flogptr,
              sizeof(cmn_smart_fulllog),
              devptr->fulllog_file,
              devptr->fulllog_backup);
}

// --------------------------------------------------------
// sampling functions
// --------------------------------------------------------

void sml_sample_temperature(cmn_smart_data* dataptr)
{
    dataptr->currlog.raw_attr.temperature.raw_low = 25;// Read temperature sensor.
}

void sml_sample_attribute(cmn_smart_device* devptr, uint16_t samrate, bool startup)
{
    unsigned char buf[512];
    int rawfd, tmpval, ret;

    int smtsize = sizeof(cmn_raw_smart);
    int accsize = sizeof(cmn_raw_acc);
    int attsize = sizeof(cmn_raw_attr);
    int attcnt = (smtsize - accsize) / attsize;

    cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
    if(NULL == dataptr) return;

    cmn_smart_currlog* logptr = &dataptr->currlog;

    if(true == startup) {
        samrate = 0;
        logptr->raw_counter = 0;
    }

    // update smart attribute
    cmn_raw_smart* rawptr = &logptr->raw_attr;

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
    if (rawfd >= 0)
    {
        ret = mmc_read_extcsd(rawfd, buf);
        if (0 == ret)
        {
            // GetValue from eMMC Register
            rawptr->prog_sector.raw_low = GET_U32(buf, 302);
            rawptr->life_typea.raw_low  = GET_U08(buf, 268);
            rawptr->life_typeb.raw_low  = GET_U08(buf, 269);
            rawptr->extcsd_ver.raw_low  = GET_U08(buf, 192);

            tmpval = (rawptr->life_typea.raw_low % 11) * 10; rawptr->life_left.raw_low = (tmpval) ? (110 - tmpval) : 0;
            tmpval = (rawptr->life_typeb.raw_low % 11) * 10; rawptr->spare_block.raw_low = (tmpval) ? (110 - tmpval) : 0;

            ret = mmc_read_erasecount(rawfd, buf);
            if (0 == ret)
            {
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
    uint8_t bufidx = (logptr->raw_counter + 1) % MAX_BUFFER_COUNT;
    cmn_raw_attr* src = (cmn_raw_attr*)&logptr->raw_attr;
    cmn_raw_attr* dst = (cmn_raw_attr*)&logptr->raw_buffer[bufidx];

    // copy raw attributes
    uint16_t i;
    for (i = 0; i < attcnt; i ++)
    {
        do { memcpy(dst, src, attsize); }
        while (0 != memcmp(src, dst, attsize));

        ++ src; ++ dst;
    }

    // copy accumulator
    do { memcpy(dst, src, accsize); }
    while (0 != memcmp(src, dst, accsize));

    ++ logptr->raw_counter;
}

void sml_update_rawsmart(void)
{
    uint8_t i;

    for (i = 0; i < sml_buffer->device_count; ++ i)
    {
        cmn_smart_device* devptr = &sml_buffer->device_list[i];
        cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
        if(NULL == dataptr) continue;

        sml_sample_attribute(devptr, TIMER_READ_INTERNAL_SMART, false);
    }
}

// --------------------------------------------------------
// utilities
// --------------------------------------------------------
bool sml_get_device_name(const char* devpath, char* devname)
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

void sml_update_fulllog(cmn_smart_data* dataptr, bool startup)
{
    if(true == startup)
    {
        dataptr->fulllog_counter = 0;
    }

    uint32_t bufidx = ((dataptr->fulllog_counter + 1) % MAX_BUFFER_COUNT);
    cmn_smart_fulllog* logptr = &dataptr->fulllog_buffer[bufidx];
    memcpy(logptr, &dataptr->fulllog, sizeof(cmn_smart_fulllog));

    // update next buffer
    ++ dataptr->fulllog_counter;
}

void sml_init_attribute(cmn_raw_smart* rawptr)
{
    #define MAP_ITEM(name,index,code) rawptr->name.attr_id = code;
    #include "smart_attr.h"
    #undef MAP_ITEM
}

bool sml_load_file(char* buffer, uint32_t size, const char* load, const char* backup)
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

// --------------------------------------------------------
// emmc utilities
// --------------------------------------------------------

int mmc_send_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buffer)
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

int mmc_read_extcsd(int fd, unsigned char* buf)
{
    return mmc_send_cmd(fd, MMC_SEND_EXT_CSD, 0, 0, buf);
}

int mmc_read_erasecount(int fd, unsigned char* buf)
{
    int ret = 0;
    unsigned char cmd[512];
    memset(cmd, 0, sizeof(__u8) * 512);

    cmd[0] = 0x07; // subcode
    cmd[4] = 0x26; // passwd for toshiba emmc
    cmd[5] = 0xE9;
    cmd[6] = 0x01;
    cmd[7] = 0xEB;

    ret = mmc_send_cmd(fd, 56, 0, 1, cmd); // arg = 0. write = true
    if (0 != ret) return ret;

    ret = mmc_send_cmd(fd, 56, 1, 0, buf); // arg = 1. write = false
    if (0 != ret) return ret;

    return 0;
}

// --------------------------------------------------------
// debug utilities
// --------------------------------------------------------

void dbg_dump_buffer(void* buffer, uint32_t size)
{
    FILE *fileptr = fopen("/home/root/dump.txt", "ab");
    if (NULL != fileptr)
    {
        fwrite(buffer, size, 1, fileptr);
        fclose(fileptr);
   }
}

void dbg_dump_device(const cmn_smart_device* devptr, char* header)
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

// --------------------------------------------------------
// static utilities
// --------------------------------------------------------

void* sml_thread_function(void* param)
{
    uint16_t attidx;
    uint8_t loop_count = 0;

    while (1)
    {
        sleep(60); // fix this number to create timer 1 minute
        if(0 == sml_buffer->device_count) continue;

        // Sample more attribute
        ++ loop_count;
        if (TIMER_READ_INTERNAL_SMART <= loop_count)
        {
            loop_count = 0;
            sml_update_rawsmart();
        }

        ++ sml_buffer->currlog_time;

        // Check timer save data to file
        uint8_t i;
        for (i = 0; i < sml_buffer->device_count; ++i)
        {
            cmn_raw_smart *rawptr;
            cmn_smart_attr *attptr;

            cmn_smart_device* devptr  = &sml_buffer->device_list[i];
            cmn_smart_data* dataptr = get_smart_data(devptr->smart_pool_idx);
            cmn_smart_fulllog* logptr = &dataptr->fulllog;

            if(NULL == dataptr) continue;

            // Save current smart
            if (TIMER_BACKUP_SMART <= sml_buffer->currlog_time)
            {
                sml_save_currlog(devptr);
            }

            ++ logptr->fulllog_time;
            if (logptr->fulllog_time >= devptr->smart_config.sampling_rate)
            {
                uint32_t last_index = logptr->entry_list[logptr->current_entry].index;

                uint16_t next_index = (logptr->current_entry + 1) % MAX_LOG_COUNT;

                cmn_smart_entry* entryptr = &logptr->entry_list[next_index];

                // copy current smart to log
                memset(entryptr, 0, sizeof(cmn_smart_entry));

                // header
                entryptr->time_stamp = logptr->fulllog_time;
                entryptr->index      = last_index + 1;

                attidx = 0;
                attptr = &entryptr->attr_list[0];
                rawptr = &dataptr->currlog.raw_attr;

                #define MAP_ITEM(name,index,code) \
                    attptr[attidx].attr_id   = rawptr->name.attr_id; \
                    attptr[attidx].value     = rawptr->name.raw_low; \
                    attptr[attidx].raw_value = rawptr->name.raw_low; \
                    ++ attidx;

                #include "smart_attr.h"
                #undef MAP_ITEM

                logptr->fulllog_time = 0;
                logptr->current_entry = next_index;

                sml_update_fulllog(dataptr, false);

                sml_save_fulllog(devptr);
            }
        } // end of for

        if (TIMER_BACKUP_SMART <= sml_buffer->currlog_time)
        {
            sml_buffer->currlog_time = 0;
        }
    } // end of while
}

// --------------------------------------------------------
// end
// --------------------------------------------------------
