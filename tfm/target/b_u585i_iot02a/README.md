# TF-M for B-U585I-IOT02A

Prebuilt TF-M and BL2 for STMicroelectronics B-U585I-IOT02A Discovery Kit.

## Building

Repository: https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git  
Tag: TF-Mv1.6.0

Modified memory layout (see [patch](flash_layout.patch)):
 - BL2 size: 96 kB (instead of 84 kB)
 - Non-secure image size: 640 kB (instead of 512 kB)

Disabled Crypto Hardware Accelerator (see [patch](crypto_hw_accelerator.patch))

Toolchain: GNU Arm Embedded Toolchain 10.3-2021.10

Configuration and Build commands:

`> mkdir build && cd build`

`> cmake .. -DTFM_PLATFORM=stm/b_u585i_iot02a -DTFM_TOOLCHAIN_FILE=../toolchain_GNUARM.cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTFM_DEV_MODE=1 -DTFM_ISOLATION_LEVEL=2`

`> cmake --build ./  -- install`

## Programming

### Programming Option Bytes

  - Connect a **USB micro-B cable** between the **STLK** connector and your **Personal Computer**
  - Start the **STM32CubeProgrammer**
  - Click on **Connect** button (ST-LINK, SWD)
  - Click on the **OB** Option Bytes tab and configure the following values for Option Bytes:

    Option Bytes **User Configuration**

    Name         | Value     
    :------------|:----------
    TZEN         | Checked
    SRAM2_RST    | Unchecked

    Option Bytes **Boot Configuration**

    Name         | Value     
    :------------|:----------
    SECBOOTADDR0 | 0x180280 (address = 0x0C014000)

  - Click on the **Apply** button to update all **OB** Option Bytes changes

### Programming BL2 and TF-M

  - Use the **STM32CubeProgrammer**
  - Click on the **Download** icon (green down arrow) that opens **Erasing & Programming** tab
  - Under **Download - File Path** select **b_u585i_iot02a_bl2.hex** file located in **bl2** directory
  - Click on the **Start Programm...** button
  - Wait for download and verify to finish
  - Under **Download - File Path** select **b_u585i_iot02a_tfm_s_signed.hex** file
  - Click on the **Start Programm...** button
  - Click on the **Disconnect** button

The layout of the microcontroller internal flash memory is described in the table below: 

| Offset     | Bank |  Pages  | Region Name                     | Size (Dec.) | Size (Hex.) |
|------------|------|---------|---------------------------------|-------------|-------------|
| 0x00000000 | 1    | 0, 7    | Scratch                         |    64 KB    |   0x10000   |
| 0x00010000 | 1    | 8, 8    | BL2 - NV Counters               |     8 KB    |   0x02000   |
| 0x00012000 | 1    | 9, 9    | BL2 - NV Counters initial value |     8 KB    |   0x02000   |
| 0x00014000 | 1    | 10, 21  | BL2 - MCUBoot HDP Code          |    96 KB    |   0x18000   |
| 0x0002C000 | 1    | 22, 22  | BL2 - SPE Shared Code           |     4 KB    |   0x01000   |
| 0x0002D000 | 1    | 22, 22  | OTP Write Protect               |     4 KB    |   0x01000   |
| 0x0002E000 | 1    | 23, 24  | NV counters area                |    16 KB    |   0x04000   |
| 0x00032000 | 1    | 25, 26  | Secure Storage Area             |    16 KB    |   0x04000   |
| 0x00036000 | 1    | 27, 28  | Internal Trusted Storage Area   |    16 KB    |   0x04000   |
| 0x0003A000 | 1    | 29, 60  | Secure image     primary slot   |   256 KB    |   0x40000   |
| 0x0007A000 | 1-2  | 61, 13  | Non-secure image primary slot   |   640 KB    |   0xA0000   |
| 0x0011A000 | 2    | 14, 45  | Secure image     secondary slot |   256 KB    |   0x40000   |
| 0x0015A000 | 2    | 46, 124 | Non-secure image secondary slot |   640 KB    |   0xA0000   |

### Restoring Option Bytes

Bootloader configures various Option Bytes for security including secure area, hide protection and write protection.

To restore the Option bytes the following procedure may be executed:  
  - Connect a **USB micro-B cable** between the **STLK** connector and your **Personal Computer**
  - Start the **STM32CubeProgrammer**
  - Click on **Connect** button (ST-LINK, SWD)
  - Click on the **OB** Option Bytes tab and configure the following values for Option Bytes:

    Option Bytes **Boot Configuration**

    Name         | Value     
    :------------|:----------
    SECBOOTADDR0 | 0x180000 (address = 0x0C000000)

    Option Bytes **Secure Area 1**

    Name         | Value     
    :------------|:----------
    SECWM1_PSTRT | 0x0  (address = 0x08000000)
    SECWM1_PEND  | 0x7f (address = 0x080fe000)
    HDP1_PEND    | 0x0  (address = 0x08000000)
    HDP1EN       | Unchecked 

    Option Bytes **Write Protection 1**

    Name         | Value     
    :------------|:----------
    WRP1A_PSTRT  | 0x7f (address = 0x080fe000)
    WRP1A_PEND   | 0x0  (address = 0x08000000)
    UNLOCK_1A    | Checked

  - Click on the **Apply** button to update all **OB** Option Bytes changes  
  - Click on the **Download** icon (green down arrow) that opens **Erasing & Programming** tab
  - Click on **Full chip erase** under **Erase flash memory** tab
  - Click on the **Disconnect** button

> Note: TZEN can only be deactivated during RDP (readout protection) regression to Level 0. 
        Follow instructions described in the STM32U5 reference manual.