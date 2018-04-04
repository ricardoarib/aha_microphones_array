function [ angles, levels, levels_dB ] = load_data ( fileID )
% receives a defined threshold and a fileID that corresponds to a .txt file
% where
%   Column 1: Angle output (degrees)
%   Column 2: Signal energy (linear)
%   Column 3: Signal energy (dB)
% 

    % Data storage
% data_mtx = readtable(fileID);
% data_mtx = table2array(data_mtx);
data_mtx =load(fileID);

angles = data_mtx(:,1);
levels = data_mtx(:,2);
levels_dB = data_mtx(:,3);

    
end