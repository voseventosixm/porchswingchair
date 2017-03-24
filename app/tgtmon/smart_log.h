#ifndef __SMART_LOG_H__
#define __SMART_LOG_H__

#include "smart_type.h"

// interface to the outside world
void smart_intialize(void);
void smart_load_data(char* dev_path);

void sml_reset_data(cmn_smart_data* dataptr);
void sml_reset_currlog(cmn_smart_data* dataptr);
void sml_reset_fulllog(cmn_smart_data* dataptr);
void sml_reset_device(cmn_smart_device* devptr);
void sml_reset_config(cmn_smart_config* confptr);
void sml_sample_temperature(cmn_smart_data* dataptr);
void smml_sample_smart(cmn_smart_device* devptr, uint16_t samrate, bool start_up);

void sml_initialize(void);
void sml_add_device(char* dev_path);
void sml_save_device(const cmn_smart_device* devptr);
void sml_save_all(void);
void sml_load_config(cmn_smart_device* devptr);
void sml_load_currlog(cmn_smart_device* devptr);
void sml_load_fulllog(cmn_smart_device* devptr);
void sml_save_currlog(const cmn_smart_device* devptr);
void sml_save_fulllog(const cmn_smart_device* devptr);

bool load_file(char* buffer, uint32_t size, const char* load, const char* backup);

void update_attr_id(cmn_raw_smart* rawptr);
bool build_device_path(const char* devpath, char* devname);
void update_raw_smart_data(void);
void update_device_fulllog(cmn_smart_data* dataptr, bool start_up);

int read_extcsd(int fd, unsigned char* buf);
int read_erasecount(int fd, unsigned char* buf);
int send_mmc_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buff);

void sml_dump_buffer(void* buffer, uint32_t size);
void sml_dump_device(const cmn_smart_device* devptr, char* header);


#endif
