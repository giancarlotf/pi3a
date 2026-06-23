// Atuadores
#include <actuators.h>

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