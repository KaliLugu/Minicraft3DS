ifeq ($(strip $(PLATFORM)),)
$(error "Please select platform with either PLATFORM=3ds or PLATFORM=switch")
endif

ifeq ($(strip $(PLATFORM)),3ds)
	include 3ds.mk
endif

ifeq ($(strip $(PLATFORM)),switch)
	include switch.mk
endif
