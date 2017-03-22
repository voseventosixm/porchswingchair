/*
 * SMART utility functions
 */

#include "smart.h"
#include "smart_common.h"

void copy_word(unsigned char* buff, int offset, unsigned short val)
{
    unsigned char* ptr = buff + offset * 2; // word offset

    ptr[0] = val & 0xFF;
    ptr[1] = (val >> 8) & 0xFF;
}

void copy_word_array(unsigned char* buff, int offset, const unsigned char* val, int len)
{
    int i;
    int blen = len * 2;
    unsigned char* ptr = buff + offset * 2; // word offset

    for (i = 0; i < blen; i+=2)
    {
        ptr[i] = val[i + 1];
        ptr[i + 1] = val[i];
    }
}

void init_smart_info(struct ata_smart_info* info)
{
    memset((void*)info, 0x00, sizeof(struct ata_smart_info)); 

    #define MAP_ITEM(name,index,code) INIT_ATTRIBUTE(info->name, index, code);
    #include "smart_attr.h"
    #undef MAP_ITEM
}

void build_identify_sector(unsigned char* buff, const struct ata_identify_info* id)
{
    memset((void*) buff, 0x00, 512);

    copy_word_array(buff, 27, id->model, 20);  // buffer, word-offset, data, word-count
    copy_word_array(buff, 10, id->serial, 10);
    copy_word_array(buff, 23, id->firmware, 4);

    copy_word(buff, 60, (id->low_cap      ) & 0xFFFF);
    copy_word(buff, 61, (id->low_cap >> 16) & 0xFFFF);

    // For 48b supported
    // copy_word(buff, 100, id->low_cap & 0xFFFF);
    // copy_word(buff, 101, (id->low_cap >> 16) & 0xFFFF);
    // copy_word(buff, 102, id->high_cap & 0xFFFF);
    // copy_word(buff, 103, (id->high_cap >> 16) & 0xFFFF);
}

void build_smart_sector(unsigned char* buff, const struct ata_smart_info* id)
{
    memset((void*) buff, 0x00, 512);

    #define MAP_ITEM(name,index,code) SET_ATTRIBUTE(buff, id->name);
    #include "smart_attr.h"
    #undef MAP_ITEM
}

void build_threshold_sector(unsigned char* buff, const struct ata_smart_info* id)
{
    memset((void*) buff, 0x00, 512);

    #define MAP_ITEM(name,index,code) SET_THRESHOLD(buff, id->name);
    #include "smart_attr.h"
    #undef MAP_ITEM
}

static void set_error(struct ata_smart_info* info, int line, int retcode)
{
    info->last_error.value = (line * 1000) + (500 - retcode);
}

void read_smartlog(char* devpath, struct ata_smart_info* info)
{
    cmn_raw_smart raw;
	
    if (NULL == get_currlog(devpath, &raw)) return;

    #define MAP_ITEM(name,index,code) info->name.value = raw.name.raw_low;
    #include "smart_attr.h"
    #undef MAP_ITEM
}

int update_mmc_smart(struct dev_info fd, struct ata_smart_info* info)
{
	read_smartlog(fd.name, info);

    return 0;
}

int handle_threshold_command(struct dev_info fd, unsigned char* outbuf)
{
    struct ata_smart_info info;

    DO_RETURN(1);

    init_smart_info(&info);

    // For threshold data, use 100 for all attributes
    #define MAP_ITEM(name,index,code) info.name.value = 100;
    #include "smart_attr.h"
    #undef MAP_ITEM

    // build output sector from ata_smart_info
    build_threshold_sector(outbuf, &info);

    return 0;    
}

int handle_smart_command(struct dev_info fd, unsigned char* outbuf)
{
    cmn_smart_device* p_smart_dev;
    struct ata_smart_info info;

    DO_RETURN(1);

    init_smart_info(&info);

    // fill dummy data
    // info.lba_read.value = 2197;
    // info.lba_written.value = 2779;
    // info.ave_erase.value = 11;
    // info.max_erase.value = 22;
    // info.life_left.value = 97;
    // info.spare_block.value = 99;
    info.temperature.value = 48;
    info.ecc_error.value = 0;
    info.prog_fail.value = 0;
    info.erase_fail.value = 0;
    info.endurance.value = 3000;

    update_mmc_smart(fd, &info);

    // build output sector from ata_smart_info
    build_smart_sector(outbuf, &info);

    // fill sampling rate
    p_smart_dev = get_smart_device(fd.name);
    if(NULL != p_smart_dev) {
        outbuf[395] = p_smart_dev->smart_config.sampling_rate & 0xFF;
        outbuf[396] = (p_smart_dev->smart_config.sampling_rate >> 8) & 0xFF;
    }

    // fill manufacture signature
    outbuf[0x182] = 'G';
    outbuf[0x183] = 'r';
    outbuf[0x184] = 'i';
    outbuf[0x185] = 'z';
    outbuf[0x186] = 'z';

    // insert smart log parameter
    outbuf[401] = SMART_LOG_SIZE_IN_SECTOR;
    outbuf[402] = SMART_LOG_SECTION_SIZE_IN_SECTOR;

    return 0;    
}

int handle_identify_command(struct dev_info fd, unsigned char* outbuf)
{
    struct ata_identify_info info;

    const char* dev_model    = "Virtium External Drive";
    const char* dev_serial   = "VTR220F5M830110102";
    const char* dev_firmware = "7744306";

    memset((void*)&info, 0x00, sizeof(info));
    
    // fill dummy data
    info.low_cap = 0x08000000; // 64GB
    memcpy((void*) info.model, (void*) dev_model, strlen(dev_model));
    memcpy((void*) info.serial, (void*) dev_serial, strlen(dev_serial));
    memcpy((void*) info.firmware, (void*) dev_firmware, strlen(dev_firmware));
    
    // build output sector from ata_identify_info
    build_identify_sector(outbuf, &info);

    return 0;
}

int handle_set_sampling_rate_command(struct dev_info fd, uint16_t rate)
{
    cmn_smart_device* p_smart_dev;
    p_smart_dev = get_smart_device(fd.name);
    if(NULL != p_smart_dev) {
        p_smart_dev->smart_config.sampling_rate = rate;
        smart_dev_mngr_save_config(p_smart_dev);
        return 0;
    }

    return -1;
}

