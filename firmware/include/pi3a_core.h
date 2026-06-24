#ifndef PI3A_CORE
#define PI3A_CORE 1

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
// #include <util/delay.h>

// TWI (I2C/IIC)
extern void TWI_init(void);
extern void TWI_start(void);
extern void TWI_write(uint8_t data);
extern uint8_t TWI_read_ack(void);
extern void TWI_stop(void);

// USART
extern void USART_init(uint16_t baud);
extern void USART_transmit(uint8_t data);
extern char USART_receive(void);
extern void USART_flush(void);

// Atraso
extern void delay_us(uint8_t);
extern void delay_ms(uint8_t);

// Cronômetro
extern void time_start(void);
extern uint8_t time_count(void);
extern void time_stop(void);

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

// ESP-01S, módulo do ESP8266
void esp_01s(void);

float velocity(uint8_t axis, float last_velocity);
float distance(uint8_t axis, float last_velocity);

#endif