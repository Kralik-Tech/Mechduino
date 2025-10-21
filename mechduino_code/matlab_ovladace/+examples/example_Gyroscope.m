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

% New tilt sensor controller must be connected to SCL and SDA pins
% SDA - pin 20, SCL - pin 21
sensor = my_arduino.get_Gyroscope();

% Initial calibration of sensor
sensor.calibrate();

% Displaying measured angles by sensor for 20 seconds
t = tic;
while(toc(t) < 20)
    [roll, pitch, yaw] = sensor.get_angles();
    disp("ROLL: " + roll + " | PITCH: " + pitch + " | YAW: " + yaw);
    [AccX, AccY, AccZ] = sensor.get_acc();
    disp("AccX: " + AccX + " | AccY: " + AccY + " | AccZ: " + AccZ);
    my_arduino.pause(500);
end
