classdef Button < controllers.Utility
    %------------------------------------------------------------------------------------------------------------
    % Button Description:
    %
    %  A Button object represents a controller for controlling the button connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 12.03.2024
    %
    % Button Syntax:
    %
    %   obj = Button(pin, port, crc)
    %
    % Button Methods:
    %
    %   Button           - The constructor creates and connects a new controller to control the button 
    %                      connected to an Arduino.
    %
    %   is_pushed        - This method returns true, if button is pushed; otherwise, it returns false.
    %
    %   get_rising_edge  - This method returns true, if a rising edge was detected; 
    %                      otherwise, it returns false.
    %
    %   get_falling_edge - This method returns true, if a falling edge was detected; 
    %                      otherwise, it returns false.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %------------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Zprava pro pripojeni noveho ovladace
        COMMAND_NEW_BUTTON       = [5, 1, 11];

        % Zprava pro zjisteni, jestli je tlacitko stisknute
        COMMAND_IS_PUSHED        = [5, 2, 48];

        % Zprava pro zjisteni detekce nabezne hrany
        COMMAND_GET_RAISING_EDGE = [5, 2, 49];

        % Zprava pro zjisteni detekce sestupne hrany
        COMMAND_GET_FALLING_EDGE = [5, 2, 50];

    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Button(pin, port, crc)
            % The constructor creates and connects a new controller to control the button 
            % connected to an Arduino.
            %
            % Button Syntax:
            %
            %   obj = Button(pin, port, crc)
            %
            % Button Inputs:
            %
            %   - pin : The digital pin number on the Arduino to which the output from the button is connected.
            %   - port: The serialport object which is used for communication with Arduino.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Button Outputs:
            %
            %   - obj: Newly created object of the Button.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor


            this@controllers.Utility(port, crc);

            if(pin < 0 || pin > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_BUTTON, pin);

            disp('The controller for a button was successfully connected.');
        end % Button

        function res = is_pushed(this)
            % This method returns true if the button is pushed; otherwise, it returns false.
            %
            % is_pushed Syntax:
            %
            %   result = obj.is_pushed()
            %
            % is_pushed Outputs:
            %
            %   - result: True if the button is pushed; otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            res = this.msg_read(this.COMMAND_IS_PUSHED, 1);
        end % is_pushed

        function res = get_rising_edge(this)
            % This method returns true if a rising edge was detected; otherwise, it returns false.
            %
            % get_rising_edge Syntax:
            %
            %   result = obj.get_rising_edge()
            %
            % get_rising_edge Outputs:
            %
            %   - result: True if a rising edge was detected; otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            res = this.msg_read(this.COMMAND_GET_RAISING_EDGE, 1);
        end % get_raising_edge

        function res = get_falling_edge(this)
            % This method returns true if a falling edge was detected; otherwise, it returns false.
            %
            % get_falling_edge Syntax:
            %
            %   result = obj.get_falling_edge()
            %
            % get_falling_edge Outputs:
            %
            %   - result: True if a falling edge was detected; otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            
            res = this.msg_read(this.COMMAND_GET_FALLING_EDGE, 1);
        end % get_falling_edge
        
    end % methods - public
end % classdef