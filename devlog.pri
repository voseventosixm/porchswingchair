
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
    app/devlog/loader/loadutil.h \
    app/devlog/loader/loadhandler.h \
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
    app/devlog/loader/loadutil.cpp \
    app/devlog/loader/loadhandler.cpp \
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
