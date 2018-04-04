function [th_vec, angle_vec ] = fc_th( th_min, th_step, th_max, myFiles )
%UNTITLED13 Summary of this function goes here
%   Detailed explanation goes here

num_files = length(myFiles);
th_vec = th_min:th_step:th_max;

j = 1;

for th = th_vec
    for i = 1:num_files
        angle_vec(i, j) = angle_from_file(myFiles(i), th);
    end
    j = j +1;
end

end

