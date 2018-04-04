function [ sol_pos_1, sol_pos_2, sol_pos_3, sol_pos_4 ] = geo_one_speaker_all_sentences( th, speaker )
%% - GEO -
% 1 speaker(s)
% all sentence(s)
% 1 position(s)

% Note: Run program inside size folder (e.g. N256)

% clc; clear;
myDir = cd; %gets directory
% th = -15;
% speaker = 3;

%% Position 1
file_name = strcat ('speaker-',num2str(speaker),'_*position-1_*.geo_results.txt');
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
file_name = strcat ('speaker-',num2str(speaker),'_*position-2_*.geo_results.txt');
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
file_name = strcat ('speaker-',num2str(speaker),'_*position-3_*.geo_results.txt');
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
file_name = strcat ('speaker-',num2str(speaker),'_*position-4_*.geo_results.txt');
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

