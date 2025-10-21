classdef Magnetic_encoder < controllers.Utility
    %---------------------------------------------------------------------------------------------------------
    % Encoder Description:
    %
    %   An Magnetic_encoder object represents a controller for controlling the Magnetic_encoder connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 14.02.2024
    %
    % Encoder Syntax:
    %
    %   obj = Magnetic_encoder(port, crc)
    %
    % Encoder Methods:
    %
    %   Magnetic_encoder   - The constructor creates and connects a new controller to control 
    %               the encoder connected to an Arduino.
    %
    %   get_angle_raw - This method returns the measured angle in raw data - number 0~4095.
    %
    %   get_angle_deg - This method returns the measured angle in degrees.
    %
    %   get_angle_rad - This method returns the measured angle in radians.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %---------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        COMMAND_NEW_ENCODER   = [4, 1, 15];

        % Zprava, ktera se odesle pro aktualniho uhlu
        COMMAND_GET_ANGLE     = [5, 2, 60];

        % Zprava, ktera se odesle pro aktualniho uhlu
        COMMAND_GET_SUM_ANGLE = [5, 2, 61];
    end

        % CLENSKE PROMENNE
    properties (Access = private)
        % Promenna uchovavajici smer otaceni motoru ovladaneho Adafruit
        % Motorshield v2
        absolute_angle_n = 0;
        prev_absolute_angle = 0;


    end


    % Pristupove metody
    methods (Access = public)

        function this = Magnetic_encoder(port, crc)
            % The constructor creates and connects a new controller to control 
            % the encoder connected to an Arduino.
            %
            % Encoder Syntax:
            %
            %   obj = Magnetic_encoder(port, crc)
            %
            % Encoder Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Encoder Outputs.
            %
            %   - obj: Newly created object of the Magnetic_encoder.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            this.msg_new_controller(this.COMMAND_NEW_ENCODER, []);

            disp("The controller for an magnetic encoder was successfully connected.");
        end % Encoder

        function angle = get_angle_raw(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_raw Syntax:
            %
            %   result = obj.get_angle_raw()
            %
            % get_angle_raw Outputs:
            %
            %   - result: The angle measured by the encoder in ticks (0~4095)
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            a = this.msg_read(this.COMMAND_GET_ANGLE, 2);
            angle = this.make_16(a(1), a(2));
        end % get_angle

        function angle = get_angle_deg(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_deg Syntax:
            %
            %   result = obj.get_angle_deg()
            %
            % get_angle_deg Outputs:
            %
            %   - result: The angle measured by the encoder in degrees.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            a = this.msg_read(this.COMMAND_GET_ANGLE, 2);
            angle = this.make_16(a(1), a(2));
            angle = angle/4096*360;
        end % get_angle

        function angle = get_angle_rad(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_rad Syntax:
            %
            %   result = obj.get_angle_rad()
            %
            % get_angle_rad Outputs:
            %
            %   - result: The angle measured by the encoder in radians.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            a = this.msg_read(this.COMMAND_GET_ANGLE, 2);
            angle = this.make_16(a(1), a(2));
            angle = angle/4096*2*pi;
        end % get_angle

        function angle = get_angle_sum_raw(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_sum_raw Syntax:
            %
            %   result = obj.get_angle_sum_raw()
            %
            % get_angle_sum_raw Outputs:
            %
            %   - result: total angle measured in ticks
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            a = this.msg_read(this.COMMAND_GET_SUM_ANGLE, 2);
            this.absolute_angle_n = this.make_16(a(1), a(2));
            angle = get_angle_raw(this);
            angle = angle + this.absolute_angle_n*4096;
             

        end % get_angle_sum

        function angle = get_angle_sum_deg(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_sum_deg Syntax:
            %
            %   result = obj.get_angle_sum_deg()
            %
            % get_angle_sum_deg Outputs:
            %
            %   - result: total angle measured in degrees.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            angle = get_angle_sum_raw(this);
            angle = angle/4096*360;
        end % get_angle_sum

        function angle = get_angle_sum_rad(this)
            % This method returns the speed measured by the encoder in revolutions per minute.
            %
            % get_angle_sum_rad Syntax:
            %
            %   result = obj.get_angle_sum_rad()
            %
            % get_angle_sum_rad Outputs:
            %
            %   - result: total angle measured in radians.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            angle = get_angle_sum_raw(this);
            angle = angle/4096*2*pi;
        end % get_angle_sum

    end % methods

end % classdef