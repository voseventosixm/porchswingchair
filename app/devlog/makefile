#This target is to ensure accidental execution of Makefile as a bash script will not execute commands like rm in unexpected directories and exit gracefully.
.prevent_execution:
	exit 0

CC = $(CXX)

#remove @ for no make command prints
DEBUG = @

APP_NAME = devlog

SOLUTION_DIR = ../..
OUTPUT_DIR = $(SOLUTION_DIR)/output

# -------------------------------------------------------
# application
# -------------------------------------------------------
APP_SRC_DIR = $(SOLUTION_DIR)/app/devlog
MAIN = $(APP_SRC_DIR)/main
LOGGER = $(APP_SRC_DIR)/logger
LOADER = $(APP_SRC_DIR)/loader
VTVIEW = $(APP_SRC_DIR)/vtview

APP_INCLUDE_DIRS += -I $(MAIN)/
APP_INCLUDE_DIRS += -I $(LOGGER)/
APP_INCLUDE_DIRS += -I $(LOADER)/
APP_INCLUDE_DIRS += -I $(VTVIEW)/

APP_SRC_FILES += $(MAIN)/appconfig.cpp $(MAIN)/appdata.cpp \
                 $(MAIN)/appmain.cpp   $(MAIN)/apputil.cpp \
                 $(MAIN)/apptest.cpp   $(MAIN)/appresource.cpp

APP_SRC_FILES += $(VTVIEW)/smart_interface.cpp  $(VTVIEW)/smart_converter.cpp \
                 $(VTVIEW)/vtview_interface.cpp 

APP_SRC_FILES += $(LOGGER)/logutil.cpp          $(LOGGER)/loghandler.cpp

APP_SRC_FILES += $(LOADER)/loadutil.cpp         $(LOADER)/loadhandler.cpp

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

UTIL_SRC_FILES += $(MISC)/rjaccess.cpp     $(MISC)/fileutil.cpp \
                  $(MISC)/debuglog.cpp     $(MISC)/errorstack.cpp

UTIL_SRC_FILES += $(GEOIP)/geoip.cpp       $(GEOIP)/geoip_city.cpp $(GEOIP)/geoip_deprecated.cpp \
                  $(GEOIP)/region_name.cpp $(GEOIP)/time_zone.cpp

UTIL_SRC_FILES += $(SMART)/smart.c         $(SMART)/smart_common.c

# -------------------------------------------------------
# Build Specs
# -------------------------------------------------------

#Aggregate all include and src directories
INCLUDE_ALL_DIRS += $(APP_INCLUDE_DIRS)
INCLUDE_ALL_DIRS += $(UTIL_INCLUDE_DIRS)

SRC_FILES += $(APP_SRC_FILES)
SRC_FILES += $(UTIL_SRC_FILES)

COMPILER_FLAGS += $(CXXFLAGS)
#If the processor is big endian uncomment the compiler flag
#COMPILER_FLAGS += -DREVERSED
COMPILER_FLAGS += -std=c++0x

MAKE_CMD = $(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(OUTPUT_DIR)/$(APP_NAME) $(LD_FLAG) $(INCLUDE_ALL_DIRS)

all:
	$(DEBUG)$(MAKE_CMD)

clean:
	rm -f $(OUTPUT_DIR)/$(APP_NAME)

