#ifdef __KERNEL__
    #include <linux/fs.h>
    #include <asm/segment.h>
    #include <asm/uaccess.h>
    #include <linux/buffer_head.h>
#else
    #include <stdio.h>
#endif

#include <linux/string.h>

#include "smart_common.h"

static cmn_smart_buffer* cmn_buffer = NULL;

void init_smart_buffer(void* ptr_dev_mngr)
{
    cmn_buffer = (cmn_smart_buffer*)ptr_dev_mngr;
}

cmn_smart_buffer* get_smart_buffer(void)
{
    return cmn_buffer;
}

cmn_smart_data* get_smart_data(uint8_t pool_index)
{
    cmn_smart_data* p_data = NULL;

    if (pool_index < MAX_DEVICE_COUNT ) {
        p_data = &cmn_buffer->device_pool[pool_index];
    }
    return p_data;
}

cmn_raw_smart* get_currlog(char* dev_path, cmn_raw_smart* buffer)
{
    cmn_smart_device* ptr_smart_dev = get_smart_device(dev_path);
    cmn_smart_data*   ptr_smart_data;

    if(NULL == ptr_smart_dev) return NULL;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if(NULL == ptr_smart_data) return NULL;

    return get_device_currlog(ptr_smart_data, buffer);
}

cmn_smart_fulllog* get_fulllog(char* dev_path, cmn_smart_fulllog* buffer)
{
    cmn_smart_fulllog* ptr_smart_log;
    cmn_smart_device* ptr_smart_dev = get_smart_device(dev_path);
    cmn_smart_data*   ptr_smart_data;

    if(NULL == ptr_smart_dev) return NULL;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if(NULL == ptr_smart_data) return NULL;

    // get smart data from active buffer
    ptr_smart_log = get_device_fulllog(ptr_smart_data, buffer);

    return ptr_smart_log;
}

uint32_t get_fulllog_id(char* dev_path)
{
    cmn_smart_device* ptr_smart_dev = get_smart_device(dev_path);
    cmn_smart_data*   ptr_smart_data;

    if(NULL == ptr_smart_dev) return 0;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if(NULL == ptr_smart_data) return 0;

    return ptr_smart_data->fulllog_counter;
}

uint32_t get_currlog_id(char* dev_path)
{
    cmn_smart_device* ptr_smart_dev = get_smart_device(dev_path);
    cmn_smart_data*   ptr_smart_data;

    if(NULL == ptr_smart_dev) return 0;

    ptr_smart_data = get_smart_data(ptr_smart_dev->smart_pool_idx);
    if(NULL == ptr_smart_data) return 0;

    return ptr_smart_data->currlog.raw_counter;
}

cmn_raw_smart* get_device_currlog(cmn_smart_data* smart_ptr, cmn_raw_smart* buffer)
{
    uint32_t active_buf_idx = (smart_ptr->currlog.raw_counter % MAX_BUFFER_COUNT);
    cmn_raw_smart* ptr_current_smart = &smart_ptr->currlog.raw_buffer[active_buf_idx];

    if(NULL != buffer) memcpy(buffer, ptr_current_smart, sizeof(cmn_raw_smart));

    return ptr_current_smart;
}

cmn_smart_fulllog* get_device_fulllog(cmn_smart_data* smart_ptr, cmn_smart_fulllog* buffer)
{
    uint32_t active_idx = (smart_ptr->fulllog_counter % MAX_BUFFER_COUNT);

    if(NULL != buffer) memcpy(buffer, &smart_ptr->fulllog_buffer[active_idx], sizeof(cmn_smart_fulllog));

    return &smart_ptr->fulllog_buffer[active_idx];
}

cmn_smart_device* get_smart_device(char* dev_path)
{
    cmn_smart_device* ptr_smart_device = NULL;
    uint8_t nDev;

    if(NULL == cmn_buffer) return NULL;

    for (nDev = 0; nDev < cmn_buffer->device_count; ++ nDev) {
        if (0 == strcmp(dev_path, cmn_buffer->device_list[nDev].device_path)) {
            ptr_smart_device = &cmn_buffer->device_list[nDev];
            break;
        }
    }

    return ptr_smart_device;
}

char* get_smartlog_buffer(uint16_t section_idx, uint16_t section_size_in_byte, cmn_smart_data* smart_data)
{
    cmn_smart_fulllog *p_full_log;
    uint32_t buffer_offset;
    char* ptr_buffer;

    p_full_log    = get_device_fulllog(smart_data, NULL);
    ptr_buffer    = (char*)p_full_log->entry_list;
    buffer_offset = section_idx * section_size_in_byte;

    return &ptr_buffer[buffer_offset];
}

void smart_dev_mngr_save_config(const cmn_smart_device* ptr_smart_dev)
{
    save_file((char*)&ptr_smart_dev->smart_config,
              sizeof(ptr_smart_dev->smart_config),
              ptr_smart_dev->config_file,
              ptr_smart_dev->config_backup);
}

#ifdef __KERNEL__
void save_file(char* buffer, uint32_t size, const char* save, const char* backup)
{
    struct file *file;
    loff_t pos = 0;
    int save_count;
    char* file_name[2];
    mm_segment_t old_fs;

    file_name[0] = (char*)save;
    file_name[1] = (char*)backup;

    if(NULL == buffer) return;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    for (save_count = 0; save_count < 2; ++ save_count) {
        file = filp_open(file_name[save_count], O_WRONLY|O_CREAT, 0644);
        if (NULL == file) break;

        pos = 0;
        vfs_write(file, buffer, size, &pos);
        vfs_fsync(file, 0);

        filp_close(file, NULL);
    }

    set_fs(old_fs);
}
#else

void save_file(char* buffer, uint32_t size, const char* save, const char* backup)
{
    FILE *ptr_file;

    // save buffer to file
    ptr_file = fopen(save, "wb");
    if (NULL == ptr_file) return;

    fwrite(buffer, size, 1, ptr_file);
    fclose(ptr_file);

    // make a backup
    ptr_file = fopen(backup, "wb");
    if (NULL == ptr_file) return;

    fwrite(buffer, size, 1, ptr_file);
    fclose(ptr_file);
}
#endif

uint16_t build_smart_log_sector(char* dev_path,
                                uint16_t section_idx,
                                uint16_t section_in_byte,
                                uint16_t sector_count,
                                char* sector_buffer)
{
    char* p_smart_log;
    cmn_smart_device*  p_smart_device;
    cmn_smart_data*    p_smart_data;
    uint16_t total_sector = 0;

    p_smart_device = get_smart_device(dev_path);

    do {
        if(NULL == p_smart_device) break;

        p_smart_data = get_smart_data(p_smart_device->smart_pool_idx);
        if(NULL == p_smart_data) break;

        p_smart_log = get_smartlog_buffer(section_idx, section_in_byte, p_smart_data);
        total_sector = sector_count;
        memcpy(sector_buffer, p_smart_log, total_sector * 512);
    } while(0);

    return total_sector;
}
