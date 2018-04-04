function [ output_angles, output_levels_dB ] = filter_data( angles, levels_dB , th)

% Lenght of data
len = length(angles);
idx = [];

% All frames below the energy level defined (threshold) are set to zero
for i = 1:len
    %    if (data_mtx (i, 3) < th || data_mtx (i, 1) == 0)
    if ( levels_dB(i) > th )
        idx = [idx i];
    end
end

output_angles = angles (idx);
output_levels_dB = levels_dB (idx);


end
