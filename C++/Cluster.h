#ifndef CLUSTER_H
#define CLUSTER_H

#include <string>
#include <vector>

class Cluster
{
	public:
		Cluster();
		Cluster(std::string name);
		Cluster(std::string name, double sim, Cluster * l, Cluster * r);
		~Cluster();
		
		//Similarity Measure get/set
		void setSimilarityMeasure(double sim);
		double getSimilarityMeasure();
		
		//Cluster Name get/set
		void setClusterName(std::string name);
		std::string getClusterName();
		
		//Left Node get/set
		void setChildLeft(Cluster * l);
		Cluster * getChildLeft();

		//Right Node get/set
		void setChildRight(Cluster * r);
		Cluster * getChildRight();

		//Set LEAF node clusterAttributes and distance matrix index
		void setClusterAttributes(std::vector<int> attrs);
		std::vector<int> & getClusterAttributes();
		int getClusterAttributesSize();
		
		//Distance Matrix Index get/set
		void setDistanceMatrixIndex(int index);
		int getDistanceMatrixIndex();

		//Leaf Indexes List get/set
		void setLeafIndexesList();
		std::vector<int> & getLeafIndexesList();

	private:
		std::string clusterName;             //name of this cluster (newick tree string at this tree position)
		std::string clusterType;             //{LEAF, CLUSTER, HEAD}

		std::vector<int> clusterAttributes;  //vector of attribute values from input file (only for LEAF nodes)
		int distanceMatrixIndex;             //index of Cluster in distance Matrix (only for LEAF nodes)
		std::vector<int> leafIndexesList;    //vector of all leaves in this cluster
		double similarityMeasure;            //similarity measure between left and right children
		
		Cluster * left;                      //left child node
		Cluster * right;                     //right child node
};

#endif
