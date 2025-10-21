/* evt_queue.cpp - slouzi k implementaci metod definovanych v evt_queue.h 
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 5.11.2023
*/

#include "evt_queue.h"

/******************************************************************************************************************************************/
/* Metoda pridavajici udalost do fronty, pokud ji uspesne prida, vrati 1, jinak vrati 0
 * in - udalost k ulozeni
 */
/******************************************************************************************************************************************/
uint8_t Fifo::write_evt(uint8_t in){
  if(tail == head+1 || (!tail && (head == FIFO_SIZE - 1))) //Kontrola jestli je fronta plna, pokud ano, nezapise udalost
    return FAIL;

  fifo[head++] = in;
  return SUCCESS;
} /* write_evt */

/******************************************************************************************************************************************/
/* Metoda vracejici udalost z fronty, pokud bude fronta prazdna vrati 0, jinak vrati 1 
 * out - vracena udalost
 */
/******************************************************************************************************************************************/
uint8_t Fifo::read_evt(uint8_t &out){
  if(head == tail) // Kontrola, jestli je fronta prazdna
    return FAIL;

  out = fifo[tail++];
  return SUCCESS;
} /* read_evt */