classdef Mechduino < handle
    %-----------------------------------------------------------------------------------------------------------
    % Mechduino Description
    %
    %   A Mechduino object represents a controller for controlling the Arduino. This object is 
    %   also used for creating and returning specific controllers for controlling individual components 
    %   connected to an Arduino. 
    %
    %
    %   Author    : Jakub Smelik
    %   Created on: 18.11.2023
    %
    % Mechduino Syntax:
    %
    %   obj = Mechduino(COM)
    %
    % Mechduino Methods:
    %
    %   Mechduino       - The constructor creates a new controller to control the Arduino.
    %
    %   pause                 - This method is used to pause the program to specific amount of time.
    %
    %   get_LED               - This factory method returns newly created controller for the LED diode.
    %
    %   get_Stepper           - This factory method returns newly created controller for the stepper motor.
    %
    %   get_DC_motor          - This factory method returns newly created controller for the DC motor.
    %
    %   get_Ultrasonic_sensor - This factory method returns newly created controller for the ultasonic sensor.
    %
    %   get_Logic_input       - This factory method returns newly created controller for the logic input.
    %
    %   get_Logic_output      - This factory method returns newly created controller for the logic output.
    %
    %   get_PWM_output        - This factory method returns newly created controller for the PWM output.
    %
    %   get_Mass_sensor       - This factory method returns newly created controller for the mass sensor.
    %
    %   get_Lidar             - This factory method returns newly created controller for the lidar.
    %
    %   get_Microphone        - This factory method returns newly created controller for the microphone.
    %
    %   get_Encoder           - This factory method returns newly created controller for the encoder.
    %
    %   get_Magnetic_encoder  - This factory method returns newly created controller for the magnetic encoder.
    %
    %   get_Gyroscope         - This factory method returns newly created controller for the tilt sensor.
    %
    %   get_Inductive_sensor  - This factory method returns newly created controller for the inductive sensor.
    %
    %   get_Button            - This factory method returns newly created controller for the button.
    %
    % See also: controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, controllers.Gyroscope, 
    %           controllers.Inductive_sensor, controllers.LED, controllers.Button, controllers.Microphone, 
    %           controllers.Logic_input, controllers.Stepper, controllers.Ultrasonic_sensor
    %
    %-----------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)

        % Prenosova rychlost UARTu
        BAUDRATE = 9600;

        % Zprava, ktera se odesle v pripade zadosti o zastaveni programu
        COMMAND_PAUSE       = [5, 3];

        % Zprava, ktera se odesle pro zjisteni verze programu
        COMMAND_GET_VERSION = [3, 4];

        % Honota, ktera se vrati pri uspesnem ulozeni zpravy do fronty
        MESSAGE_SAVED               = 1;

        % Honota, ktera se vrati v pripade neulozeni zpravy do fronty
        MESSAGE_NOT_SAVED           = 2;

        % Aktualni verze programu
        VERSION                     = 5;

        % Maximalni pocet pripojenych Ultrazvukovych senzoru
        MAX_SONARS_COUNT            = 1;

        % Maximalni pocet pripojenych krokovych motoru
        MAX_ADAFRUIT_STEPPERS_COUNT = 2;

        % Maximalni pocet pripojenych krokovych motoru ovladanych vlastnim
        % driverem
        MAX_DRIVER_STEPPERS_COUNT   = 6; 

        % Maximalni pocet pripojenych DC motoru
        MAX_DC_MOTORS_COUNT         = 4;

        % Maximalni pocet pripojenych encoderu
        MAX_ENCODERS_COUNT          = 4;

        % Maximalni pocet pripojenych senzoru naklonu
        MAX_GYROSCOPES_COUNT        = 1;

        % Maximalni pocet pripojenych tlacitek
        MAX_BUTTONS_COUNT           = 127;

    end % properties - constant

    % CLENSKE PROMENNE
    properties (Access = private)
        
        % Instance tridy serialport, slouzici k zasilani a prijimani zprav
        % mezi arduinem a matlabem
        port;

        % Kanal, na kterem je arduino pripojeno
        com;

        % Instance tridy CRC pro pocitani hodnot CRC odesilanych zprav
        crc_obj;

        % Aktualni pocet vytvorenych ovladacu pro sonar
        act_sonar_count;

        % Aktualni pocet vytvorenych ovladacu pro krokovy motor ovladanych
        % Adafruit Motorshield v2
        act_adafruit_stepper_count;

        % Aktualni pocet vytvorenych ovladacu pro krovovy motor ovladany
        % vlastnim driverem
        act_driver_stepper_count;

        % Aktualni pocet vytvorenych ovladacu pro DC motor
        act_dc_motor_count;

        % Aktualni pocet vytvorenych ovladacu pro encoder
        act_encoder_count;

        % Aktualni pocet vytvorenych ovladacu pro senzor naklonu
        act_gyroscopes_count;

        % Aktualni pocet vytvorenych ovladacu pro tlacitko
        act_buttons_count;

    end % properties - protected

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Mechduino(COM)
            % The constructor creates a new controller to control the Arduino.
            %
            % Mechduino Sytax:
            %
            %   obj = Mechduino(COM)
            %
            % Mechduino Inputs:
            %
            %   - COM: The COM port to which the Arduino is connected. This COM port must be 
            %          written in quotation marks.
            %
            % Mechduino Outputs:
            %
            %   - obj: Newly created object of the Mechduino.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            % Zkouska, jestli je jiz nahrany potrebny program v arduinu 
            % (pripadna kontrola verze programu)
            
            this.crc_obj = controllers.CRC;
            this.com     = COM;
            this.port    = serialport(COM, this.BAUDRATE, "Timeout", 1);
            pause(1);

            % Vykona se, pokud nebude nahrany potrebny program pro arduino
            if(~this.check_version())
                delete(this.port);
                this.upload();
                this.port = serialport(COM, this.BAUDRATE, "Timeout", 1);
                pause(1);
                this.check_version();
            end

            this.act_dc_motor_count          = 0;
            this.act_encoder_count           = 0;
            this.act_sonar_count             = 0;
            this.act_adafruit_stepper_count  = 0;
            this.act_driver_stepper_count    = 0;
            this.act_gyroscopes_count        = 0;
            this.act_buttons_count           = 0;
            
            pause(1);
        end % Mechduino
        
        function pause(this, pause_time)
            % This method is used to pause the program to specific amount of time.
            %
            % PAUSE Syntax:
            %
            %   obj.pause(pause_time_millis)
            %
            % PAUSE Inputs:
            %
            %   - pause_time_millis: The duration for which the program is to be paused, specified 
            %                        in millisecond. The data type of this parameter is uint16.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            pause_time = round(pause_time);

            if(pause_time < 0 || pause_time > 65535)
                disp("Error! The parameter 'pause_time' must be in the range of 0-65535.");
                return;
            end

            [top_bits, low_bits] = this.split(pause_time);

            message = [this.COMMAND_PAUSE, top_bits, low_bits];
            my_crc  = this.crc_obj.CRC_fast(message);
            
            for i = 1:2
                this.port.write([message, my_crc], "uint8");
                res   = this.port.read(2, "uint8");
                crc   = res(1);
                saved = res(2);

                while(crc == my_crc && saved == this.MESSAGE_NOT_SAVED)
                    this.port.write([this.COMMAND_PAUSE, top_bits, low_bits, my_crc], "uint8");
                    res   = this.port.read(2, "uint8");
                    crc   = res(1);
                    saved = res(2);

                    if(saved == this.MESSAGE_SAVED)
                        break;
                    end

                    if(crc ~= my_crc)
                        break;
                    end
                end

                if(crc == my_crc)
                    break;
                end

                if(i == 2)
                    disp("An error occurred while executing the command 'pause'. Try it again.");
                    return;
                end
            end

            pause(pause_time/1000);

        end % pause

        function L = get_LED(this, pin_number)
            % This factory method returns newly created controller for the LED diode.
            % 
            % get_LED Syntax:
            %
            %   led = obj.get_LED(pin)
            %
            % get_LED Inputs:
            %
            %   - pin: The digital pin number on the Arduino to which the LED diode is connected.
            %
            % get_LED Outputs:
            %
            %   - led: Newly created controller for controlling a LED diode.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            L = controllers.LED(this.port, pin_number, this.crc_obj);
        end % get_LED

        function L = get_Logic_output(this, pin_number)
            % This factory method returns newly created controller for the LED diode.
            % 
            % get_Logic_output Syntax:
            %
            %   lo = obj.get_Logic_output(pin)
            %
            % get_Logic_output Inputs:
            %
            %   - pin: The digital pin number on the Arduino to which the lo is connected.
            %
            % get_Logic_output Outputs:
            %
            %   - lo: Newly created controller for controlling a logic output, e.g. LED diode.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            L = controllers.Logic_output(this.port, pin_number, this.crc_obj);
        end % get_Logic_output

        function S = get_Stepper(this, varargin)
            % This factory method returns newly created controller for the stepper motor.
            % This method can return max 2 controllers when controlling the motors using the motorshield, 
            % or max 6 controllers when controlling the motors using the driver.
            %
            % get_Stepper Syntax:
            %
            %   This is called when the motor is connected to a motorshield:
            %
            %   stepper = obj.get_Stepper(port, pin, spr)
            %
            %   This is called when the motor is connected to a driver:
            %
            %   stepper = obj.get_Stepper(port, in1, in2, in3, in4)
            %
            % get_Stepper Inputs:
            %
            %   - pin : The number which represents the connectors to which the motor is connected.
            %           The connectors M1 and M2 are represented by number 1 and the 
            %           connectors M3 and M4 are represented by number 2. 
            %   - spr : The number of steps required for one complete revolution.
            %
            %   - in1: The digital pin number on the Arduino to which the output in1 from the driver is connected.
            %   - in2: The digital pin number on the Arduino to which the output in2 from the driver is connected.
            %   - in3: The digital pin number on the Arduino to which the output in2 from the driver is connected.
            %   - in4: The digital pin number on the Arduino to which the output in2 from the driver is connected.
            %
            % get_Stepper Outputs:
            %
            %   - stepper: Newly created controller for controlling a stepper motor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor
            
            switch(nargin)
                case 3
                    this.act_adafruit_stepper_count = this.act_adafruit_stepper_count + 1;
                    if(this.act_adafruit_stepper_count > this.MAX_ADAFRUIT_STEPPERS_COUNT)
                        error("Error! It's not possible to have more than " + this.MAX_ADAFRUIT_STEPPERS_COUNT ...
                            + " controller(s) for a stepper motor controlled by a motorshield.");
                    end
                    pin = varargin{1};
                    spr = varargin{2};
                    S = controllers.Stepper(this.port, this.crc_obj, pin, spr);
                case 5
                    this.act_driver_stepper_count = this.act_driver_stepper_count + 1;
                    if(this.act_driver_stepper_count > this.MAX_DRIVER_STEPPERS_COUNT)
                        error("Error! It's not possible to have more than " + this.MAX_DRIVER_STEPPERS_COUNT ...
                            + " controller(s) for a stepper motor controlled by a driver.");
                    end
                    pin1 = varargin{1};
                    pin2 = varargin{2};
                    pin3 = varargin{3};
                    pin4 = varargin{4};
                    S = controllers.Stepper(this.port, this.crc_obj, pin1, pin2, pin3, pin4);
                otherwise
                    error('Error! Incorrectly entered number of parameters.');
            end
            
        end % get_Stepper

        function U = get_Ultrasonic_sensor(this, trig_pin, echo_pin)
            % This factory method returns newly created controller for the ultasonic sensor.
            % This method can return only 1 controller for the ultrasonic sensor.
            %
            % get_Ultrasonic_sensor Syntax:
            %
            %   sensor = obj.get_Ultrasonic_sensor(trig_pin, echo_pin)
            %
            % get_Ultrasonic_sensor Inputs:
            %
            %   - trig_pin: The digital pin number on the Arduino to which the Trig output from the sensor is connected.
            %   - echo_pin: The digital pin number on the Arduino to which the Echo output from the sensor is connected.
            %
            % get_Ultrasonic_sensor Outputs:
            %
            %   - sensor: Newly created controller for controlling an ultrasonic sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            this.act_sonar_count = this.act_sonar_count + 1;
            
            if(this.act_sonar_count > this.MAX_SONARS_COUNT)
                error("Error! It's not possible to have more than " + this.MAX_SONARS_COUNT + ...
                    " controller(s) for a ultrasonic sensor.");
            end

            U = controllers.Ultrasonic_sensor(this.port, trig_pin, echo_pin, this.crc_obj);
        end % get_Ultrasonic_sensor

        function M = get_Logic_input(this, pin)
            % This factory method returns newly created controller for the motion sensor.
            %
            % get_Logic_input Syntax:
            %
            %   sensor = obj.get_Logic_input(pin)
            %
            % get_Logic_input Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output of the sensor is connected.
            %
            % get_Logic_input Outputs:
            %
            %   - sensor: Newly created controller for controlling a motion sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            M = controllers.Logic_input(this.port, pin, this.crc_obj);
        end % get_Logic_input

        function M = get_Analog_input(this, pin)
            % This factory method returns newly created controller for the microphone.
            %
            % get_Analog_input Syntax:
            %
            %   mic = obj.get_Analog_input(pin)
            %
            % get_Analog_input Inputs:
            %
            %   - pin : The analog pin number on the Arduino to which the output of the Analog_input is connected.
            %
            % get_Analog_input Outputs:
            %
            %   - mic: Newly created controller for controlling a Analog_input.
            % 
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            M = controllers.Analog_input(this.port, pin, this.crc_obj);
        end
        
        function M = get_DC_motor(this, varargin)
            % This factory method returns newly created controller for the DC motor.
            % This method can return max 4 controllers for the DC motors.
            %
            % get_DC_motor Syntax:
            %
            %   motor = obj.get_DC_motor(motor_port)
            %
            % get_DC_motor Inputs:
            %
            %   - motor_port: The connector number on the motorshield to which the motor is connected.
            %
            % get_DC_motor Outputs:
            %
            %   - motor: Newly created controller for controlling a DC motor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor
           
            this.act_dc_motor_count = this.act_dc_motor_count + 1;

            if(this.act_dc_motor_count > this.MAX_DC_MOTORS_COUNT)
                error("Error! It's not possible to have more than " + this.MAX_DC_MOTORS_COUNT...
                    + " controller(s) for a DC motor.");
            end

            switch(nargin)
                case 2
                    pinPWM = varargin{1};
                    M = controllers.DC_motor(this.port, this, this.crc_obj, pinPWM);
                case 3
                    pinA = varargin{1};
                    pinPWM = varargin{2};
                    M = controllers.DC_motor(this.port, this, this.crc_obj, pinA, pinPWM);
                case 4
                    pinA = varargin{1};
                    pinB = varargin{2};
                    pinPWM = varargin{3};
                    M = controllers.DC_motor(this.port, this, this.crc_obj, pinA, pinB, pinPWM);
                otherwise
                    error('Error! Incorrectly entered number of parameters.');
            end
        end % get_DC_motor

        function e = get_Encoder(this, pin)
            % This factory method returns newly created controller for the encoder.
            % This method can return max 4 controllers for the encoders.
            %
            % get_Encoder Syntax:
            %
            %   sensor = obj.get_Encoder(pin)
            %
            % get_Encoder Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output of the encoder is connected.
            %
            % get_Encoder Outputs:
            %
            %   - sensor: Newly created controller for controlling a encoder.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            this.act_encoder_count = this.act_encoder_count + 1;

            if(this.act_encoder_count > this.MAX_ENCODERS_COUNT)
                error("Error! It's not possible to have more than" + this.MAX_ENCODERS_COUNT ...
                    + " controller(s) for a encoder.");
            end

            e = controllers.Encoder(this.port, pin, this.crc_obj);
        end % get_Encoder

        function g = get_Gyroscope(this)
            % This factory method returns newly created controller for the tilt sensor.
            % This method can return only 1 controller for the tilt sensor.
            %
            % get_Gyroscope Syntax:
            %
            %   sensor = obj.get_Gyroscope()
            %
            % get_Gyroscope Outputs:
            %
            %   - sensor: Newly created controller for controlling a tilt sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            this.act_gyroscopes_count = this.act_gyroscopes_count + 1;

            if(this.act_gyroscopes_count > this.MAX_GYROSCOPES_COUNT)
                error("Error! It's not possible to have more than " + this.MAX_GYROSCOPES_COUNT ...
                    + " controller(s) for a tilt sensor.");
            end

            g = controllers.Gyroscope(this.port, this.crc_obj);
        end % get_Gyroscope

        function s = get_RGB_sensor(this, pinS0, pinS1, pinS2, pinS3, pinOut)
            % This factory method returns newly created controller for the RGB sensor.
            %
            % get_RGB_sensor Syntax:
            %
            %   sensor = obj.get_RGB_sensor(pinS0, pinS1, pinS2, pinS3, pinOut)
            %
            % get_RGB_sensor Inputs:
            %
            %   - pinS0 : The digital pin number on the Arduino to which the output S0 from the sensor is connected.
            %   - pinS1 : The digital pin number on the Arduino to which the output S1 from the sensor is connected.
            %   - pinS2 : The digital pin number on the Arduino to which the output S2 from the sensor is connected.
            %   - pinS3 : The digital pin number on the Arduino to which the output S3 from the sensor is connected.
            %   - pinOut: The digital pin number on the Arduino to which the output OUT from the sensor is connected.
            %
            % get_RGB_sensor Outputs:
            %
            %   - sensor: Newly created controller for controlling a RGB sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            s = controllers.RGB_sensor(this.port, pinS0, pinS1, pinS2, ...
                pinS3, pinOut, this.crc_obj);
        end % get_RGB_sensor

        function s = get_Inductive_sensor(this, pin)
            % This factory method returns newly created controller for the inductive sensor.
            %
            % get_Inductive_sensor Syntax:
            %
            %   sensor = obj.get_Inductive_sensor(pin)
            %
            % get_Inductive_sensor Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output of the sensor is connected.
            %
            % get_Inductive_sensor Outputs:
            %
            %   - sensor: Newly created controller for controlling an inductive sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            s = controllers.Inductive_sensor(this.port, pin, this.crc_obj);
        end

        function b = get_Button(this, pin)
            % This factory method returns newly created controller for the button.
            % This method can return max 15 controllers for the buttons.
            %
            % get_Button Syntax:
            %
            %   button = obj.get_Button(pin)
            %
            % get_Button Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output from the button is connected.
            %
            % get_Button Outputs:
            %
            %   - button: Newly created controller for controlling a push-button.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor

            this.act_buttons_count = this.act_buttons_count + 1;
            if(this.act_buttons_count > this.MAX_BUTTONS_COUNT)
                error("Error! It's not possible to have more than " + this.MAX_BUTTONS_COUNT + ...
                    " controller(s) for a button.");
            end

            b = controllers.Button(pin, this.port, this.crc_obj);
        end % get_Button

        function l = get_Lidar(this)
            % This factory method returns newly created controller for the lidar distance sensor.
            %
            % get_Lidar Syntax:
            %
            %   lidar = obj.get_Lidar()
            %
            % get_Lidar Inputs:
            %
            %   - none - Lidar must be connected to I2C pins SCL and SDA
            %
            % get_Lidar Outputs:
            %
            %   - lidar: Newly created controller for controlling a lidar distance sensor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor, controllers.Lidar


            l = controllers.Lidar(this.port, this.crc_obj);
        end % get_Lidar

        function PWM = get_PWM_output(this, pin)
            % This factory method returns newly created controller for the button.
            % This method can return max 15 controllers for the buttons.
            %
            % get_Button Syntax:
            %
            %   button = obj.get_Button(pin)
            %
            % get_Button Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output from the button is connected.
            %
            % get_Button Outputs:
            %
            %   - button: Newly created controller for controlling a push-button.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            PWM = controllers.PWM_output(this.port, pin, this.crc_obj);
        end % get_PWM_output

        function M = get_Mass_sensor(this, pinCLK, pinDATA)
            % This factory method returns newly created controller for the button.
            % This method can return max 15 controllers for the buttons.
            %
            % get_Button Syntax:
            %
            %   button = obj.get_Button(pin)
            %
            % get_Button Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output from the button is connected.
            %
            % get_Button Outputs:
            %
            %   - button: Newly created controller for controlling a push-button.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            M = controllers.Mass_sensor(this.port, pinCLK, pinDATA, this.crc_obj);
        end % get_Mass_sensor

        function ME = get_Magnetic_encoder(this)
            % This factory method returns newly created controller for the button.
            % This method can return max 15 controllers for the buttons.
            %
            % get_Button Syntax:
            %
            %   button = obj.get_Button(pin)
            %
            % get_Button Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output from the button is connected.
            %
            % get_Button Outputs:
            %
            %   - button: Newly created controller for controlling a push-button.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            ME = controllers.Magnetic_encoder(this.port, this.crc_obj);
        end % get_Magnetic_encoder

        function S = get_Servo(this, pin)
            % This factory method returns newly created controller for the
            % servo
            %
            % get_Servo Syntax:
            %
            %   Servo = obj.get_Servo(pin)
            %
            % get_Servo Inputs:
            %
            %   - pin : The digital pin number on the Arduino, which is connected to the servo.
            %
            % get_Servo Outputs:
            %
            %   - servo: Newly created controller for controlling a servo motor.
            %
            % See also: Mechduino, controllers.RGB_sensor, controllers.DC_motor, controllers.Encoder, 
            %           controllers.Gyroscope, controllers.Inductive_sensor, controllers.LED, controllers.Button, 
            %           controllers.Microphone, controllers.Logic_input, controllers.Stepper, 
            %           controllers.Ultrasonic_sensor


            S = controllers.Servo(this.port, pin, this.crc_obj);
        end % get_Magnetic_encoder

    end % methods - public

    % SOUKROME POMOCNE METODY
    methods (Access = private)
        % Funkce ktera vrati hornich a dolnich 8 bitu zadaneho 16bitoveho
        % cisla
        function [top_bits, low_bits] = split(~, num)

            top_bits = bitshift(uint16(num), -8);
            low_bits = bitand(uint16(num), uint16(0xFF));

        end % split

        % Funcke nahravajici hex kod do matlabu
        function upload(this)
            disp("Preparing an Arduino ...");

            file = 'kod_arduino.ino.hex';
            path = char(string(fileparts(which(mfilename))) + "\hex_files\");
            
            %avrdude.exe a avrdude.config museji byt ve stejne slozce jako skript v podslozce "avrdude_files"
            currentFolder = fileparts(which(mfilename));
            
            avrdudefname = '\avrdude_files\avrdude.exe';
            avrdudeconfname = '\avrdude_files\avrdude.conf';
            
            avrdudepath = sprintf('"%s%s"',currentFolder, avrdudefname);
            avrdudeconfpath = sprintf('"%s%s"',currentFolder, avrdudeconfname);
            
            
            %procesor na arduinu:
            %pro arduino mega 2560 => atmega2560 nebo m2560
            %viz datasheet str. 1-6
            procesor = 'm2560';
            
            programmer = 'wiring';
                        
            % strUpload = instrukce do cmd
            %flash - uklada do flash pameti arduina
            %w - write, zapisuje
            %i - ve formatu Intel Hex
            strUpload = sprintf('%s -C%s -p%s -c%s -P%s -D -Uflash:w:"%s%s":i', ...
                avrdudepath, avrdudeconfpath, procesor, programmer, this.com, path, file);
            
            % spusti cmd
            [status, ~] = system(strUpload);
            if(~status)
                disp("Arduino was successfully prepared. You can use it now.")
            else
                error("An error occurred while uploading an .hex file.");
            end

        end % upload

        % Pomocna funkce pro kontrolu verze programu
        % res - logicka hodnota - 1 pokud v arduinu je potrebny program, 0
        % pokud neni
        function res = check_version(this)
            my_crc = this.crc_obj.CRC_fast(this.COMMAND_GET_VERSION);

            this.port.write([this.COMMAND_GET_VERSION, my_crc], "uint8");
            r = this.port.read(3, "uint8");
            
            res = 0;

            if(numel(r) < 3)
                return;
            end
            
            % Spolu s verzi prijde i informace o ulozeni zpravy (r(2)), ta
            % nas vsak zde nezajima
            crc     = r(1); 
            version = r(3);

            if(crc ~= my_crc)
                return;
            end

            if(version ~= this.VERSION)
                disp("Uploading a firmware for an Arduino ...");
                res = 0;
                return;
            end

            res = 1;

        end % check_version

    end % methods - private

    % SKRYTE METODY
    methods (Hidden) 
        % Skryti zdedenych metod od tridy handle

        function varargout = findobj(O,varargin)
            varargout = findobj@handle(O,varargin{:});
        end

        function varargout = findprop(O,varargin)
            varargout = findprop@handle(O,varargin{:});
        end

        function varargout = addlistener(O,varargin)
            varargout = addlistener@handle(O,varargin{:});
        end
        
        function varargout = notify(O,varargin)
            varargout = notify@handle(O,varargin{:});
        end

        function varargout = listener(O,varargin)
            varargout = listener@handle(O,varargin{:});
        end

        function varargout = delete(O,varargin)
            varargout = delete@handle(O,varargin{:});
        end

        function vargout = eq(H1, H2)
            vargout = eq@handle(H1, H2);
        end

        function vargout = ge(H1, H2)
            vargout = ge@handle(H1, H2);
        end

        function vargout = gt(H1, H2)
            vargout = gt@handle(H1, H2);
        end

        function vargout = le(H1, H2)
            vargout = le@handle(H1, H2);
        end

        function vargout = lt(H1, H2)
            vargout = lt@handle(H1, H2);
        end

        function vargout = ne(H1, H2)
            vargout = ne@handle(H1, H2);
        end
    end

end % classdef