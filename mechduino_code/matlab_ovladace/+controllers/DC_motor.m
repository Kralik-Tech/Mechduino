classdef DC_motor < controllers.Utility
    %----------------------------------------------------------------------------------------------------
    % DC_motor Description:
    %
    %   A DC_motor object represents a controller for controlling the DC motor connected to an Adafruit 
    %   motorshield v2 that is connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 9.2.2024
    %
    % DC_motor Syntax:
    %
    %   obj = DC_motor(port, motor_port, crc)
    %
    % DC_motor Methods:
    %
    %   DC_motor         - The constructor creates and connects a new controller to control the DC motor 
    %                      connected to an Adafruit motorshield v2 that is connected to an Arduino.
    %
    %   set_speed        - This method is used to set a speed of the motor.
    %
    %   change_direction - This method is used to change direction of rotation of the motor.
    %
    %   run              - This method is used to turn on the motor.
    %
    %   stop             - This method is used to stop the motor.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %----------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)

    end % properties - constant

    properties(Access = private)
        % Promenna ukladajici, jestli je motor v pohybu (true) nebo ne (false)
        motor_run;
        DIR;
        PWM;
        LA;
        LB;
        speed;
        wire;
    end % properties - provate

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = DC_motor(port, my_arduino, crc, varargin)
            % The constructor creates and connects a new controller to control the DC motor connected 
            % to an Adafruit motorshield v2 that is connected to an Arduino.
            %
            % DC_motor Syntax:
            %
            %   obj = DC_motor(port, motor_port, crc)
            %
            % DC_motor Inputs:
            %
            %   - port      : The serialport object which is used for communication with Arduino.
            %   - pinA/pinB : The logical pins for motor (IN1 IN2 for 1st motor / IN3 IN4 for 2nd motor).
            %   - pinPWM    : The PWM pin for motor (EN1/EN2 for 1st/2nd motor
            %   - crc       : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % DC_motor Outputs:
            %
            %   - obj: Newly created object of the DC_motor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            this@controllers.Utility(port, crc);
            switch(nargin)
               case 4
                   this.wire = 1;
                   this.PWM = my_arduino.get_PWM_output(varargin{1});
               case 5
                   this.wire = 2;
                   this.LA = my_arduino.get_Logic_output(varargin{1});
                   this.PWM = my_arduino.get_PWM_output(varargin{2});
               case 6
                   this.wire = 3;
                   this.LA = my_arduino.get_Logic_output(varargin{1});
                   this.LB = my_arduino.get_Logic_output(varargin{2});
                   this.PWM = my_arduino.get_PWM_output(varargin{3});
               otherwise
                   error('Error! Incorrectly entered number of parameters.');
           end

            this.motor_run = false;
            this.DIR = false;

            disp("The controller for a DC motor was successfully connected.");

        end % DC_motor

        function set_speed(this, speed)
            % This method is used to set a speed of the motor.
            %
            % set_speed Syntax:
            %
            %   obj.set_speed(speed)
            %
            % set_speed Inputs:
            %
            %   - speed: The value representing the speed of the motor that is of type uint8.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            this.speed = speed;
            if this.wire == 2 &&  ~this.DIR
                this.PWM.PWM_set_s(255-speed)
            else
                this.PWM.PWM_set_s(speed)
            end
        end % set_speed

        function change_direction(this)
            % This method is used to change direction of rotation of the motor.
            %
            % change_direction Syntax:
            %
            %   obj.change_direction()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            if this.wire == 1
                disp('1 wire connection, not possible to change DIR')
            else
                if(this.motor_run)
                    this.motor_brake();
                    this.DIR = ~this.DIR;
                    this.run();
                else
                    this.DIR = ~this.DIR;
                end
            end

        end % change_direction

        function run(this)
            % This method is used to turn on the motor.
            %
            % RUN Syntax:
            %
            %   obj.run()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            if this.wire == 1
                this.PWM.PWM_set_s(this.speed);

            elseif this.wire == 2
                if this.DIR
                    this.LA.set_low();
                    this.PWM.PWM_set_s(this.speed);
                else
                    this.LA.set_hight();
                    this.PWM.PWM_set_s(255-this.speed);
                end
                
            else
                if this.DIR
                    this.LA.set_hight();
                else
                    this.LB.set_hight();
                end
                this.PWM.PWM_set_s(this.speed);
            end
            this.motor_run = true;
        end % run
        
        function stop(this)
            % This method is used to stop the motor, but not brake it.
            %
            % STOP Syntax:
            %
            %   obj.stop()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if this.wire == 3
                if this.DIR
                    this.LA.set_low();
                else
                    this.LB.set_low();
                end
                this.PWM.PWM_set_s(0);
            else
                disp('1 and 2 wire connection can only be braked, so mote be')
                this.motor_brake();
            end

            this.motor_run = false;

        end % stop

        function motor_brake(this)
            % This method is used to stop and brake the motor.
            %
            % STOP Syntax:
            %
            %   obj.motor_brake()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            if this.wire == 3
                this.LA.set_low();
                this.LB.set_low();
                this.PWM.PWM_set_s(255);
            else
                if this.wire == 2 
                    this.LA.set_low();
                end
                this.PWM.PWM_set_s(0);
            end
            this.motor_run = false;
            
        end % stop

    end % methods public

end % classdef