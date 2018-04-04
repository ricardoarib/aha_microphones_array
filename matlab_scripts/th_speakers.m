function [ th_vec, angle_vec ] = th_speakers( th_min, th_step, th_max, institution, window_size, sentence, position, method )
%[ th_vec, angle_vec ] = th_sentences( th_min, th_max, th_step, institution, window_size, speaker, position )
%   consider all speakers

th_vec = th_min:th_step:th_max;
j = 1;
angle_vec = zeros(size(th_vec));

for i = th_vec
    angle_vec(j) = allSpeakersSolution(institution, window_size, th_vec(j), method, sentence, position); 
    j = j+1;
end

figure
plot(th_vec, angle_vec, '*'); hold on;
switch position
    case 1
        plot(th_vec, 0*ones(size(th_vec)), 'r');
    case 2
        plot(th_vec, 160*ones(size(th_vec)), 'r');
    case 3
        plot(th_vec, -135*ones(size(th_vec)), 'r');
    case 4
        plot(th_vec, -70*ones(size(th_vec)), 'r');
end

grid on
axis ([th_vec(1) th_vec(end) -180 180])
xlabel ('th (dB FS)')
ylabel ('output angle (degrees)')
S_title = sprintf ('%s, N%d, sentence: %d, position: %d, method: %s', institution, window_size, sentence, position, method);
title(S_title);

end

