/* fsm.cpp- slouzi k implementaci metod definovanych v fsm.h 
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 6.11.2023
*/

#include "fsm.h"

/* Globalni promenne */
static volatile Fifo g_event_queue;   // Fronta, ktera bude ukladat udalosti stavoveho automatu - Fronta stavoveho automatu
static volatile Fifo g_matlab_queue;  // Fronta, ktera bude ukladat zpravy z matlabu            - Fronta udalosti
static volatile List g_controllers;   // Dynamicke pole slouzici k ukladani ovladadu            - Fronta ovladacu

/***************************************************************************************************************************************/
/* Funkce ukladajici novou udalost do fronty, ktera ma byt v budoucnu vykonana
 * in - ukladana udalost
 */
/***************************************************************************************************************************************/
uint8_t fsm_add_event(uint8_t in){
  return g_event_queue.write_evt(in);
} /* fsm_add_event */

/***************************************************************************************************************************************/
/* Funkce vracejici udalost k vykonani z fronty, pokud je fronta prazdna (neni tedy zadna udalost k vykonani), vrati 0, jinak vrati 1
 * out - vracena udalost k vykonani
 */
/***************************************************************************************************************************************/
uint8_t fsm_get_event(uint8_t &out){
  return g_event_queue.read_evt(out);
} /* fsm_get_event */

/***************************************************************************************************************************************/
/* Funkce ukladajici zpravu z matlabu do fronty, pri uspesnem zapsani vraci 1, jinak 0 
 * in - ukladana udalost 
*/
/***************************************************************************************************************************************/
uint8_t fsm_add_matlab_event(uint8_t in){
  return g_matlab_queue.write_evt(in);
} /* fsm_add_matlab_event */

/***************************************************************************************************************************************/
/* Pretizena funkce ukladajici udalosti do matlab fronty udalosti - vyuziva se v interruptu
 * V pripade ze se cela zprava vejde do fronty, tak ji ulozi a vrati 1, jinak nic neulozi a vrati 0
 * arr - pole, ze ktereho bude ukladat udalosti
 * size - velikost pole (pocet zprav)
*/
/***************************************************************************************************************************************/
uint8_t fsm_add_matlab_event(uint8_t *arr, uint8_t size){
  uint8_t head, tail;
  g_matlab_queue.get_head_tail(head, tail);

  // Kontrola, jestli se zprava vejde do fronty
  if( ((head < tail) && (tail - head >= size)) || ((head > tail) && (FIFO_SIZE - head + tail >= size)) || (head == tail)){
    for(uint8_t i = 1; i < size-1; i++)
      g_matlab_queue.write_evt(*(arr + i));
    
    return SUCCESS;
  }

  return FAIL;
} /* fsm_add_matlab_event */

/***************************************************************************************************************************************/
/* Funkce vracejici zpravu (udalost) z matlabu ulozenou ve fronte g_matlab_queue, pokud je fronta prazdna, vrati 0, jinak 1
 * out - vracena udalost
*/
/***************************************************************************************************************************************/
uint8_t fsm_get_matlab_event(uint8_t &out){
  return g_matlab_queue.read_evt(out);
} /* fsm_get_matlab_event */

/***************************************************************************************************************************************/
/* Pretizena funkce ukladajici udalosti k vykonani do pole 
 * arr - pole, do ktereho maji byt ulozeny udalosti
 * size - velikost pole (pocet udalosti, ktery ma byt ulozen)
*/
/***************************************************************************************************************************************/
uint8_t fsm_get_matlab_event(uint8_t *arr, uint8_t size){
  for(uint8_t i = 0; i < size; i++)
    if(!g_matlab_queue.read_evt(*(arr + i)))
      return FAIL;
  return SUCCESS;
} /* fsm_get_matlab_event */

/***************************************************************************************************************************************/
/* Funkce slouzici k vykonani aktualni udalosti
 * fsm   - struktura uchovavajici aktualni stav (predavana odkazem)
 * event - udalost v danem stavu, ktera ma byt vykonana
 */
/***************************************************************************************************************************************/
void fsm_dispatch(fsm_t &fsm, uint8_t event){
  (*fsm.state)(fsm, event);
} /* fsm_dispatch */

/***************************************************************************************************************************************/
/* Funkce slouzici k prechodu do noveho stavu (zaroven bude pouzita jako inicializace prvniho stavu) 
 * fsm       - struktura uchovavajici aktualni stav (predavana odkazem)
 * new_state - novy stav, ktery ma byt nastaven
 */
/***************************************************************************************************************************************/
void fsm_transition(fsm_t &fsm, state_fp new_state){
  fsm.state = new_state;
  fsm_dispatch(fsm , EV_ENTRY);
 } /* fsm_transition */

/***************************************************************************************************************************************/
/* Funkce vkladajici novy ovladac do pole ovladacu
 * controller - ukazatel na ovladac, ktery ma byt vlozen do pole
 */
/***************************************************************************************************************************************/
void fsm_add_controller(Controller *controller){
  g_controllers.add(controller);
} /* fsm_add_controller */

/***************************************************************************************************************************************/
/* Funkce vracejici ovladac na dane pozici z pole ovladacu
 * index - pozice, ze ktere ma byt vracen ovladac
*/
/***************************************************************************************************************************************/
Controller *fsm_get_controller(uint8_t index){
  return g_controllers[index];
} /* fsm_get_controller */

/***************************************************************************************************************************************/
/* Funkce uvolnujici veskerou dynamicky alokovanou pamet a nasledny reset arduina */
/***************************************************************************************************************************************/
void fsm_reset(void){
  g_controllers.~List();
  wdt_enable(WDTO_15MS);
  delay(200);
} /* fsm_reset */