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

// Sensores
// HC-SR04
int hc_sr04(gpio_t trigger, gpio_t echo)
{
    uint16_t count = 0;

    *trigger.ddr |= (1 << trigger.bit);
    *echo.ddr &= ~(1 << echo.bit);
    *echo.port |= (1 << echo.bit);
    *trigger.port &= ~(1 << trigger.bit);
    _delay_us(2);

    *trigger.port |= (1 << trigger.bit);
    _delay_us(10);

    *trigger.port &= ~(1 << trigger.bit);

    // Adicionar um WDT
    while (!(*echo.pin & (1 << echo.bit)))
        ;

    while (*echo.pin & (1 << echo.bit))
    {
        count++;
        _delay_us(1);
    }

    // Distância = Tempo * Velocidade
    // Velocidade do som = 0.0343 [cm/us] ~= 10/291
    return count * 10 / 291;
}

// HW-123E, módulo do MPU-6050
void mpu6050_write(uint8_t reg, uint8_t data)
{
    twi_start();
    twi_write(MPU6050_ADDR << 1);
    twi_write(reg);
    twi_write(data);
    twi_stop();
}

void mpu6050_init(void)
{
    mpu6050_write(PWR_MGMT_1, 0);
    _delay_ms(100);

    mpu6050_write(GYRO_CONFIG, FS_SEL);
    _delay_ms(2000);

    mpu6050_write(ACCEL_CONFIG, AFS_SEL);
    _delay_ms(2000);
}

void mpu6050_read_all(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    twi_start();
    twi_write(MPU6050_ADDR << 1); // write
    twi_write(ACCEL_XOUT_H);
    twi_start();
    twi_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t data[14];
    for (int i = 0; i < 13; i++)
        data[i] = twi_read_ack();

    data[13] = twi_read_nack();
    twi_stop();

    // Acelerômetro: X(0x3B, 0x3C); Y(0x3D, 0x3E); Z(0x3F, 0x40)
    *ax = ((int16_t)((data[0] << 8) | data[1])) / AFS_SENSITIVITY;
    *ay = ((int16_t)((data[2] << 8) | data[3])) / AFS_SENSITIVITY;
    *az = ((int16_t)((data[4] << 8) | data[5])) / AFS_SENSITIVITY;

    // Giroscópio: X(0x43, 0x44); Y(0x45, 0x46); Z(0x47, 0x48)
    *gx = ((int16_t)((data[8] << 8) | data[9])) / FS_SENSITIVITY;
    *gy = ((int16_t)((data[10] << 8) | data[11])) / FS_SENSITIVITY;
    *gz = ((int16_t)((data[12] << 8) | data[13])) / FS_SENSITIVITY;
}

float mpu6050_read_axis(uint8_t reg, float sensitivity)
{
    twi_start();
    twi_write(MPU6050_ADDR << 1); // write
    twi_write(reg);
    twi_start();
    twi_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t byte_h = twi_read_ack();
    uint8_t byte_l = twi_read_nack();
    twi_stop();

    return (float)((int16_t)(byte_h << 8) | byte_l) / sensitivity;
}

uint8_t mpu6050_read_reg(uint8_t reg)
{
    twi_start();
    twi_write(MPU6050_ADDR << 1); // write
    twi_write(reg);
    twi_start();
    twi_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t data = twi_read_nack();
    twi_stop();

    return data;
}

float velocity(uint8_t axis, float last_velocity)
{
    float delta_time = time_count();
    float v_n = mpu6050_read_axis(axis, AFS_SENSITIVITY) * delta_time + last_velocity;
    return v_n;
}

float distance(uint8_t axis, float last_velocity)
{
    float delta_time = time_count();
    float d_n = (last_velocity + (mpu6050_read_axis(axis, AFS_SENSITIVITY) * delta_time) / 2) * delta_time;
    return d_n;
}

// Atuadores
// MG 996R
void set_pwm(void)
{
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    ICR1 = 0x09C3F; // Frequência = 50hz
                    // (F_CPU / (8 * frequência)) - 1 = 39999
}

void mg_996r(int8_t speed, uint8_t pb_pin)
{
    if (speed > 100)
        speed = 100;
    if (speed < -100)
        speed = -100;

    int16_t pulse = SERVO_STOP + (speed * 10);
    if (pulse < SERVO_MIN)
        pulse = SERVO_MIN;
    if (pulse > SERVO_MAX)
        pulse = SERVO_MAX;

    DDRB |= (1 << pb_pin);
    if (pb_pin == 1)
        OCR1A = pulse;
    if (pb_pin == 2)
        OCR1B = pulse;
}

// Interfaces de comunicação
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
