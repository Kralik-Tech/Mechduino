classdef CRC < handle
%------------------------------------------------------------------------------------------------------------
    % CRC Description:
    %
    %  A CRC object is used to calculate the CRC value of messages that are sent to Arduino.
    %
    %   Author    : Jakub Smelik
    %   Created on: 27.03.2024
    %
    % CRC Syntax:
    %
    %   obj = CRC()
    %
    % See also: RGB_sensor, DC_motor, Encoder, Gyroscope, Inductive_sensor, LED, Button, 
    %           Microphone, Logic_input, Stepper, Ultrasonic_sensor,
    %           Utility, Mechduino
    %
    %------------------------------------------------------------------------------------------------------------

    properties (Constant, Access = private)
        POLYNOMIAL = 0xD8;
    end

    properties (Access = private)
        crcTable (1, 256);
    end

    methods (Access = public, Hidden)
        
        % Konstruktor - vypocita vsechny mozne hodnoty CRC a ulozi je do
        % tabulky
        function this = CRC()
            
            table = zeros(1, 256);

            for divident = 0:255

                remainder = uint8(divident);

                for bit = 8:-1:1

                     if(bitand(remainder, uint8(bitshift(1, 7))))
                         remainder = bitxor(bitshift(remainder, 1), uint8(this.POLYNOMIAL));
                     else
                         remainder = bitshift(remainder, 1);     
                     end

                end % for - bit

                table(divident+1) = remainder;
            
            end % for - divident
        
            this.crcTable = table;

        end % this = CRC
    
        % Zrychlena metoda pro vypocet CRC zadane zpravy
        %
        % message - Zadana zprava
        function crc = CRC_fast(this, message)

            nBytes    = numel(message);
            remainder = uint8(0);

            for byte = 1:nBytes
                data = bitxor(uint8(message(byte)), uint8(remainder));

                if(data == 255)
                    remainder = this.crcTable(256);
                else
                    remainder = this.crcTable(data+1);
                end

            end % for - byte

            crc = remainder;

        end % CRC_fast

    end % methods - public

    % SKRYTE METODY
    methods (Hidden) 
        % Skryti zdedenych metod od tridy handle

        function varargout = findobj(O,varargin)
            varargout = findobj@handle(O,varargin{:});
        end

        function varargout = findprop(O,varargin)
            varargout = findprop@handle(O,varargin{:});
        end

        function varargout = addlistener(O,varargin)
            varargout = addlistener@handle(O,varargin{:});
        end
        
        function varargout = notify(O,varargin)
            varargout = notify@handle(O,varargin{:});
        end

        function varargout = listener(O,varargin)
            varargout = listener@handle(O,varargin{:});
        end

        function varargout = delete(O,varargin)
            varargout = delete@handle(O,varargin{:});
        end

        function vargout = eq(H1, H2)
            vargout = eq@handle(H1, H2);
        end

        function vargout = ge(H1, H2)
            vargout = ge@handle(H1, H2);
        end

        function vargout = gt(H1, H2)
            vargout = gt@handle(H1, H2);
        end

        function vargout = le(H1, H2)
            vargout = le@handle(H1, H2);
        end

        function vargout = lt(H1, H2)
            vargout = lt@handle(H1, H2);
        end

        function vargout = ne(H1, H2)
            vargout = ne@handle(H1, H2);
        end
    end

end % classdef