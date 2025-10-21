/* uart.cpp - slouzi k implementaci funkci definovanych v "uart.h"
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 15.11.2023
*/

#include "uart.h"

/* Globalni promenne */
static volatile uint8_t g_message[MAX_MESSAGE_SIZE]; // Docasna fronta pro ukladani zpravy z matlabu
static volatile uint8_t g_index = 0;                 // Slouzi pro pohyb v docasne fronte

/*****************************************************************************************************************/
/* Funkce slouzici k pocatecni inicializaci uart */
/*****************************************************************************************************************/
void uart_init(void){
  // Nastaveni zapisovani
  UCSR0A |= (1 << RXC0) | (1 << UDRE0) | (1 << U2X0);

  // Povoleni interruptu pro RX
  UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  // Nastaveni poctu odesilanych bitu na 8 (velikost odesilane zpravy)
  UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

  // baud rate 9600
  UBRR0 = 207;
} /* uart_init */

/*****************************************************************************************************************/
/* Funkce odesilajicii prostrednictvim uartu zpravu 
 * msg - odesilana zprava
*/
/*****************************************************************************************************************/
void uart_send(uint8_t msg){
  while( !(UCSR0A & (1 << UDRE0)) ); // Ceka, az bude registr volny a pak odesle data
  
  UDR0 = msg;
} /* uart_send */

/*****************************************************************************************************************/
/* Interrupt, ktery se spusti v pripade prijmuti nove zpravy z matlabu
 * Prijme vektor odeslany z matlabu a ulozi ho do matlab fronty (zaroven provede kontrolu prijate zpravy)
*/
/*****************************************************************************************************************/
ISR(USART0_RX_vect){
  if( UCSR0A & (1 << RXC0) ){
    g_message[g_index++] = UDR0;
    if(g_index == MESSAGE_SIZE){
      // Kontrola zpravy
      crc message_crc = utilities_get_crc(g_message, CHECK_SIZE);
      uart_send(message_crc);

      if(utilities_crc_check(message_crc, MESSAGE_CRC)) {
        // V pripade, ze je zprava spravna, bude ulozena do fronty matlab udalosti
        if(fsm_add_matlab_event(g_message, MESSAGE_SIZE))
          uart_send(MESSAGE_SAVED);
        else
          uart_send(MESSAGE_NOT_SAVED);
      } 
      else
        uart_send(MESSAGE_NOT_SAVED);

      g_index = 0;
    } // if vnitrni
  } // if vnejsi
} /* ISR - USART0_RX */