
function symbolic_data =  kernelSAX(data, alphabet_size, varargin)
%% kernelSAX Converts a scanpath into its string representation
% Calculate the percentiles of the data as the borders of letter assignment
% bins. Then convert the data into its string representation based on that
% bins.

% validate input
p = inputParser;
validMeasures = {'xPrctile', 'yPrctile', 'xBin', 'yBin', 'circular', 'xDiffPrctile', 'yDiffPrctile', 'xDiffBin', 'yDiffBin'};
defaultMeasure = validMeasures{1};

addRequired(p,'data',@(x) ~isempty(x));
addRequired(p,'alphabet_size',@isnumeric);
addOptional(p,'measure',defaultMeasure,@(x) any(validatestring(x,validMeasures)));

parse(p,data, alphabet_size ,varargin{:});

data = p.Results.data;
alphabet_size = p.Results.alphabet_size;
measure = p.Results.measure;

% Select the data required
if(strcmp(measure, 'xPrctile') || strcmp(measure, 'xBin'))
    data = data(:,1);
end
if(strcmp(measure, 'xDiffPrctile') || strcmp(measure, 'xDiffBin'))
    data = diff(data(:,1));
end
if(strcmp(measure, 'yDiffPrctile') || strcmp(measure, 'yDiffBin'))
    data = diff(data(:,2));
end
if(strcmp(measure, 'yPrctile') || strcmp(measure, 'yBin'))
    data = data(:,2);
end
if(strcmp(measure, 'circular') || strcmp(measure, 'circular'))
    mid = [mean(data(:,1)) mean(data(:,2))];
    data = sqrt((data(:,1)-mid(1)).^2+ (data(:,2)-mid(2)).^2);
end

if(strcmp(measure, 'xPrctile') || strcmp(measure, 'yPrctile') || strcmp(measure, 'xDiffPrctile') || strcmp(measure, 'yDiffPrctile'))
%% Calculate the percentiles
% they are equally sized in terms of amount of gaze assigned.
% probability-step-size (size of a probability chunk)
prob_step_size = 1/alphabet_size;
value_step_size=quantile(data,(0:prob_step_size:1));
value_step_size(1)=-Inf; value_step_size(length(value_step_size))=Inf;
end

if(strcmp(measure, 'xBin') || strcmp(measure, 'yBin') || strcmp(measure, 'circular') || strcmp(measure, 'xDiffBin') || strcmp(measure, 'yDiffBin'))
%% Calculate the bins
% they are equally sized in terms of number of pixels.
bin_step_size = (max(data)-min(data))/alphabet_size;
value_step_size=min(data):bin_step_size:max(data);
value_step_size(1)=-Inf; value_step_size(length(value_step_size))=Inf;
end

%% binning and construct string
symbolic_data = zeros(length(data),1);
for i=1:length(data)
    % find out which letter to use by the bin the data falls into
    idx=sum(value_step_size<=data(i));
    % append
    symbolic_data(i) = idx;
end

end