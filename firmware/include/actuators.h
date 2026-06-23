#ifndef ACTUATORS
#define ACTUATORS 1

#include "pi3a_core.h"

// Valores do MG 996R
#define SERVO_MIN 0x07D0
#define SERVO_STOP 0x0BB0
#define SERVO_MAX 0x0FA0

// MG 996R
void set_pwm(void);
void mg_996r(int8_t speed, uint8_t pb_pin);

#endif