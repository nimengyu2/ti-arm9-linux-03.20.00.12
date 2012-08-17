#!/bin/sh
SRC=kernel
CROSS_COMPILE=arm-none-linux-gnueabi-
ARCH=arm
DEFAULT_CONFIG=""

case "$1" in
    clean|c)
        echo "Starting $SRC make clean CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"
        make CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH clean
        ;;
    uImage|k)
        echo "Starting $SRC make uImage CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"
        make CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH uImage
        ;;
    menuconfig)
        echo "Starting $SRC make menuconfig CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"
        make CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH menuconfig
        ;;
    modules|m)
        echo "Starting $SRC make modules CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"
        make CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH modules
        ;;
    minstall|mi)
	if [ $# -lt 2 ];then
	echo "Usage: $0 minstall|mi PATH"
	else
		if [ -d $2 ];then 
			echo "Starting $SRC make modules install CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"	
			make modules_install INSTALL_MOD_PATH=$2 CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH
		else
			echo "Dir $2 is not exist! Can not install moudules"
		fi
	fi
        ;;
    kinstall|ki)
	if [ $# -lt 2 ];then
	echo "Usage: $0 kinstall|ki PATH"
	else
		if [ -d $2 ];then 
			echo "Starting $SRC make uImage install CROSS_COMPILE=$CROSS_COMPILE ARCH=$ARCH"
			cp arch/arm/boot/uImage $2
		else
			echo "Dir $2 is not exist! Can not install kernel uImage"
		fi
	fi
        ;;
    *)
        echo "Usage: $0 {clean|c|uImage|k|modules|m}"
	echo "Usage: $0 minstall|mi PATH"
	echo "Usage: $0 kinstsall|ki PATH"
        exit 1
        ;;
esac

exit 0


#DAEMON=/usr/sbin/pcscd
#NAME=pcscd
#DESC="PCSC Daemon"
#PIDFILE=/var/run/pcscd/pcscd.pid
#ARGS=""
#test -f $DAEMON || exit 0
