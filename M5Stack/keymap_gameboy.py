

GB_UP = 0 #1
GB_DOWN = 1 #2
GB_LEFT = 2 #4
GB_RIGHT = 3 #8
GB_A = 4 #16
GB_B = 5 #32
GB_SELECT = 6 #64
GB_START = 7 #128
M5_BtnA = 8
M5_BtnB = 9
M5_BtnC = 10


km = [0] * 256
km[GB_UP] = 20
km[GB_DOWN] = 21
km[GB_RIGHT] = 22
km[GB_LEFT] = 23

km[GB_SELECT] = ord('n') 
km[GB_START] = ord('y')

km[GB_A] = 13 #ENTER
km[GB_B] = 27 #ESC

km[M5_BtnA] = 0xFF #Brightness
km[M5_BtnB] = 25 #Help
km[M5_BtnC] = 28 #F1


cArr = 'uint8_t gameboyKeyMap[] = {'
for i in range(0, 256):
    if i % 16 == 0:
        cArr += '\n'
    cArr += '0x%02x, ' % km[i]

cArr += '};'

with open('keymap_gameboy.h', 'w') as f:
    f.write(cArr)