function angle_diff = calAngularDifferences(theta1,theta2,path,M_assignment)

angle_diff = zeros(1,length(path));

% Compare scanpaths 
for k = 1:length(path)
    [i,j]=find(M_assignment==path(k));
    
    spT = [theta1(i) theta2(j)];
    spT(spT < 0) = pi + (pi + spT(spT < 0));
    spT = abs(diff(spT));
    spT(spT > pi) = 2*pi-spT(spT > pi) ;
   
%     angle_diff(k) = abs(theta1(i) - theta2(j)); % Wrong!
    angle_diff(k) = spT; 
end

