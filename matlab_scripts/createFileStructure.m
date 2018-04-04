function [ my_structure ] = createFileStructure( myFile )
%[ my_structure ] = createFileStructure( myFile )
%   Detailed explanation goes here
if strfind(myFile.folder, 'jfmp') > 0
    my_structure.institution = 'jfmp';
elseif strfind(myFile.folder, 'seb') > 0
    my_structure.institution = 'seb';
else
    my_structure.institution = 'other';
end
my_structure.window = sscanf ( myFile.folder(strfind(myFile.folder, '/N')+2:end),'%d' );
my_structure.speaker = sscanf ( myFile.name(strfind(myFile.name, 'speaker-')+8:strfind(myFile.name, '_')),'%d' );
my_structure.position = sscanf ( myFile.name(strfind(myFile.name, 'position-')+9:strfind(myFile.name, 'position-')+10),'%d' );
my_structure.sentence = sscanf ( myFile.name(strfind(myFile.name, 'sentence-')+9:strfind(myFile.name, 'sentence-')+10),'%d' );
my_structure.method = sscanf ( myFile.name( strfind(myFile.name, '.wav.')+5 : strfind(myFile.name, '.wav.')+7), '%s' );
my_structure.gender = [];
my_structure.age = [];
my_structure.subset = [];


data_mtx = readtable('participants_data_simple.csv');
data_mtx = table2cell(data_mtx);
for l = 1:size(data_mtx,1)
    if ( isequal( my_structure.institution, char( data_mtx(l, 1) ) ) )
        if ( isequal( my_structure.speaker, cell2mat(data_mtx(l,2)) ) )
            % add new features
            my_structure.gender = char(data_mtx (l,3));
            my_structure.age = char(data_mtx (l,4));
            my_structure.subset = char(data_mtx(l,5));
            break
        end
    end
end

end

