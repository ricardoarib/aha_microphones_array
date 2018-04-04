load('points.mat');

%N = [ 256 512 1024 2048 4096 8192 16384 32768 ];
%position_vec = [ 0 160 -135 -70 ];

%th_min = -30;
%th_step = 5;
%th_max = -10;
%th = th_min:th_step:th_max;

% xx = ones(length(th),1) * N ;
% yy = th' * ones(1, length(N));

% surf(xx, yy, err_mtx)
% surf(xx, yy, xx)

% xlabel('N')
% ylabel('th')
% zlabel('error')

% m = 1;
m = find( th == -20 );
n = 1;

err = zeros(13, 420);

err = cell2mat(ang_cell{m,n}(:)) - cell2mat(true_ang_cell{m,n}(:));


subplot(2,2,1)
plot( ang_cell{m,n},'*');
subplot(2,2,2)
boxplot( ang_cell{m,n}, 'notch', 'on' )
subplot(2,2,3)


boxplot( cell2mat(ang_cell(m,3:end)')', N(3:end), 'notch', 'on' )
    

