function [  ] = representFile( myFile, th )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
output = filterFile(fullfile(myFile.folder, myFile.name), th);

plot(1:length(output), output,'*');
xlabel('# Result')
ylabel('Output (degrees)')

file_structure = createFileStructure (myFile);
str_title = strcat ({file_structure.},{'N',})

end

