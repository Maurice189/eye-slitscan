%FuncSim_Instructions

%% Sets input for FuncSim
load FuncSimExampleData.mat; %scanpaths data
    %4 example paths with one fixation in each row
    %column 1 = units, column 2 = x column 3 = y, column 4 = fixation duration

%% Define alignment method
method='average'; %alignment method
    %'average' for aligning mean values within functional units
    %'reldur' for aligning fixations according to relative fixation duration within functional units

%% Calculates scanpaths similarities
[dataDiff,diff]=FuncSimCore(path1,path2,method); %FuncSim comparison
    %compares path1 and path2 as well as path1 and its scrambled derivative
    %in four dimensions(location, duration, length, direction)
    %and stores the results(diff) and the alignment matrix(dataDiff)