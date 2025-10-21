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

% for test purpose of logic output LED semafor is used
% New LO controller
GND = my_arduino.get_Logic_output(8);
ledR = my_arduino.get_Logic_output(10);
ledY = my_arduino.get_Logic_output(12);
ledG = my_arduino.get_Logic_output(14);
GND.set_low();

% Turn on for 4 seconds, then turn off for 2 seconds
ledR.set_hight();
ledY.set_hight();
ledG.set_hight();
 

% Blinking
for i = 1:10
    GND.set_hight();
    my_arduino.pause(500);
    GND.set_low();
    my_arduino.pause(500);
end
GND.set_low();
clear;