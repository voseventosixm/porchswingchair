#ifndef __SMART_COMMON_H__
#define __SMART_COMMON_H__

#include "smart_type.h"

void init_smart_buffer(void* ptr_dev_mngr);

uint32_t           get_currlog_id(char* dev_path);
cmn_raw_smart*     get_currlog(char* dev_path, cmn_raw_smart* buffer);

uint32_t           get_fulllog_id(char* dev_path);
cmn_smart_fulllog* get_fulllog(char* dev_path, cmn_smart_fulllog* buffer);

// Access internal smart data structures
cmn_smart_buffer*  get_smart_buffer(void);
cmn_smart_data*    get_smart_data(uint8_t pool_index);
cmn_raw_smart*     get_device_currlog(cmn_smart_data* smart_ptr, cmn_raw_smart* buffer);
cmn_smart_fulllog* get_device_fulllog(cmn_smart_data* smart_ptr, cmn_smart_fulllog* buffer);

char*              get_smartlog_buffer(uint16_t section_idx, uint16_t section_size_in_byte, cmn_smart_data* smart_data);

cmn_smart_device*  get_smart_device(char* dev_path);

void smart_dev_mngr_save_config(const cmn_smart_device* ptr_smart_dev);
void save_file(char* buffer, uint32_t size, const char* save, const char* backup);

uint16_t build_smart_log_sector(char* dev_path,
                                uint16_t section_idx,
                                uint16_t section_in_byte,
                                uint16_t sector_count,
                                char* sector_buffer); // return total built sector

#endif
