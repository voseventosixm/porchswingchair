#ifndef __SMART_TYPE_H__
#define __SMART_TYPE_H__

#ifdef __KERNEL__
    #include <linux/types.h>
#else
    #include <linux/stddef.h>
    #include <stdint.h>
    #include <stdbool.h>
#endif

#ifndef NULL
    #define NULL ((void *)0)
#endif

// Smart log parameter
#define MAX_ATTR_COUNT            30
#define SMART_LOG_SECTION_SIZE_IN_SECTOR 32
#define SMART_LOG_SECTION_SIZE_IN_BYTE (32*512)

#define MAX_LOG_COUNT             365
#define READ_WRITE_SCALE_IN_BYTES 33554432
#define MAX_DEVICE_PATH           128
#define MAX_DEVICE_COUNT          4

#define MAX_BUFFER_COUNT 2

#define ATTR_NAND_ENDURANCE          3000

#define DEFAULT_SMART_LOG_SAMPLING_RATE  1
#define TIMER_READ_INTERNAL_SMART        1
#define TIMER_BACKUP_SMART               1

enum cmn_enum_type { CHARACTER, NUMBER };

#pragma pack(push)
#pragma pack(1)

typedef struct _cmn_phys_token
{
    char token[64];
    char sequence_start;
    char sequence_end;
    enum cmn_enum_type enum_type;
} cmn_phys_token;

// IMPORTANT: Do not change this data structure
typedef struct _cmn_smart_attr
{
    uint8_t  attr_id;
    uint8_t  value;
    uint32_t raw_value;
} cmn_smart_attr;

typedef struct _cmn_smart_entry
{
    uint32_t index;
    uint16_t time_stamp;
    cmn_smart_attr attr_list[MAX_ATTR_COUNT];
} cmn_smart_entry;

typedef struct _cmn_raw_attr
{
    uint32_t raw_low;
    uint32_t raw_high;
    uint8_t  attr_id;
    uint8_t  threshold;
    uint8_t  worst;
} cmn_raw_attr;

typedef struct _cmn_raw_acc
{
    uint32_t curr_read;
    uint32_t curr_written;
    uint32_t curr_power;
} cmn_raw_acc;

typedef struct _cmn_raw_smart
{
    #define MAP_ITEM(name,index,code) cmn_raw_attr name;
    #include "smart_attr.h"
    #undef MAP_ITEM

    cmn_raw_acc acc;
} cmn_raw_smart;

typedef struct _cmn_smart_currlog
{
    // NOTE: These variables are updated by multi thread. So don't direct use this variable by outside.
    // Updating raw smart value.
    cmn_raw_smart raw_attr;

    uint32_t raw_counter;
    cmn_raw_smart raw_buffer[MAX_BUFFER_COUNT];
} cmn_smart_currlog;

typedef struct _cmn_smart_fulllog
{
    // Smart log
    uint16_t fulllog_time;
    uint16_t current_entry;
    cmn_smart_entry entry_list[MAX_LOG_COUNT];
} cmn_smart_fulllog;

#define SMART_LOG_SIZE_IN_SECTOR (((sizeof(cmn_smart_entry)*MAX_LOG_COUNT) + 511) / 512)

typedef struct _cmn_smart_data
{
    cmn_smart_currlog currlog;
    cmn_smart_fulllog fulllog;

    uint32_t fulllog_counter;
    cmn_smart_fulllog fulllog_buffer[MAX_BUFFER_COUNT];
} cmn_smart_data;

typedef struct _cmn_smart_config
{
    uint16_t sampling_rate;
} cmn_smart_config;

typedef struct _cmn_smart_device
{
    char device_path[MAX_DEVICE_PATH];
    char physical_path[MAX_DEVICE_PATH];

    char currlog_file[MAX_DEVICE_PATH];
    char currlog_backup[MAX_DEVICE_PATH];

    char fulllog_file[MAX_DEVICE_PATH];
    char fulllog_backup[MAX_DEVICE_PATH];

    char config_file[MAX_DEVICE_PATH];
    char config_backup[MAX_DEVICE_PATH];

    cmn_smart_config smart_config;

    uint8_t smart_pool_idx;
} cmn_smart_device;

typedef struct _cmn_smart_buffer
{
    uint8_t currlog_time;

    uint8_t device_count;
    cmn_smart_device device_list[MAX_DEVICE_COUNT];

    uint8_t allocated_pool_count;
    cmn_smart_data device_pool[MAX_DEVICE_COUNT];
} cmn_smart_buffer;

#pragma pack(pop)

#define GET_U32(buf, idx) ((buf[(idx) + 3] << 24) | (buf[(idx) + 2] << 16) | (buf[(idx) + 1] << 8) | (buf[(idx) + 0]))
#define GET_U16(buf, idx) ((buf[(idx) + 1] << 8) | (buf[(idx) + 0]))
#define GET_U08(buf, idx) ((buf[(idx) + 0]))

#define VC_GET_U32(buf, idx) ((buf[(idx) + 0] << 24) | (buf[(idx) + 1] << 16) | (buf[(idx) + 2] << 8) | (buf[(idx) + 3]))
#define VC_GET_U16(buf, idx) ((buf[(idx) + 0] << 8) | (buf[(idx) + 1]))
#define VC_GET_U08(buf, idx) ((buf[(idx) + 0]))

#endif
