function dur_diff = calDurationDifferences(dur1,dur2,path,M_assignment)

dur_diff = zeros(1,length(path));
% Compare scanpaths 
for k = 1:length(path)
    [i,j]=find(M_assignment==path(k));
%     dur1(i),dur2(j)
    dur_diff(k) = abs(dur1(i) - dur2(j))/max(dur1(i),dur2(j));
end












