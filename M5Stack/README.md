LavaXVM x M5Stack
===


python /YOUR_IDF_PATH/components/esptool_py/esptool/esptool.py --chip esp32 --port YOUR_COM_PORT --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x00200000 files/LVM.bin

