# AWS Demos
 - [coreMQTT Mutual Authentication Demo](#coremqtt-mutual-authentication-demo)
 - [Over-the-air updates via MQTT Demo](#over-the-air-updates-via-mqtt-demo)

## Prerequisites

Tools:
 - [CMSIS-Toolbox 1.1.0](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/releases/tag/1.1.0) or later
 - [Keil MDK 5.37](https://www.keil.com/download/product)
 - Arm Compiler 6.18 (part of MDK)
 - [python 3.10 or later](https://www.python.org/downloads/windows/)
   - [imgtool 1.9.0 or later](https://pypi.org/project/imgtool/)
 - [AWS CLI2](https://awscli.amazonaws.com/AWSCLIV2.msi)
 - [STM32CubeProgrammer 2.10.0 or later](https://www.st.com/en/development-tools/stm32cubeprog.html) (for STM32 hardware)
 - [Arm Virtual Hardware](https://developer.arm.com/tools-and-software/simulation-models/arm-virtual-hardware) for MPS3 platform with Corstone-300 v11.18.1 or later

CMSIS packs: required packs are listed in the [AWS.csolution.yml](AWS.csolution.yml) file

## Provisioning Application

This application is integrated into the demos. It is activated automatically if the target has not yet been provisioned. 
It is also activated during reset while holding vioBUTTON0 button pressed. 
The application exposes a provisioning API accessible through the terminal which can be used to provision the target.  

Provision the target:
 - program the demo with integrated provisioning application into the target
 - reset the target while holding vioBUTTON0 button pressed (if already provisioned)
 - execute the `provision.py --interactive` script on the PC

> Note: target specific `provision.py` script is located in the `Board/<target_name>` directory

See [Provisioning the target](Provision.md) for details.

Provisioning data is stored in TF-M NVM (Non-volatile memory):
  - Protected Storage (PS)
  - Internal Trusted Storage Area (ITS)
  - OTP / NV Counters

## coreMQTT Mutual Authentication Demo

This demo application connects to **AWS MQTT broker** using TLS with mutual authentication between the client and the server.
It demonstrates the subscribe-publish workflow of MQTT.

Visit [*coreMQTT mutual authentication demo*](https://docs.aws.amazon.com/freertos/latest/userguide/mqtt-demo-ma.html) for further information.

Please note, that the target needs to be preloaded with TF-M and provisioned to successfully run the demo application. See [Target Setup](#target-setup).

Once the target is provisioned you can:
- Build the application.
- Connect and configure the debugger.
- Program the application.
- Run the application and view messages in a terminal window.

MQTT messages can be viewed in the [**AWS IoT console**](https://docs.aws.amazon.com/iot/latest/developerguide/view-mqtt-messages.html).

Demo is available for the following targets:
 - [AVH_MPS3_Corstone-300](#target-avh_mps3_corstone-300): runs on Arm Virtual Hardware (AVH) for MPS3 platform with Corstone-300
 - [B-U585I-IOT02A](#target-b-u585i-iot02a): runs on B-U585I-IOT02A board and uses WiFi interface with on-board WiFi module

## Over-the-air updates via MQTT Demo

This application demonstrates the functionality of **AWS Over-the-air (OTA)** update via MQTT.

Visit [*Over-the-air updates demo application*](https://docs.aws.amazon.com/freertos/latest/userguide/ota-demo.html) for further information.

Please note, that the target needs to be preloaded with TF-M and provisioned to successfully run the demo application. See [Target Setup](#target-setup).

Once the target is provisioned you can:
- Build the application.
- Connect and configure the debugger.
- Program the application.
- Run the application, start OTA update and view messages in a terminal window.

See [OTA update process](OTA.md) for more details.

Demo is available for the following targets:
 - [AVH_MPS3_Corstone-300](#target-avh_mps3_corstone-300): runs on Arm Virtual Hardware (AVH) for MPS3 platform with Corstone-300
 - [B-U585I-IOT02A](#target-b-u585i-iot02a): runs on B-U585I-IOT02A board and uses WiFi interface with on-board WiFi module

## Target Setup

 - target needs to be preloaded with prebuilt TF-M. See [README.md](../../tfm/README.md) for more details.
 - target needs to be loaded with [Provisioning application](#provisioning-application).
 - target needs to be provisioned. See [Provisioning the target](Provision.md).

### Target: `AVH_MPS3_Corstone-300`

Board: AVH_MPS3_Corstone-300  
Connectivity: VSocket (Virtual Socket)

1. Use `csolution` to create `.cprj` project
  ```
  csolution convert -s AWS.csolution.yml -c <project>.<build-type>+<target-type>

      <project>:     MQTT_MutualAuth_Demo | OTA_MQTT_Update_Demo
      <build-type>:  Debug | Release
      <target-type>: AVH_MPS3_Corstone-300
  ```

2. Build `.cprj` project
  - use `cbuild`  
  `cbuild <project>.<build-type>+<target-type>.cprj`  
  - or use MDK and import `<project>.<build-type>+<target-type>.cprj` and build with MDK  
  > Note: due to current importer limitation it is necessary to manually add the following preprocessor define 
  `MBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"`

3. Sign the image (ex: `<version> = "0.9.2"`, `<security_counter> = 1`)
  - run the standalone `sign_image` script:
    - on Windows:  
      `Board\AVH_MPS3_Corstone-300\sign_image.bat <name_hex> <version> <security_counter>`
    - on Linux:  
      `./Board/AVH_MPS3_Corstone-300/sign_image.sh <name_hex> <version> <security_counter>`
  - or use MDK:
    - Options for Target - Output - enable "Create HEX File"
    - Options for Target - User - After Build/Rebuild - enable Run #1:  
      `Board\AVH_MPS3_Corstone-300\sign_image.bat $L@L <version> <security_counter>`

4. Run the demo
  - via command line (from project root directory and VHT executable in path):
    ```
    VHT_Corstone_SSE-300_Ethos-U55 -f Board/AVH_MPS3_Corstone-300/fvp_config.txt 
        -a ../../bl2/target/mps3_corstone-300_ethos-u55/bl2.axf 
    --data ../../tfm/target/mps3_corstone-300_ethos-u55/tfm_s_signed.bin@0x11000000 
    --data "<image>_signed.bin"@0x01060000
    ```
  - or in uVision:
    - configure the following settings:
      - open "Options for Target"
      - select "Debug" tab
      - under "Use" select "Models ARMv8-M Debugger" and click "Settings" end enter the following:
        - Command: `$KARM\VHT\VHT_Corstone_SSE-300_Ethos-U55.exe`
        - Arguments:
          ```
          --data ../../bl2/target/mps3_corstone-300_ethos-u55/bl2.bin@0x10000000 
          --data ../../tfm/target/mps3_corstone-300_ethos-u55/tfm_s_signed.bin@0x11000000 
          --data $L@L_signed.bin@0x01060000
          ```
        - Target: `cpu0`
        - Configuration File: `Board\AVH_MPS3_Corstone-300\fvp_config.txt`
    - start debug session and run

  > Note: running on fast computers can lead to simulation running too quickly resulting in dropping incoming data packets from the network. This will be seen as error messages in the terminal window.  
    Reduce the number of ticks to simulate for each quantum by specifying the following command line option `-Q <n>`, where `<n>` is the number of ticks (default value = 10000).  
    Example: `-Q 10`

Provisioning data which is stored in TF-M NVM can be saved after target is provisioned 
and used subsequently without the need to provision the target every time:
  - using command line:
    - add the following option to the command line: 
      `--dump mps3_board.sse300.iotss3_internal_sram1=tfm_nvm.bin@0x1f0000,0xb000`
    - run the demo and provision the target (only the first time)
    - shutdown the VHT model by Ctrl-C (it will save the TF-M NVM to `tfm_nvm.bin`)
    - add TF-M NVM as command line option for all subsequent runs: 
      `--data tfm_nvm.bin@0x311f0000`
  - or using uVision:
    - run the demo and provision the target (only the first time)
    - save TF-M NVM: execute command `SAVE tfm_nvm.hex 0x311f0000,0x311fafff`
    - convert TF-M NVM to Binary: execute python script `hex2bin.py tfm_nvm.hex tfm_nvm.bin`
    - add TF-M NVM as argument for all subsequent runs: 
      `--data tfm_nvm.bin@0x311f0000`

### Target: `B-U585I-IOT02A`

Board: B-U585I-IOT02A  
Connectivity: WiFi interface with WiFi module

1. Use `csolution` to create `.cprj` project
  ```
  csolution convert -s AWS.csolution.yml -c <project>.<build-type>+<target-type>

      <project>:     MQTT_MutualAuth_Demo | OTA_MQTT_Update_Demo
      <build-type>:  Debug | Release
      <target-type>: B-U585I-IOT02A
  ```

2. Build `.cprj` project
  - use `cbuild`  
  `cbuild <project>.<build-type>+<target-type>.cprj`  
  - or use MDK and import `<project>.<build-type>+<target-type>.cprj` and build with MDK  
  > Note: due to current importer limitation it is necessary to manually add the following preprocessor define 
  `MBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"`

3. Sign the image (ex: `<version> = "0.9.2"`, `<security_counter> = 1`)
  - run the standalone `sign_image` script:
    - on Windows:  
      `Board\B-U585I-IOT02A\sign_image.bat <name_hex> <version> <security_counter>`
    - on Linux:  
      `./Board/B-U585I-IOT02A/sign_image.sh <name_hex> <version> <security_counter>`
  - or use MDK:
    - Options for Target - Output - enable "Create HEX File"
    - Options for Target - User - After Build/Rebuild - enable Run #1:  
      `Board\B-U585I-IOT02A\sign_image.bat $L@L <version> <security_counter>`
    - Options for Target - Utilities - disable "Update Target before Debugging"

4. Run the demo
  - connect the board ST-Link USB to a PC (provides also power)
  - open terminal on PC and connect to board's serial port (Baud rate: 115200)
  - program the signed image to the target using STM32CubeProgrammer
  - reset the target
