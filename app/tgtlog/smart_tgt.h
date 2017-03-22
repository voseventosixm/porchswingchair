#ifndef __SMART_TGT_H__
#define __SMART_TGT_H__

#include "smart_type.h"
#include "scsi_cmnd.h"

void smart_tgt_create(cmn_smart_buffer* ptr_dev_mngr);
void smart_tgt_count_lba(struct scsi_cmd *cmd);
bool smart_tgt_handle_smart_log_cmd(struct scsi_cmd *cmd);

#endif
