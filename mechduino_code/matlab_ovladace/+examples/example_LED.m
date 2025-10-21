%--------------------------------------------------------------------------
% Example of LED diode control
%--------------------------------------------------------------------------
clc, clear, close all;

% Target serial conection (COMX) can be shown by command 'serialportlist'
% if you have more than one COM, the corect one is the one which disapear
% from list after you disconnect your arduino
com_port = 'COM3';



% New Arduino controller
my_arduino = Mechduino(com_port);

% New LED controller
onBoardLEDPin = 13; %LED pin adres can be defined via variable
ledboard = my_arduino.get_LED(onBoardLEDPin); % LED object creation
ledboard.light_up(); %light up LED - logic 1 on onBoardLEDPin


%% moore LED
% for LED semafor, connected to pins 2-8, where GND is on pin 2
ledGND = my_arduino.get_LED(2); %LED pin adres can be also entered directly
ledR = my_arduino.get_LED(4);
ledY = my_arduino.get_LED(6);
ledG = my_arduino.get_LED(8);


ledGND.turn_off(); % logic 0 to pin ledGND

% logic 1 to LEDs pins - turn on LED
ledR.light_up();
ledY.light_up();
ledG.light_up();

my_arduino.pause(500);

% logic 0 to LEDs pins - turn off LED
ledR.turn_off();
ledY.turn_off();
ledG.turn_off();

%% LED blinking
% Blinking independent LED
for i = 1:10
    ledR.light_up();
    my_arduino.pause(100);
    ledY.light_up();
    my_arduino.pause(100);
    ledG.light_up();
    my_arduino.pause(100);
    ledR.turn_off();
    ledY.turn_off();
    ledG.turn_off();
    my_arduino.pause(100);

end

ledR.light_up();
ledG.light_up();

% Blinking all light up leds on semafor
for i = 1:10
    ledGND.light_up();
    my_arduino.pause(200);
    ledGND.turn_off();
    my_arduino.pause(200);
end

ledGND.light_up(); %turn off whole semafor
clear;