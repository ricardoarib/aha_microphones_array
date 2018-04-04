function [ angle ] = allSpeakersSolution( institution, window, th, method, sentence, position )
% [ angle ] = allSpeakersSolution( institution, window, th, method, sentence, position )
%   Detailed explanation goes here
myFiles = selectData(institution, window, [], position, sentence, method, 0);
num_files = length(myFiles);
angle_solutions = 1:num_files;
for i = 1:num_files
    angle_solutions(i) = getMedianOutput(myFiles(i), th);
end

angle_solutions(isnan(angle_solutions)) = [];

angle = median (angle_solutions);

end

