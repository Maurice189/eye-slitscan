function spGlobal = simplifyScanpath(sp,T1,T2,Tdur)

l = inf;
spGlobal = sp;
% Loop until no further simplifications are made
% h = figure
while 1
    spGlobal = simplifyDirection(spGlobal,T2,Tdur);
%     spGlobal.fixation.dur
    spGlobal = simplifyLength(spGlobal,T1,Tdur);
%     spGlobal.fixation.dur

%     h = subplot(1,2,2)
%     plotScanpaths(spGlobal,spGlobal,'simplified sp',[1 1],[5 5])
%     drawnow
%     pause
    if l == length(spGlobal.fixation.dur)
        break
    end
    l = length(spGlobal.fixation.dur);
%     clf(h)
end

% close(h)



