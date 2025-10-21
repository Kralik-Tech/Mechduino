classdef Gyroscope < controllers.Utility
    %----------------------------------------------------------------------------------------------------
    % Gyroscope Description:
    %
    %   A Gyroscope object represents a controller for controlling the tilt
    %   sensor (gyroscope) connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 23.02.2023
    %
    % Gyroscope Syntax:
    % 
    %   obj = Gyroscope(port, crc)
    %
    % Gyroscope Methods:
    %
    %   Gyroscope  - The constructor creates and connects a new controller to control 
    %                the tilt sensor (gyroscope) connected to an Arduino.
    %
    %   get_angles - This method returns tilt angles measured by the sensor in each axis (x, y, z).
    %    
    %   get_acc - This method returns acceleration data measured by the sensor in each axis (x, y, z).
    %
    %   calibrate  - This methods is used for the initial calibration of the sensor.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %
    %----------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Prikaz pro pripojeni noveho ovladace
        COMMAND_NEW_GYROSCOPE = [4, 1, 9];

        % Prikaz pro vraceni namerenych uhlu
        COMMAND_GET_ANGLES    = [5, 2, 45];

        % Prikaz pro vraceni namerenych uhlu
        COMMAND_GET_ACC    = [5, 2, 53];

        % Prikaz pro kalibraci gyroskopu
        COMMAND_CALIBRATE     = [5, 2, 46];
    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Gyroscope(port, crc)
            % The constructor creates and connects a new controller to control the tilt sensor (gyroscope) 
            % connected to an Arduino.
            %
            % Gyroscope Syntax:
            %
            %   obj = Gyroscope(port, crc)
            %
            % Gyroscope Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Gyroscope Outputs
            %
            %   - obj: Newly created object of the Gyroscope.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            this.msg_new_controller(this.COMMAND_NEW_GYROSCOPE, []);

            disp("The controller for a tilt sensor was successfully connected.");

        end % Gyroscope

        function [roll, pitch, yaw] = get_angles(this)
            % This method returns tilt angles measured by the sensor in each axis (x, y, z).
            %
            % get_angles Syntax: 
            %
            %   [roll, pitch, yaw] = obj.get_angles()
            %
            % get_angles Outputs:
            %
            %   - roll : Tilt angle measured by the sensor in the x-axis.
            %   - pitch: Tilt angle measured by the sensor in the y-axis.
            %   - yaw  : Tilt angle measured by the sensor in the z-axis.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            res = this.msg_read(this.COMMAND_GET_ANGLES, 3, 'single');

            roll  = res(1);
            pitch = res(2);
            yaw   = res(3);
        end % get_angles

        function [AccX, AccY, AccZ] = get_acc(this)
            % This method returns acc data in each axes (x, y, z).
            %
            % get_angles Syntax: 
            %
            %   [AccX, AccY, AccZ] = obj.get_acc()
            %
            % get_angles Outputs:
            %
            %   - AccX : acceleration measured by the sensor in the x-axis.
            %   - AccY : acceleration angle measured by the sensor in the y-axis.
            %   - AccZ : acceleration angle measured by the sensor in the z-axis.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            res = this.msg_read(this.COMMAND_GET_ACC, 3, 'single');

            AccX  = res(1);
            AccY  = res(2);
            AccZ  = res(3);
        end % get_angles

        function calibrate(this)
            % This methods is used for the initial calibration of the sensor.
            %
            % CALIBRATE Syntax:
            %
            %   obj.CALIBRATE()
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            disp("Hold the sensor in horizontal position for 2 seconds.");
            pause(0.5);
            this.msg_write(this.COMMAND_CALIBRATE);
            pause(1);
            disp("The sensor was successfully calibrated.");
        end % calibrate

    end % methods - public

end % classdef