%%
close all

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% DEATH %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
distrexp = EXP2PROP.Stats_.death_cum/1000;
cdfexp   = EXP2PROP.Stats_.cdfhist;
figure
ecdfhist(cdfexp,distrexp,20)
[nexp,cexp] = ecdfhist(cdfexp,distrexp,20);
nexp = nexp/sum(nexp);

distrgre = HAZ2PROP.Stats_.death_cum/1000;
cdfgre   = HAZ2PROP.Stats_.cdfhist;
figure 
ecdfhist(cdfgre,distrgre,20)
[ngre,cgre] = ecdfhist(cdfgre,distrgre,20);
ngre = ngre/sum(ngre);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE PAPER
Figure_.legtitle      = '\textbf{Cumulative Deaths Distribution}';
Figure_.xlabel = '\textbf{Cumulative Deaths (in Thousands)}';
Figure_.bin1 = cgre;Figure_.bar1 = ngre;Figure_.color1 = [0 0.7 0.9];Figure_.BARWIDTH1 = 0.8;
Figure_.bin2 = cexp;Figure_.bar2 = nexp;Figure_.color2 = [0.3 0.8 0.7];Figure_.BARWIDTH2 = 0.7;
Figure_.leg1 = ['Greenwich:',' Mean (',num2str(HAZ2PROP.Stats_.death_cumE/1000,'%2.1f'),')',' Std (',num2str(HAZ2PROP.Stats_.death_std_cum/1000,'%2.1f'),')'];
Figure_.leg2 = ['Exponential:',' Mean (',num2str(EXP2PROP.Stats_.death_cumE/1000,'%2.1f'),')',' Std (',num2str(EXP2PROP.Stats_.death_std_cum/1000,'%2.1f'),')'];
Figure_.legy1='\textbf{Frequency}';Figure_.legy1color = [0 0 0];
Figure_.legendposition = 'northeast';
Figure_.outputdir = strcat(outputdir,'figures/');
Figure_.filename  = 'Figure4b.png';
Figurebar2ax1(Figure_);
clearvars Figure_
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% FIGURE PAPER
