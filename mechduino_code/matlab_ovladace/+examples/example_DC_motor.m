%--------------------------------------------------------------------------
% Example of DC motor control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port   = 'COM3';
pinA = 28;
pinB = 30;
pinPWM = 4;



% New Arduino controller
my_arduino = Mechduino(com_port);

% New DC motor controller
motor = my_arduino.get_DC_motor(pinA, pinB, pinPWM);

% setting speed and running for 1 seconds
motor.set_speed(135);
motor.run();
my_arduino.pause(1000);

% test of motor break (both motor wires at same logic level, motor enabled)
motor.motor_brake(); 
my_arduino.pause(1000);

motor.set_speed(135);
motor.run();
my_arduino.pause(100);

% test of motor stop (both motor wires at same logic level, motor disabled)
motor.stop();
my_arduino.pause(1000);


% stopping the motor for 3 seconds, setting new speed, changing direction and running (2x)
for i = 1:12
    motor.stop();
    motor.change_direction();
    motor.set_speed(135 + i*10);
    motor.run();
    my_arduino.pause(1000);
end

% stopping the motor
motor.stop();
clear;