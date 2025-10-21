%--------------------------------------------------------------------------
% Example of controlling stepper motor connected to a driver
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
COM  = 'COM3';

% stepper motor connection
pin1 = 28;
pin2 = 29;
pin3 = 30;
pin4 = 31;

% New Arduino controller
my_arduino = Mechduino(COM);

% New stepper motor controller
stepper = my_arduino.get_Stepper(pin1, pin2, pin3, pin4);

% Setting speed and turning by 2 revolutions
stepper.set_speed(255);
stepper.onestep();
stepper.step(2*2048, 'Direction', 'Forward'); % max step distance is 65 535 steps, i.e. 512 turns

% Waiting while the motor is moving (bussy-waiting) - in actual program is
% not nessesary, can by periodicaly ch
while(stepper.is_moving())
end


% Stopping the code for 2 seconds
my_arduino.pause(2000);

% Setting new speed
stepper.set_speed(255);


% Rotating about 200 steps forward and backward
for i = 1:10
    stepper.change_direction(); % Changing direction
    stepper.step(200);
    while(stepper.is_moving())
    end
end
clear
