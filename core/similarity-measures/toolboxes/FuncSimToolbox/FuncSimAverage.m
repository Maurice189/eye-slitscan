function dataMean=FuncSimAverage(path1,path2)
%FuncSimAverage: Averages fixation values within functional units
dataMean=[unique(path1(:,1)),... %functional units
    accumarray(path1(:,1),path1(:,2),[],@nanmean),... %mean x1
    accumarray(path1(:,1),path1(:,3),[],@nanmean),... %mean y1
    accumarray(path2(:,1),path2(:,2),[],@nanmean),... %mean x2
    accumarray(path2(:,1),path2(:,3),[],@nanmean),... %mean y2
    accumarray(path1(:,1),path1(:,4),[],@nanmean),... %mean dur1
    accumarray(path2(:,1),path2(:,4),[],@nanmean),... %mean dur2
    accumarray(path1(:,1),path1(:,5),[],@nanmean),... %mean length1
    accumarray(path2(:,1),path2(:,5),[],@nanmean),... %mean length2
    atan2d(accumarray(path1(:,1),sind(path1(:,6)),[],@nanmean),...
    accumarray(path1(:,1),cosd(path1(:,6)),[],@nanmean)),... %mean direction1
    atan2d(accumarray(path2(:,1),sind(path2(:,6)),[],@nanmean),...
    accumarray(path2(:,1),cosd(path2(:,6)),[],@nanmean))... %mean direction2
    ];
if size(path2,2)==7
    dataMean=[dataMean,accumarray(path2(:,1),path2(:,7),[],@nanmean)]; %random units
end
end