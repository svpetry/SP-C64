/* 
 * File:   keyboard.h
 * Author: svpet
 *
 * Created on March 2, 2019, 6:17 AM
 */

#ifndef KEYBOARD_H
#define	KEYBOARD_H
    
void InitKeyboard(void);
unsigned char KeybGetScancode(unsigned char *key);
void KeybGetC64Key(void);

unsigned char keyb_shift_lock;
unsigned char restore;
unsigned char c64key;
unsigned char key_break;
unsigned char key_shift;

#endif	/* KEYBOARD_H */

