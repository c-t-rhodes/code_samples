If you don't want to recompile, rename HierarchicalClustering.exe_TXT and strip off the _TXT.

Command:

<exe> <datafile> <evalfile>

HierarchicalClustering.exe 100_wine_attribute_data.dsv 100_wine_eval_data.dsv

----------------

This clustering program has a specific use case, so it assumes not only a valid 
datafile, but a matching eval file as well (in this case for a Wine dataset).
The program will take in all wine observations (tasting notes) and will perform
agglomerative (bottum-up) clustering.  Once finished, the program will ask the
user to put in commands.  Currently the only valid option is to perform a "cut"
evaluation on the clustered tree.  To see it run you can input the following:

Command: cut
Similarity Cut Point: 34
Include Noise (Single Node Clusters) [yes/no]: no

OR

Command: cut 34 no

This will show you available clusters at a 34% or higher similarity for all
members, and some other information regarding the evaldata file submitted
as argument 2.