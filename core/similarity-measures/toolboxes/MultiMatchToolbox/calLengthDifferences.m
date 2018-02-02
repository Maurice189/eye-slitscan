function length_diff = calLengthDifferences(len1,len2,path,M_assignment)

length_diff = zeros(1,length(path));

% Compare scanpaths 
for k = 1:length(path)
    [i,j]=find(M_assignment==path(k));
    length_diff(k) = abs(len1(i) - len2(j));
end




