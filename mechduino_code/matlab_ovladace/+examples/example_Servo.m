%--------------------------------------------------------------------------
% Example of DC motor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port   = 'COM3';
pin = 16;




% New Arduino controller
my_arduino = Mechduino(com_port);

% New DC motor controller
servo = my_arduino.get_Servo(pin);

% change angle for servo in range 0-180° (0-Pi rad)
for i = 0:10:180
    servo.set_angle_deg(i);
    my_arduino.pause(500);
end

% change angle for servo in range pi-0 rad (180-0°)
for i = pi:-0.1:0
    servo.set_angle_deg(i);
    my_arduino.pause(500);
end

servo.set_angle_deg(90);

clear;
