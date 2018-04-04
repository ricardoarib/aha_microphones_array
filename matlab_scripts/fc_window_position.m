function [ window_vec, angle_vec ] = fc_window_position( position, th )
%[ window_vec, angle_vec ] = fc_window_position( position, myFiles, th )
%   Detailed explanation goes here

myFiles = selectData([], [], [], position, [], [], 0, [], [], []);

[ window_vec, angle_vec ] = fc_window( myFiles, th );


end

