classdef Inductive_sensor < controllers.Utility
    %------------------------------------------------------------------------------------------------------
    % Inductive_sensor Description:
    %
    %   An Inductive_sensor object represent a controller for controlling the inductive sensor 
    %   connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 5.3.2023
    %
    % Inductive_sensor Syntax:
    %
    %   obj = Inductive_sensor(port, digital_pin, crc)
    %
    % 
    % Inductive_sensor Methods:
    %
    %  Inductive_sensor   - The constructor creates and connects a new controller to control 
    %                       the inductive sensor connected to an Arduino.
    %
    %  is_detected_object - This method returns true, if some object was detected by the sensor; 
    %                       otherwise, it returns false.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)
        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        % Digital_sensor
        NEW_SENSOR = [5, 1, 4];

        % Zprava, ktera se odesle pro ziskani aktualni hodnoty na 
        % sledovanem pinu (1 nebo 0) 
        GET_VALUE  = [5, 2, 27];
    end % properties - constant

    % Pristupove metody
    methods (Access = public)

        function this = Inductive_sensor(port, pin, crc)
            % The constructor creates and connects a new controller to control the inductive sensor 
            % connected to an Arduino.
            %
            % Inductive_sensor Syntax:
            %
            %   obj = Inductive_sensor(port, pin, crc)
            %
            % Inductive_sensor Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the output of the sensor is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Inductive_sensor Outputs:
            %
            %   - obj: Newly created object of the Inductive_sensor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin < 0 || pin > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.NEW_SENSOR, pin);

            disp('The controller for an inductive sensor was successfully connected.');
        end % Inductive_sensor

        function res = is_detected_object(this)
            % This method returns true, if some object was detected by the sensor; otherwise, it returns false.
            %
            % is_detected_object Syntax:
            %
            %   result = obj.is_detected_object()
            %
            % is_detected_object Outputs:
            %
            %   - result: True if some object was detected by the sensor; otherwise, False.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            res = ~this.msg_read(this.GET_VALUE, 1);
        end % is_detected_object

    end % methods - public

end % classdef