function [ angle, error ] = result_from_file( myFile, th, mode )
%[ angle, error ] = result_from_file( myFile, th, mode )
%   mode = { 'mean', 'median' }

tmp_vec = filterFile( fullfile(myFile.folder, myFile.name) ,th ) ;

%filtering_output;
switch mode
    case 'mean'
        angle = -median(tmp_vec);
    case 'median'
        angle = -median(tmp_vec);
end

myStructure = createFileStructure(myFile);
position = myStructure.position;

switch position
    case 1
        error = 0 - angle;
    case 2
        error = 160 - angle;
    case 3
        error = -135 - angle;
    case 4 
        error = -70 - angle;
end

end

