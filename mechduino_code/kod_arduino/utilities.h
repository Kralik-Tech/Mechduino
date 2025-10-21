/* utilities.h - obsahuje uzitecne pomocne tridy a funkce
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 27.10.2023
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>
#include "controllers.h"

/* Uzitecna makra pro cely kod */
#define SUCCESS 1U
#define FAIL    0U

/* Makra pro dynamicke pole */
#define INIT_SIZE 4U
#define INCREMENT 3U

typedef uint8_t crc;

/* Makra pro CRC */
// Vypocet CRC
#define WIDTH      (8 * sizeof(crc))
#define TOPBIT     (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8

class Controller; // Predbezna deklarace tridy Controller

/***********************************************************************************************************************/
/* Funkce inicializujici CRC pro vsechna mozna bytova cisla (0 - 255) */
/***********************************************************************************************************************/
void utilities_crc_init(void);

/***********************************************************************************************************************/
/* Funkce vracejici CRC zadane zpravy 
 * message - zprava, ktera se ma kontrolovat
 * nBytes  - pocet bytu, ktery se ma ve zprave kontrolovat
*/
/***********************************************************************************************************************/
crc utilities_get_crc(uint8_t const *message, uint8_t nBytes);

/***********************************************************************************************************************/
/* Funkce vracejici 1, pokud je hodnota CRC spravna, jinak vraci 0 
 * CRC         - zjistene CRC, ktere se bude kontrolovat
 * messgae_crc - CRC, ktere ma vyjit u spravne prijate zpravy
*/
/***********************************************************************************************************************/
uint8_t utilities_crc_check(crc CRC, uint8_t message_crc);

/***********************************************************************************************************************/
/* Trida predstavujici dynamicke pole - slouzi k ukladani ukazatelu na objekty. */
/***********************************************************************************************************************/
class List{
  private:
    Controller **arr;     // Predstavuje pole, do ktereho budou ukladany ukazatele na objekty
    uint8_t size;         // Celkova velikost aktualne pouzivaneho pole
    uint8_t last_index;   // Posledni aktualni pozice, kam lze ulozit 
    
  public:
    /***********************************************************************************************************************/
    /* Konstruktor - alokuje pamet pole o pocatecni velikosti a nastavi pocatecni hodnoty clenskych promennych */
    /***********************************************************************************************************************/
    List();

    /***********************************************************************************************************************/
    /* Destruktor - uvolni alokovanou pamet objektu a alokovanou pamet pole */
    /***********************************************************************************************************************/
		~List();

    /***********************************************************************************************************************/
    /* Metoda pridavajici novy prvek do seznamu, v pripade kdy bude pole zaplneno, alokuje novou pamet pro pole
     * ob - ukazatel na objekt, ktery ma byt pridan do seznamu
     */
    /***********************************************************************************************************************/
		void add(Controller *ob);
		
    /***********************************************************************************************************************/
    /* Pretizeni operatoru [] - vraci ukazatel na objekt ulozeny na dane pozici
     * num - pozice, ze ktere se ma vratit ukazatel
     */
    /***********************************************************************************************************************/
    Controller *operator[](uint8_t num);

}; /* List */

#endif /* UTILITIES_H */