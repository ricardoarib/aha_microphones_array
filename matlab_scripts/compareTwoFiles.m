function [  ] = compareTwoFiles( my_file_1, th_1, my_file_2, th_2 )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
figure
H = gcf;
H.Name = 'Comparison between two files';
subplot (1,2,1)
representFile (my_file_1, th_1);
subplot (1,2,2)
representFile (my_file_2, th_2);

end

