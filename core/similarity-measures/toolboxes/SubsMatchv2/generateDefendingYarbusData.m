function generateDefendingYarbusData(VariableToTest, measure)

fprintf('generateDefendingYarbusData %s %s\n', VariableToTest, measure);

% Loads the defending Yarbus data.
% out-structure: data{image, task}{subject}(x,y)
% 15 images, 7 tasks, 3 subjects
data_dir = 'Yarbus/';
file_list = dir([data_dir 'Data/sacTask/Ya*.mat']);

fprintf('Found %i files.\n', length(file_list));
noStimuli = 15;
noTasks = 7;
standard_stimulus_order = {}; % first loaded dataset determines the standard order

% this is 20x4x5, image x task x subject 
data = cell(noStimuli, noTasks);
for i=1:length(file_list)
    
    load([data_dir 'Data/sacTask/' file_list(i).name]);
    fprintf('Loading file %s: %i entries\n',file_list(i).name, length(eyeSaccade));
    task_order = file_list(i).name(4);
    stimulus_order = stimuliFilename;
    if(isempty(standard_stimulus_order))
        standard_stimulus_order = stimulus_order;
    end
    stimId = arrayfun(@(x)find(ismember(standard_stimulus_order, x),1), stimulus_order);
    data = sortDefendingData(data, task_order, stimId, eyeSaccade);
    
end

if(strcmp(VariableToTest, 'task'))
    scanpaths = data(:);
    labels = repmat(1:size(data,2), size(data,1),1);
    labels = labels(:);
    s_tmp = {};
    l_tmp = [];
    for i=1:length(scanpaths)
        for j=1:length(scanpaths{i})
            s_tmp{end+1} = scanpaths{i}{j};
            l_tmp(end+1) = labels(i);
        end
    end
    labels = l_tmp;
    scanpaths = s_tmp;
end

if(strcmp(VariableToTest, 'image'))
    data = data';
    scanpaths = data(:);
    labels = repmat(1:size(data,2), size(data,1),1);
    labels = labels(:);
    s_tmp = {};
    l_tmp = [];
    for i=1:length(scanpaths)
        for j=1:length(scanpaths{i})
            s_tmp{end+1} = scanpaths{i}{j};
            l_tmp(end+1) = labels(i);
        end
    end
    labels = l_tmp;
    scanpaths = s_tmp;
end

%% Convert data
for alphabet_size = 2:26;
SAXrep = cellfun(@(d) kernelSAX(d, alphabet_size, 'measure', measure), scanpaths, 'UniformOutput', false);
STRINGrep = cellfun(@(d) char(d+64), SAXrep, 'UniformOutput', false);

%% write sally input file
fileINPUT = fopen([data_dir '/defendingYarbus_' num2str(alphabet_size) '_' VariableToTest '_' measure '.lines'],'w');
formatINPUT = '%s %s\n';
for i=1:length(STRINGrep)
    fprintf(fileINPUT, formatINPUT, num2str(labels(i)), STRINGrep{i});
end
fclose(fileINPUT);
end



end

function data = sortDefendingData(data, task_order, stimId, eyeData)
% Participants with a "1" in the filename had task order memory, decade, people, wealth
% "2": decade, people, wealth, memory
% "3": people, wealth, memory, decade
% "4": wealth, memory, decade, people

% Encoding: memory = 1, decade = 2, people = 3, wealth = 4, usw
taskId = mod((1:length(data))+str2num(task_order)-2,7)+1;

fprintf('Assigning to tasks: %i %i %i %i %i %i %i\n', histc(taskId, 1:7));

for j = 1:length(data)
    if(isempty(data{stimId(j), taskId(j)}))
        data{stimId(j), taskId(j)} = {};
        data{stimId(j), taskId(j)}{1} = eyeData{j};
    else
        data{stimId(j), taskId(j)}{end+1} = eyeData{j};
    end
end

end


