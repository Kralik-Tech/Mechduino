function [speed] = mot_speed_limits(speed)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
speed = max(0,speed); %low speed constrant
speed = min(255,speed); %hight speed constrant
end