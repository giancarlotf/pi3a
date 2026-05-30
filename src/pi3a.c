#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pi3a_core.h>

const char MSG_SUCCESS[26] = "All's right in  the code!";

void teste_mpu(const char **test_msg)
{
    *test_msg = MSG_SUCCESS;

    twi_start();
    if ((TWSR & 0xF8) != 0x08)
    {
        *test_msg = "START Failed";
        return;
    }

    twi_write(MPU6050_ADDR << 1);
    if ((TWSR & 0xF8) != 0x18)
    {
        *test_msg = "ADDR Not found";
        twi_stop();
        return;
    }

    twi_write(PWR_MGMT_1);
    if ((TWSR & 0xF8) != 0x28)
    {
        *test_msg = "REG Not found";
        twi_stop();
        return;
    }

    twi_write(0);
    if ((TWSR & 0xF8) != 0x28)
    {
        *test_msg = "Can't write DATA";
        twi_stop();
        return;
    }

    twi_stop();
}

// Estrutura para armazenar os dados do ESP-01S
typedef struct
{
    uint16_t fileiras;
    float comprimento;
    int16_t esp_fileiras;
    float largura_max;
    uint16_t esp_sementes;
    uint8_t lado;   // 1 = dir, 2 = esq
    uint8_t enable; // Flag de controle
} configuracao_t;

configuracao_t robo_cfg = {0};

void serial_esp(void)
{
    static char buffer_rx[64];
    static uint8_t idx = 0;

    while (UCSR0A & (1 << RXC0))
    {
        char c = UDR0;

        if (c == '\n')
        {
            buffer_rx[idx] = '\0';
            idx = 0;

            if (strncmp(buffer_rx, "MSG:", 4) == 0)
            {
                lcd_set_cursor(0, 0);
                lcd_print("                ");
                lcd_set_cursor(0, 0);
                lcd_print(&buffer_rx[4]);
            }
            else if (strncmp(buffer_rx, "IP:", 3) == 0)
            {
                lcd_set_cursor(1, 0);
                lcd_print("                ");
                lcd_set_cursor(1, 0);
                lcd_print(buffer_rx);
            }
            else
            {
                char dir_str[5];
                int parseados = sscanf(buffer_rx, "%d,%f,%d,%f,%d,%s",
                                       &robo_cfg.fileiras,
                                       &robo_cfg.comprimento,
                                       &robo_cfg.esp_fileiras,
                                       &robo_cfg.largura_max,
                                       &robo_cfg.esp_sementes,
                                       dir_str);

                if (parseados >= 4) // Número mínimo de dados
                {
                    robo_cfg.lado = (strcmp(dir_str, "dir") == 0) ? 1 : 2;
                    robo_cfg.enable = 1;
                }
            }
        }
        else if (idx < 63) // Evita estouro de buffer
        {
            buffer_rx[idx++] = c;
        }
    }
}

void controle(float *x_traveled, float *y_traveled, float *x_velocity, float *y_velocity)
{
    if (robo_cfg.fileiras > 0 || *x_traveled <= robo_cfg.largura_max)
    {
        *x_traveled += distance(ACCEL_XOUT_H, *x_velocity);
        *y_traveled += distance(ACCEL_YOUT_H, *y_velocity);

        if (robo_cfg.comprimento - *y_traveled < 0.10)
        {
            // Falta implementar: para();
            if (robo_cfg.largura_max && *x_traveled < robo_cfg.largura_max)
            {
                // Falta implementar: vira(robo_cfg.lado, robo_cfg.esp_fileiras);
            }

            *y_traveled = 0;
            *x_velocity = 0;
            *y_velocity = 0;
            robo_cfg.fileiras--;
        }
    }
    else
    {
        robo_cfg.enable = 0;
    }
}

int main(void)
{
    gpio_t trig;
    trig.bit = 6;
    trig.ddr = &DDRD;
    trig.pin = &PIND;
    trig.port = &PORTD;

    gpio_t echo;
    echo.bit = 7;
    echo.ddr = &DDRD;
    echo.pin = &PIND;
    echo.port = &PORTD;

    uint8_t motor_r = 1;
    uint8_t motor_l = 2;

    int dist_flag = 0;

    float ax, ay, az;
    float gx, gy, gz;

    float x_traveled = 0;
    float y_traveled = 0;
    float x_velocity = 0;
    float y_velocity = 0;

    usart_init(9600);
    twi_init();
    _delay_ms(100);

    mpu6050_init();
    _delay_us(100);

    const char *test_msg;
    teste_mpu(&test_msg);

    lcd_init();
    _delay_ms(10);

    lcd_set_cursor(0, 0);
    lcd_print(test_msg);
    if (test_msg == MSG_SUCCESS)
    {
        lcd_set_cursor(1, 0);
        lcd_print(&test_msg[16]);
    }
    _delay_ms(3000);

    while (1)
    {
        if (!robo_cfg.enable)
        {
            checar_serial_esp();
        }
        else
        {
            controle(&x_traveled, &y_traveled, &x_velocity, &y_velocity);
            mpu6050_read_all(&ax, &ay, &az, &gx, &gy, &gz);
            _delay_ms(10);
            char accel_str[50];
            sprintf(accel_str, "%d %d", (int)ax, (int)ay);

            lcd_set_cursor(1, 0);
            lcd_print(accel_str);
            lcd_print("                ");
            _delay_ms(10);

            int dist = hc_sr04(trig, echo);
            _delay_ms(10);

            if (dist <= 30)
            {
                char dist_str[8];
                sprintf(dist_str, "%d", dist);

                lcd_set_cursor(0, 0);
                lcd_print("Obst: ");
                lcd_set_cursor(0, 6);
                lcd_print(dist_str);

                if (dist <= 15)
                {
                    lcd_set_cursor(0, 15);
                    lcd_print(" cm    *");
                }
                else
                {
                    lcd_print(" cm     ");
                }

                dist_flag = 1;
            }

            if (dist >= 35 && dist_flag)
            {
                lcd_set_cursor(0, 0);
                lcd_print("                ");
                dist_flag = 0;
            }
        }
    }

    return 1;
}
