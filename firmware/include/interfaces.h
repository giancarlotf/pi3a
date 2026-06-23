#ifndef INTERFACES
#define INTERFACES 1

#include "pi3a_core.h"

#ifndef LCD_ADDR
#define LCD_ADDR 0x27
#endif

// Pinos do PCF8574
#define LCD_RS 0
#define LCD_RW 1
#define LCD_E 2
#define LCD_BL 3

// HW-061, módulo do PCF8574
// Utilizado para a comunicação TWI (I2C/IIC) com o Display LCD 1602A
void lcd_write(uint8_t data);
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);

#endif