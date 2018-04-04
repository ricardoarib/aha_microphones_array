function [ jfmp_geo, jfmp_srp, seb_geo, seb_srp ] = visualizeResults( mode, window, th )
%[  ] = visualizeResults( mode, window, th )
%   mode = {'sentences', 'speakers'}

switch mode
    case 'sentences'
        % how many different sentences in the files?
        sentences_vec = zeros(1,7); % TODO
        sentences_vec (1,:) = 1:7;
        jfmp_geo = zeros(4,7);
        jfmp_srp = zeros(4,7);
        seb_geo = zeros(4,7);
        seb_srp = zeros(4,7);
        
        % for every different sentence, get an output
        for i = sentences_vec(1,:)
            % jfmp + geo
            jfmp_geo(1,i) = allSpeakersSolution( 'jfmp', window, th, 'geo', sentences_vec(1,i), 1 );
            jfmp_geo(2,i) = allSpeakersSolution( 'jfmp', window, th, 'geo', sentences_vec(1,i), 2 );
            jfmp_geo(3,i) = allSpeakersSolution( 'jfmp', window, th, 'geo', sentences_vec(1,i), 3 );
            jfmp_geo(4,i) = allSpeakersSolution( 'jfmp', window, th, 'geo', sentences_vec(1,i), 4 );
            % jfmp + srp
            jfmp_srp(1,i) = allSpeakersSolution( 'jfmp', window, th, 'srp', sentences_vec(1,i), 1 );
            jfmp_srp(2,i) = allSpeakersSolution( 'jfmp', window, th, 'srp', sentences_vec(1,i), 2 );
            jfmp_srp(3,i) = allSpeakersSolution( 'jfmp', window, th, 'srp', sentences_vec(1,i), 3 );
            jfmp_srp(4,i) = allSpeakersSolution( 'jfmp', window, th, 'srp', sentences_vec(1,i), 4 );
            % jfmp + geo
            seb_geo(1,i) = allSpeakersSolution( 'seb', window, th, 'geo', sentences_vec(1,i), 1 );
            seb_geo(2,i) = allSpeakersSolution( 'seb', window, th, 'geo', sentences_vec(1,i), 2 );
            seb_geo(3,i) = allSpeakersSolution( 'seb', window, th, 'geo', sentences_vec(1,i), 3 );
            seb_geo(4,i) = allSpeakersSolution( 'seb', window, th, 'geo', sentences_vec(1,i), 4 );
            % jfmp + srp
            seb_srp(1,i) = allSpeakersSolution( 'seb', window, th, 'srp', sentences_vec(1,i), 1 );
            seb_srp(2,i) = allSpeakersSolution( 'seb', window, th, 'srp', sentences_vec(1,i), 2 );
            seb_srp(3,i) = allSpeakersSolution( 'seb', window, th, 'srp', sentences_vec(1,i), 3 );
            seb_srp(4,i) = allSpeakersSolution( 'seb', window, th, 'srp', sentences_vec(1,i), 4 );
        end
        figure
        H = gcf;
        S = sprintf('Comparision: Institution & Method _ N: %d _ th: %d dBFS', window, th);
        H.Name = S;
        subplot(2,2,1)
        plot(sentences_vec(1,:), jfmp_geo, '*');
        hold on
        plot(sentences_vec(1,:), [ones(size(sentences_vec(1,:)))*0; ones(size(sentences_vec(1,:)))*160; ones(size(sentences_vec(1,:)))*-135; ones(size(sentences_vec(1,:)))*-70  ], 'r')
        hold off
        xlabel('Sentence')
        ylabel('Angle (degrees)')
        legend('P1', 'P2', 'P3', 'P4')
        title ('JFMP & GEO')
        axis ([1 7 -180 180])
        grid on
        
        subplot(2,2,2)
        plot(sentences_vec(1,:), jfmp_srp, '*');
        hold on
        plot(sentences_vec(1,:), [ones(size(sentences_vec(1,:)))*0; ones(size(sentences_vec(1,:)))*160; ones(size(sentences_vec(1,:)))*-135; ones(size(sentences_vec(1,:)))*-70  ], 'r')
        hold off
        xlabel('Sentence')
        ylabel('Angle (degrees)')
        title ('JFMP & SRP')
        axis ([1 7 -180 180])
        grid on
        
        subplot(2,2,3)
        plot(sentences_vec(1,:), seb_geo, '*');
        hold on
        plot(sentences_vec(1,:), [ones(size(sentences_vec(1,:)))*0; ones(size(sentences_vec(1,:)))*160; ones(size(sentences_vec(1,:)))*-135; ones(size(sentences_vec(1,:)))*-70  ], 'r')
        hold off
        xlabel('Sentence')
        ylabel('Angle (degrees)')
        title ('SEB & GEO')
        axis ([1 7 -180 180])
        grid on
        
        subplot(2,2,4)
        plot(sentences_vec(1,:), seb_srp, '*');
        hold on
        plot(sentences_vec(1,:), [ones(size(sentences_vec(1,:)))*0; ones(size(sentences_vec(1,:)))*160; ones(size(sentences_vec(1,:)))*-135; ones(size(sentences_vec(1,:)))*-70  ], 'r')
        hold off
        xlabel('Sentence')
        ylabel('Angle (degrees)')
        title ('SEB & SRP')
        axis ([1 7 -180 180])
        grid on
        
    case 'speakers'
        % how many different speakers in the files?
        speakers_vec = [];
        myFiles = selectData([], window, [], [], [], [], 0);
        num_files = length(myFiles);
        for i = 1:num_files
            % store all different speakers in a vector
            structure_tmp = createFileStructure(myFiles(i));
            if (~any(ismember(speakers_vec, structure_tmp.speaker))) % new element
                speakers_vec(length(speakers_vec)+1) = structure_tmp.speaker;
            end
        end
        speakers_vec = sort(speakers_vec);
        %seb_vec = [speakers_vec; zeros(8, length(speakers_vec)) ];

        jfmp_geo = zeros(4,length(speakers_vec));
        jfmp_srp = zeros(4,length(speakers_vec));
        seb_geo = zeros(4,length(speakers_vec));
        seb_srp = zeros(4,length(speakers_vec));
        
        % for every different sentence, get an output
        for i = speakers_vec
            % jfmp + geo
            jfmp_geo(1,i) = allSentencesSolution('jfmp', window, th, 'geo', speakers_vec(i), 1);
            jfmp_geo(2,i) = allSentencesSolution('jfmp', window, th, 'geo', speakers_vec(i), 2);
            jfmp_geo(3,i) = allSentencesSolution('jfmp', window, th, 'geo', speakers_vec(i), 3);
            jfmp_geo(4,i) = allSentencesSolution('jfmp', window, th, 'geo', speakers_vec(i), 4);
            % jfmp + srp
            jfmp_srp(1,i) = allSentencesSolution('jfmp', window, th, 'srp', speakers_vec(i), 1);
            jfmp_srp(2,i) = allSentencesSolution('jfmp', window, th, 'srp', speakers_vec(i), 2);
            jfmp_srp(3,i) = allSentencesSolution('jfmp', window, th, 'srp', speakers_vec(i), 3);
            jfmp_srp(4,i) = allSentencesSolution('jfmp', window, th, 'srp', speakers_vec(i), 4);
            % jfmp + geo
            seb_geo(1,i) = allSentencesSolution('seb', window, th, 'geo', speakers_vec(i), 1);
            seb_geo(2,i) = allSentencesSolution('seb', window, th, 'geo', speakers_vec(i), 2);
            seb_geo(3,i) = allSentencesSolution('seb', window, th, 'geo', speakers_vec(i), 3);
            seb_geo(4,i) = allSentencesSolution('seb', window, th, 'geo', speakers_vec(i), 4);
            % jfmp + srp
            seb_srp(1,i) = allSentencesSolution('seb', window, th, 'srp', speakers_vec(i), 1);
            seb_srp(2,i) = allSentencesSolution('seb', window, th, 'srp', speakers_vec(i), 2);
            seb_srp(3,i) = allSentencesSolution('seb', window, th, 'srp', speakers_vec(i), 3);
            seb_srp(4,i) = allSentencesSolution('seb', window, th, 'srp', speakers_vec(i), 4);
        end
        figure
        H = gcf;
        S = sprintf('Comparision: Institution & Method _ N: %d _ th: %d dBFS', window, th);
        H.Name = S;
        subplot(2,2,1)
        plot(speakers_vec, jfmp_geo, '*');
        hold on
        plot(speakers_vec, [ones(length(speakers_vec))*0; ones(length(speakers_vec))*160; ones(length(speakers_vec))*-135; ones(length(speakers_vec))*-70  ], 'r')
        hold off
        xlabel('Speaker')
        ylabel('Angle (degrees)')
        legend('P1', 'P2', 'P3', 'P4')
        title ('JFMP & GEO')
        axis ([speakers_vec(1) speakers_vec(end) -180 180])
        grid on
        
        subplot(2,2,2)
        plot(speakers_vec, jfmp_srp, '*');
        hold on
        plot(speakers_vec, [ones(length(speakers_vec))*0; ones(length(speakers_vec))*160; ones(length(speakers_vec))*-135; ones(length(speakers_vec))*-70  ], 'r')
        hold off
        xlabel('Speaker')
        ylabel('Angle (degrees)')
        legend('P1', 'P2', 'P3', 'P4')
        title ('JFMP & SRP')
        axis ([speakers_vec(1) speakers_vec(end) -180 180])
        grid on
        
        subplot(2,2,3)
        plot(speakers_vec, seb_geo, '*');
        hold on
        plot(speakers_vec, [ones(length(speakers_vec))*0; ones(length(speakers_vec))*160; ones(length(speakers_vec))*-135; ones(length(speakers_vec))*-70  ], 'r')
        hold off
        xlabel('Speaker')
        ylabel('Angle (degrees)')
        legend('P1', 'P2', 'P3', 'P4')
        title ('SEB & GEO')
        axis ([speakers_vec(1) speakers_vec(end) -180 180])
        grid on
        
        subplot(2,2,4)
        plot(speakers_vec, seb_srp, '*');
        hold on
        plot(speakers_vec, [ones(length(speakers_vec))*0; ones(length(speakers_vec))*160; ones(length(speakers_vec))*-135; ones(length(speakers_vec))*-70  ], 'r')
        hold off
        xlabel('Speaker')
        ylabel('Angle (degrees)')
        legend('P1', 'P2', 'P3', 'P4')
        title ('SEB & SRP')
        axis ([speakers_vec(1) speakers_vec(end) -180 180])
        grid on
        
end

end

