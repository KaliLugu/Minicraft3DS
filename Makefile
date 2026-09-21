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

LIBS	:= -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lpng -lz -lcitro2d -lcitro3d -lctru -lm

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

SOURCES		:=	source source/minizip source/data source/data/items source/menu source/editor source/network source/ingamemenu source/entity source/render source/engine source/engine/3ds lib
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
local: $(BUILD)

$(BUILD):
	@echo 3ds build ...
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean 3ds build files ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(TARGET).smdh $(TARGET).elf $(TARGET).cia $(TARGET).3ds build.log $(TARGET).lst resources/fonts/*

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

MAKEFLAGS += -j$(nproc)

.PHONY:  init
init:
	git config core.hooksPath .github/hooks
	chmod +x .github/hooks/pre-push fonts/minicraft3ds-font-json2bin fonts/fonts.sh
	@echo "Git hooks initialized. Pre-push hook is now active."

#---------------------------------------------------------------------------------------
# Build docker image
#---------------------------------------------------------------------------------------
all:
	docker run --rm -v "$(PWD):/Minicraft3ds" -w /Minicraft3ds devkitpro/devkitarm:20251231 \
		bash -c "bash fetch-ssl.sh && bash fonts/fonts.sh -i fonts/ -o resources/fonts/ && make clean && make local -j$$(nproc) 2>&1 | tee build.log"

cia-docker:
	docker run --rm -v "$(PWD):/Minicraft3ds" -w /Minicraft3ds devkitpro/devkitarm:20251231 \
		bash -c "bash scripts/fetch-tools.sh && scripts/fetch-ssl.sh && bash fonts/fonts.sh -i fonts/ -o resources/fonts/ && make clean && make cia -j$$(nproc) 2>&1 | tee build.log"

cia:
	echo Building 3DSX/ELF/SMDH...
	$(MAKE) -j$$(nproc)
	rm -f makerom bannertool
	echo download makerom and bannertool
	bash $(TOPDIR)/scripts/fetch-tools.sh
	echo Creating banner...
	$(TOPDIR)/bannertool makebanner -i icons-banners/banner.png -a icons-banners/audio.wav -o icons-banners/banner.bnr
	echo Creating icon...
	$(TOPDIR)/bannertool makesmdh -s "Minicraft3DS" -l "3DS Homebrew port of Notch's ludum dare game 'Minicraft', updated." -p "Davideesk/Andre111/ElijahZAwesome/Adrien" -i icons-banners/icon.png  -o icons-banners/icon.icn
	echo Creating CIA...
	$(TOPDIR)/makerom -f cia -o Minicraft3DS.cia -DAPP_ENCRYPTED=false -rsf icons-banners/Minicraft3DS.rsf -target t -exefslogo -elf Minicraft3DS.elf -icon icons-banners/icon.icn -banner icons-banners/banner.bnr
	echo Creating 3DS/CCI...
	$(TOPDIR)/makerom -rand -f cci -o Minicraft3DS.3ds -DAPP_ENCRYPTED=true -rsf icons-banners/Minicraft3DS.rsf -target t -exefslogo -elf Minicraft3DS.elf -icon icons-banners/icon.icn -banner icons-banners/banner.bnr
