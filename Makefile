ifeq ($(strip $(T)),)
$(error "Please select target platform with either T=3ds or T=switch")
endif

ifeq ($(strip $(T)),3ds)
	include 3ds.mk
endif

ifeq ($(strip $(T)),switch)
	include switch.mk
endif
