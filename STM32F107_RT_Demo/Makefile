#
# Makefile for power ctrl project. CODETYPE and MODEL_FLAG are hardcoded.
#

UNAME := $(shell uname -s)
ARCH :=$(shell uname -m)

CODETYPE :=IAP
MODEL_FLAG :=-DPROJ_NEWPWRCTRL 
 
include version.mk

CROSS_COMPILER =gcc-arm-none-eabi-9-2019-q4-major

ifeq ($(OS),Windows_NT)
TOOLS_PATH := /opt/$(CROSS_COMPILER)/bin
else
ifeq ($(UNAME), Linux)
TOOLS_PATH := /opt/toolchains/$(CROSS_COMPILER)/bin
endif
ifeq ($(UNAME),Darwin)
TOOLS_PATH := /Users/weiwen/tools/$(CROSS_COMPILER)/bin
endif
endif

PREFIX := $(TOOLS_PATH)/arm-none-eabi-
CC := $(PREFIX)gcc
AS := $(PREFIX)as
AR := $(PREFIX)ar
LD := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

SED := sed
MV := mv
RM := rm
DD := dd

SCOPY = cat $(bootloader) $(app_bin) > $(fmware)

.SUFFIXES: .o .c .s .S

TOP_DIR =.

APP_DIR = $(TOP_DIR)/apps
MIDWARE_DIR = $(TOP_DIR)/midware
LIB_DIR = $(TOP_DIR)/lib
BOARD_DIR = $(TOP_DIR)/board
RTOS_DIR = $(TOP_DIR)/rtos
SCRIPTS_DIR =$(TOP_DIR)/scripts

BSP_DIR = $(BOARD_DIR)/bsp
CHIP_DIR =$(BOARD_DIR)/chip
HAL_DIR = $(BOARD_DIR)/hal

CMSIS_DIR = $(CHIP_DIR)/stm32f107/CMSIS
CMSIS_CORE_DIR = $(CMSIS_DIR)/CM3/CoreSupport
CMSIS_SUPPORT_DIR = $(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x
DEVICE_START_DIR = $(CMSIS_SUPPORT_DIR)/startup/gcc_ride7

BSP_DRIVER_DIR =$(CHIP_DIR)/stm32f107/Driver
BSP_DRIVER_SRC_DIR =$(BSP_DRIVER_DIR)/src

MQTT_DIR = $(MIDWARE_DIR)/MQTT
SET_DIR = $(MIDWARE_DIR)/setting

RTT_DIR =$(RTOS_DIR)/rtt

RTT_CMPNT_DIR =$(RTT_DIR)/components
RTT_CM3_DIR = $(RTT_DIR)/libcpu/arm/cortex-m3
RTT_ARM_COM_DIR = $(RTT_DIR)/libcpu/arm/common
RTT_KERNEL_DIR =$(RTT_DIR)/src
RTT_DFS_DIR =$(RTT_CMPNT_DIR)/dfs
RTT_DRIVER_DIR =$(RTT_CMPNT_DIR)/drivers
RTT_FINSH_DIR=$(RTT_CMPNT_DIR)/finsh
RTT_NET_DIR =$(RTT_CMPNT_DIR)/net

FAT_DIR =$(RTT_DFS_DIR)/filesystems/elmfat

LWIP_DIR=$(RTT_NET_DIR)/lwip-2.1.2/src
LWIP_INC_DIR =$(LWIP_DIR)/include

INC_CMSIS  = -I$(CMSIS_CORE_DIR) -I$(CMSIS_SUPPORT_DIR)
INC_DRIVER = -I$(BSP_DRIVER_DIR)/inc -I$(BSP_DIR)
INC_HAL    = -I$(HAL_DIR)
INC_MIDWARE = -I$(MIDWARE_DIR) -I$(MIDWARE_DIR)/MQTT -I$(MIDWARE_DIR)/setting
INC_APP    =-I$(APP_DIR)
INC_RTT = -I$(RTT_DIR)/include  -I$(RTT_DFS_DIR)/include -I$(RTT_FINSH_DIR) -I$(RTT_DRIVER_DIR)/include -I$(RTT_DRIVER_DIR)/include/drivers -I$(RTT_DRIVER_DIR)/include/ipc
INC_LWIP = -I$(LWIP_DIR) -I$(LWIP_INC_DIR) -I$(LWIP_DIR)/arch/include
INC_FAT = -I$(FAT_DIR)

INC_ALL = $(INC_CMSIS) $(INC_DRIVER) $(INC_HAL) $(INC_MIDWARE) $(INC_APP) $(INC_RTT) $(INC_LWIP) \
		  $(INC_FAT)

OUTPUT_DIR = $(TOP_DIR)/out

###########################################################################################################

APP_FLAGS = 

#online upgrade supporting is controled only by IAP_SLAVE
#this macro is defined in code by default
#ifeq ($(CODETYPE),IAP)
#APP_FLAGS += -DIAP_SLAVE
#endif

STARTUP_DEFS = -D__STARTUP_CLEAR_BSS -D__START=main
DFLAGS =  -DSTM32F10X_CL -DUSE_STDPERIPH_DRIVER $(MODEL_FLAG)

ifneq ($(REL),yes)
DFLAGS += -DDEBUG
endif

###########################################################################################################

ver = $(APP_DIR)/version.h #do NOT change this name: included in code

startup_src =$(DEVICE_START_DIR)/startup_stm32f10x_cl.s

app_src = $(APP_DIR)/application.c                   \
          $(APP_DIR)/startup.c                       \
          $(APP_DIR)/swdog.c                         \
          $(APP_DIR)/boardtest.c                     \
          $(APP_DIR)/fw_upgrade.c                    \
          $(APP_DIR)/version.c						 \
          $(APP_DIR)/crc8.c                          \
          $(APP_DIR)/com_master.c                    \


bsp_src = $(BSP_DIR)/board.c                         \
          $(BSP_DIR)/stm32f10x_it.c                  \
          $(BSP_DIR)/retarget.c                      \
          $(BSP_DIR)/usart.c                         \
          $(BSP_DIR)/platform.c                      \
          $(BSP_DIR)/stm32_eth.c                     \
          $(BSP_DIR)/rt_stm32f10x_spi.c              \
          $(BSP_DIR)/nextai_flash.c                  \
          $(BSP_DIR)/exboard.c                       \

hal_src = $(HAL_DIR)/hal_com.c                      \
          $(HAL_DIR)/hal_led.c


chip_driver_src = \
            $(CMSIS_SUPPORT_DIR)/system_stm32f10x.c    \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_crc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_flash.c    \
			$(BSP_DRIVER_SRC_DIR)/misc.c               \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_rcc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_iwdg.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_gpio.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_usart.c    \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_dma.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_tim.c      \
#			$(BSP_DRIVER_SRC_DIR)/stm32f10x_wwdg.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_pwr.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_exti.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_bkp.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_i2c.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_adc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_dac.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_rtc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_fsmc.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_spi.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_sdio.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_dbgmcu.c   \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_can.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_cec.c      \


rtt_kernel_src = \
			$(RTT_KERNEL_DIR)/clock.c                  \
			$(RTT_KERNEL_DIR)/device.c                 \
			$(RTT_KERNEL_DIR)/idle.c                   \
			$(RTT_KERNEL_DIR)/ipc.c                    \
			$(RTT_KERNEL_DIR)/irq.c                    \
			$(RTT_KERNEL_DIR)/kservice.c               \
			$(RTT_KERNEL_DIR)/mem.c                    \
			$(RTT_KERNEL_DIR)/mempool.c                \
			$(RTT_KERNEL_DIR)/object.c                 \
			$(RTT_KERNEL_DIR)/scheduler.c              \
			$(RTT_KERNEL_DIR)/thread.c                 \
			$(RTT_KERNEL_DIR)/timer.c                  \

rtt_port_src = \
			$(RTT_CM3_DIR)/cpuport.c                   \
			$(RTT_ARM_COM_DIR)/backtrace.c             \
			$(RTT_ARM_COM_DIR)/div0.c                  \
			$(RTT_ARM_COM_DIR)/showmem.c   

rtt_port_asm = \
			$(RTT_CM3_DIR)/context_gcc.S   

#rtt_fs_src = \
			 $(RTT_DFS_DIR)/src/dfs.c                 \
			 $(RTT_DFS_DIR)/src/dfs_file.c            \
			 $(RTT_DFS_DIR)/src/dfs_fs.c              \
			 $(RTT_DFS_DIR)/src/dfs_posix.c           \


#fat_src = \
		     $(FAT_DIR)/dfs_elm.c                     \
		     $(FAT_DIR)/ff.c                          

rtt_drivers_src = \
			 $(RTT_DRIVER_DIR)/serial/serial.c        \
			 $(RTT_DRIVER_DIR)/src/completion.c       \
			 $(RTT_DRIVER_DIR)/src/dataqueue.c        

#			 $(RTT_DRIVER_DIR)/spi/spi_core.c         \
#			 $(RTT_DRIVER_DIR)/spi/spi_dev.c          \
#			 $(RTT_DRIVER_DIR)/spi/spi_flash_w25qxx.c

#			 $(RTT_DRIVER_DIR)/src/pipe.c             \
			 $(RTT_DRIVER_DIR)/src/portal.c           \
			 $(RTT_DRIVER_DIR)/src/ringbuffer.c       \
			 $(RTT_DRIVER_DIR)/src/workqueue.c       


#rtt_finsh_src = \
			 $(RTT_FINSH_DIR)/shell.c                 \
			 $(RTT_FINSH_DIR)/symbol.c                \
			 $(RTT_FINSH_DIR)/cmd.c                   \
			 $(RTT_FINSH_DIR)/finsh_compiler.c        \
			 $(RTT_FINSH_DIR)/finsh_error.c           \
			 $(RTT_FINSH_DIR)/finsh_heap.c            \
			 $(RTT_FINSH_DIR)/finsh_init.c            \
			 $(RTT_FINSH_DIR)/finsh_node.c            \
			 $(RTT_FINSH_DIR)/finsh_ops.c             \
			 $(RTT_FINSH_DIR)/finsh_parser.c          \
			 $(RTT_FINSH_DIR)/finsh_var.c             \
			 $(RTT_FINSH_DIR)/finsh_vm.c              \
			 $(RTT_FINSH_DIR)/finsh_token.c


lwip_src = \
		    $(LWIP_DIR)/api/api_lib.c                 \
		    $(LWIP_DIR)/api/api_msg.c                 \
		    $(LWIP_DIR)/api/err.c                     \
		    $(LWIP_DIR)/api/netbuf.c                  \
		    $(LWIP_DIR)/api/netdb.c                   \
		    $(LWIP_DIR)/api/netifapi.c                \
		    $(LWIP_DIR)/api/sockets.c                 \
		    $(LWIP_DIR)/api/tcpip.c                   \
			$(LWIP_DIR)/core/def.c                    \
			$(LWIP_DIR)/core/dns.c                    \
			$(LWIP_DIR)/core/init.c                   \
			$(LWIP_DIR)/core/memp.c                   \
			$(LWIP_DIR)/core/netif.c                  \
			$(LWIP_DIR)/core/pbuf.c                   \
			$(LWIP_DIR)/core/raw.c                    \
			$(LWIP_DIR)/core/stats.c                  \
			$(LWIP_DIR)/core/sys.c                    \
			$(LWIP_DIR)/core/tcp.c                    \
			$(LWIP_DIR)/core/tcp_in.c                 \
			$(LWIP_DIR)/core/tcp_out.c                \
			$(LWIP_DIR)/core/timeouts.c               \
			$(LWIP_DIR)/core/ip.c                     \
			$(LWIP_DIR)/core/udp.c                    \
			$(LWIP_DIR)/core/inet_chksum.c            \
			$(LWIP_DIR)/core/ipv4/autoip.c            \
			$(LWIP_DIR)/core/ipv4/icmp.c              \
			$(LWIP_DIR)/core/ipv4/etharp.c            \
			$(LWIP_DIR)/core/ipv4/ip4.c               \
			$(LWIP_DIR)/core/ipv4/ip4_addr.c          \
			$(LWIP_DIR)/core/ipv4/ip4_frag.c          \
			$(LWIP_DIR)/core/ipv4/dhcp.c              \
			$(LWIP_DIR)/core/ipv6/dhcp6.c             \
			$(LWIP_DIR)/core/ipv6/inet6.c             \
			$(LWIP_DIR)/core/ipv6/ip6_addr.c          \
			$(LWIP_DIR)/core/ipv6/ip6_frag.c          \
			$(LWIP_DIR)/netif/ethernet.c              \
			$(LWIP_DIR)/netif/ethernetif.c            \
			$(LWIP_DIR)/arch/sys_arch.c               \


mqtt_src = \
		   $(MQTT_DIR)/nextai_mqtt.c                  \
		   $(MQTT_DIR)/mqtt_client.c                  

set_src = \
		  $(SET_DIR)/cJSON.c                         \
		  $(SET_DIR)/nextai_setting.c



driver_src = $(chip_driver_src) $(bsp_src) $(hal_src)
os_src = $(rtt_kernel_src) $(rtt_port_src) $(rtt_drivers_src) $(rtt_finsh_src) $(rtt_fs_src)
midware_src = $(fat_src) $(lwip_src) $(mqtt_src) $(set_src) $(MIDWARE_DIR)/inodes_port.c

all_src_c = $(driver_src) $(os_src) $(midware_src) $(app_src)
all_src_asm = $(startup_src) $(rtt_port_asm)

load_drv_src = \
            $(CMSIS_SUPPORT_DIR)/system_stm32f10x.c    \
            $(BSP_DIR)/stm32f10x_it.c                  \
            $(BSP_DIR)/retarget.c                      \
            $(BSP_DIR)/bootboard.c                     \
            $(BSP_DIR)/nextai_flash.c                  \
			$(CMSIS_CORE_DIR)/core_cm3.c               \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_crc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_flash.c    \
			$(BSP_DRIVER_SRC_DIR)/misc.c               \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_rcc.c      \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_iwdg.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_gpio.c     \
			$(BSP_DRIVER_SRC_DIR)/stm32f10x_usart.c    \


load_app_src = \
          $(APP_DIR)/bootstart.c                       \
          $(APP_DIR)/version.c

bootloader_src = $(load_app_src) $(load_drv_src)

###############################################################################
loaderscript := $(SCRIPTS_DIR)/stm32f107vc_bl.ld 

ifeq ($(CODETYPE),IAP)
ldscript := $(SCRIPTS_DIR)/stm32f107vc_sl.ld
else
ldscript := $(SCRIPTS_DIR)/stm32f107vc.ld
endif

PACK_NAME = INODES_PC_405

packed_file :=$(strip $(OUTPUT_DIR)/$(PACK_NAME)_v$(GIT_VER).$(git_branch).$(shell date +%Y%m%d_%H%M%S).tar.gz)

fmware := $(OUTPUT_DIR)/$(PACK_NAME)_v$(GIT_VER)_fw.bin

app_bin := $(OUTPUT_DIR)/$(PACK_NAME)_v$(GIT_VER)_app.bin
app_elf := $(OUTPUT_DIR)/$(PACK_NAME)_v$(GIT_VER).out

memmap = $(OUTPUT_DIR)/$(PACK_NAME)_$(GIT_VER).map

boot_bin := $(OUTPUT_DIR)/bloader.bin 
boot_elf := $(OUTPUT_DIR)/bloader.out
boot_mmap := $(OUTPUT_DIR)/bloader.map
bootloader := $(OUTPUT_DIR)/bloader16.bin

cc_info :=$(OUTPUT_DIR)/BuildInfo.txt


ifeq ($(CODETYPE),IAP)
target_boot := $(boot_bin)
else
target_boot :=
endif

FLAGS_LTO = -flto

CPPFLAGS = $(INC_ALL) $(DFLAGS) -ffunction-sections -fdata-sections --specs=nano.specs \
            -Wl,--gc-sections -fno-builtin  -Wall -Wextra  -Wno-format  -Os  $(FLAGS_LTO)

CFLAGS = -mcpu=cortex-m3 -mthumb -march=armv7-m

LDFLAGS = -mthumb -mcpu=cortex-m3 -fno-builtin --specs=nosys.specs --specs=nano.specs  $(FLAGS_LTO)
LDFLAGS += -nostartfiles $(STARTUP_DEFS)  -lm  #-nostdlib -nodefaultlibs #-lgcc 

M_LDFLAGS = -T $(ldscript) -Wl,-Map=$(memmap)  $(LDFLAGS)
B_LDFLAGS = -T $(loaderscript) -Wl,-Map=$(boot_mmap)  $(LDFLAGS)

##############################################################################

main_objects =  $(addprefix $(OUTPUT_DIR)/, $(startup_src:.s=.o))
main_objects +=  $(addprefix $(OUTPUT_DIR)/, $(rtt_port_asm:.S=.o))
main_objects +=  $(addprefix $(OUTPUT_DIR)/, $(all_src_c:.c=.o))

main_assb = $(addprefix $(OUTPUT_DIR)/, $(all_src_c:.c=.s))

bootloader_obj := $(addprefix $(OUTPUT_DIR)/, $(startup_src:.s=.o))
bootloader_obj += $(addprefix $(OUTPUT_DIR)/, $(bootloader_src:.c=.bo))

dependencies := $(addprefix $(OUTPUT_DIR)/, $(all_src_c:.c=.d))
dependencies_b := $(addprefix $(OUTPUT_DIR)/, $(bootloader_src:.c=.dd))

define make-depend
  $(CC) $(CPPFLAGS) -MM $1 2> /dev/null| \
  $(SED) -e 's,\($(notdir $2)\) *:,$(dir $2)\1: ,' > $3.tmp
  $(SED) -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$//' \
      -e '/^$$/ d' \
      -e 's/$$/ :/' $3.tmp >> $3.tmp
  $(MV) $3.tmp $3
endef

define make-depend_b
  $(CC) $(CPPFLAGS) -MM $1 2> /dev/null| \
  $(SED) -e 's,\($(notdir $2)\) *:,$(dir $2)\1: ,' > $3.tmp
  $(SED) -e 's/#.*//' \
      -e 's/^[^:]*: *//' \
      -e 's/ *\\$$//' \
      -e '/^$$/ d' \
      -e 's/$$/ :/' $3.tmp >> $3.tmp
  $(SED) -e 's/.o:/.bo:/' $3.tmp > $3.tmp0
  $(MV) $3.tmp0 $3
  $(RM) $3.tmp
endef

.PHONY: all clean assb boot

all: cflags $(packed_file)
ifneq ($(DIRTY),)
	@echo Dirty Version Build
else	
	@echo Clean Version Build
endif

cflags:
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo CFLAGS= $(CPPFLAGS) $(CFLAGS) $(APP_FLAGS)
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo
	
$(packed_file): $(app_bin)
	@echo  Building model: newpowerctrl > $(cc_info)
ifeq ($(CODETYPE),IAP)
	@echo  Release Files: $(notdir $(app_bin)) $(notdir $(fmware)) >> $(cc_info)
else
	@echo  Release Files: $(notdir $(app_bin))  >> $(cc_info)
endif
	@echo  Build Time: $(shell date '+%Y-%m-%d %H:%M:%S') >> $(cc_info)
	@echo  Release Version: $(BUILD_TAG)   >> $(cc_info)
	@echo  Build Git Version: $(BUILD_INFO)   >> $(cc_info)
	@echo  Git Branch: $(git_branch)   >> $(cc_info)
	@echo  Enabled Flags: $(APP_FLAGS) >> $(cc_info)
	@echo "Compiler: $(shell $(CC) -v 2>&1 | tail -n 1) " >>$(cc_info)
	@echo  Compile By: $(shell whoami) >>$(cc_info)
	@echo  Compile At: $(shell hostname) >>$(cc_info)
ifeq ($(CODETYPE),IAP)
	@$(SCOPY)
	@tar zcvf $(packed_file) -C $(OUTPUT_DIR) $(notdir $(app_bin)) $(notdir $(fmware)) $(notdir $(cc_info)) $(notdir $(memmap)) $(notdir $(boot_mmap))
else
	@tar zcvf $(packed_file) -C $(OUTPUT_DIR) $(notdir $(app_bin)) $(notdir $(cc_info)) $(notdir $(memmap)) $(notdir $(boot_mmap))
endif
	@echo Generating $(notdir $@)

assb: $(ver) $(main_assb)

boot: $(ver) $(boot_bin)

$(app_elf): $(ver) $(main_objects) $(ldscript) $(target_boot)
	@echo Linking $(notdir $@)
	@$(LD)  -o $@ $(main_objects) $(M_LDFLAGS)

$(app_bin): $(app_elf)
	@echo Creating Binary file $(notdir $@)
	@$(OBJCOPY) -O binary $(app_elf) $(app_bin)
	@echo Done

$(boot_bin): $(boot_elf)
	@echo Creating Binary file $(notdir $@)
	@$(OBJCOPY) -O binary $(boot_elf) $(boot_bin)
	### block copy boot bin as boot loader(16K, padding with zero)
	@$(DD) if=$(boot_bin) of=$(bootloader) ibs=16384 conv=sync 2> /dev/null

$(boot_elf): $(bootloader_obj) $(loaderscript)
	@echo Linking $(notdir $@)
	@$(LD)  -o $@ $(bootloader_obj) $(B_LDFLAGS)

$(ver):
	@echo Generating version file $(ver)
	@echo " " >$(ver)
	@echo "#ifndef __INODES_VER_H__ " >>$(ver)
	@echo "#define __INODES_VER_H__ " >>$(ver)
	@echo "/* this file was generated automatically while building on Linux platform */ " >>$(ver)
	@echo "#define MAIN_VERSION $(MAIN_VER)$(MINOR_VER) " >>$(ver)
	@echo "#define COMPILE_BY \"$(shell whoami)\" " >>$(ver)
	@echo "#define COMPILE_AT \"$(shell hostname)\" " >>$(ver)
	@echo "#define COMPILER \"$(shell $(CC) -v 2>&1 | tail -n 1)\" " >>$(ver)
ifneq ($(DIRTY),)
	@echo "#define VERSION_CHECK \"dirty\" " >>$(ver)
else
	@echo "#define VERSION_CHECK \"clean\" " >>$(ver)
endif
	@echo "#define GIT_VER \"$(BUILD_TAG)\" " >>$(ver)
	@echo "#define GIT_INFO \"$(BUILD_INFO)\" " >>$(ver)
	@echo "#endif " >>$(ver)
	@echo " " >>$(ver)

clean:
	-@$(RM) -rf $(OUTPUT_DIR)/* 2> /dev/null
	-@$(RM) -rf $(ver)


$(OUTPUT_DIR)/%.o : %.c
	@echo $<
	@mkdir -p $(dir $@)
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(APP_FLAGS)  -c $< -o $@

$(OUTPUT_DIR)/%.o : %.s
	@echo $<
	@mkdir -p $(dir $@)
	@$(AS) $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/%.o : %.S
	@echo $<
	@mkdir -p $(dir $@)
	@$(AS) $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/%.bo : %.c
	@echo $<
	@mkdir -p $(dir $@)
	@$(call make-depend_b,$<,$@,$(subst .bo,.dd,$@))
	@$(CC) $(CPPFLAGS) $(CFLAGS) -DBOOTLOADER -c $< -o $@

$(OUTPUT_DIR)/%.s : %.c
	@echo $<
	@mkdir -p $(dir $@)
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(APP_FLAGS) -fno-lto -S $< -o $@

ifneq "$(MAKECMDGOALS)" "clean"
-include $(dependencies)
-include $(dependencies_b)
endif

