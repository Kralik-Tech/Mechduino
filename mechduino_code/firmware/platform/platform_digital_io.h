#ifndef PLATFORM_DIGITAL_IO_H
#define PLATFORM_DIGITAL_IO_H

#include <stdlib.h>
#include <stdint.h>
    
typedef enum {
    DIGITAL_IO_OK = 0,
    DIGITAL_IO_INVALID_ID,
    DIGITAL_IO_READ_ERROR,
    DIGITAL_IO_WRITE_ERROR
} DIGITAL_IO_ERROR_CODE;

/*
    Sets pin described by pin_id as output

    Returns DIGITAL_IO_INVALID_ID or DIGITAL_IO_OK
*/

DIGITAL_IO_ERROR_CODE set_pin_as_output(uint8_t pin_id);

/*
    Sets pin described by pin_id as input

    Returns DIGITAL_IO_INVALID_ID or DIGITAL_IO_OK
*/

DIGITAL_IO_ERROR_CODE set_pin_as_input(uint8_t pin_id);

/*
    Puts src on pin described by pin_id

    Returns DIGITAL_IO_INVALID_ID, DIGITAL_IO_WRITE_ERROR or DIGITAL_IO_OK
*/

DIGITAL_IO_ERROR_CODE pin_set(uint8_t pin_id, bool src);

/*
    Gets value from pin described by pin_id and puts it in dest

    Returns DIGITAL_IO_INVALID_ID, DIGITAL_IO_READ_ERROR or DIGITAL_IO_OK
*/

DIGITAL_IO_ERROR_CODE read_pin(uint8_t pin_id, bool dest);

#endif //PLATFORM_DIGITAL_IO_H