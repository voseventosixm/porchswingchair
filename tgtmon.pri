
CONFIG(TGTMON)
{
HEADERS += \
    app/tgtmon/be_byteshift.h \
    app/tgtmon/bs_aio.h \
    app/tgtmon/bs_ssc.h \
    app/tgtmon/bs_thread.h \
    app/tgtmon/bsg.h \
    app/tgtmon/crc32c.h \
    app/tgtmon/driver.h \
    app/tgtmon/libssc.h \
    app/tgtmon/list.h \
    app/tgtmon/log.h \
    app/tgtmon/media.h \
    app/tgtmon/mmc.h \
    app/tgtmon/parser.h \
    app/tgtmon/scsi.h \
    app/tgtmon/scsi_cmnd.h \
    app/tgtmon/shmem_util.h \
    app/tgtmon/smart_log.h \
    app/tgtmon/smart_tgt.h \
    app/tgtmon/smc.h \
    app/tgtmon/spc.h \
    app/tgtmon/ssc.h \
    app/tgtmon/target.h \
    app/tgtmon/tgtadm.h \
    app/tgtmon/tgtadm_error.h \
    app/tgtmon/tgtd.h \
    app/tgtmon/util.h \
    app/tgtmon/work.h \
    app/tgtmon/iscsi/iscsi_if.h \
    app/tgtmon/iscsi/iscsi_proto.h \
    app/tgtmon/iscsi/iscsid.h \
    app/tgtmon/iscsi/iser.h \
    app/tgtmon/iscsi/isns_proto.h \
    app/tgtmon/iscsi/md5.h \
    app/tgtmon/iscsi/param.h \
    app/tgtmon/iscsi/sha1.h \
    app/tgtmon/iscsi/transport.h

SOURCES += \
    app/tgtmon/bs.c \
    app/tgtmon/bs_aio.c \
    app/tgtmon/bs_glfs.c \
    app/tgtmon/bs_null.c \
    app/tgtmon/bs_rbd.c \
    app/tgtmon/bs_rdwr.c \
    app/tgtmon/bs_sg.c \
    app/tgtmon/bs_sheepdog.c \
    app/tgtmon/bs_ssc.c \
    app/tgtmon/concat_buf.c \
    app/tgtmon/driver.c \
    app/tgtmon/libcrc32c.c \
    app/tgtmon/libssc.c \
    app/tgtmon/log.c \
    app/tgtmon/mgmt.c \
    app/tgtmon/mmc.c \
    app/tgtmon/osd.c \
    app/tgtmon/parser.c \
    app/tgtmon/sbc.c \
    app/tgtmon/scc.c \
    app/tgtmon/scsi.c \
    app/tgtmon/shmem_util.c \
    app/tgtmon/smart_log.c \
    app/tgtmon/smart_tgt.c \
    app/tgtmon/smc.c \
    app/tgtmon/spc.c \
    app/tgtmon/ssc.c \
    app/tgtmon/target.c \
    app/tgtmon/tgtadm.c \
    app/tgtmon/tgtd.c \
    app/tgtmon/tgtimg.c \
    app/tgtmon/util.c \
    app/tgtmon/work.c \
    app/tgtmon/iscsi/chap.c \
    app/tgtmon/iscsi/conn.c \
    app/tgtmon/iscsi/iscsi_tcp.c \
    app/tgtmon/iscsi/iscsid.c \
    app/tgtmon/iscsi/iser.c \
    app/tgtmon/iscsi/iser_text.c \
    app/tgtmon/iscsi/isns.c \
    app/tgtmon/iscsi/md5.c \
    app/tgtmon/iscsi/param.c \
    app/tgtmon/iscsi/session.c \
    app/tgtmon/iscsi/sha1.c \
    app/tgtmon/iscsi/target.c \
    app/tgtmon/iscsi/transport.c

OTHER_FILES += \
    app/tgtmon/Makefile
}
