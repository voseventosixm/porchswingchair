TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += DEVLOG UTILITY
# CONFIG += DEVMON AWSPORT UTILITY

LIBS += -pthread
QMAKE_CXXFLAGS += -std=c++0x -pthread

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
    app/tgtmon/smart_usb.h \
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
    app/tgtmon/smart_usb.c \
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

CONFIG(USBLOG)
{
HEADERS += \
    app/usblog/smart_usb.h

SOURCES += \
    app/usblog/smart_usb.c
}

CONFIG(DEVLOG)
{

INCLUDEPATH += app/devlog/main
INCLUDEPATH += app/devlog/logger
INCLUDEPATH += app/devlog/vtview

HEADERS += \
    app/devlog/main/appconfig.h \
    app/devlog/main/appdata.h \
    app/devlog/main/appmain.h \
    app/devlog/main/apptest.h \
    app/devlog/main/apputil.h \
    app/devlog/main/appresource.h \
    \
    app/devlog/logger/logutil.h \
    app/devlog/logger/loghandler.h \
    \
    app/devlog/vtview/vtview_interface.h \
    app/devlog/vtview/smart_converter.h \
    app/devlog/vtview/smart_interface.h

SOURCES += \
    app/devlog/main/appconfig.cpp \
    app/devlog/main/appdata.cpp \
    app/devlog/main/appmain.cpp \
    app/devlog/main/apptest.cpp \
    app/devlog/main/apputil.cpp \
    app/devlog/main/appresource.cpp \
    \
    app/devlog/logger/logutil.cpp \
    app/devlog/logger/loghandler.cpp \
    \
    app/devlog/vtview/vtview_interface.cpp \
    app/devlog/vtview/smart_converter.cpp \
    app/devlog/vtview/smart_interface.cpp

OTHER_FILES += \
    app/devlog/makefile \
    app/devlog/makefile_arm \
    app/devlog/main/vt_usage.txt \
    app/devlog/main/configkey.def

OTHER_FILES += \
    config/devlog_config.json
}

CONFIG(DEVMON)
{

INCLUDEPATH += app/devmon/main
INCLUDEPATH += app/devmon/mqtt
INCLUDEPATH += app/devmon/mqtt/aws
INCLUDEPATH += app/devmon/vtview
INCLUDEPATH += app/devmon/vtview/smart
INCLUDEPATH += app/devmon/vtview/temp

HEADERS += \
    app/devmon/main/appconfig.h \
    app/devmon/main/appdata.h \
    app/devmon/main/appmain.h \
    app/devmon/main/apptest.h \
    app/devmon/main/apputil.h \
    app/devmon/main/appresource.h \
    \
    app/devmon/mqtt/jsonkey.h \
    app/devmon/mqtt/mqttmsg.h \
    app/devmon/mqtt/mqttutil.h \
    app/devmon/mqtt/mqtthandler.h \
    app/devmon/mqtt/mqttcallback.h \
    app/devmon/mqtt/aws/mqttjson_aws.h \
    app/devmon/mqtt/aws/mqttcallback_aws.h \
    \
    app/devmon/vtview/geoip_interface.h \
    app/devmon/vtview/vtview_interface.h \
    app/devmon/vtview/temp_interface.h \
    app/devmon/vtview/smart_converter.h \
    app/devmon/vtview/smart_interface.h

SOURCES += \
    app/devmon/main/appconfig.cpp \
    app/devmon/main/appdata.cpp \
    app/devmon/main/appmain.cpp \
    app/devmon/main/apptest.cpp \
    app/devmon/main/apputil.cpp \
    app/devmon/main/appresource.cpp \
    \
    app/devmon/mqtt/mqttmsg.cpp \
    app/devmon/mqtt/mqttutil.cpp \
    app/devmon/mqtt/mqtthandler.cpp \
    app/devmon/mqtt/mqttcallback.cpp \
    app/devmon/mqtt/aws/mqttjson_aws.cpp \
    app/devmon/mqtt/aws/mqtthandler_aws.cpp \
    app/devmon/mqtt/aws/mqttcallback_aws.cpp \
    \
    app/devmon/vtview/geoip_interface.cpp \
    app/devmon/vtview/vtview_interface.cpp \
    app/devmon/vtview/temp_interface.cpp \
    app/devmon/vtview/smart_converter.cpp \
    app/devmon/vtview/smart_interface.cpp

OTHER_FILES += \
    app/devmon/makefile \
    app/devmon/makefile_arm \
    app/devmon/main/vt_usage.txt \
    app/devmon/main/configkey.def \
    app/devmon/mqtt/jsonkey.def \
    app/devmon/mqtt/mqttmap.def

OTHER_FILES += \
    config/devmon_config.json \
    config/cloud_config.json \
    config/device_config.json \
    config/device_identify.json
}

CONFIG(UTILITY)
{

INCLUDEPATH += utility/core
INCLUDEPATH += utility/geoip
INCLUDEPATH += utility/misc
INCLUDEPATH += utility/rapidjson
INCLUDEPATH += utility/smart

HEADERS += \
    utility/misc/fileutil.h \
    utility/misc/rjaccess.h \
    utility/misc/debuglog.h \
    utility/misc/errorstack.h \
    utility/misc/stdmacro.h \
    utility/misc/stdheader.h \
    utility/misc/sysheader.h \
    \
    utility/core/CommonStruct.h \
    utility/core/CoreData.h \
    utility/core/CoreMacro.h \
    utility/core/CoreHeader.h \
    utility/core/CoreUtil.h \
    utility/core/HexFormatter.h \
    utility/core/MiscUtil.h \
    utility/core/NVMEStruct.h \
    utility/core/SmartUtil.h \
    utility/core/StringUtil.h \
    \
    utility/smart/smart.h \
    utility/smart/smart_type.h \
    utility/smart/smart_attr.h \
    utility/smart/smart_common.h \
    \
    utility/geoip/geoip.h \
    utility/geoip/geoip_city.h \
    utility/geoip/geoip_config.h \
    utility/geoip/geoip_internal.h

SOURCES += \
    utility/misc/fileutil.cpp \
    utility/misc/rjaccess.cpp \
    utility/misc/debuglog.cpp \
    utility/misc/errorstack.cpp \
    \
    utility/core/CommonStruct.cpp \
    utility/core/CoreData.cpp \
    utility/core/CoreUtil.cpp \
    utility/core/HexFormatter.cpp \
    utility/core/MiscUtil.cpp \
    utility/core/SmartUtil.cpp \
    utility/core/StringUtil.cpp \
    \
    utility/smart/smart.c \
    utility/smart/smart_common.c \
    \
    utility/geoip/geoip.cpp \
    utility/geoip/geoip_city.cpp \
    utility/geoip/geoip_deprecated.cpp \
    utility/geoip/geoip_interface.cpp \
    utility/geoip/time_zone.cpp \
    utility/geoip/region_name.cpp

OTHER_FILES += \
    utility/misc/errorcode.def \
    utility/misc/vt_version.txt \
    utility/misc/vt_license.txt
}

CONFIG(AWSPORT)
{
INCLUDEPATH += utility/aws/include
INCLUDEPATH += utility/aws/platform/linux/common
INCLUDEPATH += utility/aws/platform/linux/mbedtls
INCLUDEPATH += utility/aws/external_libs/jsmn
INCLUDEPATH += utility/aws/external_libs/mbedTLS/include

HEADERS += \
    utility/aws/include/aws_iot_error.h \
    utility/aws/include/aws_iot_json_utils.h \
    utility/aws/include/aws_iot_log.h \
    utility/aws/include/aws_iot_mqtt_client.h \
    utility/aws/include/aws_iot_mqtt_client_common_internal.h \
    utility/aws/include/aws_iot_mqtt_client_interface.h \
    utility/aws/include/aws_iot_shadow_actions.h \
    utility/aws/include/aws_iot_shadow_interface.h \
    utility/aws/include/aws_iot_shadow_json.h \
    utility/aws/include/aws_iot_shadow_json_data.h \
    utility/aws/include/aws_iot_shadow_key.h \
    utility/aws/include/aws_iot_shadow_records.h \
    utility/aws/include/aws_iot_version.h \
    utility/aws/include/network_interface.h \
    utility/aws/include/threads_interface.h \
    utility/aws/include/timer_interface.h

SOURCES += \
    utility/aws/src/aws_iot_json_utils.c \
    utility/aws/src/aws_iot_mqtt_client.c \
    utility/aws/src/aws_iot_mqtt_client_common_internal.c \
    utility/aws/src/aws_iot_mqtt_client_connect.c \
    utility/aws/src/aws_iot_mqtt_client_publish.c \
    utility/aws/src/aws_iot_mqtt_client_subscribe.c \
    utility/aws/src/aws_iot_mqtt_client_unsubscribe.c \
    utility/aws/src/aws_iot_mqtt_client_yield.c \
    utility/aws/src/aws_iot_shadow.c \
    utility/aws/src/aws_iot_shadow_actions.c \
    utility/aws/src/aws_iot_shadow_json.c \
    utility/aws/src/aws_iot_shadow_records.c
}
