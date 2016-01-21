These two scripts are older python scripts I wrote for a project to extend the
Hierarchical Clustering algorithm to group together closely related observations
into a single cluster.  What this means is that instead of a binary tree output,
you get a tree that can have many children for any given node.

What it does is instead of finding the two closest observations, it finds all the
observations in a given radius, and puts them together.  That radius increases,
(or decreases if it's the Divisive script [HDC]), and continues to group similar
clusters.

Examples of running either of these scripts:

[crhode.crhode1114] python Radial_HAC.py test.txt "|"
-----------------------------------
Clustering appears to have finished
-----------------------------------
0.000999927520752 seconds to cluster
((wine10,((wine7,wine8,wine9):0.5,(wine11,wine12):1.0):1.5):2.5,((wine1,wine2,wine3):0.5,(wine4,wine5,wine6):0.5):6.0):9.5;

--------------------------------------------------------------------------------------                                                                                                                                                             ?

[crhode.crhode1114] python Radial_HDC.py test.txt "|"
0.00100016593933 seconds to cluster
Forming Newick String ...
((wine1,wine2,wine3):0.0,(wine4,wine5,wine6):0.0,((wine10,((wine9,wine7,wine8):0.0,wine11):1.5):2.0,wine12):2.75):3.0;