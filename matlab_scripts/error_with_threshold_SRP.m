%% Error variation with threshold

clear; clc;

%% All speakers, all sentences
for th = -50:1:-10
    [srp_sol1(th+51), srp_sol2(th+51), srp_sol3(th+51), srp_sol4(th+51)] = srp_all_speakers_all_sentences(th);
end

error_1 = 0 -srp_sol1;
error_2 = 160 - srp_sol2;
error_3 = -135 - srp_sol3;
error_4 = -70 - srp_sol4;

save ('all_speaker_all_sentences_srp_N1024.mat')

figure
H = gcf;
H.Name = 'All speakers, all sentences, srp, N1024';
subplot(1,2,1)
plot(-50:1:-10,[srp_sol1; srp_sol2; srp_sol3; srp_sol4;]','*')
legend('position 1: 0º', 'position 2: 160º', 'position 3: -135º', 'position 4: -70º')
xlabel('th (dB)')
ylabel('angle (degrees)')
title({'Evolution of the output angle, for different threshold values','(all speakers and all sentences considered)','srpmetric approach - N1024'})
grid on
subplot(1,2,2)
plot(-50:1:-10,[error_1; error_2; error_3; error_4;]','*')
legend('error position 1: 0º', 'error position 2: 160º', 'error position 3: -135º', 'error position 4: -70º')
xlabel('th (dB)')
ylabel('angle (degrees)')
title({'Evolution of the output angle error, for different threshold values','(all speakers and all sentences considered)','srpmetric approach - N1024'})
grid on

savefig('all_speakers_all_sentences_srp_N1024.fig'); close(gcf);

%% All speakers, one sentence
j_th = 1;
for th = -50:2:-10
    for sentence = 1:7
        [srp_sol1(j_th,sentence), srp_sol2(j_th,sentence), srp_sol3(j_th,sentence), srp_sol4(j_th,sentence)] = srp_all_speakers_one_sentence(th, sentence);
    end
    j_th = j_th+1;
end

figure
H = gcf;
H.Name = 'All speakers, one sentence, srp, N1024';

subplot(2,2,1)
plot(-50:2:-10,srp_sol1','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 1 - N1024'})
grid on

subplot(2,2,2)
plot(-50:2:-10,srp_sol2','*'); hold on;
plot(-50:2:-10, 160*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 2 - N1024'})

grid on
subplot(2,2,3)
plot(-50:2:-10,srp_sol3','*'); hold on;
plot(-50:2:-10, -135*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 3 - N1024'})
grid on

subplot(2,2,4)
plot(-50:2:-10,srp_sol4','*'); hold on;
plot(-50:2:-10, -70*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 4 - N1024'})
grid on

savefig('all_speakers_one_sentences_srp_N1024.fig'); close(gcf);

%%

error_1 = 0 -srp_sol1;
error_2 = 160 - srp_sol2;
error_3 = -135 - srp_sol3;
error_4 = -70 - srp_sol4;

save ('all_speakers_one_sentence_srp_N1024.mat')

figure
H = gcf;
H.Name = 'All speakers, one sentence, srp, N1024 - Error';

subplot(2,2,1)
plot(-50:2:-10,error_1','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle error, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 1 - N1024'})
grid on

subplot(2,2,2)
plot(-50:2:-10,error_2','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle error, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 2 - N1024'})

grid on
subplot(2,2,3)
plot(-50:2:-10,error_3','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle error, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 3 - N1024'})
grid on

subplot(2,2,4)
plot(-50:2:-10,error_4','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('sentence 1','sentence 2','sentence 3','sentence 4','sentence 5','sentence 6','sentence 7')
title({'Evolution of the output angle error, for different threshold values','(all speakers for every sentence considered)','srpmetric approach- Position 4 - N1024'})
grid on

savefig('all_speakers_one_sentence_srp_N1024-error.fig'); close(gcf);

%% One speaker, all sentences
j_th = 1;
for th = -50:2:-10
    for speaker = 1:30
        [srp_sol1(j_th,speaker), srp_sol2(j_th,speaker), srp_sol3(j_th,speaker), srp_sol4(j_th,speaker)] = srp_one_speaker_all_sentences(th, speaker);
    end
    j_th = j_th+1;
end

%%

figure
H = gcf;
H.Name = 'One speaker, all sentences, srp, N1024';

subplot(2,2,1)
plot(-50:2:-10,srp_sol1,'*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 1 - N1024'})
grid on

subplot(2,2,2)
plot(-50:2:-10,srp_sol2','*'); hold on;
plot(-50:2:-10, 160*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 2 - N1024'})
grid on

subplot(2,2,3)
plot(-50:2:-10,srp_sol3','*'); hold on;
plot(-50:2:-10, -135*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 3 - N1024'})
grid on

subplot(2,2,4)
plot(-50:2:-10,srp_sol4','*'); hold on;
plot(-50:2:-10, -70*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 4 - N1024'})
grid on

savefig('one_speaker_all_sentences_srp_N1024.fig'); close(gcf);

%%

error_1 = 0 -srp_sol1;
error_2 = 160 - srp_sol2;
error_3 = -135 - srp_sol3;
error_4 = -70 - srp_sol4;

save ('one_speaker_all_sentences_srp_N1024.mat')

figure
H = gcf;
H.Name = 'One speaker, all sentences, srp, N1024 - Error';

subplot(2,2,1)
plot(-50:2:-10,error_1,'*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle error, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 1 - N1024'})
grid on

subplot(2,2,2)
plot(-50:2:-10,error_2','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle error, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 2 - N1024'})
grid on

subplot(2,2,3)
plot(-50:2:-10,error_3','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle error, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 3 - N1024'})
grid on

subplot(2,2,4)
plot(-50:2:-10,error_4','*'); hold on;
plot(-50:2:-10, 0*ones(size(-50:2:-10)),'r');
xlabel('th (dB)')
ylabel('angle (degrees)')
%legend ('speaker 1', 'speaker 2', 'speaker 3', 'speaker 4', 'speaker 5', 'speaker 6','speaker 7', 'speaker 8', 'speaker 9', 'speaker 10', 'speaker 11', 'speaker 12', 'speaker 13', 'speaker 14','speaker 15','speaker 16','speaker 17','speaker 18','speaker 19','speaker 20','speaker 21','speaker 22','speaker 23','speaker 24','speaker 25','speaker 26','speaker 27','speaker 28','speaker 29','speaker 30');
title({'Evolution of the output angle error, for different threshold values','(For each speaker, all sentence considered)','srpmetric approach- Position 4 - N1024'})
grid on

savefig('one_speaker_all_sentences_srp_N1024-error.fig'); close(gcf);

% %% One speaker, one sentence
% 
% j_th = 1;
% for th = -50:2:-10
%     for sentence = 1:7
%         for speaker = 1:30
%             [srp_sol1(j_th,speaker,sentence), srp_sol2(j_th,speaker,sentence), srp_sol3(j_th,speaker,sentence), srp_sol4(j_th,speaker,sentence)] = srp_one_speaker_one_sentence(th, speaker, sentence);
%         end
%     end
%     j_th = j_th+1;
% end
% 
% save ('one_speaker_one_sentence_srp_N1024.mat')
% 
% %% Position 1
% figure
% H = gcf;
% H.Name = 'One speaker, one sentence, srp, N1024 - Position 1';
% 
% subplot(2,4,1)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,1)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers',' Sentence 1','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,2)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,2)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 2','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,3)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,3)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 3','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,4)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,4)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 4','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,5)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,5)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 5','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,6)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,6)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 6','srpmetric approach- Position 1 - N1024'})
% 
% subplot(2,4,7)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,7)); hold on;
% surf( 1:30, -50:2:-10, 0*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values','and different speakers','Sentence 7','srpmetric approach- Position 1 - N1024'})
% 
% savefig('one_speaker_one_sentence_position-1_srp_N1024.fig'); close(gcf);
% 
% %% Position 2
% figure
% H = gcf;
% H.Name = 'One speaker, one sentence, srp, N1024 - Position 2';
% 
% subplot(2,4,1)
% surf( 1:30, -50:2:-10, srp_sol1(:,:,1)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers',' Sentence 1','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,2)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,2)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 2','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,3)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,3)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 3','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,4)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,4)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 4','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,5)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,5)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 5','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,6)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,6)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 6','srpmetric approach- Position 2 - N1024'})
% 
% subplot(2,4,7)
% surf( 1:30, -50:2:-10, srp_sol2(:,:,7)); hold on;
% surf( 1:30, -50:2:-10, 160*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values','and different speakers','Sentence 7','srpmetric approach- Position 2 - N1024'})
% 
% savefig('one_speaker_one_sentence_position-2_srp_N1024.fig'); close(gcf);
% 
% %% Position 3
% figure
% H = gcf;
% H.Name = 'One speaker, one sentence, srp, N1024 - Position 3';
% 
% subplot(2,4,1)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,1)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers',' Sentence 1','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,2)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,2)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 2','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,3)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,3)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 3','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,4)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,4)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 4','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,5)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,5)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 5','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,6)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,6)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 6','srpmetric approach- Position 3 - N1024'})
% 
% subplot(2,4,7)
% surf( 1:30, -50:2:-10, srp_sol3(:,:,7)); hold on;
% surf( 1:30, -50:2:-10, -135*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values','and different speakers','Sentence 7','srpmetric approach- Position 3 - N1024'})
% 
% savefig('one_speaker_one_sentence_position-3_srp_N1024.fig'); close(gcf);
% 
% %% Position 4
% figure
% H = gcf;
% H.Name = 'One speaker, one sentence, srp, N1024 - Position 4';
% 
% subplot(2,4,1)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,1)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers',' Sentence 1','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,2)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,2)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 2','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,3)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,3)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 3','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,4)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,4)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 4','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,5)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,5)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 5','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,6)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,6)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values',' and different speakers','Sentence 6','srpmetric approach- Position 4 - N1024'})
% 
% subplot(2,4,7)
% surf( 1:30, -50:2:-10, srp_sol4(:,:,7)); hold on;
% surf( 1:30, -50:2:-10, -70*ones(11,30))
% ylabel('th (dB)')
% xlabel('speaker')
% zlabel('angle (degrees)')
% title({'Output angle,',' for different threshold values','and different speakers','Sentence 7','srpmetric approach- Position 4 - N1024'})
% 
% savefig('one_speaker_one_sentence_position-4_srp_N1024.fig'); close(gcf);