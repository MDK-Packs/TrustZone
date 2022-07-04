:: Sign Image
:: sign_image.bat <name> <version> <counter>
::   Input:
::     <name>.hex: hex image to be signed
::     <version>:  version string (ex: "1.0.0")
::     <counter>:  security counter (ex: 1)
::   Output:
::     <name>_signed.bin: signed binary image (confirmed)
::     <name>_ota.bin:    signed binary for OTA (not confirmed)
::     <name>_signed.hex: signed hex image
::   Run from uVision: sign_image.bat $L@L "1.0.0" 1

@if .%1==. goto help
@if .%2==. goto help
@if .%3==. goto help

imgtool sign --version %2 --key %~dp0..\..\..\..\bl2\root-RSA-3072_1.pem --public-key-format full --align 16 --pad --pad-header --boot-record NSPE --header-size 0x400 --slot-size 0x60000 --security-counter %3 --confirm --dependencies "(0,"1.6.0"+0)" %1.hex %1_signed.bin
imgtool sign --version %2 --key %~dp0..\..\..\..\bl2\root-RSA-3072_1.pem --public-key-format full --align 16 --pad --pad-header --boot-record NSPE --header-size 0x400 --slot-size 0x60000 --security-counter %3           --dependencies "(0,"1.6.0"+0)" %1.hex %1_ota.bin

bin2hex.py --offset 0x01060000 %1_signed.bin %1_signed.hex

@exit /B 0

:help
@echo off
echo Sign Image
echo.
echo sign_image.bat ^<name^> ^<version^> ^<counter^>
echo   Input:
echo     ^<name^>.hex: hex image to be signed
echo     ^<version^>:  version string (ex: "1.0.0")
echo     ^<counter^>:  security counter (ex: 1)
echo   Output:
echo     ^<name^>_signed.bin: signed binary image (confirmed)
echo     ^<name^>_ota.bin:    signed binary for OTA (not confirmed)
echo     ^<name^>_signed.hex: signed hex image
exit /B 1
