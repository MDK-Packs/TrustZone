:: Sign Image
:: sign_image.bat <name> <version> <counter>
::   Input:
::     <name>:    hex image to be signed
::     <version>: version string (ex: "1.0.0")
::     <counter>: security counter (ex: 1)
::   Output:
::     <name>_signed.bin: signed binary image (confirmed)
::     <name>_ota.bin:    signed binary for OTA (not confirmed)
::     <name>_signed.hex: signed hex image
::   Run from uVision: sign_image.bat $L@L.hex "1.0.0" 1

@if .%1==. goto help
@if .%2==. goto help
@if .%3==. goto help

imgtool sign --version %2 --key ../../bl2/root-RSA-3072_1.pem --public-key-format full --align 16 --pad --pad-header --boot-record NSPE --header-size 0x400 --slot-size 0xA0000 --security-counter %3 --confirm --dependencies "(0,"1.6.0"+0)" %1 %~n1_signed.bin
imgtool sign --version %2 --key ../../bl2/root-RSA-3072_1.pem --public-key-format full --align 16 --pad --pad-header --boot-record NSPE --header-size 0x400 --slot-size 0xA0000 --security-counter %3           --dependencies "(0,"1.6.0"+0)" %1 %~n1_ota.bin

bin2hex.py --offset 0x0807A000 %~n1_signed.bin %~n1_signed.hex

@exit /B 0

:help
@echo off
echo Sign Image
echo.
echo sign_image.bat ^<name^> ^<version^> ^<counter^>
echo   Input:
echo     ^<name^>:    hex image to be signed
echo     ^<version^>: version string (ex: "1.0.0")
echo     ^<counter^>: security counter (ex: 1)
echo   Output:
echo     ^<name^>_signed.bin: signed binary image (confirmed)
echo     ^<name^>_ota.bin:    signed binary for OTA (not confirmed)
echo     ^<name^>_signed.hex: signed hex image
exit /B 1
