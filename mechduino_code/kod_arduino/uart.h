/* uart.h - knihovna definujici funkce, ktere slouzi ke kominikaci mezi PC a arduinem
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 15.11.2023
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "fsm.h"
#include "utilities.h"
#include "Arduino.h"

#define MAX_MESSAGE_SIZE 15U

#define MESSAGE_SAVED     1U
#define MESSAGE_NOT_SAVED 2U

#define MESSAGE_SIZE g_message[0]
#define CHECK_SIZE   MESSAGE_SIZE - 1
#define MESSAGE_CRC  g_message[MESSAGE_SIZE - 1]

/*****************************************************************************************************************/
/* Funkce slouzici k pocatecni inicializaci uartu */
/*****************************************************************************************************************/
void uart_init(void);

/*****************************************************************************************************************/
/* Funkce odesilajici prostrednictvim uartu zpravu typu
 * msg - odesilana zprava
*/
/*****************************************************************************************************************/
void uart_send(uint8_t msg);

#endif /* UART_H */