/*
 * File:   main.c
 * Author: svpet
 *
 * C64 Controller
 * Clock: 40 MHz ext. = 10 MHz int.
 *
 * pin layout:
 * RA0: KEYB_DATA (input)
 * RA1: KEYB_CLOCK (input/output)
 * RA2: TOD_50Hz
 * RA3: /NMI
 * RA4: /RESET
 * RA5: /MCLR
 * RA6:
 * RA7: CLKI
 * RB0-RB2: AX0-AX2 (address for COL/PA)
 * RB3-RB5: AY0-AY2 (address for ROW/PB)
 * RB6: /STROBE
 * RB7: SW_DATA
 * 
 * Created on 28. Februar 2019, 21:16
 */

// CONFIG1H
#pragma config OSC = ECIO       // Oscillator Selection bits (EC oscillator, port function on RA6)
#pragma config FSCM = OFF       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bit (Brown-out Reset disabled)
// BORV = No Setting

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled, RA5 input pin disabled)

// CONFIG4L
#pragma config STVR = OFF       // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Low-Voltage ICSP Enable bit (Low-Voltage ICSP disabled)

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (00200-0007FFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (000800-000FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot Block (000000-0001FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (00200-0007FFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (000800-000FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0001FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (00200-0007FFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (000800-000FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0001FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 40000000

#include <xc.h>
#include <pic18f1220.h>
#include <plib/xlcd.h>

#define RS PORTBbits.RB2
#define EN PORTBbits.RB3
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB5
#define D6 PORTBbits.RB6
#define D7 PORTBbits.RB7

#include "lcd.h"
#include "keyboard.h"

#define TIMER_HI 0xCF; // 40 MHz
#define TIMER_LO 0x2C; // 40 MHz

unsigned char shiftlock_active;

void __interrupt () Isr () {
    TMR0H = TIMER_HI; // interrupt every 10 ms
    TMR0L = TIMER_LO;

    if (LATAbits.LA2 == 0)
        LATAbits.LA2 = 1;
    else
        LATAbits.LA2 = 0;

    INTCONbits.TMR0IF = 0;
}

void Initialize() {
    
    ADCON0 = 0; // disable A/D converter
    ADCON1 = 0x0F;

    INTCON = 0b10100000; // Enable Timer0 interrupt, disable peripheral interrupts
    INTCON2 = 0b10000000; // disable PORTB pull-up
    INTCON3 = 0;
    PIE1 = 0;
    PIE2 = 0;
    IPR1 = 0;
    IPR2 = 0;
    RCON = 0;
    CCP1CON = 0;

    T0CON = 0b00000010; // Timer0: prescaler 8, 16 bit, disabled
    TMR0H = TIMER_HI; // interrupt every 10 ms
    TMR0L = TIMER_LO;

    TRISA = 0b00011001;
    LATA = 0b00000000;
    TRISB = 0b00000000;
    LATB = 0;

    // Init PS/2 keyboard
    InitKeyboard();
}

void SetC64Key(unsigned char key, unsigned char state) {
   unsigned char col, row, swaddr;
   
   col = key & 0x07;
   row = (key & 0x70) >> 4;
   swaddr = col + (row << 3);
   LATB = swaddr | 0b01000000;
   if (state > 0)
       LATBbits.LB7 = 1;
   LATBbits.LB6 = 0;
   LATBbits.LB6 = 1;
}

void MainLoop() {
    const unsigned char C64SpaceKey = 0x74;
    const unsigned char C64LeftShiftKey = 0x17;
    
    unsigned char data;
    
    shiftlock_active = 0;
    while (1) {
        KeybGetC64Key();
        
        if (keyb_shift_lock == 1) {
            if (c64key == C64LeftShiftKey)
                c64key = 0xFF;
            if (shiftlock_active == 0) {
                SetC64Key(C64LeftShiftKey, 1);
                shiftlock_active = 1;
            }
        }
        else if (shiftlock_active == 1) {
            SetC64Key(C64LeftShiftKey, 0);
            shiftlock_active = 0;
        }
        
        if (key_shift == 1 && shiftlock_active == 0) {
            if (key_break == 1)
                data = 0;
            else
                data = 1;
            SetC64Key(C64LeftShiftKey, data);
        }
        
        if (c64key != 0xFF) {
            if (key_break == 1)
                data = 0;
            else
                data = 1;
            SetC64Key(c64key, data);
        }
        
        if (restore == 1) {
            LATAbits.LA3 = 0;
            __delay_us(10);
            LATAbits.LA3 = 1;
            restore = 0;
        }
    }
}

void main() {
    
    Initialize();
    
    // reset CPU
    LATAbits.LA4 = 0;
    TRISAbits.RA4 = 0;
    __delay_ms(500);
    TRISAbits.RA4 = 1;

    T0CONbits.TMR0ON = 1; // Enable Timer0

    while (1) {
        LATAbits.LA6 = 0;
        __delay_ms(500);
        LATAbits.LA6 = 1;
        __delay_ms(500);
    }
    
    MainLoop();
    
    return;
}
