%% PID demo

clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';

% New Arduino controller
my_arduino = Mechduino(com_port);

% New PWM controller, in exaple is used LED semafor
% PWM_ports = [2 3 4 5 6 7 8 9 10 13 44 45 46];
GND = my_arduino.get_PWM_output(8);
ledY = my_arduino.get_PWM_output(7);
ledG = my_arduino.get_PWM_output(6);
ledR = my_arduino.get_PWM_output(9);
Mencoder = my_arduino.get_Magnetic_encoder();
motorPOS = my_arduino.get_DC_motor(2, 3);
motorSpeed = my_arduino.get_DC_motor(4, 5);
encoder = my_arduino.get_Encoder(31);



GND.PWM_set_s(0);

% % Blinking
% for i = 1:5:255
%     ledR.PWM_set_s(i);
%     my_arduino.pause(1);
% end
% for i = 1:5:255
%     ledY.PWM_set_s(i);
%     my_arduino.pause(1);
% end
% for i = 1:5:255
%     ledG.PWM_set_s(i);
%     my_arduino.pause(1);
% end
% for i = 1:5:255
%     GND.PWM_set_s(i);
%     my_arduino.pause(1);
% end
motorPOS.set_speed(0);
motorPOS.run();
motorSpeed.set_speed(0);
motorSpeed.run();


wanted_speed = 50;
wanted_angle_d = 200;
diff_pos = 0;
actual_speed = 0;
P_speed_constant = 0.01;
P_pos_constant = 2.5;
dir = 1;
tic
uhel_plot = zeros(1,100);
speed_plot = zeros(1,100);
wanted_uhel_plot = zeros(1,100);
wanted_speed_plot = zeros(1,100);

while(toc<100)
    angle_d = Mencoder.get_angle_deg();
    speed = encoder.get_speed();
    disp("uhel: " + angle_d + " °, rychlost" + speed);

    diff_speed = wanted_speed - speed;
    actual_speed = actual_speed + diff_speed*P_speed_constant;
    actual_speed = mot_speed_limits(actual_speed); %low speed constrant

    motorSpeed.set_speed(actual_speed);
   
    if (wanted_angle_d > angle_d) && not(dir)
        motorPOS.change_direction();
        dir = 1-dir;
    elseif wanted_angle_d < angle_d && dir
        motorPOS.change_direction();
        dir = 1-dir;
    end
    diff_pos = abs(wanted_angle_d - angle_d);


    uhel_plot = [angle_d,uhel_plot(1:end-1)];
    speed_plot = [speed,speed_plot(1:end-1)];
    wanted_uhel_plot = [wanted_angle_d,wanted_uhel_plot(1:end-1)];
    wanted_speed_plot = [wanted_speed,wanted_speed_plot(1:end-1)];
    subplot(2,1,1)
    plot(uhel_plot)
    hold on
    plot(wanted_uhel_plot)
    hold off

    subplot(2,1,2)
    plot(speed_plot)
    hold on
    plot(wanted_speed_plot)
    hold off

    motorPOS.set_speed(mot_speed_limits(abs(wanted_angle_d - angle_d)*P_pos_constant));
    wanted_angle_d = 250 + 50*sin(toc*pi);
    

end