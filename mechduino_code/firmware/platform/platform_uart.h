#ifndef PLATFORM_UART_H
#define PLATFORM_UART_H

#include <stdlib.h>
#include <stdint.h>

typedef enum {
    UART_OK = 0,
    UART_READ_ERROR,
    UART_WRITE_ERROR,
    UART_INVALID_ID,
    UART_TIMEOUT                
} UART_ERROR_CODE;


typedef enum {
    EVEN_PARITY,
    ODD_PARITY
} PARITY;


typedef UART_ID uint8_t;        // Cannot be equal to INVALID_UART_ID
typedef INVALID_UART_ID 0;


/*
    Sets UART on TX_pin and RX_pin with parametres described by baudrate, parity and stop_bits

    Returns: UART_ID (setup is successfull) or INVALID_UART_ID (setup is not successfull)
*/

UART_ID uart_setup(uint8_t TX_pin, uint8_t RX_pin, uint32_t baudrate, PARITY parity, uint8_t stop_bits);


/*
    Unsets uart with described by id

    Returns UART_INVALID_ID or UART_OK
*/

UART_ERROR_CODE unset_uart(UART_ID id);


/*
    Reads one byte from UART described by id and puts it in dest

    Returns UART_INVALID_ID, UART_TIMEOUT, UART_READ_ERROR or UART_OK
*/

UART_ERROR_CODE uart_read_byte (UART_ID id, uint8_t *dest);


/*
    Reads read_len bytes of message from UART described by id and puts it in dest, sets bytes_read to number of actually red bytes
    
    Returns UART_INVALID_ID, UART_TIMEOUT, UART_READ_ERROR or UART_OK
*/

UART_ERROR_CODE uart_read_n_bytes(UART_ID id, uint8_t *dest, uint8_t read_len, uint8_t bytes_read);


/*
    Writes src byte on UART

    Returns UART_INVALID_ID, UART_TIMEOUT, UART_WRITE_ERROR or UART_OK
*/

UART_ERROR_CODE uart_write_byte(UART_ID id, const uint8_t *src);


/*
    Writes write_len bytes from src on UART and puts number of written bytes to bytes_written

    Returns UART_INVALID_ID, UART_TIMEOUT, UART_WRITE_ERROR or UART_OK
*/

UART_ERROR_CODE uart_write_byte(UART_ID id, const uint8_t *src, uint8_t write_len, uint8_t bytes_written);

#endif //PLATFORM_UART_H