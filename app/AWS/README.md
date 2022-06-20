# AWS Demos
 - [coreMQTT Mutual Authentication Demo](#coremqtt-mutual-authentication-demo)
 - [Over-the-air updates via MQTT Demo](#over-the-air-updates-via-mqtt-demo)

## Prerequisites

Tools:
 - [CMSIS-Toolbox 0.10.2](https://github.com/Open-CMSIS-Pack/devtools/releases/tag/tools%2Ftoolbox%2F0.10.2)
 - [Keil MDK 5.37](https://www.keil.com/download/product)
 - Arm Compiler 6.18 (part of MDK)
 - [STM32CubeProgrammer v2.10.0 or later](https://www.st.com/en/development-tools/stm32cubeprog.html)

CMSIS packs: required packs are listed in the [AWS.csolution.yml](AWS.csolution.yml) file

## Target Setup

 - target needs to be preloaded with prebuilt TF-M. See [README.md](../../tfm/README.md) for more details.
 - target needs to be preloaded with [Provision application](#provision-application).
 - target needs to be provisioned. See [Provisioning the target](Provision.md).

## Provision Application

This application exposes a provisioning API accessible through the terminal which can be used to provision the target.  
See [Provisioning the target](Provision.md) for details.

Application is available for the following targets:
 - `B-U585I-IOT02A`: runs on B-U585I-IOT02A board

### Target: `B-U585I-IOT02A`

Board: B-U585I-IOT02A

1. Use `csolution` to create `.cprj` project files  
`csolution convert -s AWS.csolution.yml -c Provision.Debug+B-U585I-IOT02A`  
`csolution convert -s AWS.csolution.yml -c Provision.Release+B-U585I-IOT02A`

2. Build a specific project
  - use `cbuild`  
  `cbuild Provision.Debug+B-U585I-IOT02A.cprj`  
  `cbuild Provision.Release+B-U585I-IOT02A.cprj`  
  - or use MDK and import `Provision.<build-type>+B-U585I-IOT02A.cprj` and build with MDK  
  Note: due to current importer limitation it is necessary to manually add the following preprocessor define 
  `MBEDTLS_CONFIG_FILE=\"mbedtls_config_psa.h\"`

3. Sign the image (`<version> = "0.0.0"`, `<security_counter> = 1`)
  - run `sign_image.bat <name>.hex <version> <security_counter>`
  - or use MDK:
    - Options for Target - Output - enable "Create HEX File"
    - Options for Target - User - After Build/Rebuild - enable Run #1 `sign_image.bat $L@L.hex <version> <security_counter>`
    - Options for Target - Utilities - disable "Update Target before Debugging"

4. Run the application
  - connect the board ST-Link USB to a PC (provides also power)
  - open terminal on PC and connect to board's serial port (Baud rate: 115200)
  - program the signed image to the target using STM32CubeProgrammer
  - reset the target and execute the `provision.py` script

## coreMQTT Mutual Authentication Demo

This demo application connects to **AWS MQTT broker** using TLS with mutual authentication between the client and the server.
It demonstrates the subscribe-publish workflow of MQTT.

Visit [*coreMQTT mutual authentication demo*](https://docs.aws.amazon.com/freertos/latest/userguide/mqtt-demo-ma.html) for further information.

Please note, that the target needs to be preloaded with TF-M and provisioned to successfully run the demo application. See [Target Setup](#target-setup).

Once the target is provisioned you can:
- Build the application.
- Connect and configure the debugger.
- Program the application.
- Run the application and view messages in a debug printf or terminal window.

MQTT messages can be viewed in the [**AWS IoT console**](https://docs.aws.amazon.com/iot/latest/developerguide/view-mqtt-messages.html).

Demo is available for the following targets:
 - `B-U585I-IOT02A`: runs on B-U585I-IOT02A board and uses WiFi interface with on-board WiFi module

### Target: `B-U585I-IOT02A`

Board: B-U585I-IOT02A  
Connectivity: WiFi interface with WiFi module

1. Use `csolution` to create `.cprj` project files  
`csolution convert -s AWS.csolution.yml -c MQTT_MutualAuth_Demo.Debug+B-U585I-IOT02A`  
`csolution convert -s AWS.csolution.yml -c MQTT_MutualAuth_Demo.Release+B-U585I-IOT02A`

2. Build a specific project
  - use `cbuild`  
  `cbuild MQTT_MutualAuth_Demo.Debug+B-U585I-IOT02A.cprj`  
  `cbuild MQTT_MutualAuth_Demo.Release+B-U585I-IOT02A.cprj`  
  - or use MDK and import `MQTT_MutualAuth_Demo.<build-type>+B-U585I-IOT02A.cprj` and build with MDK  
  Note: due to current importer limitation it is necessary to manually add the following preprocessor define 
  `MBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"`

3. Sign the image (`<version> = "0.9.2"`, `<security_counter> = 1`)
  - run `sign_image.bat <name>.hex <version> <security_counter>`
  - or use MDK:
    - Options for Target - Output - enable "Create HEX File"
    - Options for Target - User - After Build/Rebuild - enable Run #1 `sign_image.bat $L@L.hex <version> <security_counter>`
    - Options for Target - Utilities - disable "Update Target before Debugging"

4. Run the demo
  - connect the board ST-Link USB to a PC (provides also power)
  - open terminal on PC and connect to board's serial port (Baud rate: 115200)
  - program the signed image to the target using STM32CubeProgrammer
  - reset the target and observe messages in the terminal

## Over-the-air updates via MQTT Demo

This application demonstrates the functionality of **AWS Over-the-air (OTA)** update via MQTT.

Visit [*Over-the-air updates demo application*](https://docs.aws.amazon.com/freertos/latest/userguide/ota-demo.html) for further information.

Please note, that the target needs to be preloaded with TF-M and provisioned to successfully run the demo application. See [Target Setup](#target-setup).

Once the target is provisioned you can:
- Build the application.
- Connect and configure the debugger.
- Program the application.
- Run the application, start OTA update and view messages in a debug printf or terminal window.

See [OTA update process](OTA.md) for more details.

Demo is available for the following targets:
 - `B-U585I-IOT02A`: runs on B-U585I-IOT02A board and uses WiFi interface with on-board WiFi module

### Target: `B-U585I-IOT02A`

Board: B-U585I-IOT02A  
Connectivity: WiFi interface with WiFi module

1. Use `csolution` to create `.cprj` project files  
`csolution convert -s AWS.csolution.yml -c OTA_MQTT_Update_Demo.Debug+B-U585I-IOT02A`  
`csolution convert -s AWS.csolution.yml -c OTA_MQTT_Update_Demo.Release+B-U585I-IOT02A`

2. Build a specific project
  - use `cbuild`  
  `cbuild OTA_MQTT_Update_Demo.Debug+B-U585I-IOT02A.cprj`  
  `cbuild OTA_MQTT_Update_Demo.Release+B-U585I-IOT02A.cprj`  
  - or use MDK and import `OTA_MQTT_Update_Demo.<build-type>+B-U585I-IOT02A.cprj` and build with MDK  
  Note: due to current importer limitation it is necessary to manually add the following preprocessor define 
  `MBEDTLS_CONFIG_FILE=\"aws_mbedtls_config.h\"`

3. Sign the image (`<version> = "0.9.2"`, `<security_counter> = 1`)
  - run `sign_image.bat <name>.hex <version> <security_counter>`
  - or use MDK:
    - Options for Target - Output - enable "Create HEX File"
    - Options for Target - User - After Build/Rebuild - enable Run #1 `sign_image.bat $L@L.hex <version> <security_counter>`
    - Options for Target - Utilities - disable "Update Target before Debugging"

4. Run the demo
  - connect the board ST-Link USB to a PC (provides also power)
  - open terminal on PC and connect to board's serial port (Baud rate: 115200)
  - program the signed image to the target using STM32CubeProgrammer
  - reset the target, start OTA update and observe messages in the terminal
