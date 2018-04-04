function [  ] = representFile( myFile, th )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
output = filterFile(fullfile(myFile.folder, myFile.name), th);

output = -output;

plot(1:length(output), output,'*');
xlabel('# Result')
ylabel('Output angle (degrees)')

file_structure = createFileStructure (myFile);

str_title = sprintf('%s, N%d, speaker: %d, position: %d, sentence: %d, method: %s, th: %d dBFS', file_structure.institution, file_structure.window, file_structure.speaker, file_structure.position, file_structure.sentence, file_structure.method, th);
title(str_title)

grid on
hold on
if file_structure.position == 1
    plot(1:length(output), 0*ones(size(output)), 'r')
elseif file_structure.position == 2
    plot(1:length(output), 160*ones(size(output)), 'r')
elseif file_structure.position == 3
    plot(1:length(output), -135*ones(size(output)), 'r')
elseif file_structure.position == 4
    plot(1:length(output), -70*ones(size(output)), 'r')
end

axis([1 length(output) -180 180])

hold off

end

