/* controllers.cpp - inicializuje metody definovane v knihovne "mechlab_lib.h"
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 27.10.2023
*/

#include "controllers.h"

/* Inicializace staticke promene tridy Controller */
uint8_t Controller::num = 0; // Deklarace statickeho clena tridy Controller reprezentujiciho pocet doposud vytvorenych objektu

/* Deklarace a inicializace statickych promennych tridy Adafruit_Motor */
Adafruit_MotorShield Adafruit_Motor::AFMS;
uint8_t Adafruit_Motor::is_afms_init = 0;

/* Deklarace statickych promennych tridy Adafruit_Stepper_motor */
uint8_t Adafruit_Stepper_motor::stepper_on[MAX_ADAFRUIT_STEPPER_COUNT];

/* Deklarace a inicializace statickych promennych tridy Driver_Stepper_motor */
uint8_t Driver_Stepper_motor::stepper_on[MAX_DRIVER_STEPPER_COUNT];
uint8_t Driver_Stepper_motor::steppers_count = 0;

/* Globalni promenne */
static EthernetClient *PT_CLIENT = NULL;  // Klient pro pripadnou komunikaci se serverem
static volatile Sonar *g_pt_sonar;        // Ukazatel na objekt sonaru (kvuli interruptu pri mereni vzdalenosti)


/******************************************************************************/
/* Funkce slouzici k odeslani zpravy do matlabu o provedeni prikazu
 * msg - zprava k odeslani do matlabu
*/
/******************************************************************************/
void controllers_send_msg(uint8_t msg){
  if(PT_CLIENT && PT_CLIENT->connected())
    PT_CLIENT->write(msg);
  else if(!PT_CLIENT)
    uart_send(msg);
} /* controllers_send_msg uint8_t */

/******************************************************************************/
/* Funkce nastavujici PT_CLIENT
 * client - ukazatel na klienta, ktery se ulozi do PT_CLIENT
 */
/******************************************************************************/
void controllers_set_client(EthernetClient &client){
  PT_CLIENT = &client;
} /* controllers_set_client */

/****************************************************************************************************/
/* Funkce slouzici jako interrupt pri mereni vzdalenosti sonarem */
/****************************************************************************************************/
void controllers_echo_check(void){
  if(g_pt_sonar->check_timer()){
    uint16_t distance = g_pt_sonar->get_ping_result() / US_ROUNDTRIP_CM;
    g_pt_sonar->set_distance(distance);
  }
} /* controllers_echo_check */

/*****************************************************************************************************************************************************************/
/* Konstruktor tridy Digital_output - vytvori novy objekt typu Digital_output a zaregistruje pin, ke kteremu je pripojena LED dioda a ulozi typ tohoto objektu 
 * client      - ukazatel na objekt klienta, pomoci ktereho lze komunikovat pres ethernet
 * pin         - pin, ke kteremu je pripojena LED dioda
 */
 /****************************************************************************************************************************************************************/
Digital_output::Digital_output(uint8_t pin) : Controller(Controller::DIGITAL_OUTPUT){
  this->pin     = pin;
  this->myIndex = get_num();

  pinMode(pin, OUTPUT);
} /* Digital_output::Digital_output */

/******************************************************************************************************************************************************/
/* Konstruktor tridy Adafruit_Stepper_motor - vytvori novy objekt typu Adaftuit_Stepper_motor a zaregistruje port, 
 * ke kteremu je motor pripojen  a ulozi typ tohoto objektu
 * client      - objekt klienta slouzici k TCP/IP komunikaci
 * port        - port na motorshieldu  ke kteremu  je motor pripojen (muze byt 1 nebo 2)
 * spr         - pocet kroku na jednu otacku (steps per rotation, musi byt zapocitana i prevodovka)
 */
 /******************************************************************************************************************************************************/
Adafruit_Stepper_motor::Adafruit_Stepper_motor(uint8_t port, uint16_t spr) : Controller(Controller::ADAFRUIT_STEPPER_MOTOR){

  if(!is_afms_init){
    AFMS.begin();
    is_afms_init = 1;
  }

  this->port           = port;
  this->spr            = spr;
  this->myIndex        = get_num();  
  this->steps          = 0;
  this->dir            = FORWARD;
  this->style          = DOUBLE;
  this->speed_freq     = MAX_ADAFRUIT_STEPPER_TICKS;
  this->millis_counter = 0;
  this->pt_myMotor     = AFMS.getStepper(spr, port);

  stepper_on[port-1] = OFF;

  timer_set_stepper(this);
} /* Adaftuit_Stepper_motor::Adafruit_Stepper_motor */

/*************************************************************************************************************************************/
/* Metoda otacejici motorem o pozadovany pocet kroku s nastavenymi otackami za minutu
 * steps - pocet kroku, o ktery se ma motor otocit
 * dir   - smer, ve kterem se ma motor otocit (FORWARD, BACKWARD)
 * style - styl kroku, ktery bude pouzit (SINGLE, DOUBLE, INTERLEAVE, MICROSTEP), implicitne nastaveny na SINGLE
 */
/*************************************************************************************************************************************/
void Adafruit_Stepper_motor::step(uint16_t steps, uint8_t dir, uint8_t style = DOUBLE) {
  this->steps = steps;
  this->dir   = dir;
  this->style = style;

  stepper_on[port-1] = ON;
  TMR1_ON;

} /* Adafruit_Stepper_motor::step */

/*************************************************************************************************************************************/
/* Metoda volana z interruptu - udela jeden krok v pozadovanem case */
/*************************************************************************************************************************************/
void Adafruit_Stepper_motor::tick(void){
  if(++millis_counter == speed_freq){
    millis_counter = 0;
    sei();
    onestep(dir, style);

    if(!(--steps))
      stepper_on[port-1] = OFF;
  }
} /* Adafruit_Stepper_motor::tick */

/*********************************************************************************************************************************************/
/* Metoda slouzici pro pokracovani otaceni motoru a pripadne nastavujici nove parametry
 * speed - nova rychlost
 * dir   - novy smer
*/
/*********************************************************************************************************************************************/
void Adafruit_Stepper_motor::a_continue(uint8_t speed, uint8_t dir){
  if(!steps)
    return;
  
  this->set_speed(speed);
  this->dir = dir;
  this->stepper_on[port-1] = ON;
  TMR1_ON;
} /* Adafruit_Stepper_motor::a_continue */

/****************************************************************************************************/
/* Inicializuje promennou p_sonar (vytvori novy objekt)
   trig_pin - pin na ktery je pripojen trigger
   echo_pin - pin na ktery je pripojen echo
*/
/****************************************************************************************************/
Sonar::Sonar(uint8_t trig_pin, uint8_t echo_pin) : Controller(Controller::SONAR){
  pinMode(trig_pin, INPUT);
  pinMode(echo_pin, INPUT);

  myIndex = get_num();

  p_sonar = NULL;
  p_sonar = new NewPing(trig_pin, echo_pin, MAX_SONAR_DISTANCE);
  
  if(!p_sonar){
    controllers_send_msg(ALLOCATION_ERROR);
    return;
  }

  g_pt_sonar = this;
} /* Sonar::Sonar */

/****************************************************************************************************/
/* Metoda merici vzdalenost v centimetrech, kterou nasledne vrati 
 * Pokud vzdalenost namerit nelze, vrati 0
 */
/****************************************************************************************************/
uint16_t Sonar::get_distance(void){
  p_sonar->ping_timer(controllers_echo_check);

  return distance;
} /* Sonar::get_distance */

/***************************************************************************************************/
/* Ulozi cislo pinu, na kterem je pripojen senzor a zaregistruje tento pin */
/***************************************************************************************************/
Digital_sensor::Digital_sensor(uint8_t pin) : Controller(Controller::DIGITAL_SENSOR){
  this->pin     = pin;
  this->myIndex = get_num();

  pinMode(pin, INPUT);
} /*Digital_sensor::Digital_sensor */

/******************************************************************************************************/
/* Konstuktor - ulozi cislo analogoveho pinu, na ktery je mikrofon pripojen a ulozi 
 * index vytvoreneho ovladace v poli ovladacu
 * pin - analogovy pin, na ktery je pripojen mikrofon
 */
/******************************************************************************************************/
Microphone::Microphone(uint8_t pin) : Controller(Controller::MICROPHONE){
  this->analog_pin          = pin;
  this->myIndex             = get_num();
  this->min_sound_amplitude = MIN_SOUND_AMPLITUDE;
} /* Microphone::Microphone */

/******************************************************************************************************/
/* Metoda vracejici hodnotu 1, pokud byl detekovan zvuk, jinak vraci 0*/
/******************************************************************************************************/
uint8_t Microphone::is_detected_sound(void){
  uint8_t mic = get_amplitude();

  if(mic > min_sound_amplitude)
    return SUCCESS;

  return FAIL;
} /* Microphone::is_detected_sound */

/******************************************************************************************************/
/* Metoda vracejici hodnotu amplitudy zvukoveho signalu */
/******************************************************************************************************/
uint8_t Microphone::get_amplitude(void){
  int16_t mic       = analogRead(analog_pin);
  int16_t amplitude = abs(mic - BASELINE);

  if(amplitude > MAX_SOUND_AMPLITUDE)
    return MAX_SOUND_AMPLITUDE;

  return (uint8_t)amplitude;
} /* Microphone::get_amplitude */

/******************************************************************************************************/
/* Konstuktor - vytvori a novy ovladac DC motoru a ulozi
 * index vytvoreneho ovladace v poli ovladacu
 * port - port na motorshieldu, kde je pripojen motor M1=1, M2=2, M3=3, M4=4
 */
 /******************************************************************************************************/
DC_motor::DC_motor(uint8_t port) : Controller(Controller::DC_MOTOR) {
  if(!is_afms_init){
    AFMS.begin();
    is_afms_init = 1;
  }

  this->myIndex        = get_num();
  this->dir            = FORWARD;
  this->is_stepper_off = OFF;
  this->pt_myMotor     = AFMS.getMotor(port);
} /* DC_motor::DC_motor */

/******************************************************************************************************/
/* Konstuktor - vytvori a novy ovladac encoderu a ulozi
 * index vytvoreneho ovladace v poli ovladacu
 * pin - digitalni pin, na kterem je pripojen encoder
 */
/******************************************************************************************************/
Encoder::Encoder(uint8_t pin) : Controller(Controller::ENCODER) {
  this->pin = pin;
  this->speed = this->init = this->time = this->acc = 0;
  this->myIndex = get_num();
  
  pinMode(pin, INPUT);
  
  timer_set_encoder(this);
  TMR1_ON;
} /* Encoder::Encoder */

/******************************************************************************************************/
/* Metoda slouzici pro vypocet rychlosti */
/******************************************************************************************************/
void Encoder::tick(void){
  acc <<= 1;
  acc |= digitalRead(pin);
  
  uint16_t act_time = millis();
  uint16_t delta_millis_time = act_time - time;

  if(acc == RISING_EDGE){
    holes ++;
    float delta_time = (float)delta_millis_time * 0.001;

    if(init)
      speed = (ENCODER_DELTA_HOLES / delta_time) * 60;
    else 
      init = 1;
    
    time = millis();
  
  } 
  else if(delta_millis_time > ENCODER_MIN_DETECTED_SPEED)
    speed = 0;

} /* Encoder::tick */



/************************************************************************************************************/
/* Konstruktor - zaregistruje piny pripojene na driver a pripravi 
 * motor k ovladani (inicializuje vsechny promenne)
 * pin1, pin2, pin3, pin4 - piny, na ktere je pripojen driver
 */
/************************************************************************************************************/
Driver_Stepper_motor::Driver_Stepper_motor(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) : Controller(Controller::DRIVER_STEPPER_MOTOR){
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);

  this->pin1 = pin1;
  this->pin2 = pin2;
  this->pin3 = pin3;
  this->pin4 = pin4;

  this->my_number = steppers_count++;
  this->my_index  = get_num();

  this->step_number    = 0;
  this->millis_counter = 0;
  this->steps          = 0;
  this->dir            = DRIVER_STEPPER_FORWARD;
  this->speed_freq     = MIN_DRIVER_STEPPER_TICKS;

  stepper_on[my_number] = OFF;

  timer_set_stepper(this);
} /* Driver_Stepper_motor::Driver_stepper_motor */

/************************************************************************************************************/
/* Metoda otacejici motorem o jeden krok
 * dir - smer, ve kterem ma byt motor otocen 
 */
/************************************************************************************************************/
void Driver_Stepper_motor::onestep(uint8_t dir){
  if(dir){
    switch(step_number){
      case 0:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        break;
      case 1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        break;
      case 2:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        digitalWrite(pin4, LOW);
        break;
      case 3:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, HIGH);
        break;
    }
  }
  else{
    switch(step_number){
      case 0:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, HIGH);
        break;
      case 1:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, HIGH);
        digitalWrite(pin4, LOW);
        break;
      case 2:
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        break;
      case 3:
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        digitalWrite(pin3, LOW);
        digitalWrite(pin4, LOW);
        break;
    }
  }

  step_number = (++step_number & DRIVER_STEPPER_KEY);

} /* Driver_Stepper_motor::onestep */

/************************************************************************************************************/
/* Metoda otacejici motorem o zadany pocet kroku
 * steps - pocet kroku, o ktere ma byt motor otocen
 * dir   - smer, ve kterem ma byt motor otocen 
 */
/************************************************************************************************************/
void Driver_Stepper_motor::step(uint16_t steps, uint8_t dir){
  this->steps = steps;
  this->dir   = dir;
  stepper_on[my_number] = ON;
  TMR1_ON;
} /* Driver_stepper_motor::step */

/************************************************************************************************************/
/* Metoda volana z interruptu - udela jeden krok v pozadovanem case  */
/************************************************************************************************************/
void Driver_Stepper_motor::tick(void){
  if(++millis_counter == speed_freq){
    millis_counter = 0;
    onestep(dir);

    if(!(--steps))
      stepper_on[my_number] = OFF;
  }
} /* Driver_Stepper_motor::tick */

/************************************************************************************************************/
/* Metoda pokracujici v otaceni motoru a pripadne nastavujici nove parametry 
 * speed - nova rychlost
 * dir   - novy smer
*/
/************************************************************************************************************/
void Driver_Stepper_motor::d_continue(uint8_t speed, uint8_t dir){
  if(!steps)
    return;
  
  set_speed(speed);
  this->dir = dir;
  stepper_on[my_number] = ON;
  TMR1_ON;
} /* Driver_Stepper_motor::d_continue */

/************************************************************************************************************/
/* Konstruktor - zahaji i2c komunikaci a nastavi potrbne udaje v gyroskopu a akcelerometru */
/************************************************************************************************************/
Gyroscope::Gyroscope(void) : Controller(Controller::GYROSCOPE){
  /* Vypnuti krokovych motoru na Adafruit motorshield */ 
  uint8_t stepper_off = 0;
  if(TCCR1B & (1 << CS10)){
    timer_adafruit_stepper_stop();
    stepper_off = 1;
  }

  /* Inicializace i2c */ 
  if(!TWCR) // Kontrola, jestli jiz neprobehla inicializace
    Wire.begin(MPU);

  /* Nastaveni senzoru */
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(150);

  accX         = accY  = accZ  = 0;
  gyroX        = gyroY = gyroZ = 0;
  accAngleX    = accAngleY     = 0;
  gyroAngleX   = gyroAngleY    = gyroAngleZ = 0;
  accErrorX    = accErrorY     = 0;
  gyroErrorX   = gyroErrorY    = gyroErrorZ = 0;
  roll         = pitch         = yaw        = 0;
  previousTime = 0;
  currentTime  = millis();

  my_index = get_num();

  timer_set_gyro(this);
  delay(150);

  TMR1_ON;

  if(stepper_off)
    timer_adafruit_stepper_continue();
} /* Gyroscope::Gyroscope */

/************************************************************************************************************/
/* Metoda pro vypocet a ulozeni aktualniho naklonu gyroskopu */
/************************************************************************************************************/
void Gyroscope::set_angles(void){
  TMR1_OFF;

  /* Precteni dat z akcelerometru */
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  accX = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;
  accY = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;
  accZ = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;

  /* Vypocet uhlu z vytazenych dat z akcelerometru */
  accAngleX = (atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI)      - accErrorX;
  accAngleY = (atan(-1 * accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI) - accErrorY;

  previousTime      = currentTime;
  currentTime       = millis();
  float elapsedTime = (currentTime - previousTime) / 1000;

  /* Precteni dat z gyroskopu */
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);

  gyroX = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;
  gyroY = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;
  gyroZ = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;

  gyroX -= gyroErrorX;
  gyroY -= gyroErrorY;
  gyroZ -= gyroErrorZ;

  /* Vypocet uhlu z dat z gyroskopu */
  gyroAngleX += gyroX * elapsedTime;
  gyroAngleY += gyroY * elapsedTime;
  gyroAngleZ += gyroZ * elapsedTime;

  /* Vypocet vyslednych uhlu */
  roll  = gyro_part * gyroAngleX + acc_part * accAngleX;
  pitch = gyro_part * gyroAngleY + acc_part * accAngleY;
  yaw   = gyroAngleZ;

  TMR1_ON;
} /* Gyroscope::set_angles */

/************************************************************************************************************/
/* Metoda pro vraceni aktualniho naklonu gyroskopu - uhly budou vraceny jako pole uint8_t pro mozne odeslani
 * tri cisel typu float pres uart
*/
/************************************************************************************************************/
void Gyroscope::get_angles(uint8_t *arr){
  float   float_angles_arr[GYRO_ANGLES_COUNT] = {roll, pitch, yaw};
  uint8_t index = 0;

  for(uint8_t i = 0; i < GYRO_ANGLES_COUNT; i++){
    angle_temp.angle = float_angles_arr[i];

    for(uint8_t j = 0; j < FLOAT_SIZE; j++)
      *(arr + index++) = angle_temp.byte[j];
  }
} /* Gyroscope::get_angles */

/************************************************************************************************************/
/* Metoda pro vraceni aktualniho zrychleni - bude vracebo jako pole uint8_t pro mozne odeslani
 * tri cisel typu float pres uart
*/
/************************************************************************************************************/
void Gyroscope::get_acc(uint8_t *arr){
  float   float_acc_arr[ACC_COUNT] = {accX,accY,accZ};
  uint8_t index = 0;

  for(uint8_t i = 0; i < ACC_COUNT; i++){
    acc_temp.acc = float_acc_arr[i];

    for(uint8_t j = 0; j < FLOAT_SIZE; j++)
      *(arr + index++) = acc_temp.byte[j];
  }
} /* Gyroscope::get_angles */

/************************************************************************************************************/
/* Metoda slouzici ke kalibraci senzoru naklonu */
/************************************************************************************************************/
void Gyroscope::calibrate(void){
  /* Vypnuti krokovych motoru na Adafruit motorshield*/
  TMR1_OFF;
  sei();
  
  /* Opakovane vytazeni hodnot z akcelerometru pri nulovem naklonu (suma) */
  for(uint8_t i = 0; i < calib_count; i++){
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);

    accX = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;
    accY = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;
    accZ = (Wire.read() << 8 | Wire.read()) / lsb_acc_sensitivity;

    accErrorX += ((atan((accY) / sqrt(pow((accX), 2) + pow((accZ), 2))) * 180 / PI));
    accErrorY += ((atan(-1 * (accX) / sqrt(pow((accY), 2) + pow((accZ), 2))) * 180 / PI));
  }

  /* Zjisteni prumerne odchylky pri nulovem naklonu z akcelerometru */
  accErrorX /= calib_count;
  accErrorY /= calib_count;

  /* Opakovane vytazeni hodnot z gyroskopu pri nulovem naklonu (suma) */
  for(uint8_t i = 0; i < calib_count; i++) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);

    gyroX = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;
    gyroY = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;
    gyroZ = (Wire.read() << 8 | Wire.read()) / lsb_gyro_sensitivity;

    gyroErrorX += gyroX;
    gyroErrorY += gyroY;
    gyroErrorZ += gyroZ;
  }

  /* Zjisteni prumerne odchylky pri nulovem naklonu z gyroskopu */
  gyroErrorX /= calib_count;
  gyroErrorY /= calib_count;
  gyroErrorZ /= calib_count;

  TMR1_ON;
} /* Gyroscope::calibrate */

/******************************************************************************************************************************************/
/* Konstruktor - ulozi a zaregistruje piny a pripravi senzor k mereni
 * pinS0  - digitalni pin, na kterem je pripojen vyvod S0 ze senzoru
 * pinS1  - digitalni pin, na kterem je pripojen vyvod S1 ze senzoru
 * pinS2  - digitalni pin, na kterem je pripojen vyvod S2 ze senzoru
 * pinS3  - digitalni pin, na kterem je pripojen vyvod S3 ze senzoru
 * pinOut - digitalni pin, na kterem je pripojen vyvod OUT ze senzoru 
*/
/******************************************************************************************************************************************/
RGB_sensor::RGB_sensor(uint8_t pinS0, uint8_t pinS1, uint8_t pinS2, uint8_t pinS3, uint8_t pinOut) : Controller(Controller::RGB_SENSOR) {
  pinMode(pinS0, OUTPUT);
  pinMode(pinS1, OUTPUT);
  pinMode(pinS2, OUTPUT);
  pinMode(pinS3, OUTPUT);
  pinMode(pinOut, INPUT);
  
  this->pinS0   = pinS0;
  this->pinS1   = pinS1;
  this->pinS2   = pinS2;
  this->pinS3   = pinS3;
  this->pinOut  = pinOut;
  this->myIndex = get_num();

  red_freq = blue_freq = green_freq = 0;

  /* Output frequency scaling: 20% */
  digitalWrite(pinS0, HIGH);
  digitalWrite(pinS1, LOW);
} /* RGB_sensor::RGB_sensor */

/************************************************************************************************************/
/* Metoda slouzici k ziskani namerenych slozek cervene, modre a zelene barvy
 * red   - vracena namerena slozka cervene barvy
 * green - vracena namerena slozka zelene barvy
 * blue  - vracena namerena slozka modre barvy
*/
/************************************************************************************************************/
void RGB_sensor::get_rgb_colors(uint16_t &red, uint16_t &green, uint16_t &blue){
  /* Mereni cervene barvy */
  digitalWrite(pinS2,LOW);
  digitalWrite(pinS3,LOW);
  // delay(5); 

  /* Nacteni frekvence cervene barvy */
  red_freq = pulseIn(pinOut, LOW);

  /* Nastaveni frekvence zelene barvy */
  digitalWrite(pinS2,HIGH);
  digitalWrite(pinS3,HIGH);
  // delay(5);

  /* Nacteni frekvence zelene barvy */
  green_freq = pulseIn(pinOut, LOW);
  
  /* Mereni modre barvy */
  digitalWrite(pinS2,LOW);
  digitalWrite(pinS3,HIGH);
  // delay(5);

  /* Nacteni frekvence modre barvy */
  blue_freq = pulseIn(pinOut, LOW);

  /* Vracnei namerenych hodnot */
  red   = red_freq;
  green = green_freq;
  blue  = blue_freq;
} /* RGB_sensor::get_rgb_colors */

/************************************************************************************************************/
/* Konstruktor - ulozi a zaregistruje pin, inicializuje vsechny promenne a zapne casovac
 * pin - digitalni pin, na kterem je pripojen vyvod S0 ze senzoru
*/
/************************************************************************************************************/
Button::Button(uint8_t pin) : Controller(Controller::BUTTON) {
  pinMode(pin, INPUT_PULLUP);

  this->pin          = pin;
  this->state        = OFF;
  this->rising_edge = OFF;
  this->falling_edge = OFF;
  this->myIndex      = get_num();

  timer_set_button(this);

  TMR1_ON;
} /* Button::Button */

/************************************************************************************************************/
/* Metoda volana z interruptu - sleduje a uklada informace o aktualnim stavu tlacitka */
/************************************************************************************************************/
void Button::tick(void){
  acc <<= 1;
  acc |= digitalRead(pin);

  if(acc == RISING_EDGE){
    state        = OFF;
    rising_edge = ON;
  }
  else if(acc == FALLING_EDGE){
    state        = ON;
    falling_edge = ON;
  }
} /* Button::tick */

/************************************************************************************************************/
/* Metoda vracejici 1, pokud byla detekovana nabezna hrana */
/************************************************************************************************************/
uint8_t Button::get_rising_edge(void){
  uint8_t temp = rising_edge;
  rising_edge = OFF;
  return temp;
} /* Button::get_rising_edge */

/************************************************************************************************************/
/* Metoda vracejici 1, pokud byla detekovana sestupna hrana */
/************************************************************************************************************/
uint8_t Button::get_falling_edge(void){
  uint8_t temp = falling_edge;
  falling_edge = OFF;
  return temp;
} /* Button::get_falling_edge */

/*****************************************************************************************************************************************************************/
/* Konstruktor tridy PWM_output - vytvori novy objekt typu PWM_output a zaregistruje pin, ke kteremu je pripojena LED dioda a ulozi typ tohoto objektu 
 * client      - ukazatel na objekt klienta, pomoci ktereho lze komunikovat pres ethernet
 * pin         - pin, ke kteremu je pripojena LED dioda
 */
 /****************************************************************************************************************************************************************/
PWM_output::PWM_output(uint8_t pin) : Controller(Controller::PWM_OUTPUT){
  this->pin     = pin;
  this->myIndex = get_num();

  pinMode(pin, OUTPUT);
  analogWrite(pin, 0);
} /* PWM_output::PWM_output */

/*****************************************************************************************************************************************************************/
/* Konstruktor tridy Tensor_module- vytvori novy objekt typu Tensor_module a zaregistruje piny, ke kterym je pripojen a ulozi typ tohoto objektu 
 * pin_data/pin_CLK       - piny, ke kterým je pripojen Tensor modul
 */
 /****************************************************************************************************************************************************************/
Tensor_module::Tensor_module(uint8_t pin_data, uint8_t pin_clk) : Controller(Controller::TENSOR_MODULE){
  HX711 sensor; 

  this->pin_data = pin_data;
  this->pin_clk  = pin_clk;
  this->myIndex  = get_num();
  sensor.begin(pin_data, pin_clk); 
  delay(100);
  
} /* Tensor_module::Tensor_module */

/************************************************************************************************************/
/* Metoda vracejici 1, pokud byla detekovana sestupna hrana */
/************************************************************************************************************/
void Tensor_module::get_mass(uint8_t *arr){
  long reading = 0;
  int32_t intreading = 0;
  uint8_t index = 0;
  TMR1_OFF;
  if (sensor.is_ready()) {
    reading = sensor.get_units();
    intreading = (int32_t)reading;
    mass_temp.mass = intreading;
    for(uint8_t j = 0; j < 4; j++)
      *(arr + index++) = mass_temp.byte[j];
    }
  else{
    mass_temp.mass = 99999999;//intreading;
    for(uint8_t j = 0; j < 4; j++)
      *(arr + index++) = mass_temp.byte[j];
    }
  TMR1_ON;

} /* Tensor_module::get_mass */


/*****************************************************************************************************************************************************************/
/* Konstruktor tridy Lidar - vytvori novy objekt typu Lidar 
 */
 /****************************************************************************************************************************************************************/
Lidar::Lidar(void) : Controller(Controller::LIDAR){
  VL53L0X sensor;

  this->myIndex  = get_num();
  Wire.begin();
  sensor.init();
  sensor.startContinuous();
  
} /* Lidar::Lidar */

/************************************************************************************************************/
/* Metoda vracejici namerenou vzdalenost */
/************************************************************************************************************/
void Lidar::get_dist(uint8_t *arr){
  float reading = 0;
  uint8_t index = 0;
  
  reading = sensor.readRangeContinuousMillimeters();
  dist_temp.distance = reading;
  for(uint8_t j = 0; j < FLOAT_SIZE; j++)
    *(arr + index++) = dist_temp.byte[j];
    

} /* Lidar::get_dist */

/*****************************************************************************************************************************************************************/
/* Konstruktor tridy Magnetic_encoder- vytvori novy objekt typu Magnetic_encoder a ulozi typ tohoto objektu 
 */
 /****************************************************************************************************************************************************************/
Magnetic_encoder::Magnetic_encoder(void) : Controller(Controller::MAG_ENC){
  AS5600_PsW sensor;

  this->myIndex  = get_num();
  sensor.init();
  sensor.changePowerMode(01);

  
} /* Magnetic_encoder::Magnetic_encoder */

/************************************************************************************************************/
/* Metoda vracejici absolutní uhel natočení v rozsahu 0-4095 na 1 otáčku */
/************************************************************************************************************/
int16_t Magnetic_encoder::get_angle(void){
  int16_t result;
  result = sensor.rawAngle();
  return result;
} /* Tensor_module::get_angle */

/************************************************************************************************************/
/* Metoda vracejici pocet otoceni encoderu */
/************************************************************************************************************/
int16_t Magnetic_encoder::get_sum_angle(void){
  return this->rotations;
} /* Tensor_module::get_sum_angle */

/************************************************************************************************************/
/* Metoda vracejici absolutní uhel natočení v rozsahu 0-4095 na 1 otacku +- 8 otacek (-32,768 ~ 32,767) */
/************************************************************************************************************/
int16_t Magnetic_encoder::get_speed(void){
  return this->speed;
} /* Tensor_module::get_speed */

/************************************************************************************************************/
/* Metoda vracejici absolutní uhel natočení v rozsahu 0-4095 na 1 otacku +- 8 otacek (-32,768 ~ 32,767) */
/************************************************************************************************************/
int16_t Magnetic_encoder::get_accel(void){
  int16_t result;
  uint16_t delta_millis_time = time - prew_time;
  if(delta_millis_time==0)
    delta_millis_time = 1;
  result = (speed - prew_speed)/delta_millis_time;
  return result;
} /* Tensor_module::get_accel */

/******************************************************************************************************/
/* Metoda slouzici pro vypocet rychlosti */
/******************************************************************************************************/
void Magnetic_encoder::tick(void){
  TMR1_OFF;

  this->prew_time = this->time;
  this->time = millis();
  this->prew_angle = this->angle;
  this->angle = sensor.rawAngle();
  uint16_t delta_millis_time = this->time - this->prew_time;
  if(delta_millis_time==0)
    delta_millis_time = 1;
  uint16_t diff_angle = abs(this->prew_angle-this->angle);
  uint8_t n_change = 0;
  int8_t dir = 1; // 1 pro zvysujici uhel, -1 pro snizujici
  if(diff_angle>2048){
    n_change = 1;
    diff_angle = 4096 - diff_angle;
  }
  if(this->angle>this->prew_angle){
    dir = 1-2*n_change;
    }
  else{
    dir = -1+2*n_change;
    }

  this->prew_speed = this->speed;  
  this->speed = diff_angle / delta_millis_time;

  this->rotations = this->rotations + n_change*dir;
  TMR1_ON;

} /* Magnetic_encoder::tick */


/*****************************************************************************************************************************************************************/
/* Konstruktor tridy Digital_output - vytvori novy objekt typu Digital_output a zaregistruje pin, ke kteremu je pripojena LED dioda a ulozi typ tohoto objektu 
 * client      - ukazatel na objekt klienta, pomoci ktereho lze komunikovat pres ethernet
 * pin         - pin, ke kteremu je pripojena LED dioda
 */
 /****************************************************************************************************************************************************************/
Servo_mech::Servo_mech(uint8_t pin) : Controller(Controller::SERVO){
  this->pin     = pin;
  this->myIndex = get_num();
  myservo.attach(pin);  // attaches the servo on pin 9 to the Servo object

} /* Digital_output::Digital_output */
