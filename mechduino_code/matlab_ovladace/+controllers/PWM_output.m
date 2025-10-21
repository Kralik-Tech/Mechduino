classdef PWM_output < controllers.Utility
    %-------------------------------------------------------------------------------------------------------
    % PWM_output Description:
    %
    %   A PWM_output object represents a controller for controlling the LED diode connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 18.11.2023
    %
    % LED Syntax:
    %
    %   obj = PWM_output(port, pin, crc)
    %
    % LED Methods:
    %
    %   PWM_output      - The constructor creates and connects a new controller to control the LED diode
    %              connected to an Arduino.
    %
    %   PWM_set_s - set duty cycle.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, Stepper, Ultrasonic_sensor
    %
    %-------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)
        % Prikaz k pripojeni noveho ovladace v arduinu
        COMMAND_NEW_PWM_OUTPUT  = [5, 1, 12];

        % Prikaz k rozsviceni nastaveni stridy
        COMMAND_SET_PWM_S = [6, 2, 54];
    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = PWM_output(port, pin_number, crc)
            % The constructor creates and connects a new controller to control the LED diode
            % connected to an Arduino.
            % 
            % PWM_output Syntax:
            %
            %   obj = PWM_output(port, pin, crc)
            %
            % PWM_output Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the PWM_output is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % PWM_output Outputs:
            % 
            %   - obj: Newly created object of the PWM_output.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);
            PWM_ports = [2 3 4 5 6 7 8 9 10 13 44 45 46];
            if ~any(pin_number == PWM_ports)
                error("Error! The pin number must be PWM output, (pins: " + char(num2str(PWM_ports)) + ")");
            end

            this.msg_new_controller(this.COMMAND_NEW_PWM_OUTPUT, pin_number);
            
            disp("The controller for a PWM output at pin " + char(num2str(pin_number)) + " was successfully connected.");

        end % LED

        function PWM_set_s(this, strida)
            % This method is used to light up the LED diode.
            %
            % light_up Syntax:
            %
            %   obj.PWM_set_s()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, Stepper, Ultrasonic_sensor
            strida = round(strida);

            if(strida < 0 || strida > 255)
                error("Error! The parameter 's' must be in the range of 0-255.");
            end
            this.msg_write_params(this.COMMAND_SET_PWM_S, strida);
        end % PWM_set_s


    end % methods

end % classdef