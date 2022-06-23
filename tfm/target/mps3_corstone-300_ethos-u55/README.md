# TF-M for Corstone-300 with Ethos-U55 for MPS3 and AVH

Prebuilt TF-M and BL2 for Corstone SSE-300 with Ethos-U55 Subsystem for MPS3 (AN552) and Arm Virtual Hardware (AVH).

## Building

Repository: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git  
Tag: TF-Mv1.6.0

Configured AVH peripherals for non-secure access (see [patch](avh.patch))

Toolchain: GNU Arm Embedded Toolchain 10.3-2021.10

Configuration and Build commands:

`> mkdir build && cd build`

`> cmake .. -G"Unix Makefiles" -DTFM_PLATFORM=arm/mps3/an552 -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake -DCONFIG_TFM_FP=hard -DTFM_PARTITION_FIRMWARE_UPDATE=ON -DMCUBOOT_DATA_SHARING=ON -DCMAKE_BUILD_TYPE=Debug -DTFM_DEV_MODE=1`

`> cmake --build ./  -- install`
