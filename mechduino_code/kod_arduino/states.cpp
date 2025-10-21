/* states.cpp - slouzi k implementaci metod definovanych v states.h
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 7.11.2023
*/

#include "states.h"

/* Globalni promenne */
static uint8_t g_index;                    // Globalni promenna ukladajici index potrebneho ovladace v poli
static uint8_t g_command;                  // Globalni promenna ukladajici konkretni akci k vykonani
static uint8_t g_split_msg[MAX_SPLIT_MSG]; //pole pro ukladani 8-bitovych hodnot, ze kterych se nasledne udela jedna 16-bitova hodnota (prip. ip adresa, nebo hromadne promenne)

/***********************************************************************************************************/
/* Stav, ve kterem bude vyhodnoceno, ktera akce bude vykonana
 * tzn. do ktereho dalsiho stavu se nasledne prepne
*/
/***********************************************************************************************************/
void states_find_action(fsm_t &fsm, uint8_t event){
  uint8_t next_event  = 0;
  uint16_t pause_time = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(EV_CHECK);
      break;
    case EV_CHECK:
      if(!fsm_get_matlab_event(next_event)){
        fsm_add_event(EV_CHECK);
        return;
      }
      fsm_add_event(next_event);
      break;
    case EV_MOVE_CONTROLLER:
      fsm_transition(fsm, &states_add_new_controller);
      break;
    case EV_MOVE_COMMAND:
      fsm_transition(fsm, &states_sort_command);
      break;
    case EV_MOVE_ETHERNET:
      fsm_transition(fsm, &states_connect_ethernet);
      break;
    case EV_PAUSE:
      fsm_get_matlab_event(g_split_msg, 2);
      pause_time = (g_split_msg[0] << 8) | g_split_msg[1];
      delay(pause_time);
      fsm_add_event(EV_CHECK);
      break;
    case EV_GET_VERSION:
      controllers_send_msg(VERSION);
      fsm_reset();
      break;
    default:
      fsm_add_event(EV_CHECK);
      break;
  }
} /* states_find_action */

/***********************************************************************************************************/
/* Stav, ve kterem bude vytvoren a pripojen novy ovladac do seznamu ovladacu 
 * a odeslana zprava do matlabu informujici na kterem indexu v poli je ovladac ulozen
*/
/***********************************************************************************************************/
void states_add_new_controller(fsm_t &fsm, uint8_t event){
  uint8_t controller_type   = 0;
  uint8_t pin               = 0;             //Pin na ktery ma byt pripojen dany ovladac
  uint16_t spr              = 0;             //pocet kroku na jednu otacku - slouzi ke krokovemu motoru
  Controller *pt_controller = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_get_matlab_event(controller_type);
      fsm_add_event(controller_type);
      break;
    case EV_ADD_DIGITAL_OUTPUT:
      fsm_get_matlab_event(pin);
      pt_controller = new Digital_output(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_ADAFRUIT_STEPPER:
      fsm_get_matlab_event(pin);
      fsm_get_matlab_event(g_split_msg, 2);
      spr = (g_split_msg[0] << 8) | (g_split_msg[1]);
      pt_controller = new Adafruit_Stepper_motor(pin, spr);
      ((Adafruit_Stepper_motor *)pt_controller)->set_speed(255); // Pocatecni nastaveni rychlosti krokoveho motoru (1 otacka za minutu)
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_SONAR:
      fsm_get_matlab_event(g_split_msg, 2); //Nacteni trig_pin a echo_pin
      pt_controller = new Sonar(g_split_msg[0], g_split_msg[1]);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_DIGITAL_SENSOR:
      fsm_get_matlab_event(pin);
      pt_controller = new Digital_sensor(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_MICROPHONE:
      fsm_get_matlab_event(pin);
      pt_controller = new Microphone(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_DC_MOTOR:
      fsm_get_matlab_event(pin);
      pt_controller = new DC_motor(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_ENCODER:
      fsm_get_matlab_event(pin);
      pt_controller = new Encoder(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_DRIVER_STEPPER:
      fsm_get_matlab_event(g_split_msg, 4);
      pt_controller = new Driver_Stepper_motor(g_split_msg[0], g_split_msg[1], g_split_msg[2], g_split_msg[3]);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_GYROSCOPE:
      pt_controller = new Gyroscope;
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_RGB_SENSOR:
      fsm_get_matlab_event(g_split_msg, 5);
      pt_controller = new RGB_sensor(g_split_msg[0], g_split_msg[1], g_split_msg[2], g_split_msg[3], g_split_msg[4]);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_BUTTON:
      fsm_get_matlab_event(pin);
      pt_controller = new Button(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_PWM_OUTPUT:
      fsm_get_matlab_event(pin);
      pt_controller = new PWM_output(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_TENSOR_MODULE:
      fsm_get_matlab_event(g_split_msg, 2); //Nacteni pin_data a pin_clk
      pt_controller = new Tensor_module(g_split_msg[0], g_split_msg[1]);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_LIDAR:
      pt_controller = new Lidar;
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_MAG_ENC:
      pt_controller = new Magnetic_encoder;
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_ADD_SERVO:
      fsm_get_matlab_event(pin);
      pt_controller = new Servo_mech(pin);
      states_add_new_controller_other_actions(pt_controller);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_add_new_controller */

/***********************************************************************************************************/
/* Pomocna funkce slouzici pro pripojeni noveho ovladace
 * controller - ukazatel na ovladac, ktery ma byt pripojen
*/
/***********************************************************************************************************/
void states_add_new_controller_other_actions(Controller *controller){
  if(!controller){
    controllers_send_msg(ALLOCATION_ERROR);
    fsm_add_event(EV_BACK);
    return;
  }

  fsm_add_controller(controller);
  controllers_send_msg(controller->get_my_index());
  fsm_add_event(EV_BACK);
}

/***********************************************************************************************************/
/* Stav, ve kterem bude zjisten jaky prikaz, na jakem ovladaci se ma vykonat
 * Vytahne potrebny ovladac z fronty, zjisti jeho typ a podle toho nastavi dalsi stav
 */
 /***********************************************************************************************************/
void states_sort_command(fsm_t &fsm, uint8_t event){
  Controller *pt_controller = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_get_matlab_event(g_command);
      fsm_get_matlab_event(g_index);
      pt_controller = fsm_get_controller(g_index);
      fsm_add_event(pt_controller->get_type());
      break;
    case EV_COMMAND_DIGITAL_OUTPUT:
      fsm_transition(fsm, &states_command_digital_output);
      break;
    case EV_COMMAND_ADAFRUIT_STEPPER:
      fsm_transition(fsm, &states_command_adafruit_stepper);
      break;
    case EV_COMMAND_SONAR:
      fsm_transition(fsm, &states_command_sonar);
      break;
    case EV_COMMAND_DIGITAL_SENSOR:
      fsm_transition(fsm, &states_command_digital_sensor);
      break;
    case EV_COMMAND_MICROPHONE:
      fsm_transition(fsm, &states_command_microphone);
      break;
    case EV_COMMAND_DC_MOTOR:
      fsm_transition(fsm, &states_command_DC_motor);
      break;
    case EV_COMMAND_ENCODER:
      fsm_transition(fsm, &states_command_encoder);
      break;
    case EV_COMMAND_DRIVER_STEPPER:
      fsm_transition(fsm, &states_command_driver_stepper);
      break;
    case EV_COMMAND_GYROSCOPE:
      fsm_transition(fsm, &states_command_gyroscope);
      break;
    case EV_COMMAND_RGB_SENSOR:
      fsm_transition(fsm, &states_command_rgb_sensor);
      break;
    case EV_COMMAND_BUTTON:
      fsm_transition(fsm, &states_command_button);
      break;
    case EV_COMMAND_PWM_OUTPUT:
      fsm_transition(fsm, &states_command_pwm_output);
      break;
    case EV_COMMAND_TENSOR_MODULE:
      fsm_transition(fsm, &states_command_tensor_module);
      break;
    case EV_COMMAND_LIDAR:
      fsm_transition(fsm, &states_command_lidar);
      break;
    case EV_COMMAND_MAG_ENC:
      fsm_transition(fsm, &states_command_magnetic_encoder);
      break;
    case EV_COMMAND_SERVO:
      fsm_transition(fsm, &states_command_servo);
      break;
    default:
      fsm_transition(fsm, &states_find_action);
      break;
  }//switch
} /* states_sort_command */

/***********************************************************************************************************/
/* Stav, ve kterem bude pripojen ethernet pro komunikaci s PC */
/***********************************************************************************************************/
void states_connect_ethernet(fsm_t &fsm, uint8_t event){
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE,0xED};
  EthernetClient client;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(EV_CONNECT);
      break;
    case EV_CONNECT:
      fsm_get_matlab_event(g_split_msg, 4);
      IPAddress server(g_split_msg[0], g_split_msg[1], g_split_msg[2], g_split_msg[3]);
      IPAddress ip(g_split_msg[0], g_split_msg[1], g_split_msg[2], 250);
      Ethernet.begin(mac, ip);
      if(client.connect(server, 5000))
        controllers_send_msg(SUCCESS);
      else{
        controllers_send_msg(ALLOCATION_ERROR);
        fsm_add_event(EV_BACK);
        return;
      }
      controllers_set_client(client);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_sort_command */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro Digital_output */
/***********************************************************************************************************/
void states_command_digital_output(fsm_t &fsm, uint8_t event){
  Digital_output *pt_ob = NULL;
  
  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_DIGITAL_HIGH:
      pt_ob = (Digital_output *)fsm_get_controller(g_index);
      pt_ob->digital_high();
      fsm_add_event(EV_BACK);
      break;
    case EV_DIGITAL_LOW:
      pt_ob = (Digital_output *)fsm_get_controller(g_index);
      pt_ob->digital_low();
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_digital_output */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro krokovy motor ovladany Adafruit Motorshield v2 */
/***********************************************************************************************************/
void states_command_adafruit_stepper(fsm_t &fsm, uint8_t event){
  uint8_t style                 = 0;
  uint8_t dir                   = 0;
  uint8_t speed                 = 0;
  uint16_t steps_16             = 0;  //prommena pro slozeni vysledneho poctu kroku nebo rychlosti
  Adafruit_Stepper_motor *pt_ob = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_ADAFRUIT_STEPPER_RELEASE:
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->release();
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_SET_SPEED:
      fsm_get_matlab_event(speed);
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->set_speed(speed);
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_ONESTEP:
      fsm_get_matlab_event(dir);
      fsm_get_matlab_event(style);
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->onestep(dir, style);
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_STEP:
      fsm_get_matlab_event(g_split_msg, 2);
      fsm_get_matlab_event(dir);
      fsm_get_matlab_event(style);
      steps_16 = (g_split_msg[0] << 8) | (g_split_msg[1]);
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->step(steps_16, dir, style);
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_STOP:
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->stop();
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_IS_MOVING:
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->is_moving());
      fsm_add_event(EV_BACK);
      break;
    case EV_ADAFRUIT_STEPPER_CONTINUE:
      pt_ob = (Adafruit_Stepper_motor *)fsm_get_controller(g_index);
      fsm_get_matlab_event(speed);
      fsm_get_matlab_event(dir);
      pt_ob->a_continue(speed, dir);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_adafruit_stepper */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro sonar */
/***********************************************************************************************************/
void states_command_sonar(fsm_t &fsm, uint8_t event){
  uint16_t distance = 0;
  uint8_t bits      = 0;      // Slouzi pro orezani hornich a dolnich 8 bitu
  Sonar *pt_ob      = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(EV_GET_DISTANCE);
      break;
    case EV_GET_DISTANCE:
      pt_ob = (Sonar *)fsm_get_controller(g_index);
      distance = pt_ob->get_distance();
      bits = distance >> 8;     // Hornich 8 bitu
      controllers_send_msg(bits);
      bits = distance & (0xFF); // Dolnich 8 bitu
      controllers_send_msg(bits);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_sonar */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro digitalni senzor */
/***********************************************************************************************************/
void states_command_digital_sensor(fsm_t &fsm, uint8_t event){
  Digital_sensor *pt_ob = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(EV_CHECK_DIGITAL_INPUT);
      break;
    case EV_CHECK_DIGITAL_INPUT:
      pt_ob = (Digital_sensor *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->check_digital_input());
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_digital_sensor */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro mikrofon */
/***********************************************************************************************************/
void states_command_microphone(fsm_t &fsm, uint8_t event){
  Microphone *pt_ob = NULL;
  uint8_t amp       = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_IS_DETECTED_SOUND:
      pt_ob = (Microphone *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->is_detected_sound());
      fsm_add_event(EV_BACK);
      break;
    case EV_GET_AMPLITUDE:
      pt_ob = (Microphone *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->get_amplitude());
      fsm_add_event(EV_BACK);
      break;
    case EV_SET_AMPLITUDE:
      pt_ob = (Microphone *)fsm_get_controller(g_index);
      fsm_get_matlab_event(amp);
      pt_ob->set_amplitude(amp);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_microphone */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro DC motor */
/***********************************************************************************************************/
void states_command_DC_motor(fsm_t &fsm, uint8_t event){
  DC_motor *pt_ob = NULL;
  uint8_t speed   = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_DC_SET_SPEED:
      fsm_get_matlab_event(speed);
      pt_ob = (DC_motor *)fsm_get_controller(g_index);
      pt_ob->set_speed(speed);
      fsm_add_event(EV_BACK);
      break;
    case EV_DC_CHANGE_DIR:
      pt_ob = (DC_motor *)fsm_get_controller(g_index);
      pt_ob->change_dir();
      fsm_add_event(EV_BACK);
      break;
    case EV_DC_RUN:
      pt_ob = (DC_motor *)fsm_get_controller(g_index);
      pt_ob->run();
      fsm_add_event(EV_BACK);
      break;
    case EV_DC_STOP:
      pt_ob = (DC_motor *)fsm_get_controller(g_index);
      pt_ob->stop();
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_DC_motor*/

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz Encoder */
/***********************************************************************************************************/
void states_command_encoder(fsm_t &fsm, uint8_t event){
  Encoder *pt_ob = NULL;
  uint16_t holes = 0;
  uint16_t speed = 0;
  uint8_t bits   = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_ENCODER_GET_SPEED:
      pt_ob = (Encoder *)fsm_get_controller(g_index);
      speed = pt_ob->get_rpm_speed();
      bits = speed >> 8;
      controllers_send_msg(bits); // Odeslani hornich 8 bitu
      bits = speed & 0xFF;
      controllers_send_msg(bits); // Odeslani dolnich 8 bitu
      fsm_add_event(EV_BACK);
      break;
    case EV_ENCODER_GET_HOLES:
      pt_ob = (Encoder *)fsm_get_controller(g_index);
      holes = pt_ob->get_holes();
      pt_ob->reset_holes();
      bits = holes >> 8;
      controllers_send_msg(bits); // Odeslani hornich 8 bitu
      bits = holes & 0xFF;
      controllers_send_msg(bits); // Odeslani dolnich 8 bitu
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_encoder */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro krokovy motor rizeny vlastnim driverem */
/***********************************************************************************************************/
void states_command_driver_stepper(fsm_t &fsm, uint8_t event){
  Driver_Stepper_motor *pt_ob = NULL;
  uint8_t speed               = 0;
  uint8_t dir                 = 0;
  uint16_t steps              = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_DRIVER_STEPPER_SET_SPEED:
      fsm_get_matlab_event(speed);
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->set_speed(speed);
      fsm_add_event(EV_BACK);
      break;
    case EV_DRIVER_STEPPER_ONESTEP:
      fsm_get_matlab_event(dir);
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->onestep(dir);
      fsm_add_event(EV_BACK);
      break;
    case EV_DRIVER_STEPPER_STEP:
      fsm_get_matlab_event(g_split_msg, 2);
      fsm_get_matlab_event(dir);
      steps = (g_split_msg[0] << 8) | g_split_msg[1];
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->step(steps, dir);
      fsm_add_event(EV_BACK);
      break;
    case EV_DRIVER_STEPPER_STOP:
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      pt_ob->stop();
      fsm_add_event(EV_BACK);
      break;
    case EV_DRIVER_STEPPER_IS_MOVING:
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->is_moving());
      fsm_add_event(EV_BACK);
      break;
    case EV_DRIVER_STEPPER_CONTINUE:
      pt_ob = (Driver_Stepper_motor *)fsm_get_controller(g_index);
      fsm_get_matlab_event(speed);
      fsm_get_matlab_event(dir);
      pt_ob->d_continue(speed, dir);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break; 
  }
} /* states_command_driver_stepper */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro senzor naklonu */
/***********************************************************************************************************/
void states_command_gyroscope(fsm_t &fsm, uint8_t event){
  Gyroscope *pt_ob = NULL;
  uint8_t angles[GYRO_ANGLES_ARR_SIZE];
  uint8_t acc[GYRO_ACC_ARR_SIZE];

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_GYROSCOPE_GET_ANGLES:
      pt_ob  = (Gyroscope *)fsm_get_controller(g_index);
      pt_ob->get_angles(angles);

      for(uint8_t i = 0; i < GYRO_ANGLES_ARR_SIZE; i++)
        controllers_send_msg( *(angles + i) );

      fsm_add_event(EV_BACK);
      break;
    case EV_GYROSCOPE_CALIBRATE:
      pt_ob = (Gyroscope *)fsm_get_controller(g_index);
      pt_ob->calibrate();
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    case EV_GYROSCOPE_GET_ACC:
      pt_ob  = (Gyroscope *)fsm_get_controller(g_index);
      pt_ob->get_acc(acc);

      for(uint8_t i = 0; i < GYRO_ACC_ARR_SIZE; i++)
        controllers_send_msg( *(acc + i) );

      fsm_add_event(EV_BACK);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_gyroscope */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro RGB senzor */
/***********************************************************************************************************/
void states_command_rgb_sensor(fsm_t &fsm, uint8_t event){
  RGB_sensor *pt_ob  = NULL;
  uint16_t colors[3] = {0, 0, 0}; // Pole pro ulozeni R slozky, G slozky a B slozky
  uint8_t  bits      = 0;         // Promenna pro rozdeleni hornich a dolnich bitu

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_RGB_SENSOR_GET_COLORS:
      pt_ob = (RGB_sensor *)fsm_get_controller(g_index);
      pt_ob->get_rgb_colors(colors[0], colors[1], colors[2]);

      for(uint8_t i = 0; i < 3; i++){
        bits = colors[i] >> 8;       // Horni bity
        controllers_send_msg(bits);
        bits = colors[i] & 0xFF;     // Dolni bity
        controllers_send_msg(bits);
      }

      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_rgb_sensor */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro tlacitko */
/***********************************************************************************************************/
void states_command_button(fsm_t &fsm, uint8_t event){
  Button *pt_ob = NULL;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_BUTTON_IS_PUSHED:
      pt_ob = (Button *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->is_pushed());
      fsm_add_event(EV_BACK);
      break;
    case EV_BUTTON_GET_RISING_EDGE:
      pt_ob = (Button *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->get_rising_edge());
      fsm_add_event(EV_BACK);
      break;
    case EV_BUTTON_GET_FALLING_EDGE:
      pt_ob = (Button *)fsm_get_controller(g_index);
      controllers_send_msg(pt_ob->get_falling_edge());
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_button */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro PWM_output */
/***********************************************************************************************************/
void states_command_pwm_output(fsm_t &fsm, uint8_t event){
  PWM_output *pt_ob = NULL;
  uint8_t strida = 255;
  
  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_SET_PWM_S:
      fsm_get_matlab_event(strida);
      pt_ob = (PWM_output *)fsm_get_controller(g_index);
      pt_ob->analog_s(strida); //(strida)
      fsm_add_event(EV_BACK);
      break;

    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_digital_output */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro Tensor_module */
/***********************************************************************************************************/
void states_command_tensor_module(fsm_t &fsm, uint8_t event){
  Tensor_module *pt_ob = NULL;
  uint8_t mass[4];
  float scale;
  
  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;

    case EV_TENSOR_ZERO:
      pt_ob = (Tensor_module *)fsm_get_controller(g_index);
      pt_ob->set_zero(); //
      fsm_add_event(EV_BACK);
      break;

    case EV_TENSOR_SET_SCALE:
      union {
        float   scale;             // vaha
        uint8_t byte[4]; // Byte na dane pozici 
      } scale_temp;
      fsm_get_matlab_event(g_split_msg, 4);
      scale_temp.byte[0] = g_split_msg[3];
      scale_temp.byte[1] = g_split_msg[2];
      scale_temp.byte[2] = g_split_msg[1];
      scale_temp.byte[3] = g_split_msg[0];

      scale = scale_temp.scale;//-2.767;
      pt_ob = (Tensor_module *)fsm_get_controller(g_index);
      pt_ob->set_scale(scale); 
      fsm_add_event(EV_BACK);
      break;

    case EV_TENSOR_GET_VALUE :
      pt_ob = (Tensor_module *)fsm_get_controller(g_index);
      pt_ob->get_mass(mass);
      for(uint8_t i = 0; i < 4; i++)
        controllers_send_msg( *(mass + i) );
        
      fsm_add_event(EV_BACK);
      break;

    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_tensor_module */


/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro Lidar */
/***********************************************************************************************************/
void states_command_lidar(fsm_t &fsm, uint8_t event){
  Lidar *pt_ob = NULL;
  uint8_t dist[FLOAT_SIZE];
  uint8_t mode;
  
  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;

    case EV_LIDAR_GET_DIST :
      pt_ob = (Lidar *)fsm_get_controller(g_index);
      pt_ob->get_dist(dist);
      for(uint8_t i = 0; i < FLOAT_SIZE; i++)
        controllers_send_msg( *(dist + i) );
        
      fsm_add_event(EV_BACK);
      break;

    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_lidar */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz Magnetic_encoder */
/***********************************************************************************************************/
void states_command_magnetic_encoder(fsm_t &fsm, uint8_t event){
  Magnetic_encoder *pt_ob = NULL;
  uint16_t angle = 0;
  uint8_t bits   = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_MAG_ENC_ANGLE:
      pt_ob = (Magnetic_encoder *)fsm_get_controller(g_index);
      angle = pt_ob->get_angle();
      bits = angle >> 8;
      controllers_send_msg(bits); // Odeslani hornich 8 bitu
      bits = angle & 0xFF;
      controllers_send_msg(bits); // Odeslani dolnich 8 bitu
      fsm_add_event(EV_BACK);
      break;
    case EV_MAG_ENC_ANGLE_SUM:
      pt_ob = (Magnetic_encoder *)fsm_get_controller(g_index);
      angle = pt_ob->get_sum_angle();
      bits = angle >> 8;
      controllers_send_msg(bits); // Odeslani hornich 8 bitu
      bits = angle & 0xFF;
      controllers_send_msg(bits); // Odeslani dolnich 8 bitu
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_magnetic_encoder */

/***********************************************************************************************************/
/* Stav, ve kterem bude vykonan prikaz pro digitalni senzor */
/***********************************************************************************************************/
void states_command_servo(fsm_t &fsm, uint8_t event){
  Servo_mech *pt_ob = NULL;
  uint8_t poss = 0;

  switch(event){
    case EV_ENTRY:
      fsm_add_event(g_command);
      break;
    case EV_SERVO_SET_POS:
      fsm_get_matlab_event(poss);
      pt_ob = (Servo_mech *)fsm_get_controller(g_index);
      pt_ob->set_angle(poss);
      fsm_add_event(EV_BACK);
      break;
    case EV_BACK:
      fsm_transition(fsm, &states_find_action);
      break;
    default:
      fsm_add_event(EV_BACK);
      break;
  }
} /* states_command_digital_sensor */
