classdef Mass_sensor < controllers.Utility
    %------------------------------------------------------------------------------------------------------
    % Mass_sensor Description:
    %
    %   A Mass_sensor object represents a controller for controlling the mass sensor connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 29.02.2024
    %
    % RGB_sensor Syntax:
    %
    %   obj = RGB_sensor(port, pinCLK, pinDATA, crc)
    %
    % RGB_sensor Methods:
    %
    %   get_value      - The constructor creates and connects a new controller to control the 
    %                     mass sensor connected to an Arduino.
    %
    %   set_scale       - This method set scale value for mass sensor - computed as sensor_value/(actual_mass*resolution)
    %                   - resolution - 1 for kg, 1000 for grams, ... if actual mass is in kg 
    %
    %   set_zero        - This method set actual measured value as zero point.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %------------------------------------------------------------------------------------------------------
    
    % KONSTANTY
    properties (Access = private, Constant)
        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        COMMAND_NEW_MASS_SENSOR = [6, 1, 13];

        % Zprava, ktera se odesle pro ziskani RGB slozek
        COMMAND_SET_ZERO     = [5, 2, 55];        
        
        % Zprava, ktera se odesle pro ziskani RGB slozek
        COMMAND_SET_SCALE     = [9, 2, 56];        
        
        % Zprava, ktera se odesle pro ziskani RGB slozek
        COMMAND_GET_VALUE     = [5, 2, 57];

    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Mass_sensor(port, pinCLK, pinDATA, crc)
            % The constructor creates and connects a new controller to control the RGB sensor 
            % connected to an Arduino.
            %
            % mass_sensor Syntax:
            %
            %   obj = Mass_sensor(port, pinCLK, pinDATA, crc)
            %
            % mass_sensor Inputs
            %
            %   - port  : The serialport object which is used for communication with Arduino.
            %   - pinCLK : The digital pin number on the Arduino to which CLK from the sensor is connected.
            %   - pinDATA: The digital pin number on the Arduino to which DATA from the sensor is connected.
            %   - crc   : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % mass_sensor Outputs
            %
            %   - obj: Newly created object of the mass_sensor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);
            if(pinCLK  < 0 || pinCLK > 255 || pinDATA < 0 || pinDATA > 255)
                error('Error! The pin number must be in the range of 0-255.');
            end
            
            this.msg_new_controller(this.COMMAND_NEW_MASS_SENSOR, ...
                [pinDATA, pinCLK]);
           
            disp('The controller for a mass sensor was successfully connected.');
        
        end % RGB_sensor
                
        function mass = get_value(this)
            % This method returns the color detected by RGB sensor as a character array.
            %
            % get_value Syntax:
            % 
            %   result = get_value()
            %
            % get_value Output:
            %
            %   - result: The measured mass.
            %
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor


            mass = this.msg_read(this.COMMAND_GET_VALUE, 1, 'int32');
            if mass == 99999999
                mass = this.msg_read(this.COMMAND_GET_VALUE, 1, 'int32');
            end
        end % get_mass

        function set_scale(this, scale)
            % This method is used to set zero and scale of the mass sensor.
            %
            % set_scale Syntax:
            %
            %   obj.set_scale(scale)
            %
            % set_scale Inputs:
            %
            %   - scale: The value representing scale value of tenzometr sensor.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            [bits0, bits1, bits2, bits3] = this.split_float(scale);
            this.msg_write_params(this.COMMAND_SET_SCALE, [bits0, bits1, bits2, bits3]);


        end % set_scale

        function set_zero(this)
            % This method is used to set a zero value for mass sensor.
            %
            % set_zero Syntax:
            %
            %   obj.set_zero()
            %
            % set_zero Inputs:
            %
            %   - none
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
            this.msg_write(this.COMMAND_SET_ZERO);
        end % set_scale
                     



    end % methods - public

end % classdef