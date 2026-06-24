#include <pi3a_core.h>

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
