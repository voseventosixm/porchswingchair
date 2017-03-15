#ifndef __NVME_STRUCT_H__
#define __NVME_STRUCT_H__

#include "CoreMacro.h"

// Data Structure for NVME Specification

struct nvme_error_log_page {
    U64        error_count;
    U16  sqid;
    U16  cmdid;
    U16  status_field;
    U16  parm_error_location;
    U64  lba;
    U32  nsid;
    U8   vs;
    U8   resv[35];
};

struct nvme_id_power_state {
    U16  max_power; // centiwatts
    U8   rsvd2;
    U8   flags;
    U32  entry_lat; // microseconds
    U32  exit_lat;  // microseconds
    U8   read_tput;
    U8   read_lat;
    U8   write_tput;
    U8   write_lat;
    U16  idle_power;
    U8   idle_scale;
    U8   rsvd19;
    U16  active_power;
    U8   active_work_scale;
    U8   rsvd23[9];
};

struct nvme_id_ctrl {
    U16  vid;
    U16  ssvid;
    char sn[20];
    char mn[40];
    char fr[8];
    U8   rab;
    U8   ieee[3];
    U8   cmic;
    U8   mdts;
    U16  cntlid;
    U32  ver;
    U32  rtd3r;
    U32  rtd3e;
    U32  oaes;
    U8   rsvd96[160];
    U16  oacs;
    U8   acl;
    U8   aerl;
    U8   frmw;
    U8   lpa;
    U8   elpe;
    U8   npss;
    U8   avscc;
    U8   apsta;
    U16  wctemp;
    U16  cctemp;
    U16  mtfa;
    U32  hmpre;
    U32  hmmin;
    U8   tnvmcap[16];
    U8   unvmcap[16];
    U32  rpmbs;
    U8   rsvd316[196];
    U8   sqes;
    U8   cqes;
    U8   rsvd514[2];
    U32  nn;
    U16  oncs;
    U16  fuses;
    U8   fna;
    U8   vwc;
    U16  awun;
    U16  awupf;
    U8   nvscc;
    U8   rsvd531;
    U16  acwu;
    U8   rsvd534[2];
    U32  sgls;
    U8   rsvd540[1508];
    struct nvme_id_power_state  psd[32];
    U8   vs[1024];
};

struct nvme_lbaf {
    U16  ms;
    U8   ds;
    U8   rp;
};

struct nvme_id_ns {
    U64  nsze;
    U64  ncap;
    U64  nuse;
    U8   nsfeat;
    U8   nlbaf;
    U8   flbas;
    U8   mc;
    U8   dpc;
    U8   dps;
    U8   nmic;
    U8   rescap;
    U8   fpi;
    U8   rsvd33;
    U16  nawun;
    U16  nawupf;
    U16  nacwu;
    U16  nabsn;
    U16  nabo;
    U16  nabspf;
    U8   rsvd46[2];
    U8   nvmcap[16];
    U8   rsvd64[40];
    U8   nguid[16];
    U8   eui64[8];
    struct nvme_lbaf  lbaf[16];
    U8   rsvd192[192];
    U8   vs[3712];
};

struct nvme_smart_log {
    U8   critical_warning;
    U8   temperature[2];
    U8   avail_spare;
    U8   spare_thresh;
    U8   percent_used;
    U8   rsvd6[26];
    U8   data_units_read[16];
    U8   data_units_written[16];
    U8   host_reads[16];
    U8   host_writes[16];
    U8   ctrl_busy_time[16];
    U8   power_cycles[16];
    U8   power_on_hours[16];
    U8   unsafe_shutdowns[16];
    U8   media_errors[16];
    U8   num_err_log_entries[16];
    U32  warning_temp_time;
    U32  critical_comp_time;
    U16  temp_sensor[8];
    U8   rsvd216[296];
};

#endif
