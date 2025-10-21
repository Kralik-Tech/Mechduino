classdef LED < controllers.Utility
    %-------------------------------------------------------------------------------------------------------
    % LED Description:
    %
    %   A LED object represents a controller for controlling the LED diode connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 18.11.2023
    %
    % LED Syntax:
    %
    %   obj = LED(port, pin, crc)
    %
    % LED Methods:
    %
    %   LED      - The constructor creates and connects a new controller to control the LED diode
    %              connected to an Arduino.
    %
    %   light_up - This method is used to light up the LED diode.
    %
    %   turn_off - This method is used to turn off the LED diode.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %-------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)
        % Prikaz k pripojeni noveho ovladace v arduinu
        COMMAND_NEW_LED  = [5, 1, 1];

        % Prikaz k rozsviceni LED diody
        COMMAND_LIGHT_UP = [5, 2, 20];

        % Prikaz k zhasnuti LED diody
        COMMAND_TURN_OFF = [5, 2, 21];
    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = LED(port, pin_number, crc)
            % The constructor creates and connects a new controller to control the LED diode
            % connected to an Arduino.
            % 
            % LED Syntax:
            %
            %   obj = LED(port, pin, crc)
            %
            % LED Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the LED diode is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % LED Outputs:
            % 
            %   - obj: Newly created object of the LED.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin_number < 0 || pin_number > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_LED, pin_number);
            
            disp("The controller for a LED diode was successfully connected.");

        end % LED

        function light_up(this)
            % This method is used to light up the LED diode.
            %
            % light_up Syntax:
            %
            %   obj.light_up()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this.msg_write(this.COMMAND_LIGHT_UP);
        end % light_up

        function turn_off(this)
            % This method is used to turn off the LED diode.
            %
            % turn_off Syntax:
            %
            %   obj.turn_off()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this.msg_write(this.COMMAND_TURN_OFF);
        end % turn_off

    end % methods

end % classdef