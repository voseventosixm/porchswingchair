#ifndef __SMART_H__
#define __SMART_H__

#ifdef __KERNEL__

#else
    #include <byteswap.h>
    #include <endian.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <signal.h>
    #include <syscall.h>
    #include <unistd.h>
    #include <stdarg.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <limits.h>
    #include <ctype.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <linux/falloc.h>
    #include <stdint.h>
#endif

#include <linux/string.h>
#include <linux/types.h>
#include <linux/stddef.h>


/* SMART utilities for synthesizing SMART / Identify sectors
 */
 
struct dev_info
{
	int desc;
	char *name;
};

struct ata_identify_info
{
    unsigned char model[40];
    unsigned char serial[20];
    unsigned char firmware[8];
    unsigned int low_cap;
    unsigned int high_cap;
};

struct attr_value
{
    unsigned int value;

    unsigned char id;
    unsigned char offset;
};

struct ata_smart_info
{
    #define MAP_ITEM(name,index,code) struct attr_value name;
    #include "smart_attr.h"
    #undef MAP_ITEM
};

extern void copy_word(unsigned char* buff, int offset, unsigned short val);
extern void copy_word_array(unsigned char* buff, int offset, const unsigned char* val, int len);

extern void init_smart_info(struct ata_smart_info* info);

extern void build_smart_sector(unsigned char* buff, const struct ata_smart_info* info);
extern void build_threshold_sector(unsigned char* buff, const struct ata_smart_info* info);
extern void build_identify_sector(unsigned char* buff, const struct ata_identify_info* info);

extern int update_ata_smart(struct dev_info fd, struct ata_smart_info* info);
extern int update_mmc_smart(struct dev_info fd, struct ata_smart_info* info);
extern int update_ata_threshold(struct dev_info fd, struct ata_smart_info* info);
extern int update_mmc_threshold(struct dev_info fd, struct ata_smart_info* info);
extern int update_ata_identify(struct dev_info fd, struct ata_smart_info* info);
extern int update_mmc_identify(struct dev_info fd, struct ata_smart_info* info);

extern int handle_smart_command(struct dev_info fd, unsigned char* buff);
extern int handle_threshold_command(struct dev_info fd, unsigned char* buff);
extern int handle_identify_command(struct dev_info fd, unsigned char* buff);
extern int handle_set_sampling_rate_command(struct dev_info fd, uint16_t rate);

extern void read_smartlog(char* devpath, struct ata_smart_info* info);

#define DUMPINT(msg, val) fprintf(stdout, "[dbg] %s: %d\n", msg, (int) (val))
#define DUMPLOC() fprintf(stdout, "[loc] [%s # %s # %d] ### code location \n", __FILE__, __FUNCTION__, __LINE__)

#define INIT_ATTRIBUTE(attr, index, code) do { attr.offset = index; attr.id = code; attr.value = 0xDD4A3; } while(0)

#define SET_THRESHOLD(buff, attr) do { \
    unsigned char* ptr = (unsigned char*) (buff) + 2 + attr.offset * 12; \
    ptr[0] = attr.id; ptr[1] = attr.value; } while(0)

#define SET_ATTRIBUTE(buff, attr) do { \
    unsigned char* ptr = (unsigned char*) (buff) + 2 + attr.offset * 12; \
    ptr[0] = attr.id; \
    ptr[3] = ptr[4] = 100; \
    ptr[5] = (attr.value      ) & 0xFF; \
    ptr[6] = (attr.value >>  8) & 0xFF; \
    ptr[7] = (attr.value >> 16) & 0xFF; \
    ptr[8] = (attr.value >> 24) & 0xFF; \
    ptr[9] = ptr[10] = 0; } while(0)

#define INIT_MMC_CMD(cmd, code, buf) do { \
    memset((void*)&cmd, 0x00, sizeof(cmd));  \
    cmd.write_flag = 0; \
    cmd.opcode = code; \
    cmd.arg = 0; \
    cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC; \
    cmd.blksz = 512; \
    cmd.blocks = 1; \
    mmc_ioc_cmd_set_data(cmd, buf); } while(0)

#define SET_U08(buf, idx, val) buf[(idx) + 0] = ((val) & 0xFF)

#endif

