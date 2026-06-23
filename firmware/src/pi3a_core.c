#include <pi3a_core.h>
/*
Substituir:
uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)))
    {
        if (--timeout == 0)
            return 1;
    }
Por um WDT
*/

// TWI (I2C/IIC)
void twi_init(void) // TWBR
{
    TWSR = 0;
    TWBR = 0x48; // 0x0C // 0x48;
    TWCR = (1 << TWEN);
}

void twi_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)))
    {
        if (--timeout == 0)
            return;
    }
}

void twi_write(uint8_t data)
{
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN);

    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)))
    {
        if (--timeout == 0)
            return;
    }
}

uint8_t twi_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)))
    {
        if (--timeout == 0)
            return 1;
    }

    return TWDR;
}

uint8_t twi_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);

    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)))
    {
        if (--timeout == 0)
            return 1;
    }

    return TWDR;
}

void twi_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// USART
void usart_init(unsigned long baud)
{
    unsigned int ubrr = (F_CPU / (16 * baud)) - 1;

    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

char usart_receive(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void usart_transmit(const char *str)
{
    while (*str)
    {
        while (!(UCSR0A & (1 << UDRE0)))
            ;
        UDR0 = *str;
        str++;
    }
}

// Microcontrolador
// ESP-01S, módulo do ESP8266
void esp_01s(void)
{
    // Test AT+<x>=?
    // Query AT+<x>?
    // Set AT+<x>=<...>
    // Execute AT+<x>

    // AT Test AT startup
    // AT+RST Restart module
    // AT+GSLP Deep-sleep mode
    // ATE AT commands echo or not
    // AT+SLEEP Sleep mode
}

float velocity(uint8_t axis, float last_velocity)
{
    float delta_time = 0; // time_count();
    float v_n = mpu6050_read_axis(axis, AFS_SENSITIVITY) * delta_time + last_velocity;
    return v_n;
}

float distance(uint8_t axis, float last_velocity)
{
    float delta_time = 0; // time_count();
    float d_n = (last_velocity + (mpu6050_read_axis(axis, AFS_SENSITIVITY) * delta_time) / 2) * delta_time;
    return d_n;
}
