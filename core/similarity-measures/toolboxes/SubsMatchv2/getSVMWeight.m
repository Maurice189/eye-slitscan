function weightString = getSVMWeight2(label_vector)
unqiueLabels = unique(label_vector);
c = histc(label_vector, unqiueLabels);

weights = 100*(1-c/sum(c));

weightString = '';
for i=1:length(weights)
    weightString = [weightString '-w' num2str(unqiueLabels(i)) ' ' num2str(weights(i),'%.0f') ' '];
end

