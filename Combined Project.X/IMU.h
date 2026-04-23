#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#endif

#define F_CPU 16000000UL

// i2c/twi pins on the 328P
#define TW_SCL_PIN PORTC5
#define TW_SDA_PIN PORTC4

#include <xc.h>
#include "twi_master.h"
#include <util/delay.h>

//Function and Struct Definitions
typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    int16_t z_axis;
} imu_data_t;

void USART_Init(unsigned int);
void USART_TransmitChar(unsigned char);
void USART_Transmit(unsigned char*);
void catch(ret_code_t*, char*);
void imu_transmit(uint8_t*, uint8_t, uint8_t, char*, ret_code_t*);
void imu_start_config(uint8_t*, ret_code_t*);
void imu_start_imu(uint8_t*, ret_code_t*);
void imu_read(uint8_t*, imu_data_t*, ret_code_t*);
void imu_init();
void imu_startup();
void imu_zero();
void imu_probe();

//Global Variables
extern ret_code_t error_code; // global error code variable to minimize space usage
extern uint8_t data[6]; // transmission data array
extern imu_data_t imu_data;
extern imu_data_t offset;

extern char output_str[64];