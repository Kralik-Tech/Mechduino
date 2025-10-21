/* fsm.h - knihovna stavoveho automatu, ktery ovlada arduino
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 6.11.2023
*/

#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <avr/wdt.h>
#include "evt_queue.h"
#include "utilities.h"

class Controller; // Predbezna deklarace tridy

typedef struct fsm_t fsm_t;                           // Struktura slouzici k ulozeni ukazatele na funkce kvuli predavani funkcim

typedef void(*state_fp)(fsm_t &fsm, uint8_t event);   // Ukazatel na funkce reprezentujici jednotlive stavy automatu

struct fsm_t{
  state_fp state;
}; /* fsm_t */

/***************************************************************************************************************************************/
/* Funkce slouzici k prechodu do noveho stavu (zaroven bude pouzita jako inicializace prvniho stavu) 
 * fsm       - struktura uchovavajici aktualni stav (predavana odkazem)
 * new_state - novy stav, ktery ma byt nastaven
 */
/***************************************************************************************************************************************/
void fsm_transition(fsm_t &fsm, state_fp new_state);

/***************************************************************************************************************************************/
/* Funkce slouzici k vykonani aktualni udalosti
 * fsm   - struktura uchovavajici aktualni stav (predavana odkazem)
 * event - udalost v danem stavu, ktera ma byt vykonana
 */
/***************************************************************************************************************************************/
void fsm_dispatch(fsm_t &fsm, uint8_t event);

/***************************************************************************************************************************************/
/* Funkce ukladajici novou udalost do fronty, ktera ma byt v budoucnu vykonana, pri uspesnem zapsani vraci 1, jinak 0
 * in - ukladana udalost
 */
/***************************************************************************************************************************************/
uint8_t fsm_add_event(uint8_t in);

/***************************************************************************************************************************************/
/* Funkce vracejici udalost k vykonani z fronty, pokud je fronta prazdna (neni tedy zadna udalost k vykonani), vrati 0, jinak vrati 1
 * out - vracena udalost k vykonani
 */
/***************************************************************************************************************************************/
uint8_t fsm_get_event(uint8_t &out);

/***************************************************************************************************************************************/
/* Funkce ukladajici zpravu z matlabu do fronty, pri uspesnem zapsani vraci 1, jinak 0 
 * in - ukladana udalost 
 */
/***************************************************************************************************************************************/
uint8_t fsm_add_matlab_event(uint8_t in);

/***************************************************************************************************************************************/
/* Pretizena funkce ukladajici udalosti do matlab fronty udalosti
 * arr - pole, ze ktereho bude ukladat udalosti
 * size - velikost pole (pocet zprav)
*/
/***************************************************************************************************************************************/
uint8_t fsm_add_matlab_event(uint8_t *arr, uint8_t size);

/***************************************************************************************************************************************/
/* Funkce vracejici zpravu (udalost) z matlabu ulozenou ve fronte g_matlab_queue, pokud je fronta prazdna, vrati 0, jinak 1
 * out - vracena udalost
 */
/***************************************************************************************************************************************/
uint8_t fsm_get_matlab_event(uint8_t &out);

/***************************************************************************************************************************************/
/* Pretizena funkce ukladajici udalosti k vykonani do pole 
 * arr - pole, do ktereho maji byt ulozeny udalosti
 * size - velikost pole (pocet udalosti, ktery ma byt ulozen)
 */
/***************************************************************************************************************************************/
uint8_t fsm_get_matlab_event(uint8_t *arr, uint8_t size);

/***************************************************************************************************************************************/
/* Funkce vkladajici novy ovladac do pole ovladacu
 * controller - ukazatel na ovladac, ktery ma byt vlozen do pole
 */
/***************************************************************************************************************************************/
void fsm_add_controller(Controller *controller);

/***************************************************************************************************************************************/
/* Funkce vracejici ovladac na dane pozici z pole ovladacu
 * index - pozice, ze ktere ma byt vracen ovladac
 */
/***************************************************************************************************************************************/
Controller *fsm_get_controller(uint8_t index);

/***************************************************************************************************************************************/
/* Funkce uvolnujici veskerou dynamicky alokovanou pamet a nasledny reset arduina */
/***************************************************************************************************************************************/
void fsm_reset(void);

#endif /* FSM_H */