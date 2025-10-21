/* Knihovna evt_queue.h slouzi k definici fronty udalosti
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 5.11.2023
*/

#ifndef EVT_QUEUE_H
#define EVT_QUEUE_H

#include <stdint.h>

#define FIFO_SIZE 128U
#define SUCCESS   1U
#define FAIL      0U

/**********************************************************************************************************************************************/
/* Trida reprezentujici frontu udalosti - budou vytvoreny 2 objekty teto tridy - jeden pro ukladani udalosti k vykonani pro stavovy automat,
 * druhy slouzici k ukladani prichozich zprav z matlabu 
 */
/**********************************************************************************************************************************************/
class Fifo{
  private:
    uint8_t *fifo[FIFO_SIZE]; // Fronta slouzici k ukladani udalosti (jednotlive udalosti budou od sebe oddeleny nulou)
    uint8_t head : 7;         // Slouzi k indexaxi pri ukladani do fronty
    uint8_t tail : 7;         // Slouzi k indexaci pri brani z fronty
  
  public:
    /******************************************************************************************************************************************/
    /* Konstruktor - inicializuje pocatecni hodnoty head a tail */
    /******************************************************************************************************************************************/
    Fifo() { head = tail = 0; }

    /******************************************************************************************************************************************/
    /* Metoda pridavajici udalost do fronty, pokud ji uspesne prida, vrati 1, jinak vrati 0
     * in - udalost k ulozeni
     */
    /******************************************************************************************************************************************/
     uint8_t write_evt(uint8_t in);
     
     /******************************************************************************************************************************************/
     /* Metoda vracejici udalost z fronty, pokud bude fronta prazdna vrati 0, jinak vrati 1 
      * out - vracena udalost
      */
    /******************************************************************************************************************************************/
     uint8_t read_evt(uint8_t &out);

     /******************************************************************************************************************************************/
     /* Metoda vracejici pozici head a tail pro kontrolu naplnenosti fronty
      * ret_head - parametr, do ktereho bude ulozena hodnota head
      * ret_tail - parametr, do ktereho bude ulozena hodnota tail
      */
     /******************************************************************************************************************************************/
    void get_head_tail(uint8_t &ret_head, uint8_t &ret_tail) { ret_head = head; ret_tail = tail; }

}; /* Fifo */

#endif /* EVT_QUEUE_H */