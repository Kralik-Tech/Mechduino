classdef Utility < handle
    %------------------------------------------------------------------------------------------------------
    % Utility Description:
    %
    %   This helper class is used as a common ancestor for every controller for Arduino. 
    %   The class defines methods for sending and receiving messages to and from Arduino, 
    %   as well as some additional utility methods.
    % 
    %   Author    : Jakub Smelik
    %   Created on: 11.02.2024
    %
    % See also: Microphone, DC_motor, Button, Encoder, Gyroscope, Inductive_sensor, Mechduino, 
    %           LED, RGB_sensor, Logic_input, Stepper, Ultrasonic_sensor, CRC
    %
    %------------------------------------------------------------------------------------------------------

    % KONSTANTY
    properties(Constant, Access = private)
        
        % Hodnota, ktera je vracena v pripade uspesneho ulozeni prikazu 
        % do fronty udalosti
        MESSAGE_SAVED    = 1;

        % Hodnota, ktera je vracena v pripade neuspesneho ulozeni prikazu do
        % fronty udalosti
        MESSAGE_NOT_SAVED = 2;

        % Hodnota, ktera je vracena v pripade chyby alokace pameti
        ALLOCATION_ERROR  = 255;

    end % Properties constant
    
    % CLENSKE PROMENNE
    properties (Access = private)

        % Instance tridy serialport slouzici ke komunikaci s arduinem
        port;

        % Index, na kterem je pripojen ovladac v poli ovladacu
        controller_index;

        % Instance tridy CRC slouzici pro vypocty CRC odesilanych zprav
        crc_obj;

    end % properties

    % DEDENE METODY
    methods (Access = protected, Hidden)
        % Konstruktor - slouzi k inicializaci promenne port
        function this = Utility(port, crc)
            this.port    = port;
            this.crc_obj = crc;
        end % function Utility

        % Metoda slouzici k odeslani zpravy (bez parametru) zadajici 
        % pripojeni noveho ovladace do arduina a naslednem prijmuti a 
        % zpracovani prichozi zpravy
        %
        % command     - prikaz k vykonani
        % pin_number  - cislo pinu, kde je dany prvek pripojen (muze byt vektor)
        function msg_new_controller(this, command, pin_number)

            message     = [command, pin_number];
            command_crc = this.crc_obj.CRC_fast(message);

            for i = 1:2
                this.port.write([message, command_crc], "uint8");
                
                res   = this.port.read(2, "uint8");
                crc   = res(1);
                saved = res(2);

                while(crc == command_crc && saved == this.MESSAGE_NOT_SAVED)
                    this.port.write([message, command_crc], "uint8");
                    res = this.port.read(2, "uint8");
                    crc   = res(1);
                    saved = res(2);

                    if(saved == this.MESSAGE_SAVED)
                        break;
                    end

                    if(crc ~= command_crc)
                        break;
                    end
                end

                if(crc == command_crc)
                    break;
                end

                if(i == 2)
                    error("An error occured while connecting the controller. Try it again.");
                end
            end

            this.controller_index = this.port.read(1, "uint8");
            
            if(this.controller_index == this.ALLOCATION_ERROR)
                error("Allocation error! There is no free memory for a new controller in the Arduino.");
            end
            
        end % msg_new_controller

        % Metoda slouzici k odeslani zpravy zadajici pripojeni noveho 
        % ovladace do arduina a naslednem prijmuti a zpracovani 
        % prichozi zpravy (s parametry)
        %
        % command     - prikaz k vykonani
        % pin_number  - cislo pinu, kde je dany prvek pripojen (muze byt vektor)
        % param       - parametry k predani (muze byt vektor)
        function msg_new_controller_params(this, command, pin_number, param)
            
            message = [command, pin_number, param];
            command_crc = this.crc_obj.CRC_fast(message);

            for i = 1:2
                this.port.write([message, command_crc], "uint8");

                res   = this.port.read(2, "uint8");
                crc   = res(1);
                saved = res(2);

                while(crc == command_crc && saved == this.MESSAGE_NOT_SAVED)
                    this.port.write([message, command_crc], "uint8");
                    res = this.port.read(2, "uint8");
                    crc   = res(1);
                    saved = res(2);

                    if(saved == this.MESSAGE_SAVED)
                        break;
                    end

                    if(crc ~= command_crc)
                        break;
                    end
                end

                if(crc == command_crc)
                    break;
                end

                if(i == 2)
                    error("An error occured while connecting the controller. Try it again.");
                end
            end

            this.controller_index = this.port.read(1, "uint8");
            
            if(this.controller_index == this.ALLOCATION_ERROR)
                error("Allocation error! There is no free memory for a new controller in the Arduino.");
            end
            
        end % msg_new_controller_params

        % Metoda slouzici k zaslani prikazu do matlabu pro vykonani
        % konkretniho prikazu s pripojenym prvkem (bez parametru)
        %
        % command     - prikaz, ktery ma byt vykonan
        % prijmuti zpravy
        function msg_write(this, command)
            
            message     = [command, this.controller_index];
            command_crc = this.crc_obj.CRC_fast(message);

            for i = 1:2
                this.port.write([message, command_crc], "uint8");
                res = this.port.read(2, "uint8");
                crc = res(1);
                saved = res(2);

                % V pripade plne fronty posila dokud se neulozi
                while(crc == command_crc && saved == this.MESSAGE_NOT_SAVED)
                    this.port.write([message, command_crc], "uint8");
                    res = this.port.read(2, "uint8");
                    crc = res(1);
                    saved = res(2);

                    if(saved == this.MESSAGE_SAVED)
                        break;
                    end

                    if(crc ~= command_crc)
                        break;
                    end
                end

                if(crc == command_crc)
                    break;
                end

                if(i == 2)
                    error("An error occurred while sending a command to the Arduino. Try it again.");
                end
            end

        end % msg_write

        % Metoda slouzici k zaslani prikazu do matlabu pro vykonani
        % konkretniho prikazu s pripojenym prvkem s parametry
        %
        % command     - prikaz, ktery ma byt vykonan [vektor]
        % param       - vektor parametru, ktere maji byt predany
        function msg_write_params(this, command, param)

            message     = [command, this.controller_index, param];
            command_crc = this.crc_obj.CRC_fast(message);

            for i = 1:2
                this.port.write([message, command_crc], "uint8");
                res = this.port.read(2, "uint8");
                crc = res(1);
                saved = res(2);
                
                % V pripade plne fronty posila dokud se neulozi
                while(crc == command_crc && saved == this.MESSAGE_NOT_SAVED)
                    this.port.write([message, command_crc], "uint8");
                    res = this.port.read(2, "uint8");
                    crc = res(1);
                    saved = res(2);

                    if(saved == this.MESSAGE_SAVED)
                        break;
                    end

                    if(crc ~= command_crc)
                        break;
                    end
                end

                if(crc == command_crc)
                    break;
                end

                if(i == 2)
                    error("An error occurred while sending a command to the Arduino. Try it again.");
                end
            end

        end % msg_write_params

        % Funkce slouzici ke zjisteni pozadovane hodnoty (napr. ze senzoru)
        % Muze byt zadan navic typ, ktery ma byt cten, pokud neni zadano
        % jinak, standartne je typ nastaven na uint8
        %
        % command     - odelany prikaz
        % bytes       - pocet ctenych bytu
        function out = msg_read(this, command, bytes, varargin)
            switch(nargin)
                case 3
                    type = "uint8";
                case 4
                    type = varargin{1};
                otherwise
                    error("Error! Incorrectly entered number of parameters.");
            end

            this.msg_write(command);
            out = this.port.read(bytes, type);
        end

        % Funkce slozici k rozlozeni 16bitove hodnoty na 2x 8 bitu
        % 
        % num - 16bitova hodnota k rozlozeni
        function [top_bits, low_bits] = split_bits(~, num)
            top_bits = bitshift(uint16(num), -8);
            low_bits = bitand(uint16(num), uint16(0xFF));
        end % split_bits

        % Funkce slouzici ke slozeni 16bitove honoty ze dvou 8bitovych
        % hodnot
        %
        % top_bits - hornich 8 bitu
        % low_lots - dolnich 8 bitu
        function out = make_16(~, top_bits, low_bits)
            out = bitor(bitshift(top_bits, 8), low_bits);
        end % make_16
        function out = make_long(~, bits1, bits2, bits3, bits4)
            out = bitor(bitor(bitshift(bits1, 24), bitshift(bits2, 16)), bitor(bitshift(bits3, 8), bits4));
        end % make_16

        function [bits0, bits1, bits2, bits3] = split_float(~, num)
            %[bits0, bits1, bits2, bits3] = [sign, exponent, mantisa] 
            % reprezentace (1+mantisa)^(exponent-127)
            %    pro exponent 0 pouze mantisa^-127
            % sign = 1/0 = 0 - positive, 1 - negative - 1bit
            % exponent 0-255 = 2^(E-127) - 8bitů
            %  zvláštní případy:
            %     0 -> pouze mantisa, hodnota od e-39 až e-45 a 0
            %     255 -> inf pro mantisa == 0; jinak NaN
            % mantisa - reprezentace čísla nomovaná v rozsahu od 1-2
            % pro exponent 0, tedy čísla e-39 až e-45 - 23 bitů
            % bits0 = [S E7 E6 E5 E4 E3 E2 E1]
            % bits1 = [E0 M22 ... M16]
            % bits2 = [M15 ... M8]
            % bits3 = [M7 ... M0]

            if num < 0
                S = 1;
                num = -num;
            else
                S = 0;
            end
            if isnan(num)
                E = ones(1,8);
                M = ones(1,23);
            elseif num > 1e38
                E = ones(1,8);
                M = zeros(1,23);
            elseif num <1e-45
                M = zeros(1,23);
                M = zeros(1,8);
            else
                N_part = dec2bin(num)-'0';
                if length(N_part)<23
                    if N_part == 0
                        N_part = [0,0];
                        depth = 140;
                    else
                        depth = 23-length(N_part)+1;
                    end
                    F_part = zeros(1,depth);
                    for i=1:depth
                        num = num-floor(num);
                        num = num*2;
                        if num > 1
                            F_part(i)=1;
                            num = num-1;
                        end
                    end
                    M = [N_part(2:end),F_part];
                    if length(M)>23
                        I = min(find(M==1,1),127);
                        M = M(I:I+23);
                        E = dec2bin(127-I)-'0';
                    else
                        E = dec2bin(127+length(N_part)-1)-'0';
                    end
                else
                    M = N_part(1:23);
                    E = dec2bin(127+length(N_part)-1)-'0';
                end
            end
            bits0 = bin2dec(char([S E(1:7)]+'0'));
            bits1 = bin2dec(char([E(8),M(1:7)]+'0'));
            bits2 = bin2dec(char([M(8:15)]+'0'));
            bits3 = bin2dec(char([M(15:end)]+'0'));
            



        end


    end % methods

    % SKRYTE METODY
    methods (Hidden)
        % Skryti zdedenych metod od handle

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