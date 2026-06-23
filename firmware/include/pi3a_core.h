#ifndef PI3A_CORE
#define PI3A_CORE 1

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef struct
{
    uint8_t bit;
    volatile uint8_t *ddr;
    volatile uint8_t *pin;
    volatile uint8_t *port;
} gpio_t;

#include "actuators.h"
#include "interfaces.h"
#include "sensors.h"

// Comentar/Remover antes de compilar
// #include <avr/iom328p.h>

// TWI (I2C/IIC)
void twi_init(void);
void twi_start(void);
void twi_write(uint8_t data);
uint8_t twi_read_ack(void);
uint8_t twi_read_nack(void);
void twi_stop(void);

// USART
void usart_init(unsigned long baud);
void usart_transmit(const char *str);
char usart_receive(void);

// ESP-01S, módulo do ESP8266
void esp_01s(void);

float velocity(uint8_t axis, float last_velocity);
float distance(uint8_t axis, float last_velocity);

#endif