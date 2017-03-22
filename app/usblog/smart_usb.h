#ifndef __SMART_USB_H__
#define __SMART_USB_H__

#include "smart_type.h"

#define ATA_PASS_THROUGH_16   0x85

#define ATA_READ_DMA        0xc8
#define ATA_WRITE_DMA       0xca
#define ATA_IDENTIFY_DEVICE 0xec
#define ATA_SMART           0xb0

void smart_usb_create(void);
void dbg_update_currlog(char* dev_path);

#endif
