// Interfaces de comunicação
#include <interfaces.h>

// HW-061, módulo do PCF8574
// Utilizado para a comunicação TWI (I2C/IIC) com o Display LCD 1602A
void lcd_write(uint8_t data)
{
    twi_start();
    twi_write(LCD_ADDR << 1);
    twi_write(data);
    twi_stop();
}

void lcd_pulse(uint8_t data)
{
    lcd_write(data | (1 << LCD_E));
    _delay_us(1);

    lcd_write(data & ~(1 << LCD_E)); // Escrita realizada na descida do sinal
    _delay_us(50);
}

void lcd_send_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (nibble << 4);

    if (rs)
        data |= (1 << LCD_RS);

    data |= (1 << LCD_BL);
    lcd_pulse(data);
}

void lcd_send_byte(uint8_t byte, uint8_t rs)
{
    lcd_send_nibble(byte >> 4, rs);
    lcd_send_nibble(byte & 0x0F, rs);
}

void lcd_init(void)
{
    _delay_ms(20);

    lcd_send_nibble(0x03, 0); // DL: 8 bit
    _delay_ms(5);
    lcd_send_nibble(0x03, 0); // DL: 8 bit
    _delay_us(100);
    lcd_send_nibble(0x03, 0); // DL: 8 bit

    lcd_send_nibble(0x02, 0); // DL: 4 bit

    lcd_send_byte(0x28, 0); // N: 2 linhas, F: 5x8
    lcd_send_byte(0x0C, 0); // D: ON
    lcd_send_byte(0x06, 0); // Define cursor
    lcd_send_byte(0x01, 0); // Clear

    _delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    if (row > 1)
        row = 1;
    if (col > 15)
        col = 15;

    uint8_t addr = (row == 0) ? 0 : 0x40; // Linha 1: 0x0n; Linha 2: 0x4n
    addr += col;
    lcd_send_byte(0x80 | addr, 0);
}

void lcd_print(const char *str)
{
    while (*str)
    {
        lcd_send_byte(*str++, 1); // Seleciona registrador de dados
    }
}