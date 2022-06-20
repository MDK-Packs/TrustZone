imgtool sign --version "1.6.0" --key ../../../bl2/root-RSA-3072.pem --public-key-format full --align 16 --pad --pad-header --boot-record SPE --header-size 0x400 --slot-size 0x40000 --security-counter 1 --confirm --dependencies "(1,0.0.0+0)" b_u585i_iot02a_tfm_s.bin b_u585i_iot02a_tfm_s_signed.bin

bin2hex.py --offset 0xC03A000 b_u585i_iot02a_tfm_s_signed.bin b_u585i_iot02a_tfm_s_signed.hex
