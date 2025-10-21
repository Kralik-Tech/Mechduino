%--------------------------------------------------------------------------
% Example of encoder control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

% New Arduino controller
my_arduino = Mechduino(com_port);

% New magnetic encoder controller must be connected to SCL and SDA pins
% SDA - pin 20, SCL - pin 21
encoder = my_arduino.get_Magnetic_encoder();


% Displaying measured speed for 10 seconds
t = tic;
while(toc(t) < 10)
    angle_d = encoder.get_angle_deg();
    angle_r = encoder.get_angle_rad();

    disp("uhel: " + angle_d + " °, to je " + angle_r + " rad");
end

clear