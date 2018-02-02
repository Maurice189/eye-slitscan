function pos_diff = calPositionDifferences(x1,x2,y1,y2,path,M_assignment)

pos_diff = zeros(1,length(path));
% Compare scanpaths 
for k = 1:length(path)
    [i,j]=find(M_assignment==path(k));
%     pos_diff(k) = abs(x1(i) - x2(j)) + abs(y1(i) -  y2(j));
    pos_diff(k) = sqrt((x1(i) - x2(j))^2 + (y1(i) -  y2(j))^2);
end












