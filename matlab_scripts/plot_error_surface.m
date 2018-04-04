load('err_mtx.mat');

%N = [ 256 512 1024 2048 4096 8192 16384 32768 ];
%position_vec = [ 0 160 -135 -70 ];

%th_min = -30;
%th_step = 5;
%th_max = -10;
%th = th_min:th_step:th_max;

xx = ones(length(th),1) * N ;
yy = th' * ones(1, length(N));

surf(xx, yy, err_mtx)
% surf(xx, yy, xx)

xlabel('N')
ylabel('th')
zlabel('error')