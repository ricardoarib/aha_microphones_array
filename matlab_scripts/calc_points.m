N = [ 256 512 1024 2048 4096 8192 16384 32768 ];
position_vec = [ 0 160 -135 -70 ];

th_min = -60;
th_step = 5;
th_max = 0;

th = th_min:th_step:th_max;

err_acc = zeros(length(th), length(N)) ;
err_cnt = zeros(length(th), length(N)) ;
ang_cell = cell(length(th), length(N));
true_ang_cell = cell(length(th), length(N));


for n = 1:length(N)
    n
    myFile = selectData('seb', N(n),[], [], [], 'geo', 0, [],[],'T');
%     myFile = selectData([], [],[], [3], [], 'geo', 1, [],[],[]); %% test signals
    for o = 1:length(myFile)
        o
        [angles, ~, levels_dB] = load_data(fullfile( myFile(o).folder, myFile(o).name));
        tmpStruct = createFileStructure( myFile(o) );
        true_ang = position_vec( tmpStruct.position )
        for m = 1:length(th)
            m
            [filtered_angles, ~] = filter_data (angles, levels_dB, th(m));
            filtered_angles
            if ~isempty(filtered_angles)
                ang = mean( filtered_angles ) ;
            else
                ang = 0 ;
            end
%             err_acc( m, n ) = err_acc( m, n ) + abs(ang - true_ang) ;
            err_acc( m, n ) = err_acc( m, n ) + ang ;
            err_cnt( m, n ) = err_cnt( m, n ) + 1 ;
            ang_cell{ m, n } = [ang_cell{ m, n } ang] ;
            true_ang_cell{ m, n } = [true_ang_cell{ m, n } true_ang] ;
        end
    end
end


err_mtx = err_acc ./ err_cnt;
save('points.mat','err_mtx','th','N', 'ang_cell', 'true_ang_cell');
