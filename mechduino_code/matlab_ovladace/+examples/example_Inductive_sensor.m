%--------------------------------------------------------------------------
% Example of inductive sensor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
COM = 'COM3';

pin_sensor = 7;
% define pin on which is inductive sensor conected 
pin_button = 20;

% define led for represent action of inductive sensor
pin_led    = 13;


% New Arduino controller
myArduino = Mechduino(COM);

% New LED and inductive sensor controllers
inductive_prox = my_arduino.get_Logic_input(sensor_pin);
led    = myArduino.get_LED(pin_led);

% Switching LED diode for 30 seconds by inductive sensor
t = tic;
while(toc(t) < 30)
    if(inductive_prox.read())
        led.light_up();
    else
        led.turn_off();
    end
end