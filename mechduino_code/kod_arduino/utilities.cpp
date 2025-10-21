/* utilities.cpp - inicializuje metody definovane v knihovne "utilities.h"
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 27.10.2023
*/

#include "utilities.h"

/* Globalni promenne */
static crc g_crc_table[256]; // tabulka hodnot crc pro kazde mozne bytove cislo

/***********************************************************************************************************************/
/* Funkce inicializujici CRC pro vsechna mozna bytova cisla (0 - 255) */
/***********************************************************************************************************************/
void utilities_crc_init(void){
  crc  remainder;

  // Vypocet vsech moznych zbytku po deleni
  for (uint16_t dividend = 0; dividend < 256; ++dividend)
  {      
     remainder = dividend << (WIDTH - 8);

    //Vypocet CRC pro konkretni cislo
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      if (remainder & TOPBIT)
        remainder = (remainder << 1) ^ POLYNOMIAL;
      else
        remainder = (remainder << 1);
    }

    g_crc_table[dividend] = remainder;
  }
} /* crc_init */

/***********************************************************************************************************************/
/* Funkce vracejici CRC zadane zpravy 
 * message - zprava, ktera se ma kontrolovat
 * nBytes  - pocet bytu, ktery se ma ve zprave kontrolovat
*/
/***********************************************************************************************************************/
crc utilities_get_crc(uint8_t const *message, uint8_t nBytes){
  uint8_t data;
  crc remainder = 0;

  for (uint8_t byte = 0; byte < nBytes; ++byte)
  {
    data = message[byte] ^ (remainder >> (WIDTH - 8));
    remainder = g_crc_table[data] ^ (remainder << 8);
  }

  return remainder;

} /* get_crc */

/***********************************************************************************************************************/
/* Funkce vracejici 1, pokud je hodnota CRC spravna, jinak vraci 0 
 * CRC         - zjistene CRC, ktere se bude kontrolovat
 * messgae_crc - CRC, ktere ma vyjit u spravne prijate zpravy
*/
/***********************************************************************************************************************/
uint8_t utilities_crc_check(crc CRC, uint8_t message_crc){
  if(CRC == message_crc)
    return SUCCESS;
  
  return FAIL;
} /* crc_check */

/***********************************************************************************************************************/
/* Konstruktor - alokuje pamet pole o pocatecni velikosti a nastavi pocatecni hodnoty clenskych promennych */
/***********************************************************************************************************************/
List::List(){

  // !!! VYRESIT PROBLEM S KONTROLOU ALOKACE !!!
  arr = NULL;
  
  arr = new Controller * [INIT_SIZE];
  if(!arr){
    controllers_send_msg(ALLOCATION_ERROR);
    return;
  }

  size = INIT_SIZE;
  last_index = 0;
} /* List */


/***********************************************************************************************************************/
/* Destruktor - uvolni alokovanou pamet objektu a alokovanou pamet pole */
/***********************************************************************************************************************/
List::~List(){
  Digital_output           *digital_output         = NULL;
  Adafruit_Stepper_motor   *adafruit_stepper_motor = NULL;
  Sonar                    *sonar                  = NULL;
  Digital_sensor           *digital_sensor         = NULL;
  Microphone               *microphone             = NULL;
  DC_motor                 *dc_motor               = NULL;
  Encoder                  *encoder                = NULL;
  Driver_Stepper_motor     *driver_stepper_motor   = NULL;
  Gyroscope                *gyroscope              = NULL;
  RGB_sensor               *rgb_sensor             = NULL;
  Button                   *button                 = NULL;
  PWM_output               *pwm_output             = NULL;
  Tensor_module            *tensor_module          = NULL;
  Lidar                    *lidar                  = NULL;
  Magnetic_encoder         *magnetic_encoder       = NULL;
  Servo_mech               *servo                  = NULL;


  //Uvolni pameti kde jsou ulozeny objekty
  for(uint8_t i = 0; i < last_index; i++){
    switch(  (*(arr + i))->get_type() ){
      case Controller::DIGITAL_OUTPUT:
        digital_output = (Digital_output *) *(arr + i);
        delete digital_output;
        break;
      case Controller::ADAFRUIT_STEPPER_MOTOR:
        adafruit_stepper_motor = (Adafruit_Stepper_motor *) *(arr + i);
        delete adafruit_stepper_motor;
        break;
      case Controller::SONAR:
        sonar = (Sonar *) *(arr + i);
        delete sonar;
        break;
      case Controller::DIGITAL_SENSOR:
        digital_sensor = (Digital_sensor *) *(arr + i);
        delete digital_sensor;
        break;
      case Controller::MICROPHONE:
        microphone = (Microphone *) *(arr + i);
        delete microphone;
        break;
      case Controller::DC_MOTOR:
        dc_motor = (DC_motor *) *(arr + i);
        delete dc_motor;
        break;
      case Controller::ENCODER:
        encoder = (Encoder *) *(arr + i);
        delete encoder;
        break;
      case Controller::DRIVER_STEPPER_MOTOR:
        driver_stepper_motor = (Driver_Stepper_motor *) *(arr + i);
        delete driver_stepper_motor;
        break;
      case Controller::GYROSCOPE:
        gyroscope = (Gyroscope *) *(arr + i);
        delete gyroscope;
        break;
      case Controller::RGB_SENSOR:
        rgb_sensor = (RGB_sensor *) *(arr + i);
        delete rgb_sensor;
        break;
      case Controller::BUTTON:
        button = (Button *) *(arr + i);
        delete button;
        break;
      case Controller::PWM_OUTPUT:
        pwm_output = (PWM_output *) *(arr + i);
        delete pwm_output;
        break;
      case Controller::TENSOR_MODULE:
        tensor_module = (Tensor_module *) *(arr + i);
        delete tensor_module;
        break;
      case Controller::LIDAR:
        lidar = (Lidar *) *(arr + i);
        delete lidar;
        break;
      case Controller::MAG_ENC:
        magnetic_encoder = (Magnetic_encoder *) *(arr + i);
        delete magnetic_encoder;
        break;
      case Controller::SERVO:
        servo = (Servo_mech *) *(arr + i);
        delete servo;
        break;
    } //switch
  } //for

  //Uvolneni pole s odkazy
  delete [] arr;
} /* ~List */

/***********************************************************************************************************************/
/* Metoda pridavajici novy prvek do seznamu, v pripade kdy bude pole zaplneno, alokuje novou pamet pro pole
 * ob - ukazatel na objekt, ktery ma byt pridan do seznamu
 */
/***********************************************************************************************************************/
void List::add(Controller *ob){
  // Kontrola preteceni pole - pokud by melo dojit k preteceni, alokuje novou pamet
  if(last_index > size){
    //Alokace nove pameti - !!! VYRESIT CHYBU ALOKACE !!!
    Controller **new_arr = NULL;

    new_arr = new Controller * [size + INCREMENT];
    if(!new_arr){
      controllers_send_msg(ALLOCATION_ERROR);
      return;
    }

    size += INCREMENT;

    //Kopirovani pole
    for(uint8_t i = 0; i < last_index; i++)
      *(new_arr + i) = *(arr + i);
    
    delete [] arr;
    arr = new_arr;
  }

  *(arr + last_index++) = ob;
} /* add */

/***********************************************************************************************************************/
/* Pretizeni operatoru [] - vraci ukazatel na objekt ulozeny na dane pozici
 * num - pozice, ze ktere se ma vratit ukazatel
 */
/***********************************************************************************************************************/
Controller *List::operator[](uint8_t num){
  //Kontrola mezi
  if(num < 0 || num >= last_index)
    return NULL;

  return *(arr + num);
} /* operator[] */