%--------------------------------------------------------------------------
% Example of motion sensor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port    = 'COM3';


% as Logic input can be used any logical sensor, as line tracing, motion
% detectino, inductive sensor, button, ... logic input don't have pull-up!
sensor_pin  = 20;
led_pin     = 13;

% New Arduino controller
my_arduino = Mechduino(com_port);

% New LED and motion sensor controllers
sensor = my_arduino.get_Logic_input(sensor_pin);
led    = my_arduino.get_LED(led_pin);

% turn on/off LED diode by logic input
t = tic;
while(toc(t) < 30)
    if(sensor.read())
        led.light_up();
    else
        led.turn_off();

    end
end

clear;