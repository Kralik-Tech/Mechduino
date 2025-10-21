/* controllers.h - obsahuje tridy (ovladace) slouzici k ovladani ovladacich prvku pripojenych k arduinu
 * DULEZITE!!! - VSECHNY ZDE VYTVORENE TRIDY MUSI DEDIT TRIDU Controller
 * Z duvodu ukladani vsech ukazatelu na objekty ruznych trid do jednoho pole.
 * Pri volani konstruktoru techto trid musi byt take zavolan konstruktor tridy Controller do ktereho se ulozi typ teto tridy (vycet).
 *
 * Vsechny tridy zde MUSI DEFINOVAT metodu "uint8_t get_my_index(void)" vracejici poradove cislo objektu (jeho index v poli).
 *
 * Autor:         Jakub Smelik
 * Vytvoreno dne: 24.10.2023
*/

#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include <SPI.h>
#include <UIPEthernet.h>
#include <Adafruit_MotorShield.h>
#include "Wire.h"
#include <stdint.h>
#include <NewPing.h>
#include <VL53L0X.h>
#include <HX711.h>
#include <AS5600_PsW.h>
#include "uart.h"
#include "fsm.h"
#include "timer.h"
#include "Arduino.h"
#include <Servo.h>


/* Cislo aktualni verze programu (posledni aktualizace 27.4.2024) */
#define VERSION 5U

/* Chybova hodnota, ktera se odesle do matlabu v pripade chyby alokace pameti */
#define ALLOCATION_ERROR 255U

/* Prichozi udalosti z matlabu */
// Zjisteni typu akce
#define EV_MOVE_CONTROLLER        1U
#define EV_MOVE_COMMAND           2U
#define EV_PAUSE                  3U
#define EV_GET_VERSION            4U
#define EV_MOVE_ETHERNET          5U

// Pripojeni noveho ovladace
#define EV_ADD_DIGITAL_OUTPUT    1U
#define EV_ADD_ADAFRUIT_STEPPER  2U
#define EV_ADD_SONAR             3U
#define EV_ADD_DIGITAL_SENSOR    4U
#define EV_ADD_MICROPHONE        5U
#define EV_ADD_DC_MOTOR          6U
#define EV_ADD_ENCODER           7U
#define EV_ADD_DRIVER_STEPPER    8U
#define EV_ADD_GYROSCOPE         9U
#define EV_ADD_RGB_SENSOR        10U
#define EV_ADD_BUTTON            11U
#define EV_ADD_PWM_OUTPUT        12U
#define EV_ADD_TENSOR_MODULE     13U
#define EV_ADD_LIDAR             14U
#define EV_ADD_MAG_ENC           15U
#define EV_ADD_SERVO             16U


// Prechod do noveho stavu
#define EV_COMMAND_DIGITAL_OUTPUT     Controller::DIGITAL_OUTPUT           //0
#define EV_COMMAND_ADAFRUIT_STEPPER   Controller::ADAFRUIT_STEPPER_MOTOR   //1
#define EV_COMMAND_SONAR              Controller::SONAR                    //2
#define EV_COMMAND_DIGITAL_SENSOR     Controller::DIGITAL_SENSOR           //3
#define EV_COMMAND_MICROPHONE         Controller::MICROPHONE               //4
#define EV_COMMAND_DC_MOTOR           Controller::DC_MOTOR                 //5
#define EV_COMMAND_ENCODER            Controller::ENCODER                  //6
#define EV_COMMAND_DRIVER_STEPPER     Controller::DRIVER_STEPPER_MOTOR     //7
#define EV_COMMAND_GYROSCOPE          Controller::GYROSCOPE                //8
#define EV_COMMAND_RGB_SENSOR         Controller::RGB_SENSOR               //9
#define EV_COMMAND_BUTTON             Controller::BUTTON                   //10
#define EV_COMMAND_PWM_OUTPUT         Controller::PWM_OUTPUT               //11
#define EV_COMMAND_TENSOR_MODULE      Controller::TENSOR_MODULE            //12
#define EV_COMMAND_LIDAR              Controller::LIDAR                    //13
#define EV_COMMAND_MAG_ENC            Controller::MAG_ENC                  //14
#define EV_COMMAND_SERVO              Controller::SERVO                    //15


// Standartni udalosti ve stavech
#define EV_ENTRY                   200U
#define EV_CHECK                   201U
#define EV_BACK                    202U
#define EV_CONNECT                 203U

// Konkretni prikaz
#define EV_DIGITAL_HIGH                20U
#define EV_DIGITAL_LOW                 21U
#define EV_ADAFRUIT_STEPPER_RELEASE    22U
#define EV_ADAFRUIT_STEPPER_SET_SPEED  23U
#define EV_ADAFRUIT_STEPPER_ONESTEP    24U
#define EV_ADAFRUIT_STEPPER_STEP       25U
#define EV_GET_DISTANCE                26U
#define EV_CHECK_DIGITAL_INPUT         27U
#define EV_ADAFRUIT_STEPPER_STOP       29U
#define EV_ADAFRUIT_STEPPER_IS_MOVING  30U
#define EV_IS_DETECTED_SOUND           32U
#define EV_GET_AMPLITUDE               33U
#define EV_SET_AMPLITUDE               34U
#define EV_DC_SET_SPEED                35U
#define EV_DC_CHANGE_DIR               36U
#define EV_DC_RUN                      37U
#define EV_DC_STOP                     38U
#define EV_ENCODER_GET_SPEED           39U
#define EV_DRIVER_STEPPER_SET_SPEED    40U
#define EV_DRIVER_STEPPER_ONESTEP      41U
#define EV_DRIVER_STEPPER_STEP         42U
#define EV_DRIVER_STEPPER_STOP         43U
#define EV_DRIVER_STEPPER_IS_MOVING    44U
#define EV_GYROSCOPE_GET_ANGLES        45U
#define EV_GYROSCOPE_CALIBRATE         46U
#define EV_RGB_SENSOR_GET_COLORS       47U
#define EV_BUTTON_IS_PUSHED            48U
#define EV_BUTTON_GET_RISING_EDGE      49U
#define EV_BUTTON_GET_FALLING_EDGE     50U
#define EV_DRIVER_STEPPER_CONTINUE     51U
#define EV_ADAFRUIT_STEPPER_CONTINUE   52U
#define EV_GYROSCOPE_GET_ACC           53U
#define EV_SET_PWM_S                   54U
#define EV_TENSOR_ZERO                 55U
#define EV_TENSOR_SET_SCALE            56U
#define EV_TENSOR_GET_VALUE            57U
#define EV_LIDAR_GET_DIST              58U
#define EV_SERVO_SET_POS               59U
#define EV_MAG_ENC_ANGLE               60U
#define EV_MAG_ENC_ANGLE_SUM           61U
#define EV_ENCODER_GET_HOLES           62U


/* Ovladani casovacu */
#define TMR1_ON  TCCR1B |= (1 << CS10) // Preddelicka 1
#define TMR1_OFF TCCR1B  = 0
#define TMR3_ON  TCCR3B |= (1 << CS30) // Preddelicka 1
#define TMR3_OFF TCCR3B  = 0
#define TMR4_ON  TCCR4B |= (1 << CS40) // Preddelicka 1
#define TMR4_OFF TCCR4B  = 0
#define TMR5_ON  TCCR5B |= (1 << CS50) // Preddelicka 1
#define TMR5_OFF TCCR5B  = 0

/* Ovladani krokoveho motoru pripojeneho na adafruit motorshield v2 */
#define ADAFRUIT_STEPPER_PORT1 1U
#define ADAFRUIT_STEPPER_PORT2 2U

#define MIN_ADAFRUIT_STEPPER_TICKS 3U
#define MAX_ADAFRUIT_STEPPER_TICKS 80U
#define MAX_ADAFRUIT_STEPPER_COUNT 2U // Maximalni pocet pripojenych krokovych motoru na motorshieldu

/* Ovladani krokoveho motoru pomoci vlastniho driveru */
#define MAX_DRIVER_STEPPER_COUNT 6U   // Maximalni pocet pripojenych krokovych motoru s vlastnim driverem
#define MAX_DRIVER_STEPPER_TICKS 40U
#define MIN_DRIVER_STEPPER_TICKS 3U
#define DRIVER_STEPPER_FORWARD   1U
#define DRIVER_STEPPER_BACKWARD  0U
#define DRIVER_STEPPER_KEY       3U

#define ON  1U
#define OFF 0U

/* Ovladani tridy Sonar */
#define MAX_SONAR_DISTANCE 450U

/* Ovladani tridy Microphone */
#define MIN_SOUND_AMPLITUDE 20U
#define MAX_SOUND_AMPLITUDE 255U

// Predstavuje cislo nulove hladiny zvuku
// AD prevodnik -> 5V = 1024 
// Mikrofon je napajen 3.3V -> 1.65V reprezentuje nulovou hladinu zvuku
// Digitalni hodnota tohoto napeti je tedy 1.65 * (1024 / 5) (trojclenka)
// Pri napajeni mikrofonu 5V musi byt hodnota teto promenne 2.5 * (1024 / 5)
#define BASELINE            338U

/* Ovladani tridy Encoder */
#define RISING_EDGE                0b01111111
#define FALLING_EDGE               0b10000000
#define ENCODER_HOLES_NUM          16U
#define ENCODER_DELTA_HOLES        1.0f / ENCODER_HOLES_NUM
#define ENCODER_MIN_DETECTED_SPEED (10000U / ENCODER_HOLES_NUM)
#define MAX_ENCODER_COUNT          4U                          // Maximalni pocet pripojenych encoderu

/* Ovladani tridy Gyroscope */
#define FLOAT_SIZE           sizeof(float)
#define LONG_SIZE            sizeof(long)

#define GYRO_ANGLES_COUNT    3U                               // Pocet uhlu (3 - roll, pitch, yaw)
#define GYRO_ANGLES_ARR_SIZE GYRO_ANGLES_COUNT * FLOAT_SIZE   // Celkovy pocet bytu pro ulozeni vsech uhlu
#define ACC_COUNT            3U                               // Pocet ZRYCHLENI (3 - X,Y,Z)
#define GYRO_ACC_ARR_SIZE    ACC_COUNT * FLOAT_SIZE           // Celkovy pocet bytu pro ulozeni vsech uhlu

/* Ovladani tridy Button */
#define MAX_BUTTONS_COUNT 15U                                 // Maximalni pocet pripojenych tlacitek


/******************************************************************************/
/* Funkce slouzici k odeslani zpravy do matlabu o provedeni prikazu
 * msg - zprava k odeslani do matlabu
*/
/******************************************************************************/
void controllers_send_msg(uint8_t msg);

/******************************************************************************/
/* Funkce nastavujici promennou PT_CLIENT
 * client - klient, jehoz adresa se ulozi do PT_CLIENT
 */
/******************************************************************************/
void controllers_set_client(EthernetClient &client);

/****************************************************************************************************/
/* Funkce slouzici jako interrupt pri mereni vzdalenosti sonarem */
/****************************************************************************************************/
void controllers_echo_check(void);

/****************************************************************************************************/
/*
 * Predek vsech trid slouzicich k ovladani prvku pripojenych k arduinu.
 * Diky nemu bude mozne ukladat odkazy objektu vsech ovladacich trid
 * pod odkaz teto tridy. Trida definuje metodu vracejici typ ulozeneho objektu, jako vycet, slouziciho
 * ke zpetnemu pretypovani a tim ziskani puvodniho objektu, se kterym lze dale pracovat. Dale 
 * definuje nektere dalsi spolecne metody, ktere budou mit zdedene objekty.
 */
/****************************************************************************************************/
class Controller{
  public:
    /*****************************************************************************************************************************/
    /* Vycet typu objektu z knihovny "controllers.h" - slouzi pro ukladani informace o typu daneho 
     * DULEZITE!!! - PRI PRIDANI NOVE OVLADACI TRIDY DO KNIHOVNY controllers.h MUSI BYT PRIDAN TYP TETO TRIDY DO TOHOTO VYCTU !!!
    */
    /*****************************************************************************************************************************/
    enum o_type { DIGITAL_OUTPUT, ADAFRUIT_STEPPER_MOTOR, SONAR, DIGITAL_SENSOR, MICROPHONE, DC_MOTOR, ENCODER,
                  DRIVER_STEPPER_MOTOR, GYROSCOPE, RGB_SENSOR, BUTTON, PWM_OUTPUT, TENSOR_MODULE, LIDAR, MAG_ENC, SERVO };

    /*****************************************************************************************************************************/
    /* Konstruktor - ulozi typ daneho objektu */
    /*****************************************************************************************************************************/
    Controller(o_type t) { type = t; num++; }

    /*****************************************************************************************************************************/
    /* Metoda vracejici typ daneho objektu */
    /*****************************************************************************************************************************/
    o_type get_type(void) { return type; }

    /*****************************************************************************************************************************/
    /* Vsechny odvozene tridy musi definovat metodu vracejici poradove cislo daneho objektu.
     * Toto cislo bude odeslano a ulozeno do matlabu z duvodu ke zpetnemu pristupu k danemu objektu v poli
     * (index pole).
    */
    /*****************************************************************************************************************************/
    virtual uint8_t get_my_index(void) = 0;

  protected:
    /*****************************************************************************************************************************/
    /* Metoda vracejici poradove cislo nove vytvoreneho objektu (resp. jeho index v poli) */
    /*****************************************************************************************************************************/
    uint8_t get_num(void) { return num-1; }

  private:
    static uint8_t num;  // Uklada informaci o poctu aktualne vytvorenych objektu
    o_type type;         // Slouzi k ulozeni typu danneho objektu

}; /* Controller */

/********************************************************************/
/* Trida slouzici k zapisovani logicke 1 nebo 0 na digitalni pin */ 
/********************************************************************/
class Digital_output : public Controller {
  private:
    /* CLENSKE PROMENNE */
    uint8_t  pin;     // Pin, ke kteremu je pripojen HW prvek pro digitalni ovladani
    uint8_t myIndex;  // Poradove cislo objektu (jeho index v poli)

  public:
    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu Digital_output a zaregistruje pin, ke kteremu je pripojen HW prvek a ulozi typ tohoto objektu 
     * pin         - pin, ke kteremu je pripojen HW prvek
     */
     /*************************************************************************************************************************************/
    Digital_output(uint8_t pin);
    
    /*************************************************************************************************************************************/
    /* Metoda nastavujici na vystup 1 */
    /*************************************************************************************************************************************/
    void digital_high(void) { digitalWrite(pin, HIGH); }
    
    /*************************************************************************************************************************************/
    /* Metoda nastavujici na vystup 0 */
    /*************************************************************************************************************************************/
    void digital_low(void) { digitalWrite(pin, LOW); } 

    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Digital_output */

/*************************************************************************************/
/* Pomocna trida (predek) pro ovladace motoru ovladanych Adafruit Motorshield v2
 * Slouzi k ulozeni a inicializaci Motorshieldu (aby byl inicializovan pouze jednou)
*/
/*************************************************************************************/
class Adafruit_Motor{
  protected:
    static Adafruit_MotorShield AFMS;  // Objekt slouzici pro ziskavani motoru
    static uint8_t is_afms_init;       // Booleovska promenna rikajici, jestli bylo AFMS inicializovano (0 - neni, 1 - je)
};

/************************************************************************************/
/* Trida slouzici k ovladani krokoveho motoru pripojeneho k Adafruit Motorshield v2 */
/************************************************************************************/
class Adafruit_Stepper_motor : public Controller, public Adafruit_Motor {
  private:
    /* STATICKE PROMENNE */
    static uint8_t stepper_on[MAX_ADAFRUIT_STEPPER_COUNT]; // Rika, jestli byl zapnut (1) nebo vypnut (0) motor na portu reprezentujici pozice v poli

    /* CLENSKE PROMENE */
    uint8_t myIndex;                    // Poradove cislo objektu (jeho index v poli)
    Adafruit_StepperMotor *pt_myMotor;  // Ukazatel na ovladac motoru pres motorshield
    uint8_t  port;                      // Port, na kterem je motor pripojen
    uint16_t spr;                       // Pocet kroku motoru na jednu otacku
    uint16_t steps;                     // Pocet kroku, ktere ma motor vykonat
    uint8_t  dir;                       // Smer, kterym ma motor jet pri volani metody step
    uint8_t  style;                     // Styl, kterym ma motor jet pri volani metody step
    uint8_t speed_freq;                 // Pocet milisekund, po kterych se ma zavolat metoda onestep
    uint8_t millis_counter;             // Pocitadlo pro pocitani milisekund

  public:

    /**********************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu Adaftuit_Stepper_motor a zaregistruje port, ke kteremu je motor pripojen  a ulozi typ tohoto objektu
     * port        - port na motorshieldu  ke kteremu  je motor pripojen (muze byt 1 nebo 2)
     * spr         - pocet kroku na jednu otacku (steps per rotation, musi byt zapocitana i prevodovka)
     */
     /*********************************************************************************************************************************************/
    Adafruit_Stepper_motor(uint8_t port, uint16_t spr);

    /*********************************************************************************************************************************************/
    /* Metoda uvolnujici vsechny piny na krokovem motoru tak, aby se mohl volne otacet */
    /*********************************************************************************************************************************************/
    void release(void) { pt_myMotor->release(); }                                                  

    /*********************************************************************************************************************************************/
    /* Metoda nastavujici rychlost otaceni krokoveho motoru
     * rpm - otacky za minutu
     */ 
    /*********************************************************************************************************************************************/
    void set_speed(uint8_t speed) { 
      speed_freq = MAX_ADAFRUIT_STEPPER_TICKS - ((MAX_ADAFRUIT_STEPPER_TICKS - MIN_ADAFRUIT_STEPPER_TICKS) / 255.0) * speed; 
    }

    /*********************************************************************************************************************************************/
    /* Metoda otacejici motorem o jeden krok bez zpozdeni 
     * dir   - smer, ve kterem se ma motor otocit (FORWARD, BACKWARD)
     * style - styl kroku, ktery bude pouzit (SINGLE, DOUBLE, INTERLEAVE, MICROSTEP)
     */
    /*********************************************************************************************************************************************/
    void onestep(uint8_t dir, uint8_t style = DOUBLE) { pt_myMotor->onestep(dir, style);  }
    
    /*********************************************************************************************************************************************/
    /* Metoda otacejici motorem o pozadovany pocet kroku s nastavenymi otackami za minutu
     * steps - pocet kroku, o ktery se ma motor otocit
     * dir   - smer, ve kterem se ma motor otocit (FORWARD, BACKWARD)
     * style - styl kroku, ktery bude pouzit (SINGLE, DOUBLE, INTERLEAVE, MICROSTEP), standartne nastaveny na DOUBLE
     */
    /*********************************************************************************************************************************************/
    void step(uint16_t steps, uint8_t dir, uint8_t style = DOUBLE);

    /*********************************************************************************************************************************************/
    /* Metoda volana z interruptu - udela jeden krok v pozadovanem case */
    /*********************************************************************************************************************************************/
    void tick(void);

    /*********************************************************************************************************************************************/
    /* Metoda okamzite zastavujici otaceni krokoveho motoru */
    /*********************************************************************************************************************************************/
    void stop(void) { stepper_on[port-1] = OFF; }

    /*********************************************************************************************************************************************/
    /* Metoda vracejici, jestli je motor v pohybu (1 - je v pohybu, 0 - neni v pohybu) */
    /*********************************************************************************************************************************************/
    uint8_t is_moving(void){
      return stepper_on[port-1];
    };

    /*********************************************************************************************************************************************/
    /* Metoda slouzici pro pokracovani otaceni motoru a pripadne nastavujici nove parametry
     * speed - nova rychlost
     * dir   - novy smer
    */
    /*********************************************************************************************************************************************/
    void a_continue(uint8_t speed, uint8_t dir);

    /*********************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*********************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Adafruit_Stepper_motor */

/********************************************************************************************************/
/* Trida slouzici pro ovladani ultrazvukoveho senzoru */
/********************************************************************************************************/
class Sonar : public Controller {
  private:
   /* Clenske promenne */
    uint8_t myIndex;                 // Index ovladace v poli ovladacu
    NewPing *p_sonar;                // Ukazatel na objekt ovladajici ultazvukovy senzor
    uint16_t volatile distance;      // Sonarem namerena vzdalenost
  
  public:
    /****************************************************************************************************/
    /* Inicializuje promennou p_sonar (vytvori novy objekt)
       trig_pin - pin na ktery je pripojen trigger
       echo_pin - pin na ktery je pripojen echo
    */
    /****************************************************************************************************/
    Sonar(uint8_t trig_pin, uint8_t echo_pin);

    /****************************************************************************************************/
    /* Metoda merici vzdalenost v centimetrech, kterou nasledne vrati 
     * Pokud vzdalenost namerit nelze, vrati 0
     */
    /****************************************************************************************************/
    uint16_t get_distance(void);

    /****************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /****************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

    /****************************************************************************************************/
    /* Metoda nastavujici clenske promenne distance a distance found (vyuziva se v interruptu)
     * distance       - namerena vzdalenost
     * distance_found - informuje, ze jiz byla vzdalenost namerena
    */
    /****************************************************************************************************/
    void set_distance(uint16_t distance) { this->distance = distance; }

    /****************************************************************************************************/
    /* Metoda vyuzivana v interruptu - slouzi pro kontrolu timeru */
    /****************************************************************************************************/
    uint8_t check_timer(void) { return p_sonar->check_timer(); }

    /****************************************************************************************************/
    /* Metoda vracejici dobu mezi odeslanim a prijmutim zvuku */
    /****************************************************************************************************/
    uint32_t get_ping_result(void) { return p_sonar->ping_result; }
}; /* Sonar */

/********************************************************************************************************/
/* Trida slouzici pro ovladani senzoru merici logickou hodnotu */
/********************************************************************************************************/
class Digital_sensor : public Controller {
  private:
    /* Clenske promenne */
    uint8_t pin;     // pin, na kterem je pripojen senzor
    uint8_t myIndex; //Index ovladace v poli ovladacu
  
  public:
    /***************************************************************************************************/
    /* Ulozi cislo pinu, na kterem je pripojen senzor a zaregistruje tento pin
     * pin, na kterem je pripojen senzor
     */
    /***************************************************************************************************/
    Digital_sensor(uint8_t pin);

    /**************************************************************************************************/
    /* Metoda vracejici logickou hodnotu, ktera je namerena (true, false) */
    /**************************************************************************************************/
    uint8_t check_digital_input(void) { return digitalRead(pin); }

    /****************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /****************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }
}; /* Digital_sensor */

/********************************************************************************************************/
/* Trida slouzici pro ovladani pripojeneho mikrofonu */
/********************************************************************************************************/
class Microphone : public Controller {
  private:
    /* Clenske promenn */
    uint8_t analog_pin;          // Analogovy pin, na ktery je pripojen mikrofon
    uint8_t myIndex;             // Index ovladace v poli ovladacu
    uint8_t min_sound_amplitude; // Minimalni amplituda zvuku pro zaznamenani logicke 1 metodou is_detected_sound
  
  public:
    /******************************************************************************************************/
    /* Konstuktor - ulozi cislo analogoveho pinu, na ktery je mikrofon pripojen a ulozi 
     * index vytvoreneho ovladace v poli ovladacu
     * pin - analogovy pin, na ktery je pripojen mikrofon
     */
    /******************************************************************************************************/
    Microphone(uint8_t pin);

    /******************************************************************************************************/
    /* Metoda vracejici hodnotu 1, pokud byl detekovan zvuk, jinak vraci 0*/
    /******************************************************************************************************/
    uint8_t is_detected_sound(void);

    /****************************************************************************************************/
    /* Nastavujici minimalni hodnotu amplitudy, 
     * nad kterou ma byt snimana logicka 1 pro metodu is_detected_sound 
     * amplitude - minimalni hodnota amplitudy zvuku, nad kterou je snimana logicka 1
     */
    /****************************************************************************************************/  
    void set_amplitude(uint8_t amplitude) { min_sound_amplitude = amplitude; }

    /******************************************************************************************************/
    /* Metoda vracejici hodnotu amplitudy zvukoveho signalu */
    /******************************************************************************************************/
    uint8_t get_amplitude(void);

    /****************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /****************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }  
}; /* Microphone */

/***************************************************************************************************************/
/* Ovladac slouzici k ovladani DC motoru */
/***************************************************************************************************************/
class DC_motor : public Controller, public Adafruit_Motor {
  private:
    /* CLENSKE PROMENNE */
    Adafruit_DCMotor *pt_myMotor; // Ukazatel na ovladac DC motoru pres motorshield
    uint8_t myIndex;              // Index ovladace v poli ovladacu
    uint8_t dir;                  // Aktualni smer otaceni
    uint8_t is_stepper_off;       // Promenna rikajici, jestli byl vypnut krokovy motor (1) nebo ne (0)

    /******************************************************************************************************/
    /* Pomocna metoda vypinajici krokovy motor (v pripade, ze je zapnut) */
    /******************************************************************************************************/
    void turn_off_stepper(void){
      if(TCCR1B & (1 << CS10)) {
        TMR1_OFF;
        is_stepper_off = 1;
      }
    }

    /******************************************************************************************************/
    /* Pomocna funkce zapinajici krokovy motor (v pripade, ze byl vypnut) */
    /******************************************************************************************************/
    void turn_on_stepper(void){
      if(is_stepper_off){
        TMR1_ON;
        is_stepper_off = 0;
      }
    }

  public:
    /******************************************************************************************************/
    /* Konstuktor - vytvori a novy ovladac DC motoru a ulozi
     * index vytvoreneho ovladace v poli ovladacu
     * port - port na motorshieldu, kde je pripojen motor M1=1, M2=2, M3=3, M4=4
     */
    /******************************************************************************************************/
    DC_motor(uint8_t port);

    /******************************************************************************************************/
    /* Metoda nastavujici rychlost otaceni DC motoru  */
    /******************************************************************************************************/
    void set_speed(uint8_t speed){
      turn_off_stepper();
      pt_myMotor->setSpeed(speed);
      turn_on_stepper();
    };

    /******************************************************************************************************/
    /* Metoda prepinajici smer otaceni motoru */
    /******************************************************************************************************/
    void change_dir(void) { (dir == FORWARD) ? (dir = BACKWARD) : (dir = FORWARD); }

    /******************************************************************************************************/
    /* Metoda zapinajici otaceni motoru */
    /******************************************************************************************************/
    void run(void) { 
      turn_off_stepper();
      pt_myMotor->run(dir);
      turn_on_stepper(); 
    }

    /******************************************************************************************************/
    /* Metoda vypinajici otaceni motoru */
    /******************************************************************************************************/
    void stop(void) {
      turn_off_stepper(); 
      pt_myMotor->run(RELEASE);
      turn_on_stepper();
    }

    /****************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /****************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; } 
}; /* DC_motor */

/***************************************************************************************************************/
/* Ovladac slouzici k ovladani rychlostniho encoderu */
/***************************************************************************************************************/
class Encoder : public Controller {
  private:
    /* CLENSKE PROMENNE */
    uint8_t pin;     // Digitalni pin, na kterem je pripojen encoder
    uint8_t acc;     // Sleduje nabeznou hranu
    uint8_t init;    // Pomocna promenna pro vypocet rychlosti (sleduje jestli uz byla detekovana prvni nabezna hrana)
    uint8_t myIndex; // Index ovladace v poli ovladadu
    uint16_t speed;  // Namerena rychlost encoderem
    uint16_t time;   // Cas predesle nabezne hrany
    uint16_t holes = 0; // pocet ticku
  
  public:
    /******************************************************************************************************/
    /* Konstuktor - vytvori a novy ovladac encoderu a ulozi
     * index vytvoreneho ovladace v poli ovladacu
     * pin - digitalni pin, na kterem je pripojen encoder
     */
    /******************************************************************************************************/
    Encoder(uint8_t pin);

    /******************************************************************************************************/
    /* Metoda slouzici pro vypocet rychlosti */
    /******************************************************************************************************/
    void tick(void);

    /******************************************************************************************************/
    /* Metoda slouzici pro vraceni aktualni rychlosti v otackach za minutu */
    /******************************************************************************************************/
    uint16_t  get_rpm_speed(void) { return speed; };

    /******************************************************************************************************/
    /* Metoda slouzici pro vraceni aktualni rychlosti v otackach za minutu */
    /******************************************************************************************************/
    uint16_t  get_holes(void) { return holes; };
    
    /******************************************************************************************************/
    /* Metoda slouzici pro vraceni aktualni rychlosti v otackach za minutu */
    /******************************************************************************************************/
    void reset_holes(void) { holes = 0; };

    /******************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /******************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; } 
}; /* Encoder */

/***************************************************************************************************************/
/* Krokovy motor 28BYJ-48 ovladany vlastnim driverem  */
/***************************************************************************************************************/
class Driver_Stepper_motor : public Controller{
  private:
    /* STATICKE PROMENNE */
    static uint8_t stepper_on[MAX_DRIVER_STEPPER_COUNT];  // Promenna rikajici, ktery motor je prave spusten (1) a ktery vypnut (0)
    static uint8_t steppers_count;                        // Promenna uchovavajici pocet vytvorenych objektu teto tridy

    /* CLENSKE PROMENNE */
    uint8_t pin1, pin2, pin3, pin4; // Digitalni piny, na ktery je pripojen driver
    uint8_t step_number;            // Pomocna promenna pro prepinani civek motoru
    uint8_t speed_freq;             // Perioda otaceni motoru o jeden krok v milisekundach (udava rychlost motoru)
    uint8_t millis_counter;         // Pocitadlo pro pocitani milisekund
    uint8_t dir;                    // Smer, kterym se ma motor otacet
    uint8_t my_number;              // Poradove cislo ovladace
    uint8_t my_index;               // Index ovladace v poli ovladacu
    uint16_t steps;                 // Pocet kroku, o ktery se ma motor otocit
  
  public:
    /************************************************************************************************************/
    /* Konstruktor - zaregistruje piny pripojene na driver a pripravi 
     * motor k ovladani (inicializuje vsechny promenne)
     * pin1, pin2, pin3, pin4 - piny, na ktere je pripojen driver
     */
    /************************************************************************************************************/
    Driver_Stepper_motor(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);

    /************************************************************************************************************/
    /* Metoda slouzici k nastaveni rychlosti otaceni motoru
     * speed - rychlost otaceni motoru (0 - 255) 
     */
    /************************************************************************************************************/    
    void set_speed(uint8_t speed) {
      speed_freq = MAX_DRIVER_STEPPER_TICKS - ((MAX_DRIVER_STEPPER_TICKS - MIN_DRIVER_STEPPER_TICKS) / 255.0) * speed;
    }

    /************************************************************************************************************/
    /* Metoda otacejici motorem o jeden krok
     * dir - smer, ve kterem ma byt motor otocen 
     */
    /************************************************************************************************************/
    void onestep(uint8_t dir);

    /************************************************************************************************************/
    /* Metoda otacejici motorem o zadany pocet kroku
     * steps - pocet kroku, o ktere ma byt motor otocen
     * dir   - smer, ve kterem ma byt motor otocen 
     */
    /************************************************************************************************************/
    void step(uint16_t steps, uint8_t dir);
    
    /************************************************************************************************************/
    /* Metoda vracejici logickou hodnotu, jestli je motor v pohybu (1) nebo ne (0) */
    /************************************************************************************************************/
    uint8_t is_moving(void) { return stepper_on[my_number]; }

    /************************************************************************************************************/
    /* Metoda slouzici k zastaveni krokoveho motoru */
    /************************************************************************************************************/
    void stop(void) { stepper_on[my_number] = OFF; }

    /************************************************************************************************************/
    /* Metoda volana z interruptu - udela jeden krok v pozadovanem case  */
    /************************************************************************************************************/
    void tick(void);

    /************************************************************************************************************/
    /* Metoda pokracujici v otaceni motoru a pripadne nastavujici nove parametry 
     * speed - nova rychlost
     * dir   - novy smer
    */
    /************************************************************************************************************/
    void d_continue(uint8_t speed, uint8_t dir);

    /************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /************************************************************************************************************/
    uint8_t get_my_index(void) { return my_index; } 
}; /* Driver_Stepper_motor */

/***************************************************************************************************************/
/* Ovladac pro gyroskop s akcelerometrem pro zjisteni naklonu senzoru */
/***************************************************************************************************************/
class Gyroscope : public Controller{
  private:
    /* KONSTANTY */
    const int8_t MPU                 = 0x68;    // Adresa gyroskopu pro i2c komunikaci (viz datasheet)
    const float gyro_part            = 0.96;    // Cast dat, ktera bude brana z gyroskopu (96%)
    const float acc_part             = 0.04;    // Cast dat, ktera bude brana z akcelerometru (4%)
    const float lsb_acc_sensitivity  = 16384.0; // Slouzi pro vypocet namereneho uhlu z akcelerometru (viz datasheet)
    const float lsb_gyro_sensitivity = 131.0;   // Slouzi pro vypocet namereneho uhlu z gyroskopu (viz datasheet)
    const uint8_t calib_count        = 200;     // Pocet mereni pro kalibraci

    /* CLENSKE PROMENNE */
    float accErrorX, accErrorY;                 // Chyba akcelerometru v osach x a y
    float gyroErrorX, gyroErrorY, gyroErrorZ;   // Chyba gyroskopu v osach x, y, z
    float accX, accY, accZ;                     // Namerena data akcelerometrem
    float gyroX, gyroY, gyroZ;                  // Namerena data gyroskopem
    float accAngleX, accAngleY;                 // Vypoctene uhly z namerenych dat z akcelerometru
    float gyroAngleX, gyroAngleY, gyroAngleZ;   // Vypoctene uhly z namerenych dat z gyroskopu
    float roll, pitch, yaw;                     // Vysledne namerene uhly (gyroskop + akcelerometr)
    float currentTime, previousTime;            // Casove udaje pro integraci v gyroskopu
    uint8_t my_index;                           // Index ovladace v poli ovladacu

    /* Unie pro rozebrani float na jednotlive 4 byty */
    union {
      float   angle;            // Uhel, pro rozbor jednotlivych bytu
      uint8_t byte[FLOAT_SIZE]; // Byte na dane pozici 
    } angle_temp;
    union {
      float   acc;            // Uhel, pro rozbor jednotlivych bytu
      uint8_t byte[FLOAT_SIZE]; // Byte na dane pozici 
    } acc_temp;
  
  public:
    /************************************************************************************************************/
    /* Konstruktor - zahaji i2c komunikaci a nastavi potrbne udaje v gyroskopu a akcelerometru */
    /************************************************************************************************************/
    Gyroscope(void);

    /************************************************************************************************************/
    /* Metoda pro vypocet a ulozeni aktualniho naklonu gyroskopu */
    /************************************************************************************************************/
    void set_angles(void);

    /************************************************************************************************************/
    /* Metoda pro vraceni aktualniho zrychlení akcelerometru - zrychlení budou vraceny jako pole uint8_t pro mozne odeslani
     * tri cisel typu float pres uart
     * arr  - pole 8bitovych hodnot typu uint8_t, do ktereho budou nahrany jednotlive byty vsech 3 uhlu
     */
    /************************************************************************************************************/
    void get_acc(uint8_t *arr);

    /************************************************************************************************************/
    /* Metoda pro vraceni aktualniho naklonu gyroskopu - uhly budou vraceny jako pole uint8_t pro mozne odeslani
     * tri cisel typu float pres uart
     * arr  - pole 8bitovych hodnot typu uint8_t, do ktereho budou nahrany jednotlive byty vsech 3 uhlu
     */
    /************************************************************************************************************/
    void get_angles(uint8_t *arr);

    /************************************************************************************************************/
    /* Metoda slouzici ke kalibraci senzoru naklonu */
    /************************************************************************************************************/
    void calibrate(void);

    /************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /************************************************************************************************************/
    uint8_t get_my_index(void) { return my_index; } 
}; /* Gyroscope */

/***************************************************************************************************************/
/* Ovladac pro RGB senzor pro detekci barev */
/***************************************************************************************************************/
class RGB_sensor : public Controller{
  private:
    /* CLENSKE PROMENNE */
    uint8_t pinS0;         // Digitalni pin, na kterem je pripojen vyvod S0 ze senzoru
    uint8_t pinS1;         // Digitalni pin, na kterem je pripojen vyvod S1 ze senzoru
    uint8_t pinS2;         // Digitalni pin, na kterem je pripojen vyvod S2 ze senzoru
    uint8_t pinS3;         // Digitalni pin, na kterem je pripojen vyvod S3 ze senzoru
    uint8_t pinOut;        // Digitalni pin, na kterem je pripojen vyvod OUT ze senzoru
    uint8_t myIndex;       // Index ovladace v poli
    uint16_t red_freq;     // Namerena frekvence cervene slozky
    uint16_t blue_freq;    // Namerena frekvence modre slozky
    uint16_t green_freq;   // Namerena frekvence zelene slozky
  
  public:
    /************************************************************************************************************/
    /* Konstruktor - ulozi a zaregistruje piny a pripravi senzor k mereni
     * pinS0  - digitalni pin, na kterem je pripojen vyvod S0 ze senzoru
     * pinS1  - digitalni pin, na kterem je pripojen vyvod S1 ze senzoru
     * pinS2  - digitalni pin, na kterem je pripojen vyvod S2 ze senzoru
     * pinS3  - digitalni pin, na kterem je pripojen vyvod S3 ze senzoru
     * pinOut - digitalni pin, na kterem je pripojen vyvod OUT ze senzoru 
    */
    /************************************************************************************************************/
    RGB_sensor(uint8_t pinS0, uint8_t pinS1, uint8_t pinS2, uint8_t pinS3, uint8_t pinOut);

    /************************************************************************************************************/
    /* Metoda slouzici k ziskani namerenych slozek cervene, modre a zelene barvy
     * red   - vracena namerena slozka cervene barvy
     * green - vracena namerena slozka zelene barvy
     * blue  - vracena namerena slozka modre barvy
    */
    /************************************************************************************************************/
    void get_rgb_colors(uint16_t &red, uint16_t &green, uint16_t &blue);

    /************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; } 
}; /* RGB_sensor */

/***************************************************************************************************************/
/* Ovladac pro tlacitka */
/***************************************************************************************************************/
class Button : public Controller {
  private:
    /* CLENSKE PROMENNE */
    uint8_t pin;            // Digitalni pin, na kterem je pripojeno tlacitko
    uint8_t acc;            // Promenna pro sledovani nabeznych a sestupnych hran
    uint8_t state;          // Aktualni stav tlacitka (stisknuto, uvolneno)
    uint8_t rising_edge;    // Pokud byla detekovana nabezna hrana, je tato promenna 1
    uint8_t falling_edge;   // Pokud byla detekovana sestupna hrana, je tato promenna 1
    uint8_t myIndex;        // Index ovladace v poli ovladacu

  public:
    /************************************************************************************************************/
    /* Konstruktor - ulozi a zaregistruje pin, inicializuje vsechny promenne a zapne casovac
     * pin - digitalni pin, na kterem je pripojen vyvod S0 ze senzoru
    */
    /************************************************************************************************************/
    Button(uint8_t pin);

    /************************************************************************************************************/
    /* Metoda volana z interruptu - sleduje a uklada informace o aktualnim stavu tlacitka */
    /************************************************************************************************************/
    void tick(void);

    /************************************************************************************************************/
    /* Metoda vracejici 1, pokud je tlacitko stisknute, jinak vraci 0 */
    /************************************************************************************************************/
    uint8_t is_pushed(void) { return state; }

    /************************************************************************************************************/
    /* Metoda vracejici 1, pokud byla detekovana nabezna hrana */
    /************************************************************************************************************/
    uint8_t get_rising_edge(void);

    /************************************************************************************************************/
    /* Metoda vracejici 1, pokud byla detekovana sestupna hrana */
    /************************************************************************************************************/
    uint8_t get_falling_edge(void);

    /************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }
}; /* Button */

class PWM_output : public Controller {
  private:
    /* CLENSKE PROMENNE */
    uint8_t pin;      // Pin, ke kteremu je pripojen HW prvek pro digitalni ovladani
    uint8_t myIndex;  // Poradove cislo objektu (jeho index v poli)

  public:
    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu PWM_output a zaregistruje pin, ke kteremu je pripojen HW prvek a ulozi typ tohoto objektu 
     * pin         - pin, ke kteremu je pripojen HW prvek
     */
     /*************************************************************************************************************************************/
    PWM_output(uint8_t pin);
    
    /*************************************************************************************************************************************/
    /* Metoda nastavujici na vystup 1 */
    /*************************************************************************************************************************************/
    void analog_s(uint8_t strida) { 
      analogWrite(pin, strida); 
      }
    
    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Digital_output */

class Tensor_module : public Controller {
  private:
    /* CLENSKE PROMENNE */
    uint8_t pin_data;     // Pin, ke kteremu jsou pripojeny data
    uint8_t pin_clk;      // Pin, ke kteremu je pripojeno cidlo modulu
    long reading;       // namerena hodnota vahy
    uint8_t myIndex;    // Poradove cislo objektu (jeho index v poli)
    float scale;
    HX711 sensor;       // Trida senzoru vahy
    int32_t intreading = 0;


    union {
      int32_t   mass;             // vaha
      uint8_t byte[4]; // Byte na dane pozici 
    } mass_temp;



  public:
    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu PWM_output a zaregistruje pin, ke kteremu je pripojen HW prvek a ulozi typ tohoto objektu 
     * pinAX       - piny, ke kteremu je pripojen HW prvek
     */
     /*************************************************************************************************************************************/
    Tensor_module(uint8_t pin_data, uint8_t pin_clk);
     

    /*************************************************************************************************************************************/
    /* Metoda nastavujici zesileni */
    /*************************************************************************************************************************************/
    void set_scale(float scale) { 
      sensor.tare(5); 
      sensor.set_scale(scale); 
      }
    
    /*************************************************************************************************************************************/
    /* Metoda nastavujici nulovou hodnotu */
    /*************************************************************************************************************************************/
    void set_zero(void) { 
      sensor.tare(5); 
 
      }
    
    /************************************************************************************************************/
    /* Metoda vracejici zmereou vahu */
    /************************************************************************************************************/
    void get_mass(uint8_t *arr);

    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Tensor_module */


class Lidar : public Controller {
  private:
    /* CLENSKE PROMENNE */
    const int8_t MPU = 0x50;    // Adresa lidaru pro i2c komunikaci (viz datasheet)
    float distance;      // vzdalenost
    uint8_t myIndex;    // Poradove cislo objektu (jeho index v poli)
    VL53L0X sensor;       // Trida senzoru vahy

    union {
      float distance;  // vzdalenost
      uint8_t byte[4]; // Byte na dane pozici 
    } dist_temp;



  public:
    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu Lidar na sbernici SPI
     */
     /*************************************************************************************************************************************/
    Lidar(void);

 
    /************************************************************************************************************/
    /* Metoda vracejici zmereou vzdalenost */
    /************************************************************************************************************/
    void get_dist(uint8_t *arr);

    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Lidar */

class Magnetic_encoder : public Controller {
  private:
    /* CLENSKE PROMENNE */
    int16_t angle_sum;  // sum of angle, -32,768 to 32,767; 4096 ticks per revolution, 16 turns
    int16_t prew_angle;      // absolute angle, 4096 ticks per revolution
    uint8_t myIndex;    // Poradove cislo objektu (jeho index v poli)
    uint16_t speed = 0;  // Namerena rychlost encoderem
    uint16_t prew_speed;
    uint16_t time = millis();   // Cas predchoziho mereni
    uint16_t prew_time;
    int16_t rotations = 0; // pocet otacek
    AS5600_PsW sensor;       // Trida senzoru vahy

    

  public:
    int16_t angle = 0;      // absolute angle, 4096 ticks per revolution

    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu PWM_output a zaregistruje pin, ke kteremu je pripojen HW prvek a ulozi typ tohoto objektu 
     * pinAX       - piny, ke kteremu je pripojen HW prvek
     */
     /*************************************************************************************************************************************/
    Magnetic_encoder(void);
  
    /************************************************************************************************************/
    /* Metoda vracejici uhel */
    /************************************************************************************************************/
    int16_t get_angle(void);

    /************************************************************************************************************/
    /* Metoda vracejici uhel */
    /************************************************************************************************************/
    void get_angle2(void);

    /************************************************************************************************************/
    /* Metoda vracejici absolutní uhel - pocet otacek*/
    /************************************************************************************************************/
    int16_t get_sum_angle(void);

    /************************************************************************************************************/
    /* Metoda vracejici rychlost */
    /************************************************************************************************************/
    int16_t get_speed(void);
    
    /************************************************************************************************************/
    /* Metoda vracejici zrychleni */
    /************************************************************************************************************/
    int16_t get_accel(void);

    /************************************************************************************************************/
    /* Metoda starajici se o zpracovani dat z preruseni */
    /************************************************************************************************************/
    void Magnetic_encoder::tick(void);

    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Magnetic_encoder */


class Servo_mech : public Controller {
  private:
    /* CLENSKE PROMENNE */
    int8_t angle;       // angle, 0 - 256 = 0° - 180°;
    uint8_t myIndex;    // Poradove cislo objektu (jeho index v poli)


    

  public:
    Servo myservo; 
    uint8_t pin;     // pin, na kterem je pripojen senzor
  
  public:
    /*************************************************************************************************************************************/
    /* Konstruktor - vytvori novy objekt typu Servo a zaregistruje pin, ke kteremu je pripojen HW prvek a ulozi typ tohoto objektu 
     * pin         - pin, ke kteremu je pripojeno Servo
     */
     /*************************************************************************************************************************************/
    Servo_mech(uint8_t pin);
  
    /************************************************************************************************************/
    /* Metoda vracejici uhel */
    /************************************************************************************************************/
    int16_t set_angle(uint8_t pos) {myservo.write(pos);}

    /*************************************************************************************************************************************/
    /* Metoda vracejici poradove cislo objektu (jeho index v poli) */
    /*************************************************************************************************************************************/
    uint8_t get_my_index(void) { return myIndex; }

}; /* Servo */

#endif /* CONTROLLERS_H */