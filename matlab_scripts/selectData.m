function [ myNewFiles ] = selectData( institution, window, speaker, position, sentence, method, test, gender, age, subset ) % mais tres parametros: m/f/[], j/s/[], a/t/[]
% [ myFiles (,my_new_structure) ] = selectData( institution, window, speaker, position, sentence, method, test, gender, age, subset ) % mais tres parametros: m/f/[], j/s/[], a/t/[]
%   Detailed explanation goes here
data_dir = 'results' ;
data_subdirs = {'jfmp/text_files','seb/text_files'} ;
data_test_noises_dir = 'test_noises';

myDirs = {} ;
for n=1:size(data_subdirs,2)
    tmp = dir( fullfile(data_dir,data_subdirs{n} ) ) ;
    for m=1:length(tmp)
        tmp2 = tmp(m).name ;
        if tmp2(1) == 'N' 
            myDirs = [ myDirs ; fullfile(data_dir,data_subdirs{n},tmp2) ] ;
        end
    end
end
myDirs = [ myDirs ; fullfile(data_dir,data_test_noises_dir) ] ;

% speaker = 3 ;
% sentence = [] ;
% window = 16384 ;


myFilteredDirs = filter_dir_list( myDirs, institution, window ) ;

myFiles = get_file_list( myFilteredDirs, [speaker], position, [sentence], method, test ) ;

myNewFiles(1) = myFiles(1);

% my Files 2 -- output

% usar createStructur... .m

num_files = length(myFiles);

for i = 1:num_files
    % create structure
    my_structure(i) = createFileStructure ( myFiles(i) );
end

safe_empty = @(x)~isempty(x)&&x ;  %% Anonymous Function to convert an empty x to false. If not empty then returns x.

j = 1;
for i = 1:num_files
    pass_gender = isempty(gender) || safe_empty(gender == my_structure(i).gender) ;
    pass_age = isempty(age) || safe_empty(age == my_structure(i).age) ;
    pass_subset = isempty(subset) || safe_empty(subset == my_structure(i).subset) ;
    pass = (pass_gender && pass_age) && pass_subset;
    if pass
        myNewFiles(j) = myFiles(i);
        j = j +1;
    end
end

% j = 1;
% for i = 1:num_files
%     k = j;
%     % all parameters filled
%     if ( ~isempty(gender) && gender == my_structure(i).gender && ~isempty(age) && age == my_structure(i).age && ~isempty(subset) && subset == my_structure(i).subset )
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 2 parameters filled
%     elseif ( isempty(gender) && ~isempty(age) && age == my_structure(i).age && ~isempty(subset) && subset == my_structure(i).subset )
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 2 parameters filled
%     elseif ( ~isempty(gender) && gender == my_structure(i).gender && isempty(age) && ~isempty(subset) && subset == my_structure(i).subset )
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 2 parameters filled
%     elseif ( ~isempty(gender) && gender == my_structure(i).gender && ~isempty(age) && age == my_structure(i).age && isempty(subset) )
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 1 parameter filled
%     elseif ( ~isempty(gender) && gender == my_structure(i).gender && isempty(age) && isempty(subset))
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 1 parameter filled
%     elseif ( isempty(gender) && ~isempty(age) && age == my_structure(i).age && isempty(subset) )
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     % 1 parameter filled
% %     elseif ( logical ( logical( isempty(gender) && isempty(age) ) && ~isempty(subset) ) && (subset == my_structure(i).subset) )
%     elseif ( and(and( isempty(gender) , isempty(age) ) , and ( ~isempty(subset) , (subset == my_structure(i).subset) ) ) )
%         i
%         myNewFiles(j) = myFiles(i);
%         j = j + 1;
%     end
%     if (k ~= j) 
%         my_newStructure(j-1)= my_structure(i);
%     end
% end
% % return array of structures (?)

% if isempty(myNewFiles)
%     myNewFiles = myFiles;
% end

end

