%% Script which writes output files
kp_out = reshape(kprime, [1600, 1]);
idx=1;

for ii = 1:2
    for jj = 1:4
        for kk = 1:2
            for ll = 1:100
                kp_out(idx) = kprime(ll, jj, ii, kk);
                idx = idx + 1;
            end
        end
    end
end

%kp_out

r2bg = [R2bad; R2good];

writematrix(kp_out, './kprime.csv');
writematrix(kcross, './kcross.csv');
writematrix(r2bg, './r2bg.csv');
writematrix(B, './coeffs.csv');