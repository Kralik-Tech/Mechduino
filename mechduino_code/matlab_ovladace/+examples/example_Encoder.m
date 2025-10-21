%--------------------------------------------------------------------------
% Example of encoder control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port           = 'COM3';
encoder_pin        = 16;
motor_pin_A        = 12;
motor_pin_PWM      = 10;


% New Arduino controller
my_arduino = Mechduino(com_port);

% New DC motor and encoder controllers
encoder = my_arduino.get_Encoder(encoder_pin);
motor = my_arduino.get_DC_motor(motor_pin_A, motor_pin_PWM);

%% 1) set speed to 100 RPM
actual_motor_speed = 0;
motor.set_speed(actual_motor_speed);
motor.run();
while(encoder.get_speed()<100)
    % Displaying measured speed 
    speed = encoder.get_speed();
    disp("Detected speed: " + speed + " RPM");
    % incrase motor speed
    actual_motor_speed = actual_motor_speed + 1;
    motor.set_speed(actual_motor_speed);
end
motor.stop();

%% 2) after 100 ticks change direction
ticks = 0;
% reset ticks counter
encoder.get_ticks();
t = tic;
motor.run();

while(toc(t) < 30)
    % Displaying measured distance
    ticks = ticks + encoder.get_ticks();
    disp("Ticks: " + ticks);
    % change dir if distance is more than 99 ticks
    if ticks > 99
        motor.change_direction();
        ticks = 0;
    end
    my_arduino.pause(50);
end
motor.stop();


clear
