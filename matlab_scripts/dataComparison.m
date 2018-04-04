function [ output_args ] = dataComparision( xx, yy, window, th )
% xx: 'speaker', 'sentence'
% yy: absolute value = 'angle', error value = 'error'

num_files = size( myFiles, 1 );
speakers_vec = [];

if yy == 'angle'
    if xx == 'speaker'
        
        % SEB
        myFiles_seb = selectData('seb', window, [],[],[],0);
        for i = 1:num_files
            % store all different speakers in a vector
            structure_tmp = createFileStructure(myFiles_seb(i));
            if (~any(ismember(speakers_vec, structure_tmp.speaker))) % new element
                speakers_vec(length(speakers_vec)+1) = structure_tmp.speaker;
            end
        end
        speakers_vec = sort(speakers_vec);
        seb_vec = [speakers_vec; zeros(8, length(speakers_vec)) ];
        
        for i = speakers_vec
            for j = 1:length(myFiles_seb)
                structure_tmp = createFileStructure(myFiles_seb(j));
                if structure_tmp.speaker == speakers_vec(i)
                    

        end
        
        % fill matrix
        for n = speakers_vec
            if
            end
        end
        
        for i = speakers_vec
            for j = 1:num_files
                structure_tmp = createFileStructure(myFiles(j));
                if structure_tmp.speaker == speakers_vec (i) % new element
                    myFile = selectData( structure_tmp.institution, structure_tmp.window, structure_tmp.speaker, structure_tmp.position, structure_tmp.sentence, structure_tmp.method, 0 );
                    output_vec(2,i) = getMedianOutput( myFile, th );
                    output_vec(3,i) = output_vec(3,i) + 1;
                end
            end
        end
        output_vec(4,:) = output_vec(2,:)./output_vec(3,:);
        
        % JFMP
        
    elseif xx == 'sentences'
        % same but for all sepakers
    end
    
elseif yy == 'error'
    %do this instead
end




end

