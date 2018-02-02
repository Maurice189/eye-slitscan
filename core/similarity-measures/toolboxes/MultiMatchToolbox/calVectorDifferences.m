function M = calVectorDifferences(x1,x2,y1,y2)

% Calculate vector differences (location as well?)
for k = 1:length(x1)
    x_diff = abs(x1(k)*ones(1,length(x2)) - x2);
    y_diff = abs(y1(k)*ones(1,length(y2)) - y2);
    M(k,:) = sqrt(x_diff.^2 + y_diff.^2);
end



