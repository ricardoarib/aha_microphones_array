function [ sol_pos_1, sol_pos_2, sol_pos_3, sol_pos_4 ] = srp_all_speakers_one_sentence ( th, sentence )
%% - srp -
% all speaker(s)
% 1 sentence(s)
% 1 position(s)

% Note: Run program inside size folder (e.g. N256)

% clc; clear;
myDir = cd; %gets directory
% th = -15;
% sentence = 7;

%% Position 1
file_name = strcat ('speaker*position-1_sentence-',num2str(sentence),'_*','.srp_results.txt');
myFiles = dir(fullfile(myDir, file_name)); %gets all txt files in struct
num_files = size(myFiles, 1); % get number of files
filtering_output_1 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(myFiles(i).name,th);
    filtering_output_1 = [filtering_output_1, tmp_vec'];
end

%filtering_output;
sol_pos_1 = -mean(filtering_output_1);

%% Position 2
file_name = strcat ('speaker*position-2_sentence-',num2str(sentence),'_*','.srp_results.txt');
myFiles = dir(fullfile(myDir, file_name)); %gets all txt files in struct
num_files = size(myFiles, 1); % get number of files
filtering_output_2 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(myFiles(i).name,th);
    filtering_output_2 = [filtering_output_2, tmp_vec'];
end

%filtering_output;
sol_pos_2 = -mean(filtering_output_2);

%% Position 3
file_name = strcat ('speaker*position-3_sentence-',num2str(sentence),'_*','.srp_results.txt');
myFiles = dir(fullfile(myDir, file_name)); %gets all txt files in struct
num_files = size(myFiles, 1); % get number of files
filtering_output_3 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(myFiles(i).name,th);
    filtering_output_3 = [filtering_output_3, tmp_vec'];
end

%filtering_output;
sol_pos_3 = -mean(filtering_output_3);

%% Position 4
file_name = strcat ('speaker*position-4_sentence-',num2str(sentence),'_*','.srp_results.txt');
myFiles = dir(fullfile(myDir, file_name)); %gets all txt files in struct
num_files = size(myFiles, 1); % get number of files
filtering_output_4 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(myFiles(i).name,th);
    filtering_output_4 = [filtering_output_4, tmp_vec'];
end

%filtering_output;
sol_pos_4 = -mean(filtering_output_4);


end

