classdef Logic_output < controllers.Utility
    %-------------------------------------------------------------------------------------------------------
    % Logic_output Description:
    %
    %   A Logic_output object represents a controller for controlling the Logic_output connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 18.11.2023
    %
    % Logic_output Syntax:
    %
    %   obj = Logic_output(port, pin, crc)
    %
    % Logic_output Methods:
    %
    %   Logic_output      - The constructor creates and connects a new controller to control the LED diode
    %              connected to an Arduino.
    %
    %   set_hight - This method is used to put logic 1 to pin.
    %
    %   set_low - This method is used to put logic 0 to pin.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %-------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)
        % Prikaz k pripojeni noveho ovladace v arduinu
        COMMAND_NEW_LOGIC_OUTPUT  = [5, 1, 1];

        % Prikaz k rozsviceni LED diody
        COMMAND_LOGIC_1 = [5, 2, 20];

        % Prikaz k zhasnuti LED diody
        COMMAND_LOGIC_0 = [5, 2, 21];
    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Logic_output(port, pin_number, crc)
            % The constructor creates and connects a new controller to control the Logic_output
            % connected to an Arduino.
            % 
            % Logic_output Syntax:
            %
            %   obj = Logic_output(port, pin, crc)
            %
            % Logic_output Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the Logic_output is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Logic_output Outputs:
            % 
            %   - obj: Newly created object of the Logic_output.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin_number < 0 || pin_number > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_LOGIC_OUTPUT, pin_number);
            
            disp("The controller for a logit output at pin " + char(num2str(pin_number)) + " was successfully connected.");

        end % LED

        function set_hight(this)
            % This method is used to set logic 1 to output.
            %
            % set_hight Syntax:
            %
            %   obj.set_hight()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this.msg_write(this.COMMAND_LOGIC_1);
        end % set_hight

        function set_low(this)
            % This method is used to set logic 0 to output.
            %
            % set_low Syntax:
            %
            %   obj.set_low()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this.msg_write(this.COMMAND_LOGIC_0);
        end % set_low

    end % methods

end % classdef