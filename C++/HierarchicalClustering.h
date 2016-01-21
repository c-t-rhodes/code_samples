#ifndef HIERARCHICALCLUSTERING_H
#define HIERARCHICALCLUSTERING_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "Cluster.h"

class HierarchicalClustering
{
	public:
		HierarchicalClustering(std::string filename);
		~HierarchicalClustering();
		void startClustering();
		void printLeavesFromCutPoint(double cut_measure, std::string noise);
		void printMostSimilarCluster();
		void loadEvalAttrFile(std::string eval_filename);

		//getters/setters
		std::string getEvalAttrFilename();

	private:
		std::string input_filename;
		int input_rows;
		int input_columns;
		std::vector<Cluster *> clusterList;
		std::vector<std::vector<double>> distanceMatrix;
		double closest_distance;

		//CALLED BY public startClustering() FUNCTION
		void readInputFile();
		void createDistanceMatrix();
		void createLowerLeftDistanceMatrix();
		void agglomerativeCluster();

		//DISTANCE Functions
		double jaccardsDistance(Cluster * c1, Cluster * c2);
		double singleLinkageDistance(Cluster * c1, Cluster * c2);
		
		//UTILITY Functions and VARIABLES
		double summedClusterScores; //summed scores when evaluting all clusters of a cut point
		double cutPointScore; //summedClusterScores divided by total clusters at that cut point
		int clusterCounter; //used when searching by cut point
		int leafCounter; //used when searching by cut point
		int numberOfGoodClusters;
		void printDistanceMatrix();
		void printTree(Cluster * node, std::string direction);

		//PRINT 
		std::ofstream cut_point_info;
		void searchTreeSpecifiedCutPoint(Cluster * node, double cut_measure, bool include_noise);
		bool checkLeafCountAboveOne(Cluster * node);
		void printLeafNames(Cluster * node, std::vector<int> & nodeIndexList);

		double convertToNormalizedDistSim(double value_to_convert);
		double convertToOriginalDistSim(double value_to_convert);

		//EVALUATION Functions and Variables
		std::string evaluation_attributes_filename;
		std::vector<std::vector<std::string>> eval_attributes_meta_info;
		std::vector<std::vector<std::string>> eval_attributes;
		double evaluateCluster(std::vector<int> nodeIndexList);

		//DON'T PRINT (used for automatic evaluations)
		void searchTreeSpecifiedCutPointNoPrint(Cluster * node, double cut_measure, bool include_noise);
		void printLeafNamesNoPrint(Cluster * node, std::vector<int> & nodeIndexList);
		double evaluateClusterNoPrint(std::vector<int> nodeIndexList);
		void find_best_cutpoint_automatically(Cluster * node);
};

#endif