/* timer.cpp - slouzi k implementaci funkci definovanych v "timer.h"
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 30.11.2023
*/

#include "timer.h"

/* Globalni promenne pro ovladani krokovych motoru */
static volatile Adafruit_Stepper_motor *g_adafruit_stepper[MAX_ADAFRUIT_STEPPER_COUNT];
static volatile Driver_Stepper_motor   *g_driver_stepper[MAX_DRIVER_STEPPER_COUNT];
static volatile uint8_t adafruit_stepper_stop = 0;

/* Globalni promenne pro ovladani encoderu */
static volatile Encoder *g_encoder[MAX_ENCODER_COUNT];

/* Globalni promenne pro ovladani encoderu */
static volatile Magnetic_encoder *g_mag_encoder = NULL;

/* Globalni promenna pro ovladani gyroskopu */
static volatile Gyroscope *g_gyro = NULL;

/* Globalni promenne pro ovladani tlacitek */
static volatile Button *g_button[MAX_BUTTONS_COUNT];

/***********************************************************************/
/* Inicializace timeru 1 */
/***********************************************************************/
void timer1_init(void){
  //Enable timer 1
  PRR0 &= ~(1 << PRTIM1);

  //Vynulovani registru A a B, ktere byly nastaveny v init()
  TCCR1A = 0;
  TCCR1B = 0;

  //Interrupt - overflow
  TIMSK1  = 0;
  TIMSK1 |= (1 << TOIE1);

  // Nastaveni pocatecni hodnoty v registru TCNT1
  TCNT1 = TMR1_INTERRUPT_TIME;

  for(uint8_t i = 0; i < MAX_ADAFRUIT_STEPPER_COUNT; i++)
    g_adafruit_stepper[i] = NULL;
  
  for(uint8_t i = 0; i < MAX_DRIVER_STEPPER_COUNT; i++)
    g_driver_stepper[i] = NULL;

  for(uint8_t i = 0; i < MAX_ENCODER_COUNT; i++)
    g_encoder[i] = NULL;


} /* timer1_init */

/***********************************************************************/
/* Nastaveni globalnich ukazatelu na krokove motory pro moznost ovladani
 * z interruptu. Zde predany krokovy motor je pripojen na Adafruit 
 * Motorshield v2.
 * stepper - ukazatel na krokovy motor
 */
/***********************************************************************/
void timer_set_stepper(Adafruit_Stepper_motor *stepper){
  for(uint8_t i = 0; i < MAX_ADAFRUIT_STEPPER_COUNT; i++)
    if(!g_adafruit_stepper[i]){
      g_adafruit_stepper[i] = stepper;
      return;
    }
} /* timer_set_stepper */

/***********************************************************************/
/* Nastaveni globalnich ukazatelu na krokove motory pro moznost ovladani
 * z interruptu. Zde predany krokovy motor je ovladany vlastnim driverem.
 * stepper - ukazatel na krokovy motor
 */
/***********************************************************************/
void timer_set_stepper(Driver_Stepper_motor *stepper){
  for(uint8_t i = 0; i < MAX_DRIVER_STEPPER_COUNT; i++)
    if(!g_driver_stepper[i]){
      g_driver_stepper[i] = stepper;
      return;
    }
} /* timer_set_stepper */

/***********************************************************************/
/* Pomocna metoda kontrolujici, jestli je nutne mit zapnuty timer1,
 * pokud ne, bude vypnut.
 */
/***********************************************************************/
void timer_check_steppers_state(void){
  uint8_t adafruit_steppers_state = 0; // 0 - krokove motory ovladane motorshieldem jsou zapnute (alespon jeden), 1 - vypnute
  
  // Kontrola krokovych motoru ovladanych motorshieldem
  for(uint8_t i = 0; i < MAX_ADAFRUIT_STEPPER_COUNT; i++){
    if( g_adafruit_stepper[i] && g_adafruit_stepper[i]->is_moving() )
      break;
    else if(i == MAX_ADAFRUIT_STEPPER_COUNT - 1)
      adafruit_steppers_state = 1;
  }
  
  // Kontrola krokovych motoru ovladanych driverem
  for(uint8_t i = 0; i < MAX_DRIVER_STEPPER_COUNT; i++){
    if(  g_driver_stepper[i] && g_driver_stepper[i]->is_moving() )
      break;
    else if( adafruit_steppers_state && (i == MAX_DRIVER_STEPPER_COUNT - 1) )
      TMR1_OFF;
  }
} /* timer_check_steppers_state */

/***********************************************************************/
/* Funkce pro zastaveni krokovych motoru pripojeneho k 
 * Adafruit motorshield
*/
/***********************************************************************/
void timer_adafruit_stepper_stop(void){
  adafruit_stepper_stop = 1;
} /* timer_adafruit_stepper_stop */

/***********************************************************************/
/* Funkce pro zapnuti krokovych motoru pripojeneho k 
 * Adafruit motorshield
*/
/***********************************************************************/
void timer_adafruit_stepper_continue(void){
  adafruit_stepper_stop = 0;
} /* timer_adafruit_stepper_continue */

/***********************************************************************/
/* Nastaveni encoderu pro mozne ovladani z interruptu na preteceni
 * timeru 1
 * encoder - ukazatel na ovladac encoderu
 */
/***********************************************************************/
void timer_set_encoder(Encoder *encoder){
  for(uint8_t i = 0; i < MAX_ENCODER_COUNT; i++)
    if(!g_encoder[i]){
      g_encoder[i] = encoder;
      return;
    }
} /* timer_set_encoder */

/***********************************************************************/
/* Nastaveni magnetickeho encoderu pro mozne ovladani z interruptu na preteceni
 * timeru 1
 * encoder - ukazatel na ovladac encoderu
 */
/***********************************************************************/
void timer_set_mag_encoder(Magnetic_encoder *magnetic_encoder){
    if(!g_mag_encoder){
      g_mag_encoder = magnetic_encoder;
      return;
      }
    
} /* timer_set_encoder */

/***********************************************************************/
/* Nastaveni gyroskopu ovladanch pomoci timeru 4 
 * gyro - ukazatel na ovladac gyroskopu
*/
/***********************************************************************/
void timer_set_gyro(Gyroscope *gyro){
  g_gyro = gyro;
} /* timer_set_gyro */

/***********************************************************************/
/* Nastaveni tlacitek, ovladanych pomoci timeru 5
 * button - ukazatel na ovladac tlacitka
*/
/***********************************************************************/
void timer_set_button(Button *button){
  for(uint8_t i = 0; i < MAX_BUTTONS_COUNT; i++)
    if(!g_button[i]){
      g_button[i] = button;
      return;
    }
} /* timer_set_button */

/***********************************************************************/
/* Interrupt pro preteceni timeru 1 */
/***********************************************************************/
ISR(TIMER1_OVF_vect){
  static uint8_t interupt_3x = 0; // Pocitadlo pro pocitani milisekund do kontroly nutnosti zapnuti timeru 1
  static uint8_t interupt_6x = 0; // Pocitadlo pro pocitani milisekund do kontroly nutnosti zapnuti timeru 1

  for(uint8_t i = 0; i < MAX_ENCODER_COUNT; i++){
    if(g_encoder[i])
      g_encoder[i]->tick();

  }

  // Ovladani krokovych motoru pripojenych na vlastnim driveru
  for(uint8_t i = 0; i < MAX_DRIVER_STEPPER_COUNT; i++){
    if( g_driver_stepper[i] && g_driver_stepper[i]->is_moving() )
      g_driver_stepper[i]->tick();

  }

  // Ovladani krokovych motoru pripojenych na Adafruit Motorshield v2
  for(uint8_t i = 0; i < MAX_ADAFRUIT_STEPPER_COUNT; i++){
    if( !adafruit_stepper_stop && g_adafruit_stepper[i] && g_adafruit_stepper[i]->is_moving() )
      g_adafruit_stepper[i]->tick();

  }



  if(interupt_3x<3)
    {
      interupt_3x++;
    }
  else
    {
      interupt_3x = 0;
        if(g_gyro){
          sei();
          g_gyro->set_angles();
        }

    }

  if(interupt_6x<6)
    {
      interupt_6x++;
    }
  else
    {
      interupt_6x = 0;
      for(uint8_t i = 0; i < MAX_BUTTONS_COUNT; i++){
        if(g_button[i])
          g_button[i]->tick();
      }
    }


  TCNT1 = TMR1_INTERRUPT_TIME;
} /* ISR - Timer1_OVF */
