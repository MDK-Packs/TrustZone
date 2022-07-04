Board: ARM AVH_MPS3_Corstone-300
----------------------------------------------

MPS3 platform for Corstone-300 simulated by Arm Virtual Hardware Targets (VHT).

The following models are available:
 - VHT_MPS3_Corstone_SSE-300: Corstone-300 for MPS3
 - VHT_Corstone_SSE-300_Ethos-U55: Corstone-300 with Ethos-U55 for MPS3
 - VHT_Corstone_SSE-300_Ethos-U65: Corstone-300 with Ethos-U55 for MPS3

TF-M with BL2 needs to be preloaded in order to run the application on the non-secure side.  
Following are the instructions how to run the VHT for Corstone-300 with Ethos-U55.

Running the VHT in uVision requires the following settings:
 - open "Options for Target"
 - select "Debug" tab
 - under "Use" select "Models ARMv8-M Debugger" and click "Settings" end enter the following:
   - Command: `$KARM\VHT\VHT_Corstone_SSE-300_Ethos-U55.exe`
   - Arguments:  
     `--data ../../bl2/target/mps3_corstone-300_ethos-u55/bl2.bin@0x10000000`  
     `--data ../../tfm/target/mps3_corstone-300_ethos-u55/tfm_s_signed.bin@0x11000000`  
     `--data $L@L_signed.bin@0x01060000`
   - Target: `cpu0`
   - Configuration File: `Board\AVH_MPS3_Corstone-300\fvp_config.txt`

Running the VHT via command line (from project root directory and VHT executable in path):  
`VHT_Corstone_SSE-300_Ethos-U55 -f Board/AVH_MPS3_Corstone-300/fvp_config.txt`  
`-a ../../bl2/target/mps3_corstone-300_ethos-u55/bl2.axf`  
`--data ../../tfm/target/mps3_corstone-300_ethos-u55/tfm_s_signed.bin@0x11000000`  
`--data "<image>_signed.bin"@0x01060000`

### System Configuration

| System Component        | Setting
|:------------------------|:----------------------------------------
| Device                  | SSE-300-MPS3
| Clock                   | 32 MHz
| Heap                    | 64 kB (configured in region_limit.h file)
| Stack (MSP)             | 1 kB (configured in region_limit.h file)

**STDIO** is routed to USART0

### CMSIS-Driver mapping

| CMSIS-Driver | Peripheral
|:-------------|:----------
| ETH_MAC0     | Ethernet LAN91C111
| ETH_PHY0     | Ethernet LAN91C111
| USART0       | USART0

| CMSIS-Driver VIO  | Physical board hardware
|:------------------|:-----------------------
| vioBUTTON0        | User Button PB1
| vioBUTTON1        | User Button PB2
| vioLED0           | User LED UL0
| vioLED1           | User LED UL1
| vioLED2           | User LED UL2
| vioLED3           | User LED UL3
| vioLED4           | User LED UL4
| vioLED5           | User LED UL5
| vioLED6           | User LED UL6
| vioLED7           | User LED UL7
