// Sensores
#include <sensors.h>

// HC-SR04
int hc_sr04(gpio_t trigger, gpio_t echo)
{
    uint16_t count = 0;

    *trigger.ddr |= (1 << trigger.bit);
    *echo.ddr &= ~(1 << echo.bit);
    *echo.port |= (1 << echo.bit);
    *trigger.port &= ~(1 << trigger.bit);
    delay_us(2);

    *trigger.port |= (1 << trigger.bit);
    delay_us(10);

    *trigger.port &= ~(1 << trigger.bit);

    while (!(*echo.pin & (1 << echo.bit)))
        ;

    while (*echo.pin & (1 << echo.bit))
    {
        count++;
        delay_us(1);
    }

    // Distância = Tempo * Velocidade
    // Velocidade do som = 0.0343 [cm/us] ~= 10/291
    return count * 10 / 291;
}

// HW-123E, módulo do MPU-6050
void mpu6050_write(uint8_t reg, uint8_t data)
{
    TWI_start();
    TWI_write(MPU6050_ADDR << 1);
    TWI_write(reg);
    TWI_write(data);
    TWI_stop();
}

void mpu6050_init(void)
{
    mpu6050_write(PWR_MGMT_1, 0);
    delay_ms(100);

    mpu6050_write(GYRO_CONFIG, FS_SEL);
    for (int i = 0; i < 8; i++)
    {
        delay_ms(250);
    }

    mpu6050_write(ACCEL_CONFIG, AFS_SEL);
    for (int i = 0; i < 8; i++)
    {
        delay_ms(250);
    }
}

void mpu6050_read_all(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    TWI_start();
    TWI_write(MPU6050_ADDR << 1); // write
    TWI_write(ACCEL_XOUT_H);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t data[14];
    for (int i = 0; i < 13; i++)
        data[i] = TWI_read_ack();

    data[13] = TWI_read_nack();
    TWI_stop();

    // Acelerômetro: X(0x3B, 0x3C); Y(0x3D, 0x3E); Z(0x3F, 0x40)
    *ax = (float)((int16_t)((data[0] << 8) | data[1])) / AFS_SENSITIVITY;
    *ay = (float)((int16_t)((data[2] << 8) | data[3])) / AFS_SENSITIVITY;
    *az = (float)((int16_t)((data[4] << 8) | data[5])) / AFS_SENSITIVITY;

    // Giroscópio: X(0x43, 0x44); Y(0x45, 0x46); Z(0x47, 0x48)
    *gx = (float)((int16_t)((data[8] << 8) | data[9])) / FS_SENSITIVITY;
    *gy = (float)((int16_t)((data[10] << 8) | data[11])) / FS_SENSITIVITY;
    *gz = (float)((int16_t)((data[12] << 8) | data[13])) / FS_SENSITIVITY;
}

float mpu6050_read_axis(uint8_t reg, float sensitivity)
{
    TWI_start();
    TWI_write(MPU6050_ADDR << 1); // write
    TWI_write(reg);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t byte_h = TWI_read_ack();
    uint8_t byte_l = TWI_read_nack();
    TWI_stop();

    return (float)((int16_t)((byte_h << 8) | byte_l)) / sensitivity;
}

uint8_t mpu6050_read_reg(uint8_t reg)
{
    TWI_start();
    TWI_write(MPU6050_ADDR << 1); // write
    TWI_write(reg);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 1); // read

    uint8_t data = TWI_read_nack();
    TWI_stop();

    return data;
}