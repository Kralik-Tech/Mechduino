/* Kod pro arduino umoznujici programovani z matlabu.
 * Vytvoreno pro Arduino MEGA2560.
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 10.11.2023
*/

#include <stdint.h>
#include "fsm.h"
#include "states.h"
#include "uart.h"
#include "utilities.h"
#include "timer.h"

int main(void){
  /* Pocatecni inicializace */
  init();
  timer1_init();
  uart_init();
  utilities_crc_init();
  
  /* Nastaveni pocatecniho stavu */
  fsm_t   fsm;
  uint8_t event;
  fsm_transition(fsm, &states_find_action);

  /* Globalni povoleni interruptu */
  sei();


  /* Nekonecna smycka - beh stavoveho automatu */
  while(1){
    if(fsm_get_event(event))
      fsm_dispatch(fsm, event);
  }

  return 0;
} /* main */