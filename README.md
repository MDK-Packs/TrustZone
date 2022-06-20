# Arm TrustZone for Cortex-M - Demo Applications

[**Arm TrustZone for Cortex-M**](https://www.arm.com/technologies/trustzone-for-cortex-m) enables System-Wide Security for IoT Devices. The technology reduces the potential for attack by isolating the critical security firmware, assets and private information from the rest of the application.

This repository contains example applications that leverage this technology.  The architecture of the application is shown in the diagram below.

![Architecture](https://user-images.githubusercontent.com/8268058/174683600-c28fa6ee-16f6-4e4b-8259-282eb62a9b9a.png)

**Applications Parts:**
- [AWS Demos](app/AWS/README.md)
- Secure second stage bootloader (BL2): [Prebuilt BL2](bl2/README.md) for various platforms
- Trusted Firmware (TF-M): [Prebuilt TF-M](tfm/README.md) for various platforms

The various AWS Demos implement for example Over-the-Air (OTA) Firmware Update. The process is shown in the following video.

[![OTA Demo on STM32U5](https://user-images.githubusercontent.com/8268058/174682995-313ee4b7-4afd-438e-aa09-9cd754696bc1.png)
](https://armkeil.blob.core.windows.net/developer/Files/videos/KeilMDK/OTA_Update_U5.mp4)
