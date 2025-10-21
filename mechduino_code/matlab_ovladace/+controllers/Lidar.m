classdef Lidar < controllers.Utility
    %------------------------------------------------------------------------------------------------------------
    % Lidar Description:
    %
    %  A Lidar object represents a controller for controlling the button connected to an Arduino.
    %
    %   Author    : Jan Kralik
    %   Created on: 15.01.2025
    %
    % Lidar Syntax:
    %
    %   obj = Lidar(port, crc)
    %
    % Lidar Methods:
    %
    %   Lidar           - The constructor creates and connects a new controller to control the button 
    %                      connected to an Arduino.
    %
    %   get_distance        - This method returns measured distance in mm.
    %
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %------------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Zprava pro pripojeni noveho ovladace
        COMMAND_NEW_LIDAR = [4, 1, 14];

       % Zprava, ktera se zasle do arduina pro ziskani namerene
        % vzdalenosti
        COMMAND_LIDAR_GET_DISTANCE = [5, 2, 58];

    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Lidar(port, crc)
            % The constructor creates and connects a new controller to control the button 
            % connected to an Arduino.
            %
            % Lidar Syntax:
            %
            %   obj = Lidar(port, crc)
            %
            % Lidar Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Lidar Outputs:
            %
            %   - obj: Newly created object of the Lidar.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor


            this@controllers.Utility(port, crc);

            this.msg_new_controller(this.COMMAND_NEW_LIDAR, []);

            disp("The controller for a lidar was successfully connected.");
        end % Lidar

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

            dis = this.msg_read(this.COMMAND_LIDAR_GET_DISTANCE, 1, 'single');
        end % get_distance
        
    end % methods - public
end % classdef