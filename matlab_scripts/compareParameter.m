function [ output_data ] = compareParameter( myFiles, to_compare, th )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

% Number of files
num_files = size(myFiles, 1);
if (num_files <= 0 )
    disp('No files with such parameters.');
    return
end

% Initialization data
plot_data = {};

switch to_compare
    case 'institution' % --------------------------------------------------INSTITUTION
        
        seb_num = 0;
        jfmp_num = 0;
        
        % which institutions are there?
        seb = []; % initialization
        jfmp = [];
        tmp_vec = [];
        for i = 1:num_files % ---are there SEB?
            if ( strfind( myFiles(i).folder, 'seb') > 0 )
                seb_num = 1;
                % Receive files and threshold, return a vector with the feasible values
                % (can be empty!)
                tmp_vec = filterFile( fullfile(myFiles(i).folder,myFiles(i).name) , th ) ;
                seb = concatenateCellArrays (seb, tmp_vec);
            elseif ( strfind( myFiles(i).folder, 'jfmp') > 0 )
                jfmp_num = 1;
                % Receive files and threshold, return a vector with the feasible values
                % (can be empty!)
                tmp_vec = filterFile( fullfile(myFiles(i).folder,myFiles(i).name) , th ) ;
                jfmp = concatenateCellArrays (jfmp, tmp_vec);
            end
        end
        
        if (seb_num == 0 && jfmp_num == 0)
            disp ('Files not found (Institution).')
        elseif (seb_num == 1 && jfmp_num == 0 )
            output_data = seb;
        elseif (seb_num == 0 && jfmp_num == 1 )
            output_data = jfmp;
        else
            output_data = {seb, jfmp};
        end
        
        
    case 'window' % -------------------------------------------------------WINDOW
        
        N_vec = [];
        tmp_vec = [];
        aux_flag = 0;
        
        for i = 1:num_files
            % 1- search for window size
            folder_name = myFile(i).folder; 
            tmp_size = sscanf ( folder_name(strfind(folder_name, '/N')+2:end),'%d' );
            
            % 2- does it exist? y = add to it
            if ~isempty(N_vec)
                for j = 1:length(N_vec)
                    new_name = ['N' num2str(tmp_size)];
                    if N_vec{j}.name == new_name
                        % already existing element, add the vector cell to it
                        aux_flag = 1;
                        % add the vector cell to it ---- TODO
                    end
                end           
            else % if empty vector
                % create new file
                
                aux_flag = 1;
            end
            
            % 3- does it NOT exist? y = create & add to it 
            if aux_flag == 0
                % crete new element
                N_vec = concatenateCellArrays(N_vec, genvarname(['N' num2str(tmp_size)]));
                % add the vector cell to it ----- TODO
                tmp_vec = filterFile( fullfile(myFiles(i).folder,myFiles(i).name) , th ) ;
                jfmp = concatenateCellArrays (jfmp, tmp_vec);
            end
        end
                
            % 4- add to vector N_vec
            N_vec = concatenateCellArrays(N_vec, new_cell);
            % 5- combine N_vec into output_data
        
%         if window_size > 0
%             search_string = sprintf('N%d',window_size) ;
%             %search_string = 'N1024' ;
%             %search_string = 'test_noises' ;
%             myFilteredDirs = {} ;
%             for n=1:length(myDirs)
%                 if ~isempty( strfind( myDirs{n}, search_string ) )
%                     myFilteredDirs = [ myFilteredDirs ; myDirs{n} ] ;
%                 end
%             end
%             myDirs = myFilteredDirs ;
%         end
%         
%         out_dir_list = myDirs ;
        
    case 'speaker' % ------------------------------------------------------SPEAKER
    case 'position' % -----------------------------------------------------POSITION
    case 'sentence' % -----------------------------------------------------SENTENCE
    case 'method' % -------------------------------------------------------METHOD
        
end

end

