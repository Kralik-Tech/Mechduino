classdef Ultrasonic_sensor < controllers.Utility
    %------------------------------------------------------------------------------------------------------
    % Ultrasonic_sensor Description:
    %
    %   An Ultrasonic_sensor object represents a controller for controlling the ultrasonic sensor 
    %   connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 21.11.2023
    %
    % Ultrasonic_sensor Syntax:
    %
    %   obj = Ultrasonic_sensor(port, trig_pin, echo_pin, crc)
    %
    % Ultrasonic_sensor Methods:
    %
    %   Ultrasonic_sensor - The constructor creates and connects a new controller to control
    %                       the Ultrasonic sensor connected to an Arduino.
    %
    %   get_distance      - This method returns the distance measured by the sensor in centimeters.
    % 
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %------------------------------------------------------------------------------------------------------
    
    % KONSTANTY
    properties (Access = private, Constant)

        % Zprava, ktera se zasle do arduina jako zadost o pripojeni nove
        % knihovny
        COMMAND_ADD_SONAR    = [6, 1, 3];

        % Zprava, ktera se zasle do arduina pro ziskani namerene
        % vzdalenosti
        COMMAND_GET_DISTANCE = [5, 2, 26];
    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Ultrasonic_sensor(port, trig_pin, echo_pin, crc)
            % The constructor creates and connects a new controller to controll the 
            % Ultrasonic sensor connected to an Arduino.
            %
            % Ultrasonic_sensor Syntax:
            %
            %   obj = Ultrasonic_sensor(port, trig_pin, echo_pin, crc)
            %
            % Ultrasonic_sensor Inputs:
            % 
            %   - port    : The serialport object which is used for communication with Arduino.
            %   - trig_pin: The digital pin number on the Arduino to which the Trig output from the sensor is connected.
            %   - echo_pin: The digital pin number on the Arduino to which the Echo output from the sensor is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Ultrasonic_sensor Outputs:
            %
            %   - obj: Newly created object of the Ultrasonic_sensor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(trig_pin < 0 || trig_pin > 255 || echo_pin < 0 || echo_pin > 255)
                error("Error! The 'trig_pin' and 'echo_pin' number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_ADD_SONAR, [trig_pin, echo_pin]);

            disp("The controller for an ultrasonic sensor was successfully connected.");

        end % Ultrasonic_sensor

        function dis = get_distance(this)
            % This method returns the distance measured by the sensor in centimeters.
            %
            % get_distance Syntax:
            %
            %   result = obj.get_distance()
            %
            % get_distance Outputs:
            %
            %   - result: The distance measured by the sensor in centimeters
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            bits = this.msg_read(this.COMMAND_GET_DISTANCE, 2);
            dis = this.make_16(bits(1), bits(2));
            dis = dis * 10; % from cm to mm
        end % get_distance

    end % methods - public

end % classdef