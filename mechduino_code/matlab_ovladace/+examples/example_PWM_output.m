%--------------------------------------------------------------------------
% Example of LED diode control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

% New Arduino controller
my_arduino = Mechduino(com_port);

% New PWM controller, in exaple is used LED semafor
% PWM_ports = [2 3 4 5 6 7 8 9 10 13 44 45 46];
GND = my_arduino.get_PWM_output(2);
ledR = my_arduino.get_PWM_output(4);
ledY = my_arduino.get_PWM_output(6);
ledG = my_arduino.get_PWM_output(8);
GND.PWM_set_s(0);

% % Blinking
for i = 1:255
    ledR.PWM_set_s(i);
    my_arduino.pause(1);
end
for i = 1:255
    ledY.PWM_set_s(i);
    my_arduino.pause(1);
end
for i = 1:255
    ledG.PWM_set_s(i);
    my_arduino.pause(1);
end
for i = 1:255
    GND.PWM_set_s(i);
    my_arduino.pause(1);
end


clear;