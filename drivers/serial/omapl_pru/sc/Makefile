EXTRA_CFLAGS += -I$(srctree)/drivers/serial/omapl_pru/pru/hal/sc/include -I$(srctree)/drivers/serial/omapl_pru/pru/hal/common/include

sc_rdr-objs :=  ti_omapl_pru_sc.o \
                    ../pru/hal/sc/src/suart_api.o \
                    ../pru/hal/sc/src/suart_utils.o \
                    ../pru/hal/common/src/pru.o

obj-$(CONFIG_SMARTCARD_RDR_SERIAL_OMAPL_PRU)    += sc_rdr.o
