km = [0] * 256




for ch in range(ord('A'), ord('Z') + 1):
    km[ch] = ch + 32
    km[ch + 32] = ch + 32


km[13] = 13 # ENTER
km[32] = 13 # SPACE => ENTER

# k, m, $, n was mapped to direction buttons, use Fn+k/m/n/j/l or aA+k/m/n/j/l to input original keys
km[ord('k')] = 20 # k => UP
km[ord('m')] = 21 # m => DOWN
km[ord('$')] = 22 # $ => RIGHT
km[ord('n')] = 23 # n => LEFT
km[ord('j')] = 19 # j => PgUp
km[ord('l')] = 14 # l => PgDn
km[ord('u')] = 28 # u => F1
km[ord('i')] = 29 # i => F2
km[ord('o')] = 30 # o => F3
km[ord('p')] = 31 # p => F4

km[182] = ord('j')
km[183] = ord('k')
km[184] = ord('l')
km[192] = ord('m')
km[191] = ord('n')
km[ord('{')] = ord('u')
km[ord('}')] = ord('i')
km[ord('[')] = ord('o')
km[ord(']')] = ord('p')

km[ord('1')] = ord('t')
km[ord('2')] = ord('y')
km[ord('3')] = ord('u')
km[ord('4')] = ord('g')
km[ord('5')] = ord('h')
km[ord('6')] = ord('j')
km[ord('7')] = ord('v')
km[ord('8')] = ord('b')
km[ord('9')] = ord('n')
km[ord('0')] = ord('0')
km[189] = 19 # Fn+V => PgUp
km[190] = 14 # Fn+B => PgDn
km[ord('.')] = ord('.') # Sym+M => .

km[8] = 27 # Backspace => ESC
km[186] = 25 # Fn+Z => Help
km[187] = 26 # Fn+X => Shift
km[188] = 18 # Fn+C => Input Method
km[ord('~')] = 0xFF # Fn+Q => Brightness

print('keyMap: ')
for i in range(0, 256):
    print('%s => 0x%02x' %(repr(chr(i)), km[i]))

cArr = 'uint8_t keyboardKeyMap[] = {'
for i in range(0, 256):
    if i % 16 == 0:
        cArr += '\n'
    cArr += '0x%02x, ' % km[i]

cArr += '};'

with open('keymap_keyboard.h', 'w') as f:
    f.write(cArr)
    