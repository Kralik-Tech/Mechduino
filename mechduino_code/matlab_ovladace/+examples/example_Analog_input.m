%--------------------------------------------------------------------------
% Example of microphone control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

pin_AI  = 1; % analog pin
pin_led  = 13;

% New Arduino controller
my_arduino = Mechduino(com_port);

% New microphone controller
mic = my_arduino.get_Analog_input(pin_AI);

% New LED controller
led = my_arduino.get_LED(pin_led);

t = tic;
light = 1;

% Setting of minimum detected amplitude for method 'is_detected_sound'
mic.set_tresshold(100);

t = tic;
while(toc(t) < 30)
    % Displaying measured amplitude of sound
    val = mic.get_value();
    disp("Amplitude :" + val);
    pause(0.1);
    
    % Switching LED diode by detected sound
    if(mic.is_higher() && toc(t) > 1)
        if(light > 0)
            led.light_up();
        else
            led.turn_off();
        end
        light = light * -1;
        t = tic;
    end
end