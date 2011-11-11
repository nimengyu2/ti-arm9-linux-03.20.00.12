#ifdef CONFIG_OMAPL_SUART_FW_SOURCE
#$(if $(wildcard drivers/serial/omapl_pru/pru), , \
#		$(shell ln -s $(CONFIG_OMAPL_SUART_FW_SOURCE) drivers/serial/omapl_pru/pru))
#endif

#ifdef CONFIG_SERIAL_SUART_OMAPL_PRU
#EXTRA_CFLAGS += -I$(srctree)/drivers/serial/omapl_pru/pru/hal/uart/include -I$(srctree)/drivers/serial/omapl_pru/pru/hal/common/include
#endif

#ifdef CONFIG_SMARTCARD_RDR_SERIAL_OMAPL_PRU
#EXTRA_CFLAGS += -I$(srctree)/drivers/serial/omapl_pru/pru/hal/sc/include -I$(srctree)/drivers/serial/omapl_pru/pru/hal/common/include
#endif

#suart_emu-objs :=	ti_omapl_pru_suart.o \
					pru/hal/uart/src/suart_api.o \
					pru/hal/uart/src/suart_utils.o \
					pru/hal/common/src/pru.o

#sc_rdr-objs := 	ti_omapl_pru_sc.o \
					pru/hal/sc/src/suart_api.o \
					pru/hal/sc/src/suart_utils.o \
					pru/hal/common/src/pru.o

obj-$(CONFIG_SERIAL_SUART_OMAPL_PRU)	+= suart/
#obj-$(CONFIG_SERIAL_SUART_OMAPL_PRU)	+= suart_emu.o

#obj-$(CONFIG_SMARTCARD_RDR_SERIAL_OMAPL_PRU)	+= sc_rdr.o
obj-$(CONFIG_SMARTCARD_RDR_SERIAL_OMAPL_PRU)	+= sc/

ccflags-$(CONFIG_SUART_DEBUG_DEVICES) := -DDEBUG
