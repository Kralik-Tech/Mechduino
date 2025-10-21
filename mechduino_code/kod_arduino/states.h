/* states.h - knihovna definujici jednotlive stavy ve stavovem automatu, ktere mohou nastat
 * 
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 7.11.2023
*/

#ifndef STATES_H
#define STATES_H

#include <UIPEthernet.h>
#include <SPI.h>
#include <stdint.h>
#include "fsm.h"
#include "evt_queue.h"
#include "controllers.h"
#include "Arduino.h"

#define MAX_SPLIT_MSG 7U

/***********************************************************************************************************/
/* Stav, ve kterem bude vyhodnoceno, ktera akce bude vykonana
 * tzn. do ktereho dalsiho stavu se nasledne prepne
*/
/***********************************************************************************************************/
void states_find_action(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vytvoren a pripojen novy ovladac do seznamu ovladacu */
/***********************************************************************************************************/
void states_add_new_controller(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Pomocna funkce slouzici pro pripojeni noveho ovladace
 * controller - ukazatel na ovladac, ktery ma byt pripojen
*/
/***********************************************************************************************************/
void states_add_new_controller_other_actions(Controller *controller);

/***********************************************************************************************************/
/* Stav, ve kterem bude zjisten jaky prikaz, na jakem ovladaci se ma vykonat
 * Vytahne potrebny ovladac z fronty, zjisti jeho typ a podle toho nastavi dalsi stav
 */
 /***********************************************************************************************************/
void states_sort_command(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude pripojen ethernet pro komunikaci s PC */
/***********************************************************************************************************/
void states_connect_ethernet(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro Digital_output */
/***********************************************************************************************************/
void states_command_digital_output(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro krokovy motor ovladany Adafruit Motorshield v2 */
/***********************************************************************************************************/
void states_command_adafruit_stepper(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro sonar */
/***********************************************************************************************************/
void states_command_sonar(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro digitalni senzor */
/***********************************************************************************************************/
void states_command_digital_sensor(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro mikrofon */
/***********************************************************************************************************/
void states_command_microphone(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro DC motor */
/***********************************************************************************************************/
void states_command_DC_motor(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz Encoder */
/***********************************************************************************************************/
void states_command_encoder(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro krokovy motor rizeny vlastnim driverem */
/***********************************************************************************************************/
void states_command_driver_stepper(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro senzor naklonu */
/***********************************************************************************************************/
void states_command_gyroscope(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro RGB senzor */
/***********************************************************************************************************/
void states_command_rgb_sensor(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro tlacitko */
/***********************************************************************************************************/
void states_command_button(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro pwm_output */
/***********************************************************************************************************/
void states_command_pwm_output(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro tensor_module */
/***********************************************************************************************************/
void states_command_tensor_module(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro lidar */
/***********************************************************************************************************/
void states_command_lidar(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro magnetic_encoder */
/***********************************************************************************************************/
void states_command_magnetic_encoder(fsm_t &fsm, uint8_t event);

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro servo */
/***********************************************************************************************************/
void states_command_servo(fsm_t &fsm, uint8_t event);

#endif /* STATES_H */