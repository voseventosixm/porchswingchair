#This target is to ensure accidental execution of Makefile as a bash script will not execute commands like rm in unexpected directories and exit gracefully.
.prevent_execution:
	exit 0

CC = $(CXX)

#remove @ for no make command prints
DEBUG = @

APP_NAME = devmon

SOLUTION_DIR = ../..
OUTPUT_DIR = $(SOLUTION_DIR)/output

# -------------------------------------------------------
# devmon application
# -------------------------------------------------------
APP_SRC_DIR = $(SOLUTION_DIR)/app/devmon
MAIN = $(APP_SRC_DIR)/main
MQTT = $(APP_SRC_DIR)/mqtt
PORT = $(APP_SRC_DIR)/mqtt/aws
VTVIEW = $(APP_SRC_DIR)/vtview

APP_INCLUDE_DIRS += -I $(MAIN)/
APP_INCLUDE_DIRS += -I $(MQTT)/
APP_INCLUDE_DIRS += -I $(PORT)/
APP_INCLUDE_DIRS += -I $(VTVIEW)/

APP_SRC_FILES += $(MAIN)/appconfig.cpp $(MAIN)/appdata.cpp \
                 $(MAIN)/appmain.cpp   $(MAIN)/applog.cpp \
                 $(MAIN)/apptest.cpp   $(MAIN)/appresource.cpp \
                 $(MAIN)/apputil.cpp

APP_SRC_FILES += $(VTVIEW)/vtview_interface.cpp $(VTVIEW)/temp_interface.cpp \
                 $(VTVIEW)/smart_interface.cpp  $(VTVIEW)/smart_converter.cpp

APP_SRC_FILES += $(MQTT)/mqttutil.cpp         $(MQTT)/mqttmsg.cpp \
                 $(MQTT)/mqttcallback.cpp     $(MQTT)/mqtthandler.cpp \
                 $(PORT)/mqttcallback_aws.cpp $(PORT)/mqtthandler_aws.cpp \
                 $(PORT)/mqttjson_aws.cpp

# -------------------------------------------------------
# Utility modules
# -------------------------------------------------------
UTIL_SRC_DIR = $(SOLUTION_DIR)/utility
CORE = $(UTIL_SRC_DIR)/core
MISC = $(UTIL_SRC_DIR)/misc
GEOIP = $(UTIL_SRC_DIR)/geoip
SMART = $(UTIL_SRC_DIR)/smart
RAPID = $(UTIL_SRC_DIR)/rapidjson

UTIL_INCLUDE_DIRS += -I $(CORE)/
UTIL_INCLUDE_DIRS += -I $(MISC)/
UTIL_INCLUDE_DIRS += -I $(GEOIP)/
UTIL_INCLUDE_DIRS += -I $(SMART)/
UTIL_INCLUDE_DIRS += -I $(RAPID)/

UTIL_SRC_FILES += $(CORE)/StringUtil.cpp \
                  $(CORE)/CommonStruct.cpp $(CORE)/CoreData.cpp \
                  $(CORE)/CoreUtil.cpp     $(CORE)/HexFormatter.cpp \
                  $(CORE)/MiscUtil.cpp     $(CORE)/SmartUtil.cpp

UTIL_SRC_FILES += $(MISC)/rjaccess.cpp     $(MISC)/fileutil.cpp

UTIL_SRC_FILES += $(GEOIP)/geoip.cpp       $(GEOIP)/geoip_city.cpp $(GEOIP)/geoip_deprecated.cpp \
                  $(GEOIP)/region_name.cpp $(GEOIP)/time_zone.cpp  $(GEOIP)/geoip_interface.cpp

UTIL_SRC_FILES += $(SMART)/smart.c         $(SMART)/smart_common.c

# -------------------------------------------------------
# AWS IOT SDK
# -------------------------------------------------------

IOT_CLIENT_DIR = $(UTIL_SRC_DIR)/aws

PLATFORM_DIR = $(IOT_CLIENT_DIR)/platform/linux/mbedtls
PLATFORM_COMMON_DIR = $(IOT_CLIENT_DIR)/platform/linux/common

IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/include
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/external_libs/jsmn
IOT_INCLUDE_DIRS += -I $(PLATFORM_COMMON_DIR)
IOT_INCLUDE_DIRS += -I $(PLATFORM_DIR)

IOT_SRC_FILES += $(shell find $(IOT_CLIENT_DIR)/src/ -name '*.c')
IOT_SRC_FILES += $(shell find $(IOT_CLIENT_DIR)/external_libs/jsmn -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')
IOT_SRC_FILES += $(shell find $(PLATFORM_COMMON_DIR)/ -name '*.c')

#TLS - mbedtls
MBEDTLS_DIR = $(IOT_CLIENT_DIR)/external_libs/mbedTLS
TLS_LIB_DIR = $(MBEDTLS_DIR)/library
TLS_INCLUDE_DIR = -I $(MBEDTLS_DIR)/include
EXTERNAL_LIBS += -L$(TLS_LIB_DIR)
LD_FLAG += -Wl,-rpath,$(TLS_LIB_DIR)
LD_FLAG += -ldl $(TLS_LIB_DIR)/libmbedtls.a $(TLS_LIB_DIR)/libmbedcrypto.a $(TLS_LIB_DIR)/libmbedx509.a -lpthread -lrt

# -------------------------------------------------------
# Build Specs
# -------------------------------------------------------

#Aggregate all include and src directories
INCLUDE_ALL_DIRS += $(IOT_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(TLS_INCLUDE_DIR)
INCLUDE_ALL_DIRS += $(APP_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(UTIL_INCLUDE_DIRS)

SRC_FILES += $(APP_SRC_FILES)
SRC_FILES += $(IOT_SRC_FILES)
SRC_FILES += $(UTIL_SRC_FILES)

# Logging level control
LOG_FLAGS += -DENABLE_IOT_DEBUG
LOG_FLAGS += -DENABLE_IOT_INFO
LOG_FLAGS += -DENABLE_IOT_WARN
LOG_FLAGS += -DENABLE_IOT_ERROR

COMPILER_FLAGS += $(CXXFLAGS)
COMPILER_FLAGS += $(LOG_FLAGS)
#If the processor is big endian uncomment the compiler flag
#COMPILER_FLAGS += -DREVERSED
COMPILER_FLAGS += -std=c++0x

MBED_TLS_MAKE_CMD = $(MAKE) -C $(MBEDTLS_DIR)

PRE_MAKE_CMD = $(MBED_TLS_MAKE_CMD)
MAKE_CMD = $(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(OUTPUT_DIR)/$(APP_NAME) $(LD_FLAG) $(EXTERNAL_LIBS) $(INCLUDE_ALL_DIRS)

all:
	$(PRE_MAKE_CMD)
	$(DEBUG)$(MAKE_CMD)
	$(POST_MAKE_CMD)

clean:
	rm -f $(OUTPUT_DIR)/$(APP_NAME)
	$(MBED_TLS_MAKE_CMD) clean

