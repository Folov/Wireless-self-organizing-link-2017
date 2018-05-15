#!/bin/bash

export  PATH=/WorkSpace/code/openwrt/openwrt_WRT1900ac/openwrt/staging_dir/toolchain-arm_cortex-a9+vfpv3_gcc-4.8-linaro_uClibc-0.9.33.2_eabi/bin:$PATH
export  STAGING_DIR=/home/guest/work_gyh/staging_dir

# arm-openwrt-linux-gcc ./*.c -o udpsrv-arm1900 -g -Wall -std=c99

export	PATH=/WorkSpace/code/openwrt/LEDE_WRT3200/source/staging_dir/toolchain-arm_cortex-a9+vfpv3_gcc-5.5.0_musl_eabi/bin:$PATH

make

exit
