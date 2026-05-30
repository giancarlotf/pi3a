#ifndef PI3A_CORE
#define PI3A_CORE 1

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define LCD_ADDR 0x27
#define MPU6050_ADDR 0x68

// Valores do MG 996R
#define SERVO_MIN 0x07D0
#define SERVO_STOP 0x0BB0
#define SERVO_MAX 0x0FA0

// Registradores do MPU-6050
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H 0x43
#define GYRO_YOUT_H 0x45
#define GYRO_ZOUT_H 0x47
#define PWR_MGMT_1 0x6B

// Configurações do MPU-6050
#define AFS_SEL 0x18         // +/- 16g
#define AFS_SENSITIVITY 2048 // BIT Range / Full Scale Range
#define FS_SEL 0x18          // +/- 2000°/s
#define FS_SENSITIVITY 16.4f // BIT Range / Full Scale Range

// Pinos do PCF8574
#define LCD_RS 0
#define LCD_RW 1
#define LCD_E 2
#define LCD_BL 3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Comentar/Remover antes de compilar
// #include <avr/iom328p.h>

typedef struct
{
    uint8_t bit;
    volatile uint8_t *ddr;
    volatile uint8_t *pin;
    volatile uint8_t *port;
} gpio_t;

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

// HC-SR04
int hc_sr04(gpio_t trigger, gpio_t echo);

// HW-123E, módulo do MPU-6050
void mpu6050_write(uint8_t reg, uint8_t data);
void mpu6050_init(void);
void mpu6050_read_all(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);
uint8_t mpu6050_read_reg(uint8_t reg);
float velocity(uint8_t axis, float last_velocity);
float distance(uint8_t axis, float last_velocity);

// MG 996R
void set_pwm(void);
void mg_996r(int8_t speed, uint8_t pb_pin);

// HW-061, módulo do PCF8574
// Utilizado para a comunicação TWI (I2C/IIC) com o Display LCD 1602A
void lcd_write(uint8_t data);
void lcd_init(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);

#endif
