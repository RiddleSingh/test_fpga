format long g;
%% Loads two sets of workspace data
% todo: make this a function, and write a script comparing arbitrary combos
%DATA_FOLDER = "ref_data/";
FILE1 = "kcross_fpga_nkM4-nk100_i0_d0_k0.txt";
FILE2 = "mat_nk100_nkm4_kcross.txt";
FILE3 = "kpo_fpga_nkM4-nk100_i0_d0_k0.txt";
FILE4 = "mat_nk100_nkm4_kprime.txt";
COMPARE_KP = true;
COMPARE_KC = false;

kc1 = importdata(FILE1);
kc2 = importdata(FILE2);
kp1 = importdata(FILE3);
kp2 = importdata(FILE4);

% Compare KP (requires same grid sizes)
if COMPARE_KP
    rmse = sqrt(mean(kp1-kp2).^2);
    fprintf("[kp] rmse: %15.9f\n", rmse);
    
    mean_kp1 = mean(kp1);
    mean_kp2 = mean(kp2);
    fprintf("Mean of kp1 = %15.9f\n", mean_kp1);
    fprintf("Mean of kp2 = %15.9f\n", mean_kp2);
    
    std_kp1 = std(kp1);
    std_kp2 = std(kp2);
    fprintf("Std of kp1 = %15.9f\n", std_kp1);
    fprintf("Std of kp2 = %15.9f\n", std_kp2);

    kp_diff_mean = mean((abs(kp1 - kp2)./kp2)*100, 'omitnan');
    fprintf("absolute mean difference = %20.12f\n", kp_diff_mean);

    kp_diff_max = max((abs(kp1 - kp2)./kp2)*100);
    fprintf("absolute maximum std difference = %15.9f\n", kp_diff_max);

    fprintf("median kp1 = %15.9f\n", median(kp1));
    fprintf("median kp2 = %15.9f\n", median(kp2));

    Q_kp1 = quantile(kp1, 3);
    Q_kp2 = quantile(kp2, 3);
    fprintf("Quantile of kp1 = %15.9f\n", Q_kp1);
    fprintf("Quantile of kp2 = %15.9f\n", Q_kp2);

    [h, p, ks2stat] = kstest2(kp1, kp2);
    if h == 0 % do not reject null
        hstr = "Do not reject null == same";
    else % reject null
        hstr = "Reject null == same";
    end
    fprintf("[kp] Kolmogorov-Smirnov h: %s, p: %.g, ks2stat: %.g\n", ...
        hstr, p, ks2stat);
end

% Compare kcross
if COMPARE_KC
    rmse = sqrt(mean(kc1-kc2).^2);
    fprintf("[kc] rmse: %f\n", rmse);
    
    fprintf("Mean of kc1 = %15.9f\n", mean(kc1));
    fprintf("Mean of kc2 = %15.9f\n", mean(kc2));
    
    fprintf("Std of kc1 = %15.9f\n", std(kc1));
    fprintf("Std of kc2 = %15.9f\n", std(kc2));
    
    C = (abs(kc1 - kc2)./kc2)*100;
    D = mean(C);
    fprintf("absolute mean difference = %15.9f\n", D);

    E = max(C);
    fprintf("absolute maximum std difference = %15.9f\n", E);

    fprintf("median kc1 = %15.9f\n", median(kc1));
    fprintf("median kc2 = %15.9f\n", median(kc2));

    Q_kc1 = quantile(kc1, 3);
    Q_kc2 = quantile(kc2, 3);
    fprintf("Quantile of kc1 = %15.9f\n", Q_kc1);
    fprintf("Quantile of kc2 = %15.9f\n", Q_kc2);

    [h, p, ks2stat] = kstest2(kc1, kc2);
    if h == 0 % do not reject null
        hstr = "Do not reject null == same";
    else % reject null
        hstr = "Reject null == same";
    end
    fprintf("[kc] Kolmogorov-Smirnov h: %s, p: %.g, ks2stat: %.g\n", ...
            hstr, p, ks2stat);
end
% Compare kmts
% rmse = sqrt(mean(kmts1-kmts2).^2);
% fprintf("[kmts] rmse: %f\n", rmse);
