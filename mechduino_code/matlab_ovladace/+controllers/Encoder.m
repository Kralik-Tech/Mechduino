classdef Encoder < controllers.Utility
    %---------------------------------------------------------------------------------------------------------
    % Encoder Description:
    %
    %   An Encoder object represents a controller for controlling the encoder connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 14.02.2024
    %
    % Encoder Syntax:
    %
    %   obj = Encoder(port, pin, crc)
    %
    % Encoder Methods:
    %
    %   Encoder   - The constructor creates and connects a new controller to control 
    %               the encoder connected to an Arduino.
    %
    %   get_speed - This method returns the speed measured by the encoder in revolutions per minute.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %---------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        COMMAND_NEW_ENCODER = [5, 1, 7];

        % Zprava, ktera se odesle pro zjisteni rychlosti motoru
        COMMAND_GET_SPEED   = [5, 2, 39];

        % Zprava, ktera se odesle pro zjisteni rychlosti motoru
        COMMAND_GET_HOLES   = [5, 2, 62];
    end

    % Pristupove metody
    methods (Access = public)

        function this = Encoder(port, pin, crc)
            % The constructor creates and connects a new controller to control 
            % the encoder connected to an Arduino.
            %
            % Encoder Syntax:
            %
            %   obj = Encoder(port, pin, crc)
            %
            % Encoder Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the output of the encoder is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Encoder Outputs.
            %
            %   - obj: Newly created object of the Encoder.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin < 0 || pin > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_ENCODER, pin);

            disp("The controller for an encoder was successfully connected.");
        end % Encoder

        function speed = get_speed(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_speed Syntax:
            %
            %   result = obj.get_speed()
            %
            % get_speed Outputs:
            %
            %   - result: The speed measured by the encoder in revolutions per minute.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            s = this.msg_read(this.COMMAND_GET_SPEED, 2);
            speed = this.make_16(s(1), s(2));
        end % get_speed

        function holes = get_ticks(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_speed Syntax:
            %
            %   result = obj.get_speed()
            %
            % get_speed Outputs:
            %
            %   - result: The speed measured by the encoder in revolutions per minute.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            s = this.msg_read(this.COMMAND_GET_HOLES, 2);
            holes = this.make_16(s(1), s(2));
        end % get_speed

    end % methods

end % classdef