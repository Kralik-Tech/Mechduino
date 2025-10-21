classdef RGB_sensor < controllers.Utility
    %------------------------------------------------------------------------------------------------------
    % RGB_sensor Description:
    %
    %   A RGB_sensor object represents a controller for controlling the RGB sensor connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 29.02.2024
    %
    % RGB_sensor Syntax:
    %
    %   obj = RGB_sensor(port, pinS0, pinS1, pinS2, pinS3, pinOut, crc)
    %
    % RGB_sensor Methods:
    %
    %   RGB_sensor      - The constructor creates and connects a new controller to control the 
    %                     RGB sensor connected to an Arduino.
    %
    %   get_color       - This method returns the color detected by RGB sensor as a character array.
    %
    %   get_rgb         - This method returns the color detected by RGB sensor in its RGB components.
    %
    % See also: Logic_input, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Mechduino, 
    %           Microphone, Button, Stepper, Ultrasonic_sensor
    %------------------------------------------------------------------------------------------------------
    
    % KONSTANTY
    properties (Access = private, Constant)
        % Zprava, ktera se odesle pro pripojeni noveho ovladace
        COMMAND_NEW_RGB_SENSOR = [9, 1, 10];

        % Zprava, ktera se odesle pro ziskani RGB slozek
        COMMAND_GET_COLORS     = [5, 2, 47];

    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = RGB_sensor(port, pinS0, pinS1, pinS2, pinS3, pinOut, crc)
            % The constructor creates and connects a new controller to control the RGB sensor 
            % connected to an Arduino.
            %
            % RGB_sensor Syntax:
            %
            %   obj = RGB_sensor(port, pinS0, pinS1, pinS2, pinS3, pinOut, crc)
            %
            % RGB_sensor Inputs
            %
            %   - port  : The serialport object which is used for communication with Arduino.
            %   - pinS0 : The digital pin number on the Arduino to which the output S0 from the sensor is connected.
            %   - pinS1 : The digital pin number on the Arduino to which the output S1 from the sensor is connected.
            %   - pinS2 : The digital pin number on the Arduino to which the output S2 from the sensor is connected.
            %   - pinS3 : The digital pin number on the Arduino to which the output S3 from the sensor is connected.
            %   - pinOut: The digital pin number on the Arduino to which the output OUT from the sensor is connected.
            %   - crc   : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % RGB_sensor Outputs
            %
            %   - obj: Newly created object of the RGB_sensor.
            %
            % See also: RGB_sensor, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Mechduino, 
            %           Microphone, Button, Logic_input, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);
            
            if(pinS0  < 0 || pinS0 > 255 || pinS1 < 0 || pinS1 > 255 || ...
               pinS2  < 0 || pinS2 > 255 || pinS3 < 0 || pinS3 > 255 || ...
               pinOut < 0 || pinOut > 255)
                error('Error! The pin number must be in the range of 0-255.');
            end
            
            this.msg_new_controller(this.COMMAND_NEW_RGB_SENSOR, ...
                [pinS0, pinS1, pinS2, pinS3, pinOut]);
           
            disp('The controller for a RGB sensor was successfully connected.');
        
        end % RGB_sensor
                
        function color = get_color(this)
            % This method returns the color detected by RGB sensor as a character array.
            %
            % get_color Syntax:
            % 
            %   result = get_color()
            %
            % get_color Output:
            %
            %   - result: The color detected by RGB sensor as a character array.
            %
            % Enumerate of the colors:
            %
            %   - 'RED'
            %   - 'YELLOW'
            %   - 'GREEN'
            %   - 'BLUE'
            %   - 'PINK'
            %   - 'PURPLE'
            %   - 'BROWN'
            %   - 'BLACK'
            %   - 'WHITE'
            %   - 'NULL' - This value is returned if the sensor cannot detect the color.
            %
            % See also: RGB_sensor, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Mechduino, 
            %           Microphone, Button, Logic_input, Stepper, Ultrasonic_sensor


            [r, g, b] = this.get_sensor_data();
            
            if(r >= 20 && r <= 36 && g >= 65 && g <= 100 && b >= 50 && b <= 80)
                color = 'RED';
            elseif(r >= 15 && r <= 30 && g >= 15 && g <= 30 && b >= 35 && b <= 50)
                color = 'YELLOW';
            elseif(r >= 70 && r <= 95 && g >= 50 && g <= 75 && b >= 55 && b <= 75)
                color = 'GREEN';
            elseif(r >= 75 && r <= 105 && g >= 45 && g <= 60 && b >= 25 && b <= 50)
                color = 'BLUE';
            elseif(r >= 20 && r <= 35 && g >= 45 && g <= 60 && b >= 30 && b <= 45)
                color = 'PINK';
            elseif(r >= 35 && r <= 70 && g >= 45 && g <= 95 && b >= 30 && b <= 60)
                color = 'PURPLE';
            elseif(r >= 35 && r <= 55 && g >= 65 && g <= 90 && b >= 55 && b <= 80)
                color = 'BROWN';
            elseif(r >= 85 && r <= 140 && g >= 80 && g <= 140 && b >= 70 && b <= 110)
                color = 'BLACK';
            elseif(r >= 25 && r <= 40 && g >= 25 && g <= 40 && b >= 20 && b <= 40)
                color = 'GREY';
            elseif(r >= 10 && r <= 30 && g >= 10 && g <= 30 && b >= 10 && b <= 25)
                color = 'WHITE';
            else
                color = 'NULL';
            end

        end % get_color

        function rgb = get_rgb(this)
            % This method returns the color detected by RGB sensor in its RGB components.
            %
            % get_rgb Syntax:
            % 
            %   result = obj.get_rgb()
            %
            % get_rgb Outputs:
            %
            %   - result: Vector of the detected RGB components -> [R, G, B].
            %
            % See also: RGB_sensor, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Mechduino, 
            %           Microphone, Button, Logic_input, Stepper, Ultrasonic_sensor


            color = this.get_color();

            if(strcmp(color, 'RED'))
                r = 1;
                g = 0;
                b = 0;
            elseif(strcmp(color, 'YELLOW'))
                r = 1;
                g = 1;
                b = 0;
            elseif(strcmp(color, 'GREEN'))
                r = 0;
                g = 1;
                b = 0;
            elseif(strcmp(color, 'BLUE'))
                r = 0;
                g = 0;
                b = 1;
            elseif(strcmp(color, 'PINK'))
                r = 1;
                g = 0;
                b = 1;
            elseif(strcmp(color, 'PURPLE'))
                r = 0.4940;
                g = 0.1840;
                b = 0.5560;
            elseif(strcmp(color, 'BROWN'))
                r = 0.6;
                g = 0.3;
                b = 0.1;
            elseif(strcmp(color, 'BLACK'))
                r = 0;
                g = 0;
                b = 0;
            elseif(strcmp(color, 'GREY'))
                r = 0.5;
                g = 0.5;
                b = 0.5;
            elseif(strcmp(color, 'WHITE') || strcmp(color, 'NULL'))
                r = 1;
                g = 1;
                b = 1;
            end

            rgb = [r, g, b];

        end % get_r_g_b

    end % methods - public

    % SOUKROME METODY
    methods (Access = private)

        % Metoda vracejici namerene hodnoty r, g, b slozky
        function [r, g, b] = get_sensor_data(this)
            res = this.msg_read(this.COMMAND_GET_COLORS, 6);

            r = this.make_16(res(1), res(2));
            g = this.make_16(res(3), res(4));
            b = this.make_16(res(5), res(6));
        end % get_sensor_data

    end % methods - private

end % classdef