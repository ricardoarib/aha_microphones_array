% %% Error variation with parameters
% 
% Nwindow_vec = [256 512 1024 2048 4096 8192 16384 32768];
% speaker = [];
% position = [];
% method = [];
% gender = [];
% age = [];
% subset = 'T';
% 
% myFiles_seb{:,1} = selectData('seb', Nwindow_vec(1), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,2} = selectData('seb', Nwindow_vec(2), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,3} = selectData('seb', Nwindow_vec(3), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,4} = selectData('seb', Nwindow_vec(4), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,5} = selectData('seb', Nwindow_vec(5), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,6} = selectData('seb', Nwindow_vec(6), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,7} = selectData('seb', Nwindow_vec(7), speaker, position, method, method, 0, gender, age, subset);
% myFiles_seb{:,8} = selectData('seb', Nwindow_vec(8), speaker, position, method, method, 0, gender, age, subset);
% 
% %%
% myFiles_jfmp{:,1} = selectData('jfmp', Nwindow_vec(1), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,2} = selectData('jfmp', Nwindow_vec(2), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,3} = selectData('jfmp', Nwindow_vec(3), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,4} = selectData('jfmp', Nwindow_vec(4), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,5} = selectData('jfmp', Nwindow_vec(5), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,6} = selectData('jfmp', Nwindow_vec(6), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,7} = selectData('jfmp', Nwindow_vec(7), speaker, position, method, method, 0, gender, age, subset);
% myFiles_jfmp{:,8} = selectData('jfmp', Nwindow_vec(8), speaker, position, method, method, 0, gender, age, subset);
% 
% all_myFiles = [ myFiles_seb; myFiles_jfmp ];

%% Choose certain N from

th_min = -30;
th_step = 10;
th_max = -10;

th_vec = -30:5:-10;

l_th_vec = length(th_vec);
l_Nwindow_vec = length(Nwindow_vec);

angle_mtx = zeros(l_th_vec, l_Nwindow_vec);
angle_error_mtx = zeros(size(angle_mtx));

result_angle_seb = 0;
result_angle_error_seb = 0;
result_angle_jfmp = 0;
result_angle_error_jfmp = 0;

j = 1;

for n = 1 : l_Nwindow_vec
    for th = th_vec
        
        for f = 1:length(all_myFiles{1,n}(:)) %seb
            [tmp_angle_seb, tmp_angle_error_seb] = result_from_file ( all_myFiles{1,n}(f), th, 'median') ;
            result_angle_seb = result_angle_seb + tmp_angle_seb;
            result_angle_error_seb = result_angle_error_seb + tmp_angle_error_seb;
        end
        result_angle_seb = result_angle_seb / length(all_myFiles{1,n}(:));
        result_angle_error_seb = result_angle_error_seb / length(all_myFiles{1,n}(:));
        
        for f = 1:length(all_myFiles{2,n}(:)) %jfmp
            [tmp_angle_jfmp, tmp_angle_error_jfmp] = result_from_file ( all_myFiles{2,n}(f), th, 'median') ;
            result_angle_jfmp = result_angle_jfmp + tmp_angle_seb;
            result_angle_error_jfmp = result_angle_error_jfmp + tmp_angle_error_jfmp;
        end
        result_angle_jfmp = result_angle_seb / length(all_myFiles{2,n}(:));
        result_angle_error_jfmp = result_angle_error_jfmp / length(all_myFiles{2,n}(:));
        
        tmp_angle = mean([tmp_angle_seb, tmp_angle_jfmp]);
        
        tmp_angle_error = mean([tmp_angle_error_seb, tmp_angle_error_jfmp]);
        result_angle_error =  + tmp_angle_error;
        
        angle_mtx (j,n) = mean([result_angle_seb, result_angle_jfmp]);
        angle_error_mtx (j,n) = mean( [result_angle_error_seb, result_angle_error_jfmp] );
        
        j = j + 1;
   
    end
    j = 1;
end
    