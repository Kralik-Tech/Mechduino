classdef Analog_input < controllers.Utility
    %-----------------------------------------------------------------------------------------------------------
    % Analog_input Description:
    %
    %   A Analog_input object represents a controller for controlling the microphone connected to an Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 7.2.2024
    %
    % Analog_input Syntax:
    %
    %   obj = Analog_input(port, pin, crc)
    %
    % Analog_input Methods:
    %
    %   Analog_input      - The constructor creates and connects a new controller to control the Analog_input 
    %                       connected to an Arduino.
    %
    %   is_higher         - This method returns true if the value on
    %                       analog input is higher than tresshold
    %
    %   get_value         - This method returns value on analog input.
    %
    %   set_tresshold     - This method sets the value of tresshold required by the method
    %                       is_higher.
    %
    % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
    %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor
    %-----------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties (Constant, Access = private)
        % Prikaz, ktery je zasilan pro pripojeni noveho ovladace
        COMMAND_NEW_AI    = [5, 1, 5];

        % Prikaz, ktery je zaslan pro zjisteni, jestli je detekovan zvuk
        COMMAND_IS_HIGHER = [5, 2, 32];

        % Prikaz, ktery je zaslan pro zjisteni namerene amplitudy zvuku
        COMMAND_GET_VALUE     = [5, 2, 33];

        % Prikaz, ktery je zaslan pro nastaveni minimalni snimane amplitudy
        % zvuku
        COMMAND_SET_TRESSHOLD     = [6, 2, 34];

    end % properties - constant

    % PRISTUPOVE METODY
    methods (Access = public)

        function this = Analog_input(port, pin_number, crc)
            % The constructor creates and connects a new controller to control the microphone 
            % connected to an Arduino.
            %
            % Microphone Syntax:
            %
            %   obj = Analog_input(port, pin, crc)
            %
            % Microphone Inputs:
            %
            %   - port: The serialport object which is used for communication with Arduino.
            %   - pin : The analog pin number on the Arduino to which the output of the Analog_input is connected.
            %   - crc : A CRC object which is used to calculate the CRC value of messages sent to Arduino.
            %
            % Analog_input Outputs:
            %
            %   - obj: Newly created object of the Microphone.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Analog_input, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            this@controllers.Utility(port, crc);

            if(pin_number < 0 || pin_number > 255)
                error("Error! The pin number must be in the range of 0-255.");
            end

            this.msg_new_controller(this.COMMAND_NEW_AI, pin_number);

            disp("The controller for a microphone was successfully connected.");

        end % Microphone

        function out = is_higher(this)
            %  This method returns true if the value is higher than
            %  treshold
            %
            % is_higher Syntax:
            %
            %   result = obj.is_higher()
            %
            % is_higher Outputs:
            %
            %   - result: True if the value is higher than treshold 
            %             otherwise, false.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor


            out = this.msg_read(this.COMMAND_IS_HIGHER, 1);
        end % is_detected_sound

        function amplitude = get_value(this)
            % This method returns amplitude of sound measured by the microphone.
            %
            % get_value Syntax:
            %
            %   result = obj.get_value()
            %
            % get_value Outputs:
            %
            %   - result: The amplitude of sound measured by the microphone.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor


            amplitude = this.msg_read(this.COMMAND_GET_VALUE, 1);
        end % get_amplitude

        function set_tresshold(this, amplitude)
            % This method sets the level of detected sound required by the method is_detected_sound.
            %
            % set_treshold Syntax:
            %
            %   obj.set_treshold(amplitude)
            %
            % set_treshold Inputs:
            %
            %   - amplitude: The level of detected sound required by the method is_detected_sound.
            %
            % See also: Button, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Lidar, Logic_output, Magnetic_encoder, 
            %           Mass_sensor, Mechduino, Microphone, Logic_input, PWM_output, Stepper, Ultrasonic_sensor

            amplitude = round(amplitude);

            if(amplitude < 0 || amplitude > 255)
                error("Error! The parameter 'amplitude' must be in the range of 0-255.");
            end

            this.msg_write_params(this.COMMAND_SET_TRESSHOLD, amplitude)
        end % set_amplitude

    end % methods - public

end %classdef