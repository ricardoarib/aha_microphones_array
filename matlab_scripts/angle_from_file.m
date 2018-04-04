function [ angle, error ] = result_from_file( myFile, th, mode )
%[ angle, error ] = result_from_file( myFile, th, mode )
%   mode = { 'mean', 'median' }

tmp_vec = filterFile( fullfile(myFile.folder, myFile.name) ,th ) ;

%filtering_output;
switch mode
    case 'mean'
angle = -median(tmp_vec);
    case 'median'
        

end

