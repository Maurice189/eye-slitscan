function [dataDiff,diff]=FuncSimCore(path1,path2,method)
%ScanPathSIM: Calculates the similarity of scanpaths
%using the sub-action sequenced mean linear distance method
%see Foerster & Schneider (2013)
% INPUT:  path1 & path2 = functional unit IDs in 1st, x in 2nd, y in 3nd column
%                         unit could be sub-action, timestamps, fixation index, or constant
%         method        = alignment method
%                         'average' for aligning mean values within functional units
%                         'reldur' for aligning events (e.g., fixations) within functional units according to their relative duration within the unit
% OUTPUT: dataDiff   = labeled cell matrix with aligned path1 and path2,
%                      path1 and its unit scrambled derivative pathR 
%         diff       = struct of calculated BetweenPath and
%                      RandomBaseline Difference (unit-scrambled) scores for chosen method
%
% RMF, 2016

%% Tests if all variables are preset in the right way
if ~ischar(method)||isempty(method)||(strcmp(method,'average')&&strcmp(method,'reldur'))
    errmsg=sprintf('method is empty or in wrong format');
    err=MException('FuncSim:inputVar:methodWrong',errmsg);
    throw(err)
end
if ~isnumeric(path1)||~isnumeric(path2)||isempty(path1)||isempty(path2)||...
        size(path1,1)<2||size(path2,1)<2||~size(path1,2)==4||~size(path2,2)==4||...
        size(unique(path1(:,1)),1)<2||size(unique(path2(:,1)),1)<2   
    errmsg=sprintf('too few data points or units, or data in wrong format');
    err=MException('FuncSim:inputVar:dataWrong',errmsg);
    throw(err)
end

%% Tests double entries and mark them
ind1=find(all(path1(2:end,2:end)-path1(1:end-1,2:end)==0,2)); %indices of double entries in path1
ind2=find(all(path2(2:end,2:end)-path2(1:end-1,2:end)==0,2)); %indices of double entries in path2

%% Calculates all dimensions
path1=[path1,...
    [sqrt((path1(2:end,2)-path1(1:end-1,2)).^2+(path1(2:end,3)-path1(1:end-1,3)).^2);NaN],... %lengths of path1
    [atan2d(path1(2:end,3)-path1(1:end-1,3),path1(2:end,2)-path1(1:end-1,2));NaN]... %directions of path1
    ];
if ~isempty(ind1)
    for n=size(ind1):-1:1
        path1(ind1(n),5:6)=path1(ind1(n)+1,5:6);
            %substitue length and direction of doubled fixations with right value
    end
end

path2=[path2,...
    [sqrt((path2(2:end,2)-path2(1:end-1,2)).^2+(path2(2:end,3)-path2(1:end-1,3)).^2);NaN],... %lengths of path2
    [atan2d(path2(2:end,3)-path2(1:end-1,3),path2(2:end,2)-path2(1:end-1,2));NaN]... %directions of path2
    ];
if ~isempty(ind2)
    for n=size(ind2):-1:1
        path2(ind2(n),5:6)=path2(ind2(n)+1,5:6);
            %substitue length and direction of doubled fixations with right value
    end
end

%% Inserts lacking units with NaN rows
uns=1:max([unique(path1(:,1));unique(path2(:,1))]); %complete unit vector
un1=unique(path1(:,1));un2=unique(path2(:,1)); %unit vectors of path1 and path2
lacking=setdiff(uns,un1); %lacking units in path1
path1(end+1:end+size(lacking,2),:)=[lacking',NaN(size(lacking,2),5)];
    %insert lacking units with NaN characteristics
lacking=setdiff(uns,un2); %lacking untis in path2
path2(end+1:end+size(lacking,2),:)=[lacking',NaN(size(lacking,2),5)];
    %insert lacking units with NaN characteristics

%% Sort row according to functional unit order
path1=sortrows(path1,1); path2=sortrows(path2,1);

%% Scrambles functional units
[path1,pathR]=FuncSimScram(path1);

%% Creates result cell array with header
header='Units X1 Y1 X2 Y2 Dur1 Dur2 Len1 Len2 Dir1 Dir2'; %observed values header
header=sprintf('%s%s',header,' BPDloc BPDdur BPDlen BPDdir'); %BetweenPathWithinUnitDifferences BPD header
header=sprintf('%s%s',header,' RandomUnits X1scram Y1scram Dur1scram Len1scram Dir1scram'); %scrambled values header
header=sprintf('%s%s',header,' RBDloc RBDdur RBDlen RBDdir'); %WithinPathBetweenUnitRandomBaselineDifferences RBD header
h=textscan(header,'%s');
if strcmp(method,'average')
    BPD=FuncSimDiff(FuncSimAverage(path1,path2));
    RBD=FuncSimDiff(FuncSimAverage(path1,pathR));
elseif strcmp(method,'reldur')
    BPD=FuncSimDiff(FuncSimRelDur(path1,path2));
    RBD=FuncSimDiff(FuncSimRelDur(path1,pathR));
    sizeDiff=size(BPD,1)-size(RBD,1);
    if sizeDiff<0
        BPD(end+1:end+abs(sizeDiff),:)=nan;
    elseif sizeDiff>0
        RBD(end+1:end+abs(sizeDiff),:)=nan;
    end
end
dataDiff=[h{1}';num2cell(BPD),num2cell(RBD(:,[12,4,5,7,9,11,13,14,15,16]))];

%% Creates struct with mean distance values
diff=struct(...
    'Method',method,...
    'BPDloc',nanmean(cell2mat(dataDiff(2:end,12))),'RBDloc',nanmean(cell2mat(dataDiff(2:end,22))),...
    'BPDdur',nanmean(cell2mat(dataDiff(2:end,13))),'RBDdur',nanmean(abs(cell2mat(dataDiff(2:end,23)))),...
    'BPDlen',nanmean(cell2mat(dataDiff(2:end,14))),'RBDlen',nanmean(abs(cell2mat(dataDiff(2:end,24)))),...
    'BPDdir',nanmean(cell2mat(dataDiff(2:end,15))),'RBDdir',nanmean(abs(cell2mat(dataDiff(2:end,25))))...
    );
disp(diff);

end