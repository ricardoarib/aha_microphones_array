function [ angle ] = allSentencesSolution( institution, window, th, method, speaker, position )
% [ angle ] = allSentencesSolution( institution, window, th, method, speaker, position )
%   Detailed explanation goes here

myFiles = selectData(institution, window, speaker, position, [], method, 0);
num_files = length(myFiles);
angle_solutions = 1:num_files;
for i = 1:num_files
    angle_solutions(i) = getMedianOutput(myFiles(i), th);
end

angle = median (angle_solutions);

end

