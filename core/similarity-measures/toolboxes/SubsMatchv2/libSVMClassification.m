function libSVMClassification(filepattern)
% taskpattern is something to be used like
% fprintf('features_%s_%s.libsvm', num2str(ngram_length), num2str(alphabet_length));

% Classify features into classes using libLinear.
% Requires precomputed features (by sally)

addpath('liblinear/matlab');

% Data to load 
datadir = 'Yarbus/'; % this will use features on messor


% SVM classify
acc_array = [];
for ngram_length = 1:10
    for alphabet_length=2:26
        filename = sprintf(filepattern, num2str(ngram_length), num2str(alphabet_length));
        [label_vector, instance_matrix] = libsvmread([datadir filename]); 
        % leave-one-out crossvalidation (build-in version of libSVM)
        %libSvmParam = ['-t 0 -v ' num2str(length(label_vector)) ' -q ']; 
        % faster, 10-fold crossvalidation
        % libSVM
        %libSvmParam = ['-t 0 -v ' num2str(10) ' -q ']; 
        % liblinear
        libSvmParam = ['-v ' num2str(10) ' -q ']; 
        % balance weights for unequal class sizes
        weights = getSVMWeight(label_vector);
        libSvmParam = [libSvmParam weights];

        acc_array(ngram_length, alphabet_length) = train(label_vector, instance_matrix, libSvmParam);
        fprintf('libSVM alphabet_length=%i ngram_length=%i \n', alphabet_length, ngram_length);
    end
end

acc_array = acc_array(:,2:end);

fprintf('Guess chance: %.2f, max SVM: %.2f\n', max(histc(label_vector, unique(label_vector)))/length(label_vector)*100, max(acc_array(:)));

% Construct a filename for the results
filename = sprintf(filepattern, num2str(0), num2str(0)); filename = filename(1:end-7); filename = [filename '_results.mat'];
save([datadir filename], 'acc_array');

% Visualize crossvalidation
%filename = sprintf(filepattern, num2str(0), num2str(0)); filename = filename(1:end-7); filename = [filename '_results.fig'];
%imagesc(acc_array);
%savefig([datadir taskdir filename]);
