#
#  Makefile for the Linux Controller Area Network drivers.
#

obj-$(CONFIG_CAN_VCAN)		+= vcan.o

obj-$(CONFIG_CAN_DEV)		+= can-dev.o
can-dev-y			:= dev.o

obj-y				+= usb/

obj-$(CONFIG_CAN_SJA1000)	+= sja1000/
obj-$(CONFIG_CAN_MSCAN)		+= mscan/
obj-$(CONFIG_CAN_AT91)		+= at91_can.o
obj-$(CONFIG_CAN_TI_HECC)	+= ti_hecc.o
obj-$(CONFIG_CAN_TI_OMAPL_PRU)	+= omapl_pru/
obj-$(CONFIG_CAN_MCP251X)	+= mcp251x.o
obj-$(CONFIG_CAN_BFIN)		+= bfin_can.o

ccflags-$(CONFIG_CAN_DEBUG_DEVICES) := -DDEBUG
