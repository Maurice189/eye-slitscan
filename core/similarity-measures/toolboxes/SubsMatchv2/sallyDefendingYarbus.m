function sallyDefendingYarbus(VariableToTest, measure)

messor_dir = 'Yarbus/';
%% Feature embedding
% This will run the command, but not wait for it to finish.
commandsIssued = 0;
for ngram_length = 1:10
    for alphabet_length=2:26
        % cnt + l2 norm
        call_sally = ['sudo sally --input_format lines --lines_regex "^(\\+|-)?[0-9]+" --ngram_len ' num2str(ngram_length) ' --granularity bytes --vect_embed cnt --vect_norm l2 --output_format libsvm ' messor_dir '/defendingYarbus_' num2str(alphabet_length) '_' VariableToTest '_' measure '.lines ' messor_dir '/defendingYarbus_' num2str(ngram_length) '_' num2str(alphabet_length) '_' VariableToTest '_' measure '.libsvm'];
        system(call_sally);

        fprintf('string-kernel alphabet_length=%i ngram_length=%i %s %s;\n', alphabet_length, ngram_length, VariableToTest, measure);
        commandsIssued = commandsIssued+1;
    end
end
