
#include <xc.h>
#include <pic18f1220.h>

#define _XTAL_FREQ 40000000

#include "keyboard.h"

#define SHIFT_COL 1
#define SHIFT_ROW 3

void InitKeyboard(void) {
    keyb_shift_lock = 0;
    restore = 0;
    key_break = 0;
    key_shift = 0;
}

unsigned char KeybGetScancode(unsigned char *key) {
    unsigned short i;
    unsigned char result;
    
    TRISAbits.RA1 = 1;
    __delay_us(50);
    
    *key = 0;
    
    // read start bit
    i = 0;
    while (PORTAbits.RA1 == 1 && i < 20) {
        __delay_us(10);
        i++;
    }
    
    if (PORTAbits.RA1 == 0) {
        while (PORTAbits.RA1 == 0) ;
        
        // read data bits
        for (i = 0; i < 8; i++) {
            while (PORTAbits.RA1 == 1) ;
            *key >>= 1;
            if (PORTAbits.RA0 == 1)
                *key += 0x80;
            while (PORTAbits.RA1 == 0) ;
        }
        
        // read parity bit
        while (PORTAbits.RA1 == 1) ;
        while (PORTAbits.RA1 == 0) ;
        
        // read stop bit
        while (PORTAbits.RA1 == 1) ;
        while (PORTAbits.RA1 == 0) ;
        
        result = 1;
    }
    else
        result = 0;
    
    LATAbits.LA1 = 0; // pull clock line to 0
    TRISAbits.RA1 = 0; // clock line = output
    return result;
}

void KeybGetC64Key(void) {
    unsigned char scode;
    unsigned char special;
    
    c64key = 255;
    while (c64key == 255) {
        
        if (KeybGetScancode(&scode) == 0)
            return;
        
        // special key
        if (scode == 0xE0) {
            special = 1;
            KeybGetScancode(&scode);
        }
        else
            special = 0;
        
        // break code
        if (scode == 0x0F) {
            key_break = 1;
            KeybGetScancode(&scode);
        }
        else
            key_break = 0;
        
        if (special == 1) {
            switch (scode) {
                case 0x12: // RESTORE (Pause/Break)
                    if (key_break == 1) {
                        KeybGetScancode(&scode);
                        KeybGetScancode(&scode);
                    }
                    else {
                        restore = 1;
                        return;
                    }
                    break;
                case 0x6C:
                    c64key = 0x63; // CLR/HOME
                    break;
                case 0x75: // UP
                    c64key = 0x07;
                    key_shift = 1;
                    break;
                case 0x6B: // LEFT
                    c64key = 0x02;
                    key_shift = 1;
                    break;
                case 0x72: // DOWN
                    c64key = 0x07;
                    break;
                 case 0x74: // RIGHT
                     c64key = 0x02;
                     break;
            }
        } 
        else {
            switch (scode) {
                case 0x1C: // A
                    c64key = 0x12;
                    break;
                case 0x32: // B
                    c64key = 0x34;
                    break;
                case 0x21: // C
                    c64key = 0x24;
                    break;
                case 0x23: // D
                    c64key = 0x22;
                    break;
                case 0x24: // E
                    c64key = 0x16;
                     break;
                case 0x2B: // F
                    c64key = 0x25;
                    break;
                case 0x34: // G
                    c64key = 0x32;
                    break;
                case 0x33: // H
                    c64key = 0x35;
                    break;
                case 0x43: // I
                    c64key = 0x41;
                    break;
                case 0x3B: // J
                    c64key = 0x42;
                    break;
                case 0x42: // K
                    c64key = 0x45;
                    break;
                case 0x4B: // L
                    c64key = 0x53;
                    break;
                case 0x3A: // M
                    c64key = 0x44;
                    break;
                case 0x31: // N
                    c64key = 0x47;
                    break;
                case 0x44: // O
                    c64key = 0x46;
                    break;
                case 0x4D: // P
                    c64key = 0x51;
                    break;
                case 0x15: // Q
                    c64key = 0x76;
                    break;
                case 0x2D: // R
                    c64key = 0x21;
                    break;
                case 0x1B: // S
                    c64key = 0x15;
                    break;
                case 0x2C: // T
                    c64key = 0x26;
                    break;
                case 0x3C: // U
                    c64key = 0x36;
                    break;
                case 0x2A: // V
                    c64key = 0x37;
                    break;
                case 0x1D: // W
                    c64key = 0x11;
                    break;
                case 0x22: // X
                    c64key = 0x27;
                    break;
                case 0x1A: // Y
                    c64key = 0x31;
                    break;
                case 0x35: // Z
                    c64key = 0x14;
                    break;
                case 0x45: // 0
                    c64key = 0x43;
                    break;
                case 0x16: // 1
                    c64key = 0x70;
                    break;
                case 0x1E: // 2
                    c64key = 0x73;
                    break;
                case 0x26: // 3
                    c64key = 0x10;
                    break;
                case 0x25: // 4
                    c64key = 0x13;
                    break;
                case 0x2E: // 5
                    c64key = 0x20;
                    break;
                case 0x36: // 6
                    c64key = 0x23;
                    break;
                case 0x3D: // 7
                    c64key = 0x30;
                    break;
                case 0x3E: // 8
                    c64key = 0x33;
                    break;
                case 0x46: // 9
                    c64key = 0x40;
                    break;
                case 0x4E: // -
                    c64key = 0x53;
                    break;
                case 0x55: // =
                    c64key = 0x65;
                    break;
                case 0x66: // DEL (Backspace)
                    c64key = 0x00;
                    break;
                case 0x0E: // <-
                    c64key = 0x71;
                    break;
                case 0x5A: // RETURN
                    c64key = 0x01;
                    break;
                case 0x76: // RUN STOP (Escape)
                    c64key = 0x77;
                    break;
                case 0x54: // @
                    c64key = 0x56;
                    break;
                case 0x5B: // *
                    c64key = 0x61;
                    break;
                case 0x4C: // [ :
                    c64key = 0x55;
                    break;
                case 0x52: // ] ;
                    c64key = 0x62;
                    break;
                case 0x41: // ,
                    c64key = 0x57;
                    break;
                case 0x49: // .
                    c64key = 0x54;
                    break;
                case 0x4A: // /
                    c64key = 0x67;
                    break;

                case 0x05: // F1
                    c64key = 0x04;
                    break;
                case 0x06: // F2
                    c64key = 0x04;
                    key_shift = 1;
                    break;
                case 0x04: // F3
                    c64key = 0x05;
                    break;
                case 0x0C: // F4
                    c64key = 0x05;
                    key_shift = 1;
                    break;
                case 0x03: // F5
                    c64key = 0x06;
                    break;
                case 0x0B: // F6
                    c64key = 0x06;
                    key_shift = 1;
                    break;
                case 0x38:
                    c64key = 0x03; // F7
                    break;
                case 0x0A: // F8
                    c64key = 0x03;
                    key_shift = 1;
                    break;
                case 0x11: // C= (ALT)
                    c64key = 0x75;
                    break;
                case 0x01: // £ (F9)
                    c64key = 0x60;
                    break;
                case 0x09: // + (F10)
                    c64key = 0x50;
                    break;
                case 0x29: // SPACE
                    c64key = 0x74;
                    break;

                case 0x12: // left shift
                    c64key = 0x17;
                    break;
                case 0x59: // right shift
                    c64key = 0x64;
                    break;
                case 0x14: // CTRL (CTRL left, TAB)
                case 0x0D:
                    c64key = 0x72;
                    break;
                case 0x00: // ARROW UP (# on PS2 keyboard)
                    c64key = 0x66;
                    break;
                case 0x58:
                    if (key_break == 0)
                        keyb_shift_lock = 1 - keyb_shift_lock;
                    break;
            }
        }
    }
}