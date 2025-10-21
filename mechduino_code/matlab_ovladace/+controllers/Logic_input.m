classdef Logic_input < controllers.Utility
    %---------------------------------------------------------------------------------------------------------
    % Logic_input Description:
    %
    %   A Logic_input object represents a controller for controlling the logic input 
    %   connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 22.11.2023
    %
    % Logic_input Syntax:
    %
    %   obj = Logic_input(port, digital_pin, crc)
    % 
    % Logic_input Methods:
    %
    %  Logic_input - The constructor creates and connects a new controller to control the motion sensor
    %                  connected to an Arduino.
    %
    %  check_motion  - This method returns true if motion was detected by motion sensor; otherwise, false.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %---------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Access = private, Constant)

        % Zprava, ktera se odesle pro pripad pripojeni noveho ovladace
        COMMAND_ADD_SENSOR   = [5, 1, 4];

        % Zprava, ktera se odesle pro pripad zjisteni pohybu
        COMMAND_READ = [5, 2, 27];
    end % properties constant
    
    % PRISTUPOVE METODY
    methods (Access = public)
       
        function this = Logic_input(port, pin, crc)
            % The constructor creates and connects a new controller to control the motion sensor
            % connected to an Arduino.
            %
            % Logic_input Syntax:
            %
            %   obj = Logic_input(port, pin, crc)
            %
            % Logic_input Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The digital pin number on the Arduino to which the output of the sensor is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Logic_input Outputs:
            %
            %   - obj: Newly created object of the Logic_input.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin < 0 || pin > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_ADD_SENSOR, pin)
    
            disp("The controller for a motion sensor was successfully connected.");

        end % Logic_input

        function out = read(this)
            % This method returns true if motion was detected by motion sensor; otherwise, false.
            %
            % check_motion Syntax:
            %
            %   result = obj.check_motion()
            %
            % check_motion Output:
            %
            %   - result: True if motion was detected by motion sensor; otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            
            out = this.msg_read(this.COMMAND_READ, 1);
        end % check_motion

    end % methods public

end % classdef