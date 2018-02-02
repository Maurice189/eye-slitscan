function batchDefendingYarbus(measure, varargin)
validSteps = {'generateData', 'computeFeatures', 'classify', 'complete'};
p = inputParser;
addOptional(p,'singlestep','complete',@(x) any(validatestring(x,validSteps)));
parse(p,varargin{:});

Variables = {'task', 'image'};

if(strcmp(p.Results.singlestep, validSteps{1}) || strcmp(p.Results.singlestep, validSteps{end}))
    % generate data
    for v = 1:length(Variables)
        generateDefendingYarbusData(Variables{v}, measure);
    end
end

if(strcmp(p.Results.singlestep, validSteps{2}) || strcmp(p.Results.singlestep, validSteps{end}))
    % compute features
    for v = 1:length(Variables)
        sallyDefendingYarbus(Variables{v}, measure);
    end
end

if(strcmp(p.Results.singlestep, validSteps{3}) || strcmp(p.Results.singlestep, validSteps{end}))
    % classify
    for v = 1:length(Variables)
        filepattern = ['defendingYarbus_%s_%s_' Variables{v} '_' measure '.libsvm'];
        libSVMClassification(filepattern);
    end
end
