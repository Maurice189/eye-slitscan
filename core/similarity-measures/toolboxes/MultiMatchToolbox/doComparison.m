function rv=doComparison(data1,data2)
%S = doComparison(fixationData1,fixationData2)
%pass in two n x 3 fixation arrays and do a MultiMatch comparison
%returns S, a 5 item column vector with similarity values for each
%dimension

% Parameters for the proposed algorithm
sz = [1024 768];%changed to size from JOV experiment
%CHANGE GLOBAL THRESH ACCORDINGLY?
globalThreshold = 0.1*sqrt(sz(1)^2+sz(2)^2); % Diagonal/10 ~ 5 degrees.
directionThreshold = 45;
durationThreshold = inf;

if size(data1,1)<3 && size(data2,1)<3 %return NaN if scanpaths are too short
rv=NaN;
return
else
    bx1=data1(:,1)>0 & data1(:,1) <1024;
    bx2=data2(:,1)>0 & data2(:,1) <1024;
    by1=data1(:,2)>0 & data1(:,2) <768;
    by2=data2(:,2)>0 & data2(:,2) <768;
    
    %return NaN if any fixations go out of bounds
    %TOO CONSERVATIVE?
    if all(bx1) && all(bx2) && all(by1) && all(by2)
    
    %transform into scanpath structure
    sp1 = generateStructureArrayScanpath(data1);
    sp2 = generateStructureArrayScanpath(data2);

    %--------------------------------------------------------------------------
    % Compare scanpaths 
    %--------------------------------------------------------------------------

    % ... using the proposed method
    [sp1,sp2,rv,path,M_assignment,ki,kj] = mainProposed(sp1,sp2,globalThreshold,directionThreshold,durationThreshold,sz);

    %print results to the prompt
    disp(['Vector similarity = ',num2str(rv(1))])
    disp(['Direction similarity = ',num2str(rv(2))])
    disp(['Length similarity = ',num2str(rv(3))])
    disp(['Position similarity = ',num2str(rv(4))])
    disp(['Duration similarity = ',num2str(rv(5))])
    
    else
        rv=NaN;
    end
end




