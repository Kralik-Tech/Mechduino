%--------------------------------------------------------------------------
% Example of ultrasonic sensor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

trig_pin = 7;
echo_pin = 8;

% New Arduino controller
my_arduino = Mechduino(com_port);

% New ultrasonic sensor controller
sonar = my_arduino.get_Ultrasonic_sensor(trig_pin, echo_pin);

% Measuring and displaying distance for a 20 second
t = tic;
while(toc(t) < 20)
    dis = sonar.get_distance();
    disp("Measured distance: " + dis + " mm.");
    my_arduino.pause(500);
end