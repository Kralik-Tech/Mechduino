classdef Servo < controllers.Utility
    %----------------------------------------------------------------------------------------------------
    % Servo Description:
    %
    %   A Servo object represents a controller for controlling the DC motor connected to an Adafruit 
    %   motorshield v2 that is connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 9.2.2024
    %
    % Servo Syntax:
    %
    %   obj = Servo(port, pin, crc)
    %
    % DC_motor Methods:
    %
    %   Servo            - The constructor creates and connects a new controller to control the DC motor 
    %                      connected to an Adafruit motorshield v2 that is connected to an Arduino.
    %
    %   set_angle        - This method is used to set a angle of the servo motor.
    %
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %----------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Prikaz, ktery je zasilan pro pripojeni noveho ovladace
        COMMAND_NEW_SERVO     = [5, 1, 16];

        % Prikaz, ktery je zaslan pro nastaveni uhlu
        COMMAND_SET_ANGLE     = [6, 2, 59];

    end % properties - constant

    properties(Access = private)
        % Promenna ukladajici, jestli je motor v pohybu (true) nebo ne (false)
        angle;
    end % properties - provate

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Servo(port, pin, crc)
            % The constructor creates and connects a new controller to control the servo motor 
            %
            % Servo Syntax:
            %
            %   obj = Servo(port, pin, crc)
            %
            % DC_motor Inputs:
            %
            %   - port      : The serialport object which is used for communication with Arduino.
            %   - pin       : The logical pins for control servo.
            %   - crc       : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % DC_motor Outputs:
            %
            %   - obj: Newly created object of the DC_motor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            this@controllers.Utility(port, crc);
           
            if(pin < 0 || pin > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_SERVO, pin);

            disp("The controller for a servo motor was successfully connected.");

        end % DC_motor

        function set_angle_deg(this, angle)
            % This method is used to set a angle of the servo motor.
            %
            % set_angle_deg Syntax:
            %
            %   obj.set_angle_deg(angle)
            %
            % set_angle_deg Inputs:
            %
            %   - angle: The value representing the angle of the servo in degree.
            %            If it out of boundaries (0-180°) it was autamaticly crop. 
            %            Resolution is 1°
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            if angle>180
                angle = 180;
            elseif angle <0
                angle = 0;
            end
            this.angle = round(angle);
            this.msg_write_params(this.COMMAND_SET_ANGLE, this.angle)


        end % set_angle_deg

        function set_angle_rad(this, angle)
            % This method is used to set a angle of the servo motor.
            %
            % set_angle_rad Syntax:
            %
            %   obj.set_angle_rad(angle)
            %
            % set_angle_rad Inputs:
            %
            %   - angle: The value representing the angle of the servo in radians.
            %            If it out of boundaries (0-pi) it was autamaticly crop. 
            %            Resolution is 1°, about 0,0175 rad.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            
            angle = angle/pi*180;
            if angle>180
                angle = 180;
            elseif angle <0
                angle = 0;
            end
            this.angle = round(angle);
            this.msg_write_params(this.COMMAND_SET_ANGLE, this.angle)


        end % set_angle_rad


    end % methods public

end % classdef