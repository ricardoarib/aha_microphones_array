function [ sol_pos_1, sol_pos_2, sol_pos_3, sol_pos_4 ] = geo_all_speakers_all_sentences( th )
%% - GEO -
% all speaker(s)
% all sentence(s)
% 1 position(s)

% Note: Run program inside size folder (e.g. N256)

%clc; clear;
%myDir = cd; %gets directory
% th = -15;
data_dir = 'results' ;
data_subdirs = {'jfmp/text_files','seb/text_files'} ;
data_test_noises_dir = 'test_noises';

myDirs = {} ;
for n=1:size(data_subdirs,2)
    tmp = dir( fullfile(data_dir,data_subdirs{n} ) )
    for m=1:length(tmp)
        tmp2 = tmp(m).name ;
        if tmp2(1) == 'N' 
            myDirs = [ myDirs ; fullfile(data_dir,data_subdirs{n},tmp2) ] ;
        end
    end
end
myDirs = [ myDirs ; fullfile(data_dir,data_test_noises_dir) ] ;


myFilteredDirs = filter_dir_list( myDirs, [], -1024 ) ;


%% Position 1
%myFiles = dir(fullfile(myDir,'speaker*position-1_*.geo_results.txt')); %gets all txt files in struct
% myFiles = [] ;
% for n=1:length(myFilteredDirs)
%     tmp_files = dir(fullfile(myFilteredDirs{n},'speaker*position-1_*.geo_results.txt')) ;
%     myFiles = [myFiles ; tmp_files ] ;
% end
myFiles = get_file_list( myFilteredDirs, [], 1, [], 'geo', 0 ) ;

num_files = size(myFiles, 1); % get number of files
filtering_output_1 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile( fullfile(myFiles(i).folder,myFiles(i).name) ,th ) ;
    filtering_output_1 = [filtering_output_1, tmp_vec'];
end

%filtering_output;
sol_pos_1 = -mean(filtering_output_1);

%% Position 2
%myFiles = dir(fullfile(myDir,'speaker*position-2_*.geo_results.txt')); %gets all txt files in struct
myFiles = get_file_list( myFilteredDirs, [], 2, [], 'geo', 0 ) ;

num_files = size(myFiles, 1); % get number of files
filtering_output_2 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(fullfile(myFiles(i).folder,myFiles(i).name),th);
    filtering_output_2 = [filtering_output_2, tmp_vec'];
end

%filtering_output;
sol_pos_2 = -mean(filtering_output_2);

%% Position 3
%myFiles = dir(fullfile(myDir,'speaker*position-3_*.geo_results.txt')); %gets all txt files in struct
myFiles = get_file_list( myFilteredDirs, [], 3, [], 'geo', 0 ) ;

num_files = size(myFiles, 1); % get number of files
filtering_output_3 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(fullfile(myFiles(i).folder,myFiles(i).name),th);
    filtering_output_3 = [filtering_output_3, tmp_vec'];
end

%filtering_output;
sol_pos_3 = -mean(filtering_output_3);

%% Position 4
%myFiles = dir(fullfile(myDir,'speaker*position-4_*.geo_results.txt')); %gets all txt files in struct
myFiles = get_file_list( myFilteredDirs, [], 4, [], 'geo', 0 ) ;

num_files = size(myFiles, 1); % get number of files
filtering_output_4 = []; % initialization
tmp_vec = [];

% Receive files and threshold, return a vector with the feasible values
% (can be empty!)
for i = 1:num_files 
    tmp_vec = filterFile(fullfile(myFiles(i).folder,myFiles(i).name),th);
    filtering_output_4 = [filtering_output_4, tmp_vec'];
end

%filtering_output;
sol_pos_4 = -mean(filtering_output_4);



end

