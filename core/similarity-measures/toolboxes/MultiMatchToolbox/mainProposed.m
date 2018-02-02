function [sp1,sp2,results,path,M_assignment,ki,kj] = mainProposed(sp1,sp2,T1,T2,Tdur,sz)

global debugMode


%--------------------------------------------------------------------------
% Simplify/compress scanpaths (find global scanpath)
%--------------------------------------------------------------------------


%--------------------------------------------------------------------------
% Compare scanpaths pairwise
%--------------------------------------------------------------------------
len_i = length(sp1);
len_j = length(sp2);

l = 1;
results = zeros(5,len_i*len_j);
for i  = 1:len_i
    for j  = 1:len_j
        
        % Simplify scanpaths
        sp1(i) = simplifyScanpath(sp1(i),T1,T2,Tdur);
        sp2(j) = simplifyScanpath(sp2(j),T1,T2,Tdur);

        % Vector differences
        M = calVectorDifferences(sp1(i).saccade.lenx,sp2(j).saccade.lenx,sp1(i).saccade.leny,sp2(j).saccade.leny);
        M = M + eps;

        % Align the scanpaths and find the cost associated with the best alignment
        [dist,path,M_assignment] = shortestPath(M);
        % Mapping between fixations in different scanpaths
        ki = zeros(len_i,len_j,length(path));
        kj = ki;
        for k = 1:length(path)
            [ki(i,j,k),kj(i,j,k)] = find(M_assignment == path(k));
        end

        %--------------------------------------------------------------------------
        % Compare scanpaths for other dimensions
        % (along the aligned path)
        %--------------------------------------------------------------------------
        results(1,l) = median(calVectorDifferencesAlongPath(sp1(i).saccade.lenx,sp2(j).saccade.lenx,sp1(i).saccade.leny,sp2(j).saccade.leny,path,M_assignment));
        results(2,l) = median(calAngularDifferences(sp1(i).saccade.theta,sp2(j).saccade.theta,path,M_assignment));
        results(3,l) = median(calLengthDifferences(sp1(i).saccade.len,sp2(j).saccade.len,path,M_assignment));
        results(4,l) = median(calPositionDifferences(sp1(i).saccade.x,sp2(j).saccade.x,sp1(i).saccade.y,sp2(j).saccade.y,path,M_assignment));
        results(5,l) = median(calDurationDifferences(sp1(i).fixation.dur,sp2(j).fixation.dur,path,M_assignment));
        l = l+1;

    end
end

% Normalize results (check these!)
results(1,i) = 1 - results(1,i)/(2*sqrt(sz(1)^2+sz(2)^2)); % Normalize against screen 2*diagonal (maximum theoritical difference)
results([3 4],i) = 1 - results([3 4],i)/sqrt(sz(1)^2+sz(2)^2); % Normalize against screen diagonal (maximum theoritical difference)
results(2,i) = 1 - results(2,i)/(pi); % Normalize against pi/2 (maximum theoritical difference)
results(5,i) = 1 - results(5,i);
% disp(['Vector diff: ',num2str(results(1,:))])
% disp(['Position diff: ',num2str(results(4,:))])
% disp(['Length diff: ',num2str(results(3,:))])
% disp(['Angular diff: ',num2str(results(2,:))])
% disp(['Duration: ',num2str(results(5,:))])


% % Plot simplified scanpaths
% sp1.saccade.x,sp2.saccade.x
% plotScanpaths(sp1,sp2)
% title('Simplified scanpaths')
% pause(2)


% delete([imwritePath,'Scanpath.pdf'])
% set(gca,'fontsize',fontSize)
% set(gcf,'color','none'),set(gca,'color','none')
% axis([0 sz 0 sz])
% legend({'Scanpath 1','Scanpath 2','Mapping'})
% pause(2)
% export_fig([imwritePath,'Scanpath.pdf'],'-pdf')

% close all
% return





