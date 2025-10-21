%--------------------------------------------------------------------------
% Example of tilt sensor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
COM = 'COM3';

% New Arduino controller
my_arduino = Mechduino(COM);

% New lidar sensor controller, lidar must be connected to SCL and SDA pins
% SDA - pin 20, SCL - pin 21
lidar = my_arduino.get_Lidar();
 

% Measuring and displaying distance for a 20 second, 
% limits of measured distance is approximately 2 - 70 cm
t = tic;
while(toc(t) < 5)
    dis = lidar.get_distance();
    disp("Measured distance: " + dis + " mm.");
end
clear