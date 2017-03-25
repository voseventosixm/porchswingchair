#ifndef __SMART_LOG_H__
#define __SMART_LOG_H__

#include "smart_type.h"

// interface to the outside world
void smartlog_intialize(void);
void smartlog_load_data(char* dev_path);

// reset
void sml_initialize(void);
void sml_reset_data(cmn_smart_data* dataptr);
void sml_reset_currlog(cmn_smart_data* dataptr);
void sml_reset_fulllog(cmn_smart_data* dataptr);
void sml_reset_device(cmn_smart_device* devptr);
void sml_reset_config(cmn_smart_config* confptr);
void sml_reset_fulllog_object(cmn_smart_fulllog* logptr);

// load/save smartlog
void sml_add_device(char* devpath);
void sml_load_config(cmn_smart_device* devptr);
void sml_load_currlog(cmn_smart_device* devptr);
void sml_load_fulllog(cmn_smart_device* devptr);

void sml_save_all(void);
void sml_save_device(const cmn_smart_device* devptr);
void sml_save_currlog(const cmn_smart_device* devptr);
void sml_save_fulllog(const cmn_smart_device* devptr);

// sampling smart attributes
void sml_update_rawsmart(void);
void sml_sample_temperature(cmn_smart_data* dataptr);
void sml_sample_attribute(cmn_smart_device* devptr, uint16_t samrate, bool startup);

// utilities
bool load_file(char* buffer, uint32_t size, const char* load, const char* backup);
void init_attribute(cmn_raw_smart* rawptr);
bool get_device_name(const char* devpath, char* devname);
void sml_update_fulllog(cmn_smart_data* dataptr, bool startup);

// mmc ioctl
int mmc_read_extcsd(int fd, unsigned char* buf);
int mmc_read_erasecount(int fd, unsigned char* buf);
int mmc_send_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buff);

// debug
void dbg_dump_buffer(void* buffer, uint32_t size);
void dbg_dump_device(const cmn_smart_device* devptr, char* header);


#endif
