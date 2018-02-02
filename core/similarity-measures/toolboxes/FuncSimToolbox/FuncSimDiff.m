function data=FuncSimDiff(data)        
%FuncSimDiff: Calculates differences across aligned lines
%in location, duration, length, and direction
data(:,end+1:end+4)=...
    [sqrt((data(:,2)-data(:,4)).^2+(data(:,3)-data(:,5)).^2),... %Euclidean Location distances
    abs(data(:,6)-data(:,7)),... %Duration differences
    abs(data(:,8)-data(:,9)),... %Length differences
    abs(data(:,10)-data(:,11)),... %Direction differences
    ];
data(data(:,end)>180,end)=360-data(data(:,end)>180,end); %correct direction differences over 180°
end
