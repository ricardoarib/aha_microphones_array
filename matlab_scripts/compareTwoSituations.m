function [  ] = compareTwoSituations( institution_1, window_1, speaker_1, position_1, sentence_1, method_1, th_1, institution_2, window_2, speaker_2, position_2, sentence_2, method_2, th_2 )
%[  ] = compareTwoSituations( institution_1, window_1, speaker_1, position_1, sentence_1, method_1, th_1, institution_2, window_2, speaker_2, position_2, sentence_2, method_2, th_2 )
%   Detailed explanation goes here

% find files
my_file_1 = selectData( institution_1, window_1, speaker_1, position_1, sentence_1, method_1, 0 );
my_file_2 = selectData( institution_2, window_2, speaker_2, position_2, sentence_2, method_2, 0 );

compareTwoFiles(my_file_1, th_1, my_file_2, th_2);


end

