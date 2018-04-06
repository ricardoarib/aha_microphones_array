%load('points.mat');
load('points_seb_geo_tunning.mat');
%load('points_seb_srp_tunning.mat');
%load('points_jfmp_geo_tunning.mat');
%load('points_jfmp_srp_tunning.mat');

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
%n = 1;
n = find( N == 16384 ) ;

%err = zeros(13, 420);
%err = cell2mat(ang_cell{m,n}(:)) - cell2mat(true_ang_cell{m,n}(:));

clf

% subplot(4,2,1)
% plot( ang_cell{m,n},'*');
% subplot(4,2,2)
% boxplot( ang_cell{m,n}, 'notch', 'on' )


col = @(x)reshape(x,numel(x),1); % function to reshape any matrix to a column vector
row = @(x)reshape(x,1,numel(x)); % function to reshape any matrix to a row vector
cell2col    = @(C)col( cell2mat(row(C)) ); % function to convert cells to a single column vector
cell2row    = @(C)row( cell2mat(row(C)) ); % function to convert cells to a single row vector

cell2counts = @(C)row( cellfun(@(x)sum(~isnan(x)),C) ); % function to count valid points in cells. Returns a row vector.

% functions to get a column[/row] vector with the id of the cells (id's are arbitrary increasing numbers)
cellid2col = @(C)col( cell2mat( arrayfun(@(I)I*ones(numel(C{I}),1),col(1:numel(C)),'uni',0) ) );
cellid2row = @(C)row( cell2mat( arrayfun(@(I)I*ones(numel(C{I}),1),col(1:numel(C)),'uni',0) ) );

selectbyid = @(M,Mid,id) M( find( Mid == id ) ) ;
medianrow  = @(M,Mid) row( arrayfun( @(x)median( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
meanrow    = @(M,Mid) row( arrayfun( @(x)  mean( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
varrow     = @(M,Mid) row( arrayfun( @(x)   var( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
stdrow     = @(M,Mid) row( arrayfun( @(x)   std( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;


plot2=@(C,varargin)plot( cellid2col(C), cell2col(C), varargin{:} );
plot3=@(C,X,varargin)plot( X(cellid2col(C)), cell2col(C), varargin{:} );
boxplot2=@(C,varargin)boxplot( cell2row(C), cellid2row(C) , varargin{:} );
boxplot3=@(C,X,varargin)boxplot( cell2row(C), X(cellid2row(C)) , varargin{:} );


angles      = cell2col(   ang_cell(m,:) );
true_angles = cell2col(   true_ang_cell(m,:) );
counts      = cell2counts(   ang_cell(m,:) );
err = angles - true_angles ;
err = err + 360 * (err <= -180) ;
err = err - 360 * (err > 180) ;
ids         = cellid2col( ang_cell(m,:) );
x = N(ids) ;
err_medians = medianrow(err,ids) ;
err_means   =   meanrow(err,ids) ;
err_var     =    varrow(err,ids) ;
err_std     =    stdrow(err,ids) ;

subplot(4,2,1)
%plot(x,angles,'o')
plot(x,err,'o')
%plot2( ang_cell(m,:), 'o' )
%plot3( ang_cell(m,:), N, 'o' )
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off')
grid on
xlabel('window size (N) [samples]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('threshold = %d dBFS',th(m)))

subplot(4,2,2)
%boxplot( angles, x, 'notch', 'on' )
%boxplot( err, x, 'notch', 'on' )
boxplot( err, x, 'notch', 'on', 'Widths', .8*counts/max(counts)+.0001 )
%boxplot2( ang_cell(m,:), 'notch', 'on' )
%boxplot3( ang_cell(m,:), N, 'notch', 'on' )
grid on
xlabel('window size (N) [samples]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('threshold = %d dBFS \n Box widths are proportional to sample size.',th(m)))
set(gca,'ytick',[-180 -90 0 90 180])

subplot(4,2,3)
plot( N', [err_medians' err_means' err_var'/1000 err_std'], '*-' )
grid on
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off','XMinorGrid','off')
set(gca,'xlim',[min(N) max(N)])
xlabel('window size (N) [samples]')
ylabel('angle error medians/mean/var [degrees]')
title(sprintf('threshold = %d dBFS',th(m)))
legend('median','mean','variance/1000','std')
%set(gca,'ytick',[-180 -90 0 90 180])
set(gca,'ytick',[-180:45:180])

subplot(4,2,4)
plot( N, counts/max(counts)*100, '*-' )
grid on
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off','XMinorGrid','off')
set(gca,'xlim',[min(N) max(N)])
xlabel('window size (N) [samples]')
ylabel('phrases detected [%]')
title(sprintf('threshold = %d dBFS',th(m)))


angles      = cell2col(   ang_cell(:,n) );
true_angles = cell2col(   true_ang_cell(:,n) );
counts      = cell2counts(   ang_cell(:,n) );
err = angles - true_angles ;
err = err + 360 * (err <= -180) ;
err = err - 360 * (err > 180) ;
ids         = cellid2col( ang_cell(:,n) );
x = th(ids) ;
err_medians = medianrow(err,ids) ;
err_means   =   meanrow(err,ids) ;
err_var     =    varrow(err,ids) ;
err_std     =    stdrow(err,ids) ;


 
subplot(4,2,5)
%plot(x,angles,'o')
plot(x,err,'o')
%plot2( ang_cell(:,n), 'o' )
%plot3( true_ang_cell(:,n), th, 'o' )
grid on
xlabel('threshold [dBFS]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('window size = %d samples',N(n)))

subplot(4,2,6)
%boxplot( angles, x, 'notch', 'on' )
%boxplot( err, x, 'notch', 'on' )
boxplot( err, x, 'notch', 'on', 'Widths', .8*counts/max(counts)+.0001 )
%boxplot2( ang_cell(:,n), 'notch', 'on' )
%boxplot3( true_ang_cell(:,n), th, 'notch', 'on' )
grid on
xlabel('threshold [dBFS]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('window size = %d samples \n Box widths are proportional to sample size.',N(n)))
set(gca,'ytick',[-180 -90 0 90 180])

subplot(4,2,7)
plot(th,err_medians,'*-')
plot( th', [err_medians' err_means' err_var'/1000 err_std'], '*-' )
grid on
xlabel('threshold [dBFS]')
ylabel('angle error medians/means/var [degrees]')
title(sprintf('window size = %d samples',N(n)))
legend('median','mean','variance/1000','std')
%set(gca,'ytick',[-180 -90 0 90 180])
set(gca,'ytick',[-180:45:180])


subplot(4,2,8)
plot(th,counts/max(counts)*100,'*-')
grid on
xlabel('threshold [dBFS]')
ylabel('phrases detected [%]')
title(sprintf('window size = %d samples',N(n)))

