function [ mean_p1, mean_p2, mean_p3, mean_p4 ] = getMeanAngles( myFiles, th )

sum_angles = zeros(1,4);
num_angles = zeros(1,4);

for i = 1:length(myFiles)
    structure_tmp = createFileStructure(myFiles(i));
    switch structure_tmp.position
        case 1
            tmp_angle = getMedianOutput( myFiles(i), th );
            sum_angles(1) = sum_angles(1) + tmp_angle;
            num_angles(1) = num_angles(1) + 1;
        case 2
            tmp_angle = getMedianOutput( myFiles(i), th );
            sum_angles(2) = sum_angles(2) + tmp_angle;
            num_angles(2) = num_angles(2) + 1;
        case 3
            tmp_angle = getMedianOutput( myFiles(i), th );
            sum_angles(3) = sum_angles(3) + tmp_angle;
            num_angles(3) = num_angles(3) + 1;
        case 4
            tmp_angle = getMedianOutput( myFiles(i), th );
            sum_angles(4) = sum_angles(4) + tmp_angle;
            num_angles(4) = num_angles(4) + 1;
    end
end

mean_angles = sum_angles./num_angles;

mean_p1 = mean_angles(1);
mean_p2 = mean_angles(2);
mean_p3 = mean_angles(3);
mean_p4 = mean_angles(4);

end

