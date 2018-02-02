% Thomas Kuebler
% University of Tuebingen
% 2016 v2.00

% at first run, compile liblinear
cd liblinear/matlab
make;
cd ../..

batchDefendingYarbus('xPrctile', 'complete');
