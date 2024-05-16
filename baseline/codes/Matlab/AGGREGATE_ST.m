% The program for the article "Solving the incomplete markets model with
% aggregate uncertainty using the Krusell-Smith algorithm" from the special 
% JEDC issue edited by Den Haan, Judd and Juillard (2008)  
%
% Written by Lilia Maliar, Serguei Maliar and Fernando Valli (2008)

function [kmts,kcross]  = AGGREGATE_ST(interp_type, T,idshock,agshock,km_max,km_min,kprime,km,k,epsilon2,k_min,k_max,kcross,a2);

kmts=zeros(T,1);         % a time series of the mean of capital distribution 
errcount = 0;
for t=1:T
  
   kmts(t)=mean(kcross); % find the t-th observation of kmts by computing 
                         % the mean of the t-th period cross-sectional 
                         % distribution of capital
   kmts(t)=kmts(t)*(kmts(t)>=km_min)*(kmts(t)<=km_max)+km_min*(kmts(t)<km_min)+km_max*(kmts(t)>km_max); % restrict kmts to be within [km_min, km_max]
   
   % To find kmts(t+1), we should compute a new cross-sectional distribution 
   % at t+1. For this purpose, we first find kprime by interpolation for 
   % realized kmts(t) and agshock(t) (kprimet below) and then use it to 
   % compute new kcross by interpolation (kcrossn below) given the previous 
   % kcross and the realized idshock(t)
   
   %% ORIGINAL 4D
   kprimet4=interpn(k,km,a2,epsilon2,kprime,k, kmts(t),agshock(t),epsilon2,interp_type);
      % a four-dimensional capital function at time t is obtained by fixing
      % known kmts(t) and agshock (t)
   
      
   kprimet=squeeze(kprimet4); % the size of kprimet4 is ngridk*1*1*nstates_id; 
                              % in kprimet, all singleton dimensions (i.e.,
                              % those with only one column per page) are removed
   
   %% 2x 2D
   %t2_11 = interpn(k,km,kprime(:,:,1,1),k,kmts(t),interp_type);
   %t2_12 = interpn(k,km,kprime(:,:,1,2),k,kmts(t),interp_type);
   %kprimet = [t2_11 t2_12];
   
   %t2_11 = interpn(k,km,kprime(:,:,agshock(t),1),k,kmts(t),interp_type);
   %t2_12 = interpn(k,km,kprime(:,:,agshock(t),2),k,kmts(t),interp_type);
   %tt = [t2_11 t2_12];
   %dtt = sum(kprimet(:) == tt(:));
   %sqt = kprimet;
   %xx = sqt-tt;
   %xx(xx ~= 0);
   %maxdiff = max(abs(xx(:)));
   
   %if (dtt ~= 200 && maxdiff > 1e-15)
       
%        t2_21 = interpn(k,km,kprime(:,:,2,1),k,kmts(t),interp_type);
%        t2_22 = interpn(k,km,kprime(:,:,2,2),k,kmts(t),interp_type);
%        tta = [t2_11 t2_21];
%        ttb = [t2_11 t2_22];
%        ttc = [t2_12 t2_21];
%        ttd = [t2_12 t2_22];
%        tte = [t2_21 t2_22];
%        ttf = [t2_12 t2_11];
%        
%        sa = sum(kprimet(:) == tta(:));
%        sb = sum(kprimet(:) == ttb(:));
%        sc = sum(kprimet(:) == ttc(:));
%        sd = sum(kprimet(:) == ttd(:));
%        se = sum(kprimet(:) == tte(:));
%        sf = sum(kprimet(:) == ttf(:));
       
%       disp_str = sprintf("Error at %d [%d] [a:%d] (others: %d %d %d %d %d %d)", t, dtt, agshock(t), sa, sb, sc, sd, se, sf);
       
       %disp_str = sprintf("Error at %d [%d] [a:%d] maxdiff: %g ", t, dtt, agshock(t), maxdiff);
       %disp(disp_str); 
       %errcount = errcount + 1;
   %end
   
   kcrossn=interpn(k,epsilon2,kprimet,kcross,idshock(t,:),interp_type); 
                              % given kcross and idiosyncratic shocks we
                              %compute kcrossn
                              
   kcrossn=kcrossn.*(kcrossn>=k_min).*(kcrossn<=k_max)+k_min*(kcrossn<k_min)+k_max*(kcrossn>k_max); % restrict kcross to be within [k_min, k_max]
   kcross=kcrossn;
end

disp(errcount);

end