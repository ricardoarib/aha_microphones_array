function [ sol_pos_1, sol_pos_2, sol_pos_3, sol_pos_4 ] = srp_test_noises( th )
%% - srp -
% all speaker(s)
% all sentence(s)
% 1 position(s)

% Note: Run program inside size folder (e.g. N256)

%clc; clear;
myDir = cd; %gets directory
% th = -15;

%% Position 1
myFiles = dir(fullfile(myDir,'white_noise_p1_srp*')); %gets all txt files in struct
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
myFiles = dir(fullfile(myDir,'white_noise_p2_srp*')); %gets all txt files in struct
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
myFiles = dir(fullfile(myDir,'white_noise_p3_srp*')); %gets all txt files in struct
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
myFiles = dir(fullfile(myDir,'white_noise_p4_srp*')); %gets all txt files in struct
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

