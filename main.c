#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

#include "twi/twi_master.h"
#include "bno055_definitions.c"

// i2c/twi pins on the 328P
#define TW_SCL_PIN PORTC5
#define TW_SDA_PIN PORTC4

typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} imu_data_t;

// this is based on the twi dev's error catch function
// I added an input to be able to know where the program is failing
//
// hangs the program if the transmission fails
void catch(ret_code_t* error_code, char* str) {
    if (*error_code != SUCCESS) {
        printf("Something went wrong: %s", str);
        while(1);
    }
}

// this exists to make my life easier
// this should make memory usage lower by copying values as little as possible
void imu_transmit(uint8_t * data, uint8_t ADDRESS, uint8_t value, uint8_t delay, char* str, ret_code_t* error_code) {
    data[0] = ADDRESS;
    data[1] = value;
    *error_code = tw_master_transmit(IMU, data, sizeof(data), false);
    catch(error_code, str);
    _delay_ms(delay);
}

void imu_read(uint8_t* data, imu_data_t* imu_data, ret_code_t* error_code) {
    data[0] = EUL_DATA_X_LSB;
    *error_code = tw_master_transmit(IMU, data, 1, true);
    catch(error_code, "IMU_SEND_REQ");

    *error_code = tw_master_receive(IMU, data, 6);
    catch(error_code, "READ_IMU");

    imu_data->x_axis = (int16_t)(data[1] << 8 | data[0]);
    imu_data->y_axis = (int16_t)(data[3] << 8 | data[2]);
    imu_data->z_axis = (int16_t)(data[5] << 8 | data[4]);
}

ret_code_t error_code; // global error code variable to minimize space usage
uint8_t data; // transmission data array
imu_data_t imu_data;

/// enter main power mode, and enter config mode
void imu_init() {
    imu_transmit(&data, PWR_MODE, 0x0, 0, "PWR_INIT", &error_code);
    imu_transmit(&data, OPR_MODE, 0x0, 20, "CONFIG_INIT", &error_code);
    imu_transmit(&data, UNIT_SEL, 0x0, 0, "UNITS_INIT", &error_code);
}


/// enter the device IMU offsets after initializing into config mode
void init_gyro_with_offset(int16_t x_offset, int16_t y_offset, int16_t z_offset) {
    imu_init();
    imu_transmit(&data, GYR_OFFSET_X_LSB, x_offset, 0, "GYRO_OFFSET_X", &error_code);
    imu_transmit(&data, GYR_OFFSET_X_MSB, x_offset >> 8, 0, "GYRO_OFFSET_X", &error_code);
    imu_transmit(&data, GYR_OFFSET_Y_LSB, y_offset, 0, "GYRO_OFFSET_Y", &error_code);
    imu_transmit(&data, GYR_OFFSET_Y_MSB, y_offset >> 8, 0, "GYRO_OFFSET_Y", &error_code);
    imu_transmit(&data, GYR_OFFSET_Z_LSB, z_offset, 0, "GYRO_OFFSET_Z", &error_code);
    imu_transmit(&data, GYR_OFFSET_Z_MSB, z_offset >> 8, 0, "GYRO_OFFSET_Z", &error_code);
}



int main() {
    tw_init(IMU, true);
    imu_init();
    imu_transmit(&data, OPR_MODE, 0x8, 7, "IMU_MODE", &error_code); // set the device into IMU mode to start reading values

    while(1) {
        imu_read(&data, &imu_data, &error_code);
        printf("");
    }

    return 1;
}
