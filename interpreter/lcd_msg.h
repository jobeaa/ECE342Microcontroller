/*   ADAPTED FROM:
//   MSP430FR4133 Demo -  LCD_E, Display "HELLO WORLD" using blink memory.

//  Kathryn Adamsky
//  Texas Instruments Inc.
//  June 2016
*/

#include <msp430.h>

#ifndef LCD_MSG_H_
#define LCD_MSG_H_

void showChar(char c, int position);
void showCharB(char c, int position);
void write_msg(char *s1, char *s2);
void LCD_init(void);


// Define word access definitions to LCD memories
#define LCDMEMW ((int*)LCDMEM)
#define LCDBMEMW ((int*)LCDBMEM)



#endif
