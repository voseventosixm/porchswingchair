#include "smart_common.h"
#include "smart_tgt.h"
#include "tgtd.h"
#include "scsi.h"

void smart_tgt_create(cmn_smart_buffer* buffptr)
{
    DO_SKIP();
    init_smart_buffer(buffptr);
}

void smart_tgt_count_lba(struct scsi_cmd *cmd)
{
    uint32_t length = 0;

    DO_SKIP();
    cmn_smart_device *devptr;
    cmn_smart_data* dataptr;

    devptr = get_smart_device(cmd->dev->path);
    if (NULL == devptr) return;

    dataptr = get_smart_data(devptr->smart_pool_idx);
    if (NULL == dataptr) return;

    cmn_raw_smart *rawptr = &dataptr->currlog.raw_attr;

    switch (cmd->scb[0])
    {
    case ORWRITE_16:
        /* TODO */
        break;

    case COMPARE_AND_WRITE:
        /* TODO */
        break;

    case SYNCHRONIZE_CACHE:
    case SYNCHRONIZE_CACHE_16:
        break;

    case WRITE_VERIFY:
    case WRITE_VERIFY_12:
    case WRITE_VERIFY_16:
        length = scsi_get_out_length(cmd);
        rawptr->acc.curr_read    += length;
        rawptr->acc.curr_written += length;
        break;

    case WRITE_6:
    case WRITE_10:
    case WRITE_12:
    case WRITE_16:
        length = scsi_get_out_length(cmd);
        rawptr->acc.curr_written += length;
        break;

    case WRITE_SAME:
    case WRITE_SAME_16:
        /* TODO */
        break;

    case READ_6:
    case READ_10:
    case READ_12:
    case READ_16:
        length = scsi_get_in_length(cmd);
        rawptr->acc.curr_read += length;
        break;

    case PRE_FETCH_10:
    case PRE_FETCH_16:
        /* TODO */
        break;

    case VERIFY_10:
    case VERIFY_12:
    case VERIFY_16:
        length = scsi_get_out_length(cmd);
        rawptr->acc.curr_read += length;
        break;

    case UNMAP:
        /* TODO */
        break;

    case ATA_PASS_THROUGH_16:
        switch (cmd->scb[14]) {
            case ATA_IDENTIFY_DEVICE:
            case ATA_SMART:
                break;

            case ATA_READ_DMA:
                length = scsi_get_in_length(cmd);
                rawptr->acc.curr_read += length;
                break;

            case ATA_WRITE_DMA:
                length = scsi_get_out_length(cmd);
                rawptr->acc.curr_written += length;
                break;

            default:
                break;
        }
        break;

    default:
        break;
    }

    if (rawptr->acc.curr_read >= READ_WRITE_SCALE_IN_BYTES) {
        rawptr->lba_read.raw_low += rawptr->acc.curr_read / READ_WRITE_SCALE_IN_BYTES;
        rawptr->acc.curr_read %= READ_WRITE_SCALE_IN_BYTES;
    }

    if (rawptr->acc.curr_written >= READ_WRITE_SCALE_IN_BYTES) {
        rawptr->lba_written.raw_low += rawptr->acc.curr_written / READ_WRITE_SCALE_IN_BYTES;
        rawptr->acc.curr_written %= READ_WRITE_SCALE_IN_BYTES;
    }
}

bool smart_tgt_handle_smart_log_cmd(struct scsi_cmd *cmd)
{
    char* buffer;
    uint16_t sector_count;
    uint16_t build_count;
    uint16_t section_idx;

    DO_RETURN(false);
    if(0xA0 != cmd->scb[8]) return false;

    buffer       = scsi_get_in_buffer(cmd);
    sector_count = cmd->scb[6];
    section_idx  = cmd->scb[13] & 0x0F;

    build_count = build_smart_log_sector(cmd->dev->path,
                                                   section_idx,
                                                   SMART_LOG_SECTION_SIZE_IN_BYTE,
                                                   sector_count,
                                                   buffer);

    return (build_count == sector_count);
}
