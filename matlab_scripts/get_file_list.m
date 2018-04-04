function [ file_list ] = get_file_list( dir_list, speaker, position, sentence, method, test )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

if speaker >= 0 
    speaker_str = sprintf('%d',speaker) ;
else
    speaker_str = '*' ;
end 

if position >= 0 
    pos_str = sprintf('%d',position) ;
else
    pos_str = '*' ;
end 

if sentence >= 0 
    sentence_str = sprintf('%d',sentence) ;
else
    sentence_str = '*' ;
end 

if isempty(method)
    method = '*' ;
end

path_string = sprintf( 'speaker-%s_noise-1_position-%s_sentence-%s_main_*.wav.%s_results.txt', speaker_str, pos_str, sentence_str, method   ) ;


if test
    path_string = sprintf( 'white_noise_p%s_%s_results_output.txt', pos_str, method ) ;
end


myFiles = [] ;
for n=1:length(dir_list)
    tmp_files = dir(fullfile(dir_list{n},path_string)) ;
    myFiles = [myFiles ; tmp_files ] ;
end


file_list = myFiles ;

end

