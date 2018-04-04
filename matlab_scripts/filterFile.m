function [ result ] = filterFile( fileID, th )
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
    % Lenght of data
len = size(data_mtx,1);

    % All frames below the energy level defined (threshold) are set to zero
for i = 1:len
%    if (data_mtx (i, 3) < th || data_mtx (i, 1) == 0)
    if (data_mtx (i, 3) < th) 
        data_mtx (i, :) = [0 0 0];
    end
end

    % Remove all rows of zeros
data_mtx (~any(data_mtx,2), :) = [];

    % Return output
if (size(data_mtx,1) == 0)  % If all the frames are below the threshold and the resulting output is empty return
    result = [];            % empty
else                        % outherwise
    result = data_mtx(:,1); % useful values
end

    
end
