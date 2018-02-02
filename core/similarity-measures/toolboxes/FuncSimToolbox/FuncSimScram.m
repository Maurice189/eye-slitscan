function [path,pathR]=FuncSimScram(path)
%FuncSimScram: Scrambles observed values across functional units
Us=unique(path(:,1)); %ordered units
RandUs=sortrows([rand(size(Us,1),1),Us],1);RandUs(:,1)=[]; %randomly sorted units
while min(abs(RandUs(:,1)-Us))==0 %while any unit at old place
    ind=find(RandUs(:,1)-Us==0); %indices of all same placed units
    RandUsNew=RandUs;
    if length(ind)==1 && ind~=1 %if one value at old place and not the first
        RandUsNew(ind,:)=RandUs(ind-1,:);RandUsNew(ind-1,:)=RandUs(ind,:);RandUs=RandUsNew;
        %exchange old placed value with previous value
    elseif length(ind)==1 && ind==1 %if one value at old place and it is the first
        RandUsNew(ind,:)=RandUs(ind+1,:);RandUsNew(ind+1,:)=RandUs(ind,:);RandUs=RandUsNew;
        %exchange old placed value with next value
    elseif length(ind)>1
        %if more than one value at old place
        RandUsNew(ind(1:floor(length(ind)/2)),:)=RandUs(ind(ceil(length(ind)/2)+1:end),:);
        RandUsNew(ind(ceil(length(ind)/2)+1:end),:)=RandUs(ind(1:floor(length(ind)/2)),:);
        %exchange first half with last half of same placed units
        RandUs=RandUsNew;
    end
end
pathR=path;pathR(:,7)=pathR(:,1);
for n=1:max(Us)
    pathR(pathR(:,1)==RandUs(n),1)=n+max(Us);
end
pathR=sortrows([pathR(:,1)-max(Us),pathR(:,2:end)],1);
end