function [ window_vec, angle_vec ] = fc_window( myFiles, th )
%UNTITLED15 Summary of this function goes here
%   Detailed explanation goes here

num_files = length(myFiles);
window_vec = [];
filling_line = ones(1,4);
j=1;

for i = 1:num_files
    tmp_structure = createFileStructure (myFiles(i));
    if isempty(window_vec) % empty vector
        window_vec (1) = tmp_structure.window;
        angle_vec (1,1) = angle_from_file(myFiles(i), th);
        filling_line(1) = 2;
    elseif (nnz(ismember(window_vec, tmp_structure.window)) > 0) % already exists
        idx = find(ismember(window_vec, tmp_structure.window));
        angle_vec (filling_line(idx),idx) = angle_from_file(myFiles(i), th);
        filling_line(idx) = filling_line(idx) + 1;
    elseif (nnz(ismember(window_vec, tmp_structure.window)) == 0)
        window_vec(length(window_vec)+1) = tmp_structure.window;
        angle_vec (filling_line(idx),length(window_vec)) = angle_from_file(myFiles(i), th);
        filling_line(idx) = filling_line(length(window_vec)) + 1;
    end
    
end

end

