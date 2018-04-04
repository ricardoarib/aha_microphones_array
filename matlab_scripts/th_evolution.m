function [ angle_vec ] = th_evolution (  th_min, th_step, th_max, institution, window, method, sentence, position, plot_on_off)
%UNTITLED12 Summary of this function goes here
%   Detailed explanation goes here

th_vec = th_min:th_step:th_max;
angle_vec = size(th_vec);
n = 1;


for th = th_vec
    angle_vec (n) = allSpeakersSolution( institution, window, th, method, sentence, position );
    n = n +1 ;
end

if plot_on_off == 'on'
    
    figure
    plot (th_vec, angle_vec, '*'); hold on;
    
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
    
    xlabel ('th (dbFS)')
    ylabel ('angle (degrees)')
    S = sprintf('Evolution of threshold for %s, N%d, method: %s, sentence: %d, position: %d. \n All speakers considered!', institution, window, method, sentence, position);
    title (S);
elseif plot_on_off == 'off'
    return
end

end

