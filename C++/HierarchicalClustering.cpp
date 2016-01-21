#include "HierarchicalClustering.h"

//PUBLIC FUNCTIONS
HierarchicalClustering::HierarchicalClustering(std::string filename)
{
	input_filename = filename;
	evaluation_attributes_filename = "";
	input_rows = 0;
	input_columns = 0;
	closest_distance = 999999.99;
}

HierarchicalClustering::~HierarchicalClustering()
{

}

//READ IN DATASET, CALCULATE DISTNACE MATRIX (JACCARDS), AND PERFORM CLUSTERING
void HierarchicalClustering::startClustering()
{
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "Starting Hierarchical Clustering Program" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	
	//Creates initial clusters with row values
	readInputFile();

	//From each initial cluster (and row values), create the similarity matrix
	createDistanceMatrix();
	//createLowerLeftDistanceMatrix();
	//printDistanceMatrix();

	//Using the array of Clusters and similarity matrix, perform agglomerative clustering
	agglomerativeCluster();

	//Print Tree to see
	//printTree(clusterList[0], "HEAD");
}

void HierarchicalClustering::printLeavesFromCutPoint(double cut_measure, std::string noise)
{
	clusterCounter = 0;
	leafCounter = 0;
	double converted_cut_measure = 0.0;
	summedClusterScores = 0.0;
	cutPointScore = 0.0;
	numberOfGoodClusters = 0;
    cut_point_info.open ("cut_point_info.txt");
	
	//return immediately if cut_measure is invalid
	if(cut_measure < 0.0 || cut_measure > 100.0)
	{
		std::cout << "Cut Measure must be between 0.0 % and 100.0 %. Try again." << std::endl << std::endl;
		cut_point_info.close();
		return;
	}
	else
	{
		converted_cut_measure = convertToOriginalDistSim(cut_measure); 
	}

	if(noise.compare("no") == 0 || noise.compare("NO") == 0)
	{
		std::cout << "Ingoring Noises (Single-Node Clusters)" << std::endl << std::endl;
		searchTreeSpecifiedCutPoint(clusterList[0], converted_cut_measure, false);
	}
	else if (noise.compare("yes") == 0 || noise.compare("YES") == 0)
	{
		std::cout << "Including Noises (Single-Node Clusters)" << std::endl << std::endl;
		searchTreeSpecifiedCutPoint(clusterList[0], converted_cut_measure, true);
	}
	else
	{
		std::cout << "Invalid \"Include Noise\" command.  Try again." << std::endl << std::endl;
		cut_point_info.close();
		return;
	}

	//cutPointScore = leafCounter / (summedClusterScores / clusterCounter) * 1000;
	cutPointScore = summedClusterScores;

	std::cout << "------------------------" << std::endl;
	std::cout << "Cut-Point Score = " << cutPointScore << std::endl;
	std::cout << "# Good Clusters = " << numberOfGoodClusters << std::endl;
	std::cout << "------------------------" << std::endl << std::endl;

	cut_point_info << "------------------------" << std::endl;
	cut_point_info << "Cut-Point Score = " << cutPointScore << std::endl;
	cut_point_info << "# Good Clusters = " << numberOfGoodClusters << std::endl;
	cut_point_info << "------------------------" << std::endl << std::endl;

	cut_point_info.close();

}

void HierarchicalClustering::printMostSimilarCluster()
{
	clusterCounter = 0;

	std::cout << "----------------------------" << std::endl;
	std::cout << "Most Similar Cluster in tree" << std::endl;
	std::cout << "----------------------------" << std::endl;

	//searchTreeSpecifiedCutPoint(clusterList[0], closest_distance, false);
	printLeavesFromCutPoint(convertToNormalizedDistSim(closest_distance),"no");
	
	find_best_cutpoint_automatically(clusterList[0]);
}

void HierarchicalClustering::loadEvalAttrFile(std::string eval_filename)
{	
	std::ifstream ifile(eval_filename);
	int row_counter = 0;

	if (ifile) 
	{
		evaluation_attributes_filename = eval_filename;
		std::cout << "Reading in from file (" << evaluation_attributes_filename << ") ... ";

		//In case it was previously loaded, delete all rows in vector
		for(unsigned int i = 0; i < eval_attributes_meta_info.size(); i++)
		{
			eval_attributes_meta_info.erase(eval_attributes_meta_info.begin());
		}

		//In case it was previously loaded, delete all rows in vector
		for(unsigned int i = 0; i < eval_attributes.size(); i++)
		{
			eval_attributes.erase(eval_attributes.begin());
		}

		std::cout << "Loading attribute vectors" << std::endl;

		while(!ifile.eof()) // To get you all the lines.
		{
			std::string currentline;
			getline(ifile,currentline);
			
			if(currentline.size() == 0)
				continue;

			row_counter++;

			std::vector<std::string> temp;
				
			std::istringstream ss(currentline);
			while (ss.good())
			{
				std::string token;
				std::getline(ss, token, '|');
				temp.push_back(token);
			}

			if(row_counter <= 2)
				eval_attributes_meta_info.push_back(temp);
			else
				eval_attributes.push_back(temp);
		}
	}
	else
	{
		std::cout << "Could not open file (" << eval_filename << "). Try again." << std::endl << std::endl;
		return;
	}

	std::cout << "Complete!" << std::endl << std::endl;

	ifile.close();
}

//Getters/Setters
std::string HierarchicalClustering::getEvalAttrFilename()
{
	return evaluation_attributes_filename;
}

//***************************************
//PRIVATE FUNCTIONS
//***************************************

//Read in input file.  For each line (non-header), create a new Cluster object and put into vector. 
//Each cluster object will contain a vector of that lines attributes (vector of ints)
void HierarchicalClustering::readInputFile()
{
	std::cout << "Reading in file ... ";

	std::string currentline;
	int row_counter = 0;

	std::ifstream infile;
	infile.open (input_filename);
	while(!infile.eof()) // To get you all the lines.
	{
		getline(infile,currentline);
		row_counter++;

		//If line is not empty and the line is now the first row (header)
		if(currentline.length() > 0 && row_counter != 1)
		{
			//row counter, string for 1st column value, vector for all other columns (int values)
			int col_counter = 0;
			std::string row_name = "";
			std::vector<int> attributes;

			//Split line into array
			std::istringstream ss(currentline);
			while (ss.good())
			{
				std::string token;
				std::getline(ss, token, '|');

				col_counter++;
				if(col_counter == 1)
					row_name = token;
				else
					attributes.push_back(atoi(token.c_str()));
			}

			Cluster * temp = new Cluster(row_name);
			temp->setClusterAttributes(attributes);
			temp->setDistanceMatrixIndex((int)clusterList.size()); //since we want it to start at 0, call before you place in vector
			temp->setLeafIndexesList(); //since this is a leaf, it'll set its leafindexeslist to itself (only happens on leaf node)
			clusterList.push_back(temp);
		}
	}
	
	infile.close();

	std::cout << "Complete!" << std::endl << std::endl;

	input_rows = clusterList.size();
	input_columns = clusterList[0]->getClusterAttributesSize();

	std::cout << "Input File Stats" << std::endl;
	std::cout << " - Total Rows    = " << input_rows << std::endl;
	std::cout << " - Total Columns = " << input_columns << std::endl << std::endl;
}

//CREATE A 2D MATRIX WITH THE JACCARDS DISTANCE BETWEEN ALL INPUT ROWS
void HierarchicalClustering::createDistanceMatrix()
{
	std::cout << "Creating distance matrix ... " << std::endl;
	
	//Resize 2d vector to vector be a square matrix of size INPUT_ROWS x INPUT_ROWS.  Default all values to -1.0
	distanceMatrix.resize(input_rows,std::vector<double>(input_rows,-1.0));

	//Calculate the full square distance matrix. Each cluster against every other cluster.
	for(int i = 0; i < input_rows; i++)
	{	
		for(int j = 0; j < input_rows; j++)
		{
			if(i == j)
				distanceMatrix[i][j] = 0.0;
			else
				distanceMatrix[i][j] = jaccardsDistance(clusterList[i],clusterList[j]);
		}

		
		if(i == input_rows - 1)
		{
			std::cout << "\r                            ";
			std::cout << "\rComplete!" << std::endl << std::endl;
		}
		else
			std::cout << "\rCompleted Row: " << i + 1;
	}
}

//FUTURE OPTIMIZATION (Since upper right triangle = lower left triangle, you only
//need to calculate one of the triangles to get all distances/similarities)
void HierarchicalClustering::createLowerLeftDistanceMatrix()
{
	std::cout << "Creating distance matrix ... " << std::endl;
	
	//Resize 2d vector to vector be a square matrix of size INPUT_ROWS x INPUT_ROWS.  Default all values to -1.0
	distanceMatrix.resize(input_rows,std::vector<double>(input_rows,-1.0));

	//Calculate the full square distance matrix. Each cluster against every other cluster.
	for(int i = 0; i < input_rows; i++)
	{	
		for(int j = 0; j < i; j++)
		{
			if(i == j)
				distanceMatrix[i][j] = 0.0;
			else
				distanceMatrix[i][j] = jaccardsDistance(clusterList[i],clusterList[j]);
		}

		
		if(i == input_rows - 1)
		{
			std::cout << "\r                            ";
			std::cout << "\rComplete!" << std::endl << std::endl;
		}
		else
			std::cout << "\rCompleted Row: " << i + 1;
	}
}

//AGGLOMERATIVE HIERARCHICAL CLUSTERING
//We start with a vector of clusters, where each cluster is a single input row (observation)
//While the vector contains more than 1 cluster, we do the following:
//    Using the Distnace Matrix, find the 2 clusters with the lowest distance (highest similarity)
//    Create a new parent cluster with those 2 found clusters as children
//    Push new parent cluster to end of vector, while removing the 2 individual children cluster from the vector
//    The parent cluster contains pointers to the children cluster
//    Repeat
//The final, single, cluster in the vector is now our head node in a binary tree.
void HierarchicalClustering::agglomerativeCluster()
{
	std::cout << "Starting Agglomerative Clustering ..." << std::endl;
	std::cout << "\rClusters Remaining: " << clusterList.size();

	while(clusterList.size() > 1)
	{
		int currentClusterIndex1 = -1;
		int currentClusterIndex2 = -1;
		double currentSmallestDistance = 9999999.99;
		
		//find current smallest distance value (single linkage default)
		for(unsigned int i = 0; i < clusterList.size(); i++)
		{
			for(unsigned int j = i + 1; j < clusterList.size(); j++)
			{
				Cluster * c1 = clusterList[i];
				Cluster * c2 = clusterList[j];
				double tempSim = singleLinkageDistance(c1, c2);

				if(tempSim < currentSmallestDistance)
				{
					currentClusterIndex1 = i;
					currentClusterIndex2 = j;
					currentSmallestDistance = tempSim;
				}
			}
		}

		//Record smallest distance
		if(currentSmallestDistance < closest_distance)
			closest_distance = currentSmallestDistance;

		//Create new cluster (and add closest clusters as left/right of new one)
		//Remove children from clusterList. Parent cluster should have list of indexes from distancematrix
		Cluster * c1 = clusterList[currentClusterIndex1];
		Cluster * c2 = clusterList[currentClusterIndex2];
		std::string tempname = std::string("(") + c1->getClusterName() + "," + c2->getClusterName() + ")";
		Cluster * newCluster = new Cluster(tempname, currentSmallestDistance, c1, c2);
		newCluster->setLeafIndexesList();
		clusterList.push_back(newCluster);

		//std::cout << "New Cluster: " << newCluster->getClusterName() << std::endl;

		clusterList.erase(clusterList.begin() + currentClusterIndex2);    //remove index2 first so we make sure both
		clusterList.erase(clusterList.begin() + currentClusterIndex1);    //are removed correctly

		std::cout << "\rClusters Remaining: " << clusterList.size() << "     ";
	}

	std::cout << "\rComplete!                            " << std::endl << std::endl;

	clusterList[0]->setClusterName(clusterList[0]->getClusterName() + std::string(";"));

	//std::cout << "\nNewick String = " << clusterList[0]->getClusterName() << std::endl << std::endl;
}

//**********************************************
//Distance Functions
//**********************************************

//JACCARDS DISTANCE/SIMILARITY
//This function is a similarity calculation between observations to find diversity in attribute sets.
//It's the ratio of the 2 observations' attribute intersection divided by the observationss union
//https://en.wikipedia.org/wiki/Jaccard_index
//Our dataset uses weighted data values, so this helps alleviate attribute bias 
double HierarchicalClustering::jaccardsDistance(Cluster * c1, Cluster * c2)
{
	std::vector<int> & v1 = c1->getClusterAttributes();
	std::vector<int> & v2 = c2->getClusterAttributes();
	
	double num_intersect = 0;
	double num_v1_distinct = 0;
	double num_v2_distinct = 0;
	double sim;

	if(v1.size() != input_columns)
	{
		std::cout << "ERROR: The following row does not match expected attribute count.\n";
		std::cout << c1->getClusterName() << std::endl;
		system("pause");
		exit(1);
	}

	if(v2.size() != input_columns)
	{
		std::cout << "ERROR: The following row does not match expected attribute count.\n";
		std::cout << c2->getClusterName() << std::endl;
		system("pause");
		exit(1);
	}

	for(int i = 0; i < input_columns; i++)
	{
		if(v1[i] != 0 && v2[i] != 0 && v1[i] == v2[i])
			num_intersect +=  v1[i];
		
		if(v1[i] != 0 && v2[i] == 0)
			num_v1_distinct += v1[i];

		if(v1[i] == 0 && v2[i] != 0)
			num_v2_distinct += v2[i];
	}

	sim = num_intersect / (num_intersect + num_v1_distinct + num_v2_distinct);
	sim = 1.0 - sim;

	return sim;
}

//SINGLE LINKAGE describes the distance between two clusters, which is described
//as the two points between both clusters that share the closest position.  Compare
//to COMPLETE LINKAGE or AVERAGE LINKAGE.
double HierarchicalClustering::singleLinkageDistance(Cluster * c1, Cluster * c2)
{
	double sim = 9999999.99;

	//Get both cluster's leaf indexes in distance matrix
	std::vector<int> & c1leaves = c1->getLeafIndexesList();
	std::vector<int> & c2leaves = c2->getLeafIndexesList();

	//Find the SINGLE LINK distance between the two clusters (aka point in c1 with shortest distance to point in c2)
	for(unsigned int i = 0; i < c1leaves.size(); i++)
	{
		int c1index = c1leaves[i];
		
		for(unsigned int j = 0; j < c2leaves.size(); j++)
		{
			int c2index = c2leaves[j];

			if(distanceMatrix[c1index][c2index] < sim)
			{
				sim = distanceMatrix[c1index][c2index];
			}
		}
	}

	//Return distance of these two clusters
	return sim;
}

//**********************************************
//UTILITY Functions
//**********************************************

//PRINT OUT THE FULL 2D DISTANCE MATRIX
void HierarchicalClustering::printDistanceMatrix()
{
	for(unsigned int i = 0; i < distanceMatrix.size(); i++)
	{
		for(unsigned int j = 0; j < distanceMatrix[i].size(); j++)
		{
			std::cout << distanceMatrix[i][j] << " ";
		}

		std::cout << std::endl;
	}
}

//FROM ANY GIVEN NODE, PRINT OUT THE DIRECTION WE TRAVERSE THROUGH THE TREE
//AS WELL AS THE NAME OF THE LEAF NODES AS YOU GET TO THEM IN POST-ORDER
void HierarchicalClustering::printTree(Cluster * node, std::string direction)
{
	if(node == NULL)
		return;
	
	std::cout << direction << std::endl;

	printTree(node->getChildLeft(), "LEFT");
	printTree(node->getChildRight(), "RIGHT");

	std::cout << node->getClusterName() << std::endl;
}

//FROM THE USER SPECIFIED CUT POINT, PRINT THE CLUSTERS (CONTAINING AT LEAST 2 OBSERVATIONS)
//IF THE USER SELECTS TO INCLUDE NOISE, WE WILL ALWAYS PRINT SINGLE OBSERVATION CLUSTERS AT OR 
//BELOW THAT CUTPOINT
void HierarchicalClustering::searchTreeSpecifiedCutPoint(Cluster * node, double cut_measure, bool include_noise)
{
	if(node == NULL)
		return;

	//Similarity Measure is DISTANCE, so top of tree is HIGH distance value, bottom of tree is LOW distance value
	if(node->getSimilarityMeasure() > cut_measure)
	{
		searchTreeSpecifiedCutPoint(node->getChildLeft(),cut_measure, include_noise);
		searchTreeSpecifiedCutPoint(node->getChildRight(), cut_measure, include_noise);
	}
	else
	{ 	
		if(include_noise == true)
		{
			clusterCounter++;

			double current_sim = node->getSimilarityMeasure();
			
			if(current_sim != -1)
				current_sim = convertToNormalizedDistSim(current_sim);

			std::cout << "CLUSTER #" << clusterCounter << " (Similarity = " << current_sim << " %)" << std::endl;
			cut_point_info << "CLUSTER #" << clusterCounter << " (Similarity = " << current_sim << " %)" << std::endl;
			std::vector<int> nodeIndexList;
			printLeafNames(node,nodeIndexList);
			
			if(current_sim != -1)
			{
				double clusterScore = evaluateCluster(nodeIndexList);
				summedClusterScores += clusterScore;
			}
			
			std::cout << std::endl;
			cut_point_info << std::endl;
		}
		else
		{
			if(checkLeafCountAboveOne(node) == true)
			{
				clusterCounter++;
				double current_sim = convertToNormalizedDistSim(node->getSimilarityMeasure());
				std::cout << "CLUSTER #" << clusterCounter << " (Similarity = " << current_sim << " %)" << std::endl;
				cut_point_info << "CLUSTER #" << clusterCounter << " (Similarity = " << current_sim << " %)" << std::endl;
				std::vector<int> nodeIndexList;
				printLeafNames(node,nodeIndexList);
				double clusterScore = evaluateCluster(nodeIndexList);
				std::cout << "   Cluster Score = " << clusterScore << std::endl;
				cut_point_info << "   Cluster Score = " << clusterScore << std::endl;
				std::cout << std::endl;
				cut_point_info << std::endl;

				summedClusterScores += clusterScore;
			}
		}
	}
}

//CHECK TO SEE IF CURRNET NODE CONTAINS ANY CHILDREN
bool HierarchicalClustering::checkLeafCountAboveOne(Cluster * node)
{
	if(node->getChildLeft() == NULL && node->getChildRight() == NULL)
		return false;
	else
		return true;
}

//TRAVERSE TREE (POST-ORDER) AND PRINT LEAF NAMES
void HierarchicalClustering::printLeafNames(Cluster * node, std::vector<int> & nodeIndexList)
{
	if(node == NULL)
		return;

	printLeafNames(node->getChildLeft(), nodeIndexList);
	printLeafNames(node->getChildRight(), nodeIndexList);

	if(node->getChildLeft() == NULL && node->getChildRight() == NULL)
	{
		nodeIndexList.push_back(node->getDistanceMatrixIndex());
		std::cout << " - " << node->getClusterName() << std::endl;
		cut_point_info << " - " << node->getClusterName() << std::endl;
	}
}

//CONVERTS NORMALIZED DISTANCE VALUE BACK TO ORIGINAL JACCARDS VALUE
double HierarchicalClustering::convertToOriginalDistSim(double value_to_convert)
{
	double converted_value =  (double)1.0 - (value_to_convert / (double)100.0);
	
	return converted_value;
}

//CONVERTS ORIGINAL JACCARDS VALUE TO NORMALIZED VALUE FOR USE
double HierarchicalClustering::convertToNormalizedDistSim(double value_to_convert)
{
	double converted_value = (double)100.0 - (value_to_convert * (double)100.0);

	return converted_value;
}

//EVALUATION Functions
double HierarchicalClustering::evaluateCluster(std::vector<int> nodeIndexList)
{
	if(eval_attributes.size() == 0)
	{
		std::cout << "EVALUATION ERROR: No Cluster Evaluation Attributes have been loaded." << std::endl << std::endl;
		return -1.0;
	}
	
	std::cout << std::endl;
	cut_point_info << std::endl;
	
	std::cout << "   ================== " << std::endl;
	std::cout << "   Cluster Evaluation " << std::endl;
	std::cout << "   ================== " << std::endl;

	cut_point_info << "   ================== " << std::endl;
	cut_point_info << "   Cluster Evaluation " << std::endl;
	cut_point_info << "   ================== " << std::endl;
	
	double summedMaxScores = 0.0;
	double productMaxScores = 1.0;
	int total_nodes = (int)nodeIndexList.size();
	int total_columns = eval_attributes[nodeIndexList[0]].size();

	for(int j = 1; j < total_columns; j++)
	{
		std::map<std::string, int> stringMap;
		std::string column_type = eval_attributes_meta_info[0][j];
		std::string column_name = eval_attributes_meta_info[1][j];
		double average = 0.0;
		double minimum = 99999999.99;
		double maximum = -1.0;
		
		for(int k = 0; k < total_nodes; k++)
		{
			int index = nodeIndexList[k];

			std::string temp = eval_attributes[index][j];

			stringMap[temp]++;
		}

		std::cout << "   " << column_name << " (" << column_type << ")" << std::endl;
		cut_point_info << "   " << column_name << " (" << column_type << ")" << std::endl;

		if(column_type.compare("STRING") == 0)
		{
			double current_col_max_average = 0.0;
			
			for (std::map<std::string,int>::iterator it=stringMap.begin(); it!=stringMap.end(); ++it)
			{
				std::string key = it->first;
				int value = it->second;
				average = (double)value / (double)total_nodes * 100.0;
				std::cout << "     " << key << " => " << average << "%" << std::endl;
				cut_point_info << "     " << key << " => " << average << "%" << std::endl;

				if(average > current_col_max_average)
					current_col_max_average = average;
			}

			summedMaxScores += current_col_max_average;
			productMaxScores *= ((double)current_col_max_average / 100.0);
		}
		else if (column_type.compare("NUMBER") == 0)
		{
			for (std::map<std::string,int>::iterator it=stringMap.begin(); it!=stringMap.end(); ++it)
			{
				double number_value = atof(it->first.c_str());
				int times_value = it->second;

				average = average + (number_value * (double)times_value);

				if(number_value < minimum)
					minimum = number_value;

				if(number_value > maximum)
					maximum = number_value;
			}

			std::cout << "     Minimum Value = " << minimum << std::endl;
			std::cout << "     Maximum Value = " << maximum << std::endl;
			std::cout << "     Average value = " << average / (double)total_nodes << std::endl;

			cut_point_info << "     Minimum Value = " << minimum << std::endl;
			cut_point_info << "     Maximum Value = " << maximum << std::endl;
			cut_point_info << "     Average value = " << average / (double)total_nodes << std::endl;
		}
		else
		{
			std::cout << "     Unknown column type for this column." << std::endl;
			cut_point_info << "     Unknown column type for this column." << std::endl;
		}
		
		std::cout << std::endl;
		cut_point_info << std::endl;
		stringMap.clear();
	}

	std::cout << std::endl;
	cut_point_info << std::endl;

	//Keep count of how many leaf nodes we're dealing with for this cut point
	leafCounter += total_nodes;

	//Return score for this specific cluster
	//return (summedMaxScores * total_nodes);

	if(summedMaxScores >= 280)
		numberOfGoodClusters++;

	return productMaxScores * total_nodes;
}

//==========================================================
//              DON'T PRINT FUNCTIONS
//==========================================================
void HierarchicalClustering::searchTreeSpecifiedCutPointNoPrint(Cluster * node, double cut_measure, bool include_noise)
{
	if(node == NULL)
		return;

	//Similarity Measure is DISTANCE, so top of tree is HIGH distance value, bottom of tree is LOW distance value
	if(node->getSimilarityMeasure() > cut_measure)
	{
		searchTreeSpecifiedCutPointNoPrint(node->getChildLeft(),cut_measure, include_noise);
		searchTreeSpecifiedCutPointNoPrint(node->getChildRight(), cut_measure, include_noise);
	}
	else
	{ 	
		if(include_noise == true)
		{
			clusterCounter++;

			double current_sim = node->getSimilarityMeasure();
			
			if(current_sim != -1)
				current_sim = convertToNormalizedDistSim(current_sim);

			std::vector<int> nodeIndexList;
			printLeafNamesNoPrint(node,nodeIndexList);
			
			if(current_sim != -1)
			{
				double clusterScore = evaluateClusterNoPrint(nodeIndexList);
				summedClusterScores += clusterScore;
			}
		}
		else
		{
			if(checkLeafCountAboveOne(node) == true)
			{
				clusterCounter++;
				double current_sim = convertToNormalizedDistSim(node->getSimilarityMeasure());
				std::vector<int> nodeIndexList;
				printLeafNamesNoPrint(node,nodeIndexList);
				double clusterScore = evaluateClusterNoPrint(nodeIndexList);

				summedClusterScores += clusterScore;
			}
		}
	}
}

void HierarchicalClustering::printLeafNamesNoPrint(Cluster * node, std::vector<int> & nodeIndexList)
{
	if(node == NULL)
		return;

	printLeafNamesNoPrint(node->getChildLeft(), nodeIndexList);
	printLeafNamesNoPrint(node->getChildRight(), nodeIndexList);

	if(node->getChildLeft() == NULL && node->getChildRight() == NULL)
	{
		nodeIndexList.push_back(node->getDistanceMatrixIndex());
	}
}

double HierarchicalClustering::evaluateClusterNoPrint(std::vector<int> nodeIndexList)
{
	if(eval_attributes.size() == 0)
	{
		std::cout << "EVALUATION ERROR: No Cluster Evaluation Attributes have been loaded." << std::endl << std::endl;
		return -1.0;
	}
	
	double summedMaxScores = 0.0;
	double productMaxScores = 1.0;
	int total_nodes = (int)nodeIndexList.size();
	int total_columns = eval_attributes[nodeIndexList[0]].size();

	for(int j = 1; j < total_columns; j++)
	{
		std::map<std::string, int> stringMap;
		std::string column_type = eval_attributes_meta_info[0][j];
		std::string column_name = eval_attributes_meta_info[1][j];
		double average = 0.0;
		double minimum = 99999999.99;
		double maximum = -1.0;
		
		for(int k = 0; k < total_nodes; k++)
		{
			int index = nodeIndexList[k];

			std::string temp = eval_attributes[index][j];

			stringMap[temp]++;
		}

		if(column_type.compare("STRING") == 0)
		{
			double current_col_max_average = 0.0;
			
			for (std::map<std::string,int>::iterator it=stringMap.begin(); it!=stringMap.end(); ++it)
			{
				std::string key = it->first;
				int value = it->second;
				average = (double)value / (double)total_nodes * 100.0;

				if(average > current_col_max_average)
					current_col_max_average = average;
			}

			summedMaxScores += current_col_max_average;
			productMaxScores *= ((double)current_col_max_average / 100.0);
		}
		else if (column_type.compare("NUMBER") == 0)
		{
			for (std::map<std::string,int>::iterator it=stringMap.begin(); it!=stringMap.end(); ++it)
			{
				double number_value = atof(it->first.c_str());
				int times_value = it->second;

				average = average + (number_value * (double)times_value);

				if(number_value < minimum)
					minimum = number_value;

				if(number_value > maximum)
					maximum = number_value;
			}
		}
		else
		{
			//std::cout << "     Unknown column type for this column." << std::endl;
		}
		stringMap.clear();
	}

	//Keep count of how many leaf nodes we're dealing with for this cut point
	leafCounter += total_nodes;

	//Return score for this specific cluster
	//return (summedMaxScores * total_nodes);

	if(summedMaxScores >= 280)
		numberOfGoodClusters++;

	return productMaxScores * total_nodes;
}

void HierarchicalClustering::find_best_cutpoint_automatically(Cluster * node)
{
	std::cout << "Attemping to find best cut point..." << std::endl;
	
	double best_sim = convertToNormalizedDistSim(closest_distance);
	double worst_sim = 0.0;
	double current_sim_to_check = best_sim;
	double decrementAmount = 0.10;

	std::ofstream myfile;
    myfile.open ("cut_point.csv");
	myfile << "CUT POINT,#Clusters,#Wines,SCORE,#GoodClusters(70%)\n";

	while(current_sim_to_check >= worst_sim)
	{
		clusterCounter = 0;
		leafCounter = 0;
		summedClusterScores = 0.0;
		cutPointScore = 0.0;
		numberOfGoodClusters = 0;
		
		searchTreeSpecifiedCutPointNoPrint(node,convertToOriginalDistSim(current_sim_to_check),false);

		//cutPointScore =  (summedClusterScores / (double)clusterCounter);
		cutPointScore = summedClusterScores;
		//cutPointScore = numberOfGoodClusters;
		myfile << current_sim_to_check << "%," << clusterCounter << "," << leafCounter << "," << cutPointScore << "," << numberOfGoodClusters <<std::endl;

		current_sim_to_check -= decrementAmount;
	}

	myfile.close();
}