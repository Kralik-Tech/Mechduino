/* timer.h - knihovna slouzici pro pouziti casovacu
 * casovace 1 a 5 slouzi k ovladani krokovych motoru
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 30.11.2023
*/

#ifndef TIMER_H
#define TIMER_H

#include "controllers.h"

#define TMR1_INTERRUPT_TIME 0xFFFF - 15999  // Preteceni kazdou 1 milisekundu 

#define TMR1_CHECK_DELAY 100U

class Adafruit_Stepper_motor; // Predbezna deklarace tridy Adafruit_Stepper_motor
class Driver_Stepper_motor;   // Predbezna deklarace tridy Driver_Stepper_motor
class Encoder;                // Predbezna deklarace tridy Encoder
class Magnetic_encoder;       // Predbezna deklarace tridy Magnetic_ncoder
class Gyroscope;              // Predbezna deklarace tridy Gyroscope
class Button;                 // Predbezna deklarace tridy Button

 
/***********************************************************************/
/* Inicializace timeru 1 */
/***********************************************************************/
void timer1_init(void);

/***********************************************************************/
/* Nastaveni globalnich ukazatelu na krokove motory pro moznost ovladani
 * z interruptu. Zde predany krokovy motor je pripojen na
 * Adafruit Motorshield v2.
 * stepper - ukazatel na krokovy motor
 */
/***********************************************************************/
void timer_set_stepper(Adafruit_Stepper_motor *stepper);

/***********************************************************************/
/* Nastaveni globalnich ukazatelu na krokove motory pro moznost ovladani
 * z interruptu. Zde predany krokovy motor je ovladany vlastnim driverem.
 * stepper - ukazatel na krokovy motor
 */
/***********************************************************************/
void timer_set_stepper(Driver_Stepper_motor *stepper);

/***********************************************************************/
/* Pomocna metoda kontrolujici, jestli je nutne mit zapnuty timer1,
 * pokud ne, bude vypnut
 */
/***********************************************************************/
void timer_check_steppers_state(void);

/***********************************************************************/
/* Funkce pro zastaveni krokovych motoru pripojeneho k 
 * Adafruit motorshield
*/
/***********************************************************************/
void timer_adafruit_stepper_stop(void);

/***********************************************************************/
/* Funkce pro zapnuti krokovych motoru pripojeneho k 
 * Adafruit motorshield
*/
/***********************************************************************/
void timer_adafruit_stepper_continue(void);

/***********************************************************************/
/* Nastaveni encoderu pro mozne ovladani z interruptu na preteceni
 * timeru 1
 * encoder - ukazatel na ovladac encoderu
 */
/***********************************************************************/
void timer_set_encoder(Encoder *encoder);

/***********************************************************************/
/* Nastaveni magnetickeho encoderu pro mozne ovladani z interruptu na preteceni
 * timeru 1
 * encoder - ukazatel na ovladac encoderu
 */
/***********************************************************************/
void timer_set_mag_encoder(Magnetic_encoder *magnetic_encoder);

/***********************************************************************/
/* Nastaveni gyroskopu ovladanych pomoci timeru 4 
 * gyro - ukazatel na ovladac gyroskopu
*/
/***********************************************************************/
void timer_set_gyro(Gyroscope *gyro);

/***********************************************************************/
/* Nastaveni tlacitek, ovladanych pomoci timeru 5
 * button - ukazatel na ovladac tlacitka
*/
/***********************************************************************/
void timer_set_button(Button *button);

#endif /* TIMER_H */