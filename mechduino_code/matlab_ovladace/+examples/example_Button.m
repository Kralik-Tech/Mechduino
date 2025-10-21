%--------------------------------------------------------------------------
% Example of button control - method 'is_pushed'.
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
COM        = 'COM3';

% define pin on which is button conected - class button have internal pull-up
% resistor, so in disconected state is logical 1
pin_button = 20;

% define led for represent action of button
pin_led    = 13;

% New Arduino controller
myArduino = Mechduino(COM);


% New button and LED controllers
led    = myArduino.get_LED(pin_led);
led.turn_off();
button = myArduino.get_Button(pin_button);

% Turning LED diode on and off for 30 seconds using the button
t = tic;
while(toc(t) < 30)
    if(button.is_pushed())
        led.light_up();
    else
        led.turn_off();
    end
end

% Turning LED diode on and off for 30 seconds using rising and falling
t = tic;
while(toc(t) < 30)
    if(button.get_rising_edge())
        led.turn_off();
    end

    if(button.get_falling_edge())
        led.light_up();
    end
end

clear;