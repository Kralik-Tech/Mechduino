%--------------------------------------------------------------------------
% Example of mass sensor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

% New Arduino controller
my_arduino = Mechduino(com_port);


% New mass sensor controller
CLK_pin = 3;
DATA_pin = 2;
vaha = my_arduino.get_Mass_sensor(CLK_pin, DATA_pin);


pause(1); % sensor stabilization

% sensor have internal multipler which can be set to scale measured value
% to actual mass 
vaha.set_scale(-2.767); % scale cca -2.7 ~ -2.8 for resolution 0.1g
% Measuring and displaying mass for a 20 second
my_arduino.pause(100) % pause for change application

t = tic;
while(toc(t) < 5)
    mass = vaha.get_value();
    disp("Zmerena vaha je : " + mass/10 + " g.")
    my_arduino.pause(500)
end

disp("Nastaveni nuly")
vaha.set_zero(); % set new zero point
my_arduino.pause(100) % pause for change application

t = tic;
while(toc(t) < 5)
    mass = vaha.get_value();
    disp("Zmerena vaha je : " + mass/10 + " g.")
    my_arduino.pause(500)
end


clear
