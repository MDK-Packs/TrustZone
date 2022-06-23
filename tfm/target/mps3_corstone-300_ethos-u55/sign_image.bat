imgtool sign --version "1.6.0" --key ../../../bl2/root-RSA-3072.pem --public-key-format full --align 16 --pad --pad-header --boot-record SPE --header-size 0x400 --slot-size 0x60000 --security-counter 1 --confirm --dependencies "(1,0.0.0+0)" tfm_s.bin tfm_s_signed.bin

bin2hex.py --offset 0x11000000 tfm_s_signed.bin tfm_s_signed.hex
