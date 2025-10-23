#ifndef PLATFORM_TIMER
#define PLATFORM_TIMER

#include <stdlib.h>

/** Configurable variables **/

typedef uint16_t TIMER_PRECISION;       // Sets precision of used timers
typedef uint8_t TIMER_ID_TYPE;
#define MAXIMUM_CONCURRENT_TIMERS 255
typedef uint16_t WATCHDOG_PRECISION;    // Sets precision of watchdog timer

/** Library functions and constants **/

typedef enum {
    SECOND_TIMER,
    MILLISECOND_TIMER
} TIMER_MODE;

typedef enum {
    TIMER_OK = 0,
    TIMER_INVALID_ID,
    TIMER_OVERFLOW,
    TIMER_TOO_MANY_TIMERS           // Reached maximum number of supported timers 
} TIMER_ERROR_CODE;


/*
    Sets up and starts new timer in mode specified by mode and puts its id in timer_id

    Returns TIMER_TOO_MANY_TIMERS or TIMER_OK.
*/

TIMER_ERROR_CODE setup_timer(TIMER_MODE mode, TIMER_ID_TYPE timer_id);


/*
    Gets value from timer specified by timer_id and puts it in dest

    Returns TIMER_INVALID_ID, TIMER_OVERFLOW or TIMER_OK.
*/

TIMER_ERROR_CODE get_timer(TIMER_ID_TYPE timer_id, TIMER_PRECISION dest);


/*
    Stops timer specified by timer_id

    Returns TIMER_INVALID_ID or TIMER_OK
*/

TIMER_ERROR_CODE stop_timer(TIMER_ID_TYPE timer_id);


/*
    Resets timer specified by timer_id to 0

    Returns TIMER_INVALID_ID or TIMER_OK
*/

TIMER_ERROR_CODE reset_timer(TIMER_ID_TYPE timer_id);


/*
    Starts watchdog timer with timeout_period specified in milliseconds

    Returns TIMER_TOO_MANY_TIMERS or TIMER_OK
*/

TIMER_ERROR_CODE start_watchdog(WATCHDOG_PRECISION timeout_period);


/*
    Stops watchdog timer
*/

void stop_watchdog(void);

#endif // PLATFORM_TIMER