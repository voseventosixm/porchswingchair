#ifndef __SMART_TGT_H__
#define __SMART_TGT_H__

#include "smart_type.h"
#include "scsi_cmnd.h"

void tgt_init_buffer(cmn_smart_buffer* ptr_dev_mngr);
void tgt_update_workload(struct scsi_cmd *cmd);
bool tgt_handle_smartlog(struct scsi_cmd *cmd);

#endif
