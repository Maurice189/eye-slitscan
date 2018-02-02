function sp1 = generateStructureArrayScanpath(data)

len = size(data,1);

for i = 1:len
    
    sp1.fixation.x(i) = data(i,1);         
    sp1.fixation.y(i) = data(i,2);                         
    sp1.fixation.dur(i) = data(i,3);   
    
    if i < len
        sp1.saccade.x(i) = data(i,1);   
        sp1.saccade.y(i) = data(i,2);
    end
    
    if i > 1
        sp1.saccade.lenx(i-1) = (sp1.fixation.x(i) - sp1.saccade.x(i-1));
        sp1.saccade.leny(i-1) = (sp1.fixation.y(i) - sp1.saccade.y(i-1));
        [sp1.saccade.theta(i-1),sp1.saccade.len(i-1)] = cart2pol(sp1.saccade.lenx(i-1),sp1.saccade.leny(i-1));
    end

    
end

