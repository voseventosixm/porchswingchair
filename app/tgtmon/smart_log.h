#ifndef __SMART_LOG_H__
#define __SMART_LOG_H__

#include "smart_type.h"

void smart_intialize(void);
void smart_load_data(char* dev_path);

void smart_att_read_temperature(cmn_smart_data* ptr_smart_data);
void smart_data_set_default(cmn_smart_data* ptr_smart_data);
void smart_data_reset_currlog(cmn_smart_data* ptr_smart_data);
void smart_data_reset_fulllog(cmn_smart_data* ptr_smart_data);
void smart_dev_reset(cmn_smart_device* ptr_smart_device);
void smart_dev_reset_config(cmn_smart_config* ptr_config);
void smart_dev_sampling_raw_smart(cmn_smart_device* ptr_smart_device, uint16_t sampling_rate, bool start_up);

void smart_dev_mngr_initialize(void);
void smart_dev_mngr_add(char* dev_path);
void smart_dev_mngr_save(const cmn_smart_device* ptr_smart_dev);
void smart_dev_mngr_save_all(void);
void smart_dev_mngr_save_fulllog(const cmn_smart_device* ptr_smart_dev);
void smart_dev_mngr_load_fulllog(cmn_smart_device* ptr_smart_dev);
void smart_dev_mngr_save_currlog(const cmn_smart_device* ptr_smart_dev);
void smart_dev_mngr_load_currlog(cmn_smart_device* ptr_smart_dev);
void smart_dev_mngr_load_config(cmn_smart_device* ptr_smart_dev);

bool load_file(char* buffer, uint32_t size, const char* load, const char* backup);

void* thread_update_smart_att(void* pParam);
void set_smart_id(cmn_raw_smart* smart_data);
bool search_token_physical_device(const char* dev_path, char* dev_name);
void update_raw_smart_data(void);
void update_device_fulllog(cmn_smart_data* ptr_smart, bool start_up);

int read_extcsd(int fd, unsigned char* buf);
int read_erasecnt(int fd, unsigned char* buf);
int send_mmc_cmd(int fd, int cmd, int arg, int wflag, unsigned char* buff);

//void debug_log_cmd(struct scsi_cmd *cmd);
void debug_dump_log(void* buffer, uint32_t size);
void debug_dump_smart_device(const cmn_smart_device* ptr_smart_dev, char* header);


#endif
