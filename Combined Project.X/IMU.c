/* 
 * File:   main.c
 * Author: Jesus Ruiz Ramos
 *
 * Created on April 19, 2026, 4:00 PM
 */
#define F_CPU 16000000
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "bno055_definitions.c"
#include <string.h>
#include "IMU.h"

ret_code_t error_code; // global error code variable to minimize space usage
uint8_t data[6]; // transmission data array
imu_data_t imu_data;
imu_data_t offset;

char output_str[64];

void USART_Init(unsigned int ubrr) {
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0) | (3<<UCSZ00);
}

void USART_TransmitChar(unsigned char data) {
    /* wait for a clear buffer */
    while (!(UCSR0A & (1<<UDRE0))) { ; }
    /* send data */
    UDR0 = data;
}

void USART_Transmit(unsigned char * str) {
    for (size_t i = 0; i < strlen(str); i++) {
        USART_TransmitChar(str[i]);
    }
}

// this is based on the twi dev's error catch function
// I added an input to be able to know where the program is failing
//
// hangs the program if the transmission fails
void catch(ret_code_t* error_code, char* str) {
    if (*error_code != SUCCESS) {
        USART_Transmit(str);
        while(1);
    }
}

// this exists to make my life easier
// this should make memory usage lower by copying values as little as possible
void imu_transmit(uint8_t * data, uint8_t ADDRESS, uint8_t value, char* str, ret_code_t* error_code) {
    data[0] = ADDRESS;
    data[1] = value;
    _delay_ms(10);
    *error_code = tw_master_transmit(IMU, data, 2, false);
    catch(error_code, str);
}

void imu_start_config(uint8_t* data, ret_code_t* error_code) {
    data[0] = OPR_MODE;
    data[1] = 0x0;
    *error_code = tw_master_transmit(IMU, data, 2, false);
    catch(error_code, "START_CONFIG");
    _delay_ms(20);
}

void imu_start_imu(uint8_t* data, ret_code_t* error_code) {
    data[0] = OPR_MODE;
    data[1] = 0x8;
    *error_code = tw_master_transmit(IMU, data, 2, false);
    catch(error_code, "START_IMU");
    _delay_ms(7);
}

void imu_read(uint8_t* data, imu_data_t* imu_data, ret_code_t* error_code) {
    data[0] = EUL_DATA_X_LSB;
    *error_code = tw_master_transmit(IMU, data, 1, false);
    catch(error_code, "IMU_SEND_REQ");

    *error_code = tw_master_receive(IMU, data, 6);
    catch(error_code, "READ_IMU");

    imu_data->x_axis = (int16_t)(data[1] << 8 | data[0]);
    imu_data->y_axis = (int16_t)(data[3] << 8 | data[2]);
    imu_data->z_axis = (int16_t)(data[5] << 8 | data[4]);
}

/// enter main power mode, and enter config mode
void imu_init() {
    imu_transmit(data, PWR_MODE, 0x0, "PWR_INIT", &error_code);
    imu_start_config(data, &error_code);
    imu_transmit(data, UNIT_SEL, 0x0, "UNITS_INIT", &error_code);
}

void imu_startup(){
    _delay_ms(1000);
    USART_Init(MYUBRR);
    USART_Transmit("Starting up...\n\r");
    tw_init(TW_FREQ_400K, true);
    imu_start_config(data, &error_code);
    imu_init();
    imu_start_imu(data, &error_code);
    _delay_ms(50);
}

void imu_zero(){
    USART_Transmit("Getting new zero...\r\n");
    
    uint8_t i = 0;
    while(i<5) {
        imu_read(data, &imu_data, &error_code);
        i++;
    }
    //imu_data_t offset;
    offset.x_axis = imu_data.x_axis*-1;
    offset.y_axis = imu_data.y_axis*-1;
    offset.z_axis = imu_data.z_axis*-1;
    
    snprintf(output_str, sizeof(output_str), "Zero Reading: X: %d Y: %d Z: %d\r\n", offset.x_axis, offset.y_axis, offset.z_axis);
    USART_Transmit((unsigned char*)output_str);
}

void imu_probe(){
    imu_read(data, &imu_data, &error_code);

    snprintf(output_str, sizeof(output_str), "X: %d Y: %d Z: %d\r\n", 
        imu_data.x_axis+offset.x_axis, imu_data.y_axis+offset.y_axis, imu_data.z_axis+offset.z_axis);
    USART_Transmit((unsigned char*)output_str);
}

/*int main() {
    _delay_ms(1000);
    USART_Init(MYUBRR);
    USART_Transmit("Starting up...\n\r");
    tw_init(TW_FREQ_400K, true);
    imu_init();
    imu_start_imu(data, &error_code);
    _delay_ms(50);

    USART_Transmit("Getting new zero...\r\n");

    uint8_t i = 0;
    while(i<5) {
        imu_read(data, &imu_data, &error_code);
        i++;
    }
    //imu_data_t offset;
    offset.x_axis = imu_data.x_axis*-1;
    offset.y_axis = imu_data.y_axis*-1;
    offset.z_axis = imu_data.z_axis*-1;
    
    snprintf(output_str, sizeof(output_str), "Zero Reading: X: %d Y: %d Z: %d\r\n", offset.x_axis, offset.y_axis, offset.z_axis);
    USART_Transmit((unsigned char*)output_str);
    
    USART_Transmit("Reading...\r\n");
    while(1) {
        imu_read(data, &imu_data, &error_code);
        
        snprintf(output_str, sizeof(output_str), "X: %d Y: %d Z: %d\r\n", 
             imu_data.x_axis+offset.x_axis, imu_data.y_axis+offset.y_axis, imu_data.z_axis+offset.z_axis);
        USART_Transmit((unsigned char*)output_str);
        _delay_ms(100);
    }
    
    USART_Transmit("Exiting Loop: Something Went Wrong...");

    return 1;
}*/
