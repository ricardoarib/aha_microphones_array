function [ median_output ] = getMedianOutput( myFile, th )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
output = filterFile( fullfile(myFile.folder, myFile.name), th );
median_output = -median (output);

end

