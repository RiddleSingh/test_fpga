%% Create Costs and Energy Savings
clear
clc
close all

%filename = '/Users/alessandroperi/GoogleDrive/Projects/WorkStudy/FPGA/matlab/results1.txt';
%filename = '/Users/alessandroperi/GoogleDrive/Projects/WorkStudy/FPGA/matlab/results1/fpgaI-nKM4-nk100-time-tot.txt';

% INPUTFOLDER = '/Users/lucasladenburger/Desktop/2021-Fall/Cost-energy-calc/input/';
% OUTPUTFOLDER = '/Users/lucasladenburger/Desktop/2021-Fall/Cost-energy-calc/results/';

devicelist = {'cpu-cores','fpga'};
KMlist = {'4','8'};
klist = {'100','200','300'};

fpgaIpower = 33;
fpgaSingleKernel = 22;

for iKM=1:length(KMlist)
    NKM = KMlist{iKM};
    for ik=1:length(klist)
        Nk = klist{ik};
        for id=1:length(devicelist)
            chip = devicelist{id};
            switch chip
                case 'cpu-cores'
                    
                    corelist = {'1','8','48'};
                    
                case 'fpga'
                    corelist = {'I','II','III'};
                    
            end
            
            for inumberofchips=1:length(corelist)
                
                device = strcat(chip,corelist{inumberofchips});
                
                %Collect time
                filename = strcat(INPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-time-tot.txt');
                fileID = fopen(filename,'r');
                time(id,inumberofchips,iKM,ik) = fscanf(fileID,'%f');
                fclose(fileID);
                clearvars filename fileID;
                % Save Time Trimmed

                filename = strcat(OUTPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-time-tot.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',time(id,inumberofchips,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
                
                
                %%%%%%%%%%%%%%%%%%%%%%%
                switch device 
                    case 'cpu-cores1'
                        price = 0.119;
                    case 'cpu-cores8'
                        price = 0.952;
                    case 'cpu-cores48'
                        price = 5.712;
                    case 'fpgaI'
                        price = 1.65;
                    case 'fpgaII'
                        price = 3.30;
                    case 'fpgaIII'
                        price = 13.2;
                        
                end
                %%%%%%%%%%%%%%%%%%%%%%%
                cost(id,inumberofchips,iKM,ik) = time(id,inumberofchips,iKM,ik)/ 3600 * price;
                % Save Cost Trimmed
                filename = strcat(OUTPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-cost.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',cost(id,inumberofchips,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
                %%%%%%%%%%%%%%%%%%%%%%%
                switch device 
                    case 'cpu-cores1'
                        watts = 8;  %(2/96)*384
                    case 'cpu-cores8'
                        watts = 64;  %(16/96)*384
                    case 'cpu-cores48'
                        watts = 384;    %TDP for processor
                    case 'fpgaI'
                        watts = 33;
                    case 'fpgaII'
                        watts = 66;
                    case 'fpgaIII'
                        watts = 264;
                        
                end
                energy(id,inumberofchips,iKM,ik) = time(id,inumberofchips,iKM,ik) * watts;
                %Save Energy Trimmed
                filename = strcat(OUTPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-energy.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',energy(id,inumberofchips,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
            end
            
        end
    end
end

%{
%%Table 10
for id=1:length(devicelist)
    chip = devicelist{id};
            switch chip
                case 'cpu-cores'
                    
                    corelist = {'1','8','48'};
                    
                case 'fpga'
                    corelist = {'I','II','III'};     
            end
    for inumberofchips=1:length(corelist)
                
                device = strcat(chip,corelist{inumberofchips});
                
                %Collect time
                filename = strcat(INPUTFOLDER,device,'-nKM4-nk100-time-tot.txt');
                fileID = fopen(filename,'r');
                time10(id,inumberofchips) = fscanf(fileID,'%f');
                fclose(fileID);
                clearvars filename fileID;
                % Save Time Trimmed
                filename = strcat(OUTPUTFOLDER,device,'-nKM4-nk100-time-tot.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',time10(id,inumberofchips));
                fclose(fileID);
                clearvars filename fileID;
                
        switch device 
                    case 'cpu-cores1'
                        price = 0.119;
                    case 'cpu-cores8'
                        price = 0.952;
                    case 'cpu-cores48'
                        price = 5.712;
                    case 'fpgaI'
                        price = 1.65;
                    case 'fpgaII'
                        price = 3.30;
                    case 'fpgaIII'
                        price = 13.2;                
        end
        cost10(id,inumberofchips) = time10(id,inumberofchips)/ 3600 * price;
        % Save Cost Trimmed
        filename = strcat(OUTPUTFOLDER,device,'-nKM4-nk100-cost.txt');
        fileID = fopen(filename,'w');
        fprintf(fileID,'%4.2f',cost10(id,inumberofchips));
        fclose(fileID);
        clearvars filename fileID;
        %Save energy trimmed
        switch device 
                    case 'cpu-cores1'
                        watts = 0;
                    case 'cpu-cores8'
                        watts = 0;
                    case 'cpu-cores48'
                        watts = 0;
                    case 'fpgaI'
                        watts = 18;
                    case 'fpgaII'
                        watts = 36;
                    case 'fpgaIII'
                        watts = 144;
                        
        end
                energy10(id,inumberofchips) = time10(id,inumberofchips) * watts;
                %Save Energy Trimmed
                filename = strcat(OUTPUTFOLDER,device,'-nKM4-nk100-energy.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',energy10(id,inumberofchips));
                fclose(fileID);
                clearvars filename fileID;
    end
end

%%Table11
for id=1:length(devicelist)
    chip = devicelist{id};
            switch chip
                case 'cpu-cores'
                    
                    corelist = {'8','48'};
                    
                case 'fpga'
                    corelist = {'I','II','III'};     
            end
    for inumberofchips=1:length(corelist)
        for iKM=1:length(KMlist)
            for ik=1:length(klist)
                Nk = klist{ik};
                NKM = KMlist{iKM};
                device = strcat(chip,corelist{inumberofchips});
               %Collect time
                filename = strcat(INPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-time-tot.txt');
                fileID = fopen(filename,'r');
                time11(id,inumberofchips,iKM,ik) = fscanf(fileID,'%f');
                fclose(fileID);
                clearvars filename fileID;
                % Save Time Trimmed

                filename = strcat(OUTPUTFOLDER,device,'-nKM',NKM,'-nk',Nk,'-time-tot.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',time11(id,inumberofchips,iKM,ik));
                fclose(fileID);
                clearvars filename fileID; 
            end
        end
    end
end

%%Table 3
for id=1:length(devicelist)
    chip = devicelist{id};
            switch chip
                case 'cpu-cores'
                    
                    corelist = {'1','8','48'};
                    
                case 'fpga'
                    corelist = {'I','II','III'};     
            end
    for inumberofchips=1:length(corelist)
                
                device = strcat(chip,corelist{inumberofchips});
                
                %Collect time
                filename = strcat(INPUTFOLDER,device,'-nKM4-nk100-time-tot.txt');
                fileID = fopen(filename,'r');
                time3(id,inumberofchips) = fscanf(fileID,'%f');
                fclose(fileID);
                clearvars filename fileID;
                %speedup
                speedup3(id,inumberofchips) = round(time(1,icpu,iKM,ik)/time(2,ifpga,iKM,ik));
                filename = strcat(OUTPUTFOLDER,devicelistcpu{icpu},'-',devicelistfpga{ifpga},'-nKM',NKM,'-nk',Nk,'-speedup','.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%d',speedup(icpu,ifpga,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
    end
end
%}
devicelistcpu = {'cpu-cores1','cpu-cores8','cpu-cores48'};
devicelistfpga = {'fpgaI','fpgaII','fpgaIII'};

for iKM=1:length(KMlist)
    NKM = KMlist{iKM};
    for ik=1:length(klist)
        Nk = klist{ik};
        for icpu=1:length(devicelistcpu)
            for ifpga = 1:length(devicelistfpga)
                % Speedup
                speedup(icpu,ifpga,iKM,ik) = (time(1,icpu,iKM,ik)/time(2,ifpga,iKM,ik));
                filename = strcat(OUTPUTFOLDER,devicelistcpu{icpu},'-',devicelistfpga{ifpga},'-nKM',NKM,'-nk',Nk,'-speedup','.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',speedup(icpu,ifpga,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
                
                % Costs Savings
                costsavings(icpu,ifpga,iKM,ik) = cost(2,ifpga,iKM,ik)/cost(1,icpu,iKM,ik)*100;
                filename = strcat(OUTPUTFOLDER,devicelistcpu{icpu},'-',devicelistfpga{ifpga},'-nKM',NKM,'-nk',Nk,'-costsavings','.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',costsavings(icpu,ifpga,iKM,ik));
                fclose(fileID);
                clearvars filename fileID;
                
                %Energy Savings
                energysavings(icpu,ifpga,iKM,ik) = energy(2,ifpga,iKM,ik)/energy(1,icpu,iKM,ik)*100;
                filename = strcat(OUTPUTFOLDER,devicelistcpu{icpu},'-',devicelistfpga{ifpga},'-nKM',NKM,'-nk',Nk,'-energysavings','.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',energysavings(icpu,ifpga,iKM,ik));
                %fprintf(fileID,'%i',1); %temporary until we get correct energy measures
                fclose(fileID);
                clearvars filename fileID;
            end
        end
    end
end




%%Table 4
    filename = strcat(INPUTFOLDER,'fpgaI-knl-1-nKM4-nk100-time-tot.txt');
    fileID = fopen(filename,'r');
    time4 = fscanf(fileID,'%f');
    fclose(fileID);
    clearvars filename fileID;
                       
% Save Time Trimmed
    filename = strcat(OUTPUTFOLDER,'fpgaI-knl-1-nKM4-nk100-time-tot.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',time4);
    fclose(fileID);
    clearvars filename fileID;

%Speedup
    speedup4 = (time(1,1,1,1)/time4);
%     speedup4 = round(time(1,1,1,1)/time4);
    filename = strcat(OUTPUTFOLDER,'cpu-cores1-fpgaI-knl-1-nKM4-nk100-speedup.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',speedup4);
    fclose(fileID);
    clearvars filename fileID;
    
%Cost savings
    cost4 = time4/3600 * 1.65;
    filename = strcat(OUTPUTFOLDER,'fpgaI-knl-1-nKM4-nk100-cost.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',cost4);
    fclose(fileID);
    clearvars filename fileID;
    costsavings4 = cost4/cost(1,1,1,1)*100;
                filename = strcat(OUTPUTFOLDER,'cpu-cores1-fpgaI-knl-1-nKM4-nk100-costsavings.txt');
                fileID = fopen(filename,'w');
                fprintf(fileID,'%4.2f',costsavings4);
                fclose(fileID);
                clearvars filename fileID;
                
%Energy savings
%     energy4 = time4*fpgaIpower;
    energy4 = time4*fpgaSingleKernel;
    filename = strcat(OUTPUTFOLDER, 'fpgaI-knl-1-nKM4-nk100-energy.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',energy4);
    fclose(fileID);
    clearvars filename fileID;
%     energysavings4 = energy(1,1,1,1)/energy4*100;
    energysavings4 = 100*energy4/energy(1,1,1,1);
    filename = strcat(OUTPUTFOLDER,'cpu-cores1-fpgaI-knl-1-nKM4-nk100-energysavings.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',energysavings4);
%     fprintf(fileID,'%i',1); %temporary until we get correct energy measures
    fclose(fileID);
    clearvars filename fileID;
    
%%Table 6
devices = {'cpuI-base-','fpgaI-base-'};
for idevice=1:length(devices)
   device6=devices(idevice);
filename = strcat(INPUTFOLDER, devices{idevice},'knl-1-nKM4-nk100-time-tot.txt');
    fileID = fopen(filename,'r');
    time6(idevice) = fscanf(fileID,'%f');
    fclose(fileID);
    clearvars filename fileID;
%Save time trimmed
filename = strcat(OUTPUTFOLDER,devices{idevice},'knl-1-nKM4-nk100-time-tot.txt');
    fileID = fopen(filename,'w');
    fprintf(fileID,'%4.2f',time6(idevice));
    fclose(fileID);
    clearvars filename fileID;
end

%gridlist = {'nKM4-nk100','nKM8-nk300'};
% speedlist = {'base-knl-1','pip-knl-1','prec-knl-1','datpar-knl-1','datpar-knl-3'};
speedlist = {'base-knl-1','pip-knl-1','datpar-knl-1','datpar-knl-3'};
%for igrid=1:length(gridlist);
filename = strcat(INPUTFOLDER, 'cpuI-base-knl-1-nKM4-nk100-time-tot.txt');
fileID = fopen(filename,'r');
cputime8 = fscanf(fileID, '%f');
fclose(fileID);
clearvars filename fileID;
    for ispeed=1:length(speedlist);
        %save time
        speedmethod = speedlist(ispeed);
        %gridsize = gridlist{igrid};
        %filename = strcat(INPUTFOLDER,'fpgaI-',speedlist{ispeed},'-',gridlist{igrid},'-time-tot.txt');
        filename = strcat(INPUTFOLDER,'fpgaI-',speedlist{ispeed},'-nKM4-nk100','-time-tot.txt');
        fileID = fopen(filename,'r');
        %time8(igrid,ispeed) = fscanf(fileID,'%f');
        time8(ispeed) = fscanf(fileID, '%f');
        fclose(fileID);
        clearvars filename fileID;
        %if strcmp(speedmethod,speedlist{1})==1
         %   filename = strcat(OUTPUTFOLDER,'fpgaI-',speedlist{ispeed}, '-',gridlist{igrid},'-time-tot.txt');
          %  fileID = fopen(filename,'w');
           % fprintf(fileID,'%4.2f',time8(igrid));
            %fclose(fileID);
            %clearvars filename fileID;
        %end
        
    end
%end

%Speedup
%for igrid=1:length(gridlist);
    for ispeed=1:length(speedlist);
        %speedup8(igrid,ispeed) = round(time8(igrid,1)/time8(igrid,ispeed));
        speedup8(ispeed) = cputime8/time8(ispeed);
        %filename = strcat(OUTPUTFOLDER,'fpgaI-',speedlist{1},'-fpgaI-',speedlist{ispeed},'-',gridlist{igrid},'speedup.txt');
        filename = strcat(OUTPUTFOLDER,'cpuI-base-knl-1','-fpgaI-',speedlist{ispeed},'-nKM4-nk100-','speedup.txt');
        fileID = fopen(filename,'w');
        %fprintf(fileID,'%d',speedup8(igrid,ispeed));
        fprintf(fileID,'%4.2f',speedup8(ispeed));
        fclose(fileID);
        clearvars filename fileID;
    end
%end
speedinverse=time8(1)/cputime8;
filename = strcat(OUTPUTFOLDER,'cpuI-base-knl-1-fpgaI-base-knl-1-nKM4-nk100-speedupinverse.txt');
fileID = fopen(filename,'w');
fprintf(fileID, '%4.2f',speedinverse);
fclose(fileID);
clearvars filename fileID;



    
    
    
    
    
    
    
    






