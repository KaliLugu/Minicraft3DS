#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
#
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
#---------------------------------------------------------------------------------
SOURCES		:=	source source/minizip source/data source/menu source/editor source/network source/ingamemenu source/entity source/render source/engine
INCLUDES	:=	include
ROMFS		:=	resources

APP_TITLE	:= Minicraft
APP_DESCRIPTION	:= Minicraft was originally created by Markus "Notch" Perrson.
APP_AUTHOR	:= andre111

PLATFORM	:= switch

ifeq ($(strip $(PLATFORM)),3ds)
	include 3ds.mk
endif

ifeq ($(strip $(PLATFORM)),switch)
	include switch.mk
endif
