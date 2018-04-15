%load('points.mat');
%load('points_seb_geo_tunning.mat');
%load('points_seb_geo_validation.mat');
%load('points_seb_srp_tunning.mat');
%load('points_seb_srp_validation.mat');
%load('points_jfmp_geo_tunning.mat');
%load('points_jfmp_geo_validation.mat');
%load('points_jfmp_srp_tunning.mat');
%load('points_jfmp_srp_validation.mat');

%load('points_seb_geo_TV.mat');
load('points_seb_srp_TV.mat');
%load('points_jfmp_geo_TV.mat');
%load('points_jfmp_srp_TV.mat');

%load('points_all_geo_T.mat');
%load('points_all_srp_T.mat');
%load('points_all_geo_V.mat');
%load('points_all_srp_V.mat');



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
%m = find( th == -15 );
%n = 1;
n = find( N == 16384 ) ;
%n = find( N == 32768 ) ;

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

cell2counts      = @(C)row( cellfun(@(x)sum(~isnan(x)),C) ); % function to count valid points in cells. Returns a row vector.
cell2countstotal = @(C)row( cellfun(@(x)length(x),C) ); % function to count all points in cells (valid+invalid). Returns a row vector.

% functions to get a column[/row] vector with the id of the cells (id's are arbitrary increasing numbers)
cellid2col = @(C)col( cell2mat( arrayfun(@(I)I*ones(numel(C{I}),1),col(1:numel(C)),'uni',0) ) );
cellid2row = @(C)row( cell2mat( arrayfun(@(I)I*ones(numel(C{I}),1),col(1:numel(C)),'uni',0) ) );

selectbyid = @(M,Mid,id) M( find( Mid == id ) ) ;
medianrow  = @(M,Mid) row( arrayfun( @(x)median( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
meanrow    = @(M,Mid) row( arrayfun( @(x)  mean( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
varrow     = @(M,Mid) row( arrayfun( @(x)   var( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;
stdrow     = @(M,Mid) row( arrayfun( @(x)   std( selectbyid(M,Mid,x), 'omitnan' ) , unique(Mid) ) ) ;


plot2=@(C,varargin)plot( cellid2col(C), cell2col(C), varargin{:} );
plot4=@(C,X,varargin)plot( X(cellid2col(C)), cell2col(C), varargin{:} );
boxplot2=@(C,varargin)boxplot( cell2row(C), cellid2row(C) , varargin{:} );
boxplot4=@(C,X,varargin)boxplot( cell2row(C), X(cellid2row(C)) , varargin{:} );


angles       = cell2col(   ang_cell(m,:) );
true_angles  = cell2col(   true_ang_cell(m,:) );
counts       = cell2counts(   ang_cell(m,:) );
total_counts = cell2countstotal ( ang_cell(m,:) );
err = angles - true_angles ;
err = err + 360 * (err <= -180) ;
err = err - 360 * (err > 180) ;
ids         = cellid2col( ang_cell(m,:) );
x = N(ids) ;
err_medians = medianrow(err,ids) ;
err_means   =   meanrow(err,ids) ;
err_var     =    varrow(err,ids) ;
err_std     =    stdrow(err,ids) ;

disp( sprintf('err_medians [ N=%d th=%d ] = %.1f', N(n), th(m), err_medians(n)) )
disp( sprintf('err_std     [ N=%d th=%d ] = %.1f', N(n), th(m), err_std(n)) )
disp( sprintf('detected    [ N=%d th=%d ] = %.1f %%', N(n), th(m), counts(n)/total_counts(n)*100) )


figure(99)

subplot(4,2,1)
%plot(x,angles,'o')
plot(x,err,'o')
%plot2( ang_cell(m,:), 'o' )
%plot4( ang_cell(m,:), N, 'o' )
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off')
grid on
xlabel('window size (N) [samples]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('threshold = %d dBFS',th(m)))

%subplot(4,2,2)
figure(1)
clf
%boxplot( angles, x, 'notch', 'on' )
%boxplot( err, x, 'notch', 'on' )
boxplot( err, x, 'notch', 'on', 'Widths', .8*counts/max(counts)+.0001 )
%boxplot2( ang_cell(m,:), 'notch', 'on' )
%boxplot4( ang_cell(m,:), N, 'notch', 'on' )
grid on
%xlabel('window size (N) [samples]')
xlabel('window size')
%ylabel('angle error (estimated-true) [degrees]')
ylabel('angle error [degrees]')
%title(sprintf('threshold = %d dBFS \n Box widths are proportional to sample size.',th(m)))
set(gca,'ytick',[-180 -90 0 90 180])

%set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 8 6])
set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 16 12])
print -depsc 'figure1.eps'

figure(99)

subplot(4,2,[1 3])
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
%plot( N, counts/max(counts)*100, '*-' )
plot( N, counts./total_counts*100, '*-' )
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
total_counts = cell2countstotal ( ang_cell(:,n) );
err = angles - true_angles ;
err = err + 360 * (err <= -180) ;
err = err - 360 * (err > 180) ;
ids         = cellid2col( ang_cell(:,n) );
x = th(ids) ;
err_medians = medianrow(err,ids) ;
err_means   =   meanrow(err,ids) ;
err_var     =    varrow(err,ids) ;
err_std     =    stdrow(err,ids) ;

disp( sprintf('err_medians [ N=%d th=%d ] = %.1f', N(n), th(m), err_medians(m)) )
disp( sprintf('err_std     [ N=%d th=%d ] = %.1f', N(n), th(m), err_std(m)) )
disp( sprintf('detected    [ N=%d th=%d ] = %.1f %%', N(n), th(m), counts(m)/total_counts(m)*100) )


 
subplot(4,2,5)
%plot(x,angles,'o')
plot(x,err,'o')
%plot2( ang_cell(:,n), 'o' )
%plot4( true_ang_cell(:,n), th, 'o' )
grid on
xlabel('threshold [dBFS]')
ylabel('angle error (estimated-true) [degrees]')
title(sprintf('window size = %d samples',N(n)))

figure(2)
clf

%subplot(4,2,6)
%boxplot( angles, x, 'notch', 'on' )
%boxplot( err, x, 'notch', 'on' )
boxplot( err, x, 'notch', 'on', 'Widths', .8*counts/max(counts)+.0001 )
%boxplot2( ang_cell(:,n), 'notch', 'on' )
%boxplot4( true_ang_cell(:,n), th, 'notch', 'on' )
grid on
xlabel('threshold [dBFS]')
%ylabel('angle error (estimated-true) [degrees]')
ylabel('angle error [degrees]')
%title(sprintf('window size = %d samples \n Box widths are proportional to sample size.',N(n)))
set(gca,'ytick',[-180 -90 0 90 180])

set(gca,'xlim',[4.5  12.5])
%set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 8 6])
set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 16 12])
print -depsc 'figure2.eps'

figure(3)
clf
%subplot(4,2,[5 7])
%plot(th,err_medians,'*-')
%plot( th', [err_medians' err_means' err_var'/1000 err_std'], '*-' )
plot( th', [err_medians' err_std'], '*-' )
grid on
xlabel('threshold [dBFS]')
ylabel('angle error [degrees]')
%title(sprintf('window size = %d samples',N(n)))
%legend('median','mean','variance/1000','std')
legend('median','std','location','southwest')
set(gca,'ytick',[-180 -90 0 90 180])
%set(gca,'ytick',[-180:45:180])


set(gca,'xlim',[-40  -5])
set(gca,'ylim',[-90  90])
%set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 8 6])
set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 16 6])
print -depsc 'figure3.eps'


figure(4)
clf
%subplot(4,2,8)
plot(th,counts/max(counts)*100,'*-')
grid on
xlabel('threshold [dBFS]')
ylabel('phrases detected [%]')
%title(sprintf('window size = %d samples',N(n)))

set(gca,'xlim',[-40  -5])
%set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 8 6])
set(gcf,'PaperUnits','centimeters','PaperPosition',[0 0 16 6])
set(gcf,'PaperUnits','centimeters','PaperSize',[16 6])
print -depsc 'figure4.eps'


figure(99)

angles      = cell2col(   ang_cell );
true_angles = cell2col(   true_ang_cell );
counts      = cell2counts(   ang_cell );
err = angles - true_angles ;
err = err + 360 * (err <= -180) ;
err = err - 360 * (err > 180) ;
idsN         = cellid2col( ang_cell(1,:) );
idsth        = cellid2col( ang_cell(:,1) );
x = N(idsN) ;
y = th(idsth) ;

err_medians_3d = [] ;
err_means_3d   = [] ;
err_var_3d     = [] ;
err_std_3d     = [] ;
for n=1:length(N)
    ids_3d         = cellid2col( ang_cell(:,n) );
    err_medians_3d = [ err_medians_3d ; medianrow(err,ids) ] ;
    err_means_3d   = [ err_means_3d ; meanrow(err,ids) ] ;
    err_var_3d     = [ err_var_3d ; varrow(err,ids) ] ;
    err_std_3d     = [ err_std_3d ; stdrow(err,ids) ] ;
end

figure(98)

subplot(2,1,1)
[thth,NN] = meshgrid(th,N);
%plot3(x,y,err_medians,'*')
%plot3(NN,thth,err_medians,'*')
surf(NN,thth,err_medians_3d,err_medians_3d)
% surf(xx, yy, err_mtx)
% surf(xx, yy, xx)
grid on
xlabel('N')
ylabel('th')
zlabel('error median')
%grid on
%xlabel('threshold [dBFS]')
%ylabel('phrases detected [%]')
%title(sprintf('window size = %d samples',N(n)))
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off','XMinorGrid','off')


subplot(2,1,2)
[thth,NN] = meshgrid(th,N);
%plot3(x,y,err_medians,'*')
%plot3(NN,thth,err_medians,'*')
surf(NN,thth,err_std_3d,err_std_3d)
% surf(xx, yy, err_mtx)
% surf(xx, yy, xx)
grid on
xlabel('N')
ylabel('th')
zlabel('error std')
%grid on
%xlabel('threshold [dBFS]')
%ylabel('phrases detected [%]')
%title(sprintf('window size = %d samples',N(n)))
set(gca,'xscale','log')
set(gca,'xtick',N, 'XMinorTick','off','XMinorGrid','off')
