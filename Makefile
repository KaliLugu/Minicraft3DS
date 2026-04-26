#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET		:=	Minicraft3DS
BUILD		:=	build_3ds

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	:=	-g -Wall -O2 -mword-relocations \
			-ffunction-sections \
			-Wno-error \
			-Wno-error=incompatible-pointer-types \
			$(ARCH)

CFLAGS	+=	$(INCLUDE) -D__3DS__

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lpng -lz -lcitro2d -lcitro3d -lctru -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(DEVKITPRO)/portlibs/3ds $(CTRULIB)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

SOURCES		:=	source source/minizip source/data source/data/items source/menu source/editor source/network source/ingamemenu source/entity source/render source/engine source/engine/3ds
INCLUDES	:=	include
ROMFS		:=	resources

export APP_TITLE	:= Minicraft
export APP_DESCRIPTION	:= Minicraft was originally created by Markus "Notch" Perrson.
export APP_AUTHOR	:= Davideesk/andre111/adrien

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) 

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ and libEGL dependent projects
#---------------------------------------------------------------------------------
export LD	:=	$(CC)
#---------------------------------------------------------------------------------

export OFILES_SOURCES 	:=	$(CFILES:.c=.o)

export OFILES := $(OFILES_SOURCES)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export APP_ICON := $(TOPDIR)/icons-banners/icon.png

export _3DSXFLAGS += --smdh=$(CURDIR)/$(TARGET).smdh
export _3DSXFLAGS += --romfs=$(CURDIR)/$(ROMFS)

# Export DEPSDIR to fix dependency file path issue
export DEPSDIR  := $(CURDIR)/$(BUILD)

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@echo 3ds build ...
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean 3ds build files ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(TARGET).smdh $(TARGET).elf $(TARGET).cia $(TARGET).3ds compile.log

#---------------------------------------------------------------------------------
else

DEPSDIR ?= .

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).3dsx	:	$(OUTPUT).elf $(OUTPUT).smdh

$(OUTPUT).elf	:	$(OFILES)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

.PHONY:  init
init:
	git config core.hooksPath .github/hooks
	chmod +x .github/hooks/pre-push
	@echo "Git hooks initialized. Pre-push hook is now active."