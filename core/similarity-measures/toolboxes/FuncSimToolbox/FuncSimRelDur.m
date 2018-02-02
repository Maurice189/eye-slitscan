function dataRelDur=FuncSimRelDur(path1,path2)
%FuncSimRelDur: Aligns fixations within functional units according ot relative fixation durations
dataRelDur=[path1(1,:),path2(1,2:end)]; 
n=[1,1]; %indices of currently observed fixation of path1 & path2
DurCum=[0,0]; %cumulative duration of currently observed functional unit
while true
    if (path1(n(1),4)+DurCum(1))/nansum(path1(path1(:,1)==path1(n(1),1),4))...
            <(path2(n(2),4)+DurCum(2))/nansum(path2(path2(:,1)==path2(n(2),1),4))
        %if currently observed path1 fixation has smaller relative
        %duration than currently observed path2 fixation
        dataRelDur=[dataRelDur;[path1(n(1)+1,1),...
            path1(n(1)+1,2:6),path2(n(2),2:end)]];
        %double currently observed path1 fixation
        DurCum=DurCum+[path1(n(1),4),0];n=n+[1,0]; %update indices and cumulative duration
    elseif (path1(n(1),4)+DurCum(1))/nansum(path1(path1(:,1)==path1(n(1),1),4))...
            >(path2(n(2),4)+DurCum(2))/nansum(path2(path2(:,1)==path2(n(2),1),4))
        %if currently observed path2 fixation has smaller relative
        %duration than currently observed path1 fixation
        dataRelDur=[dataRelDur;mean(path1(n(1),1),path2(n(2)+1,1)),...
            path1(n(1),2:6),path2(n(2)+1,2:end)];
        %double currently observed path1 fixation
        DurCum=DurCum+[0,path2(n(2),4)];n=n+[0,1]; %update indices and cumulative duration
    else
        if ~isempty(find(path1(:,1)==dataRelDur(end,1)+1,1))
            %if all fixations of one functional unit aligned
            DurCum=[0,0]; %reset cumulative duration
            n=[find(path1(:,1)==dataRelDur(end,1)+1,1),...
                find(path2(:,1)==dataRelDur(end,1)+1,1)];
                %find indices of first fixation of next functional unit 
            dataRelDur=[dataRelDur;path1(n(1),1),...
                path1(n(1),2:6),path2(n(2),2:end)];
                %insert first fixation of next functional unit to aligned data
        else
            break
        end
    end
end
dataRelDur=dataRelDur(:,[1:3,7,8,4,9,5,10,6,11:end]);
end