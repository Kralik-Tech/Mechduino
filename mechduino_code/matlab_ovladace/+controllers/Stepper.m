classdef Stepper < controllers.Utility
    %----------------------------------------------------------------------------------------------------------
    % Stepper Decription:
    %
    %   A Stepper object represents a controller for controlling the stepper motor connected to an Adafruit
    %   motorshield v2 or a driver that is connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 19.11.2023
    %
    % Stepper Syntax:
    %
    %   obj = Stepper(port, crc, pin, spr)
    %   obj = Stepper(port, crc, in1, in2, in3, in4)
    %
    % Stepper Methods:
    %
    %   Stepper          - The constructor creates and connects a new controller to control the stepper motor 
    %                      connected to a motorshield or a driver that is connected to an Arduino.
    %
    %   set_speed        - This method is used to set a speed of the motor.
    %
    %   onestep          - This method is used to move the motor by 1 step.
    %
    %   step             - This method is used to move the motor by the specified number of steps.
    %
    %   is_moving        - This method returns true, if the motor is moving; otherwise, it returns false.
    %
    %   change_direction - This method is used to change direction of rotation of the motor.
    %
    %   stop             - This method is used to stop the motor.
    %
    %   m_continue       - This method is used to complete the remaining steps after the motor has been 
    %                      stopped using the stop method.
    %
    % See also: % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Ultrasonic_sensor
    %
    %----------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)
        % Zprava, ktera ma poslat pro pripojeni noveho ovladace
        COMMAND_NEW_ADAFRUIT_STEPPER       = [7, 1, 2];

        % Zprava, ktera se ma poslat pro prikaz release
        COMMAND_ADAFRUIT_RELEASE           = [5, 2, 22];

        % Zprava, ktera se ma poslat pro prikaz set_speed
        COMMAND_ADAFRUIT_SET_SPEED         = [6, 2, 23];

        % Zprava, ktera se ma poslat pro prikaz onestep
        COMMAND_ADAFRUIT_ONESTEP           = [7, 2, 24];

        % Zprava, ktera se ma poslat pro prikaz step
        COMMAND_ADAFRUIT_STEP              = [9, 2, 25];

        % Zprava, ktera se ma poslat pro prikaz stop
        COMMAND_ADAFRUIT_STOP              = [5, 2, 29];

        % Zprava, ktera se odesle pro zjisteni, jestli je motor v pohybu
        COMMAND_ADAFRUIT_IS_MOVING         = [5, 2, 30];

        % Zprava, ktera se odesle pro pokracovani otaceni motoru (adafruit)
        COMMAND_ADAFRUIT_CONTINUE          = [7, 2, 52];

        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        % Driver_Stepper_motor
        COMMAND_NEW_DRIVER_STEPPER         = [8, 1, 8];

        % Zprava, ktera se odesle pro zaslani prikazu set_speed
        COMMAND_DRIVER_SET_SPEED           = [6, 2, 40];

        % Zprava, ktera se odesle pro zaslani prikazu onestep
        COMMAND_DRIVER_ONESTEP             = [6, 2, 41];

        % Zprava, ktera se odesle pro zaslani prikazu step
        COMMAND_DRIVER_STEP                = [8, 2, 42];

        % Zprava, ktera se odesle pro zaslani prikazu stop
        COMMAND_DRIVER_STOP                = [5, 2, 43];

        % Zprava, ktera se odesle pro zaslani prikazu is_moving
        COMMAND_DRIVER_IS_MOVING           = [5, 2, 44];

        % Zprava, ktera se odesle pro zaslani prikazu continue (driver)
        COMMAND_DRIVER_CONTINUE            = [7, 2, 51];

        % Ovladac, ktery bude pouzit pro motor pripojeny na Adafruit
        % Motorshield v2
        ADAFRUIT_MOTOR = 1;

        % Ovladac, ktery bude pouzit pro motor pripojen na vlastni driver
        DRIVER_MOTOR   = 2;

    end % properties - constant
    
    % CLENSKE PROMENNE
    properties (Access = private)
        % Promenna uchovavajici smer otaceni motoru ovladaneho Adafruit
        % Motorshield v2
        adafruit_dir;

        % Promenna uchovavajici smer otaceni motoru ovladaneho vlastnim
        % driverem
        driver_dir;

        % Typ ovladace, ktery bude pouzit:
        % Ovladac pro motor pripojen na Adafruit Motorshield v2
        % Ovladac pro motor ovladany vlastnim driverem
        type;

        % Aktualni rychlost otaceni motoru
        speed;
    end

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Stepper(port, crc, varargin)
            % The constructor creates and connects a new controller to control the stepper motor connected 
            % to a motorshield or a driver that is connected to an Arduino.
            %
            % Stepper Syntax:
            %
            %   obj = Stepper(port, crc, pin, spr)           - This is called when the motor is connected to a motorshield.
            %   obj = Stepper(port, crc, in1, in2, in3, in4) - This is called when the motor is connected to a driver.
            %
            % Stepper Inputs
            %
            %   - port: The serialport object which is used for communication with Arduino.
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
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Stepper Outputs:
            %
            %   - obj: Newly created object of the Stepper.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            
           this@controllers.Utility(port, crc);
           
           this.speed = 255;

           switch(nargin)
               case 4
                   this.type = this.ADAFRUIT_MOTOR;
                   pin       = varargin{1};
                   spr       = varargin{2};
               case 6
                   this.type = this.DRIVER_MOTOR;
                   pin1      = varargin{1};
                   pin2      = varargin{2};
                   pin3      = varargin{3};
                   pin4      = varargin{4};
               otherwise
                   error('Error! Incorrectly entered number of parameters.');
           end

           if(this.type == this.ADAFRUIT_MOTOR)

               if((pin ~= 1) && (pin ~= 2))
                   error("Error! The pin number must be in the range of 1-2.");
               end
    
               if(spr < 1 || spr > 65535)
                   error("Error! The 'spr' value must be in the range of 0-65535.");
               end
               
               [top_bits, low_bits] = this.split_bits(spr);
    
               this.msg_new_controller_params(this.COMMAND_NEW_ADAFRUIT_STEPPER, ...
                   pin, [top_bits, low_bits]);
               
               this.adafruit_dir = 1;

           else

               if(pin1 < 0 || pin1 > 255 || pin2 < 0 || pin2 > 255 ||...
                  pin3 < 0 || pin3 > 255 || pin4 < 0 || pin4 > 255)
                   error("Error! The pin number must be in the range of 0-255.");
               end

               this.msg_new_controller_params(this.COMMAND_NEW_DRIVER_STEPPER, ...
                   pin1, [pin2, pin3, pin4]);

               this.driver_dir = 1;

           end
    
               disp("The controller for a stepper motor was successfully connected.");

        end % Stepper

        function release(this)
            % This method removes all power from the motor. This method can be used only if the
            % motor is connected to the motorshield.
            %
            % RELEASE Syntax:
            %
            %   obj.RELEASE()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor       

            if(this.type == this.ADAFRUIT_MOTOR)
                this.msg_write(this.COMMAND_ADAFRUIT_RELEASE);
            end
        end % release

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
            
            if(speed < 0 || speed > 255)
                error("Error! The 'speed' value must be in the range of 0-255.");
            end
            
            speed = round(speed);

            this.speed = speed;

            if(this.type == this.ADAFRUIT_MOTOR)
                this.msg_write_params(this.COMMAND_ADAFRUIT_SET_SPEED, speed);
            else
                this.msg_write_params(this.COMMAND_DRIVER_SET_SPEED, speed);
            end

        end % set_speed

        function onestep(this, varargin)
        % This method is used to move the motor by 1 step.
        %
        % ONESTEP Syntax:
        %
        %   obj.ONESTEP()
        %   obj.ONESTEP('Direction', 'Forward')
        %   obj.ONESTEP('Direction', 'Backward')
        %
        % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
        %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if(this.type == this.ADAFRUIT_MOTOR)

                switch(nargin)
                    case 1
                        num_style = this.get_number_style('Double');
                    case 3
                        if(strcmp(varargin{1}, 'Style'))
                            num_style = this.get_number_style(varargin{2});
                        elseif(strcmp(varargin{1}, 'Direction'))
                            num_style         = this.get_number_style('Double');
                            this.adafruit_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    case 5
                        if(strcmp(varargin{1}, 'Style') && strcmp(varargin{3}, 'Direction'))
                            num_style         = this.get_number_style(varargin{2});
                            this.adafruit_dir = this.get_number_dir(varargin{4});
                        elseif(strcmp(varargin{1}, 'Direction') && strcmp(varargin{3}, 'Style'))
                            num_style         = this.get_number_style(varargin{4});
                            this.adafruit_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    otherwise
                        error('Error! Incorrectly entered number of parameters.');
                end

                this.msg_write_params(this.COMMAND_ADAFRUIT_ONESTEP,...
                         [this.adafruit_dir, num_style]);
            else
                
                switch(nargin)
                    case 1
                    case 3
                        if(strcmp(varargin{1}, 'Direction'))
                            this.driver_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    otherwise
                        error('Error! Incorrectly entered number of parameters.');
                end

                this.msg_write_params(this.COMMAND_DRIVER_ONESTEP, this.driver_dir);
            end

        end % onestep

        function step(this, steps, varargin)
            % This method is used to move the motor by the specified number of steps.
            %
            % STEP Syntax:
            %
            %   obj.STEP(steps)
            %   obj.STEP(steps, 'Direction', 'Forward')
            %   obj.STEP(steps, 'Direction', 'Backward')
            %
            % STEP Inputs:
            %
            %   - steps: The value representing the specified number of steps that is type of uint16.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
             
            if(steps < 1 || steps > 65535)
                error("Error! The 'steps' value must be in the range of 0-65535.");
            end
            
            steps = round(steps);

            [top_bits, low_bits] = this.split_bits(steps);
            
            if(this.type == this.ADAFRUIT_MOTOR)
                
                switch(nargin)
                    case 2
                        num_style = this.get_number_style('Double');
                    case 4
                        if(strcmp(varargin{1}, 'Style'))
                            num_style = this.get_number_style(varargin{2});
                        elseif(strcmp(varargin{1}, 'Direction'))
                            num_style = this.get_number_style('Double');
                            this.adafruit_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    case 6
                        if(strcmp(varargin{1}, 'Style') && strcmp(varargin{3}, 'Direction'))
                            num_style         = this.get_number_style(varargin{2});
                            this.adafruit_dir = this.get_number_dir(varargin{4});
                        elseif(strcmp(varargin{1}, 'Direction') && strcmp(varargin{3}, 'Style'))
                            num_style         = this.get_number_style(varargin{4});
                            this.adafruit_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    otherwise
                        error('Error! Incorrectly entered number of parameters.');
                end

                this.msg_write_params(this.COMMAND_ADAFRUIT_STEP,...
                    [top_bits, low_bits, this.adafruit_dir, num_style]);

            else
                
                switch(nargin)
                    case 2
                    case 4
                        if(strcmp(varargin{1}, 'Direction'))
                            this.driver_dir = this.get_number_dir(varargin{2});
                        else
                            error("Error! Unrecognized property " + string(varargin{1}) + ".");
                        end
                    otherwise
                        error('Error! Incorrectly entered number of parameters.');
                end

                this.msg_write_params(this.COMMAND_DRIVER_STEP, ...
                    [top_bits, low_bits, this.driver_dir]);

            end

        end % step
       
        function stop(this)
            % This method is used to stop the motor.
            %
            % STOP Syntax:
            %
            %   obj.STOP();
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if(this.type == this.ADAFRUIT_MOTOR)
                this.msg_write(this.COMMAND_ADAFRUIT_STOP);
            else
                this.msg_write(this.COMMAND_DRIVER_STOP);
            end

        end % stop
        
        function out = is_moving(this)
            % This method returns true, if the motor is moving; otherwise, it returns false.
            %
            % is_moving Syntax:
            %
            %   result = obj.is_moving()
            %
            % is_moving Outputs:
            %
            %   - retult: True, if the motor is moving; otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if(this.type == this.ADAFRUIT_MOTOR)
                out = this.msg_read(this.COMMAND_ADAFRUIT_IS_MOVING, 1);
            else
                out = this.msg_read(this.COMMAND_DRIVER_IS_MOVING, 1);
            end

        end % i
        function change_direction(this)
            % This method is used to change direction of rotation of the motor.
            %
            % change_direction Syntax:
            %
            %   obj.change_direction()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if(this.type == this.ADAFRUIT_MOTOR)
                if(this.adafruit_dir == 1)
                    this.adafruit_dir = 2;
                else
                    this.adafruit_dir = 1;
                end
            else
                if(this.driver_dir == 1)
                    this.driver_dir = 0;
                else
                    this.driver_dir = 1;
                end
            end
            
            if(this.is_moving())
                this.stop();
                this.m_continue();
            end

        end % change_direction

        function m_continue(this)
            % This method is used to complete the remaining steps after the motor has been 
            % stopped using the stop method.
            %
            % m_continue Syntax:
            %
            %   obj.m_continue()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if(this.type == this.ADAFRUIT_MOTOR)
                this.msg_write_params(this.COMMAND_ADAFRUIT_CONTINUE, ...
                    [this.speed, this.adafruit_dir]);
            else
                this.msg_write_params(this.COMMAND_DRIVER_CONTINUE, ...
                    [this.speed, this.driver_dir]);
            end
        end % m_continue

    end % methods - public

        % SOUKROME POMOCNE METODY
    methods (Access = private)

        % Funkce vracejici cislo dir podle zadaneho retezce v
        % metodach step a onestep
        %
        % dir   - retezec zadaneho smeru
        function num_dir = get_number_dir(this, dir)

            if(strcmp(dir, 'Forward'))
                num_dir = 1;
            elseif(strcmp(dir, 'Backward'))
                if(this.type == this.ADAFRUIT_MOTOR)
                    num_dir = 2;
                else
                    num_dir = 0;
                end
            else
                error("Error! Unrecognized property " + string(dir) + ".");
            end

        end
        
        % Metoda vracejici cislo style podle zadaneho textoveho retezce
        %
        % style - retezec zadaneho stylu
        function num_style = get_number_style(~, style)

            if(strcmp(style,     'Single'))
                num_style = 1;
            elseif(strcmp(style, 'Double'))
                num_style = 2;
            elseif(strcmp(style, 'Interleave'))
                num_style = 3;
            elseif(strcmp(style, 'Microstep'))
                num_style = 4;
            else
                error("Error! Unrecognized property " + string(style) + ".");
            end

        end % get_number

    end % methods - private

end % classdef