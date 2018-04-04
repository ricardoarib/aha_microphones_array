function [ out_dir_list ] = filter_dir_list( in_dir_list, institution, window_size )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

myDirs = in_dir_list ;

if ~isempty(institution)
    search_string = institution ;
    myFilteredDirs = {} ;
    for n=1:length(myDirs)
        if ~isempty( strfind( myDirs{n}, search_string ) )
            myFilteredDirs = [ myFilteredDirs ; myDirs{n} ] ;
        end
    end
    myDirs = myFilteredDirs ;
end


if window_size > 0
    search_string = sprintf('N%d',window_size) ;
    %search_string = 'N1024' ;
    %search_string = 'test_noises' ;
    myFilteredDirs = {} ;
    for n=1:length(myDirs)
        if ~isempty( strfind( myDirs{n}, search_string ) )
            myFilteredDirs = [ myFilteredDirs ; myDirs{n} ] ;
        end
    end
    myDirs = myFilteredDirs ;
end

out_dir_list = myDirs ;

end

