#include "Cluster.h"

//CONSTRUCTORS-DESTRUCTOR
Cluster::Cluster()
{
	clusterName = "";
	similarityMeasure = -1.0;
	distanceMatrixIndex = -1;
	left = NULL;
	right = NULL;
}

Cluster::Cluster(std::string name)
{
	clusterName = name;
	similarityMeasure = -1.0;
	distanceMatrixIndex = -1;
	left = NULL;
	right = NULL;
}

Cluster::Cluster(std::string name, double sim, Cluster * l, Cluster * r)
{
	clusterName = name;
	similarityMeasure = sim;
	distanceMatrixIndex = -1;
	this->left = l;
	this->right = r;
}

Cluster::~Cluster()
{

}

//GETTERS-SETTERS
void Cluster::setSimilarityMeasure(double sim)
{
	similarityMeasure = sim;
}

double Cluster::getSimilarityMeasure()
{
	return similarityMeasure;
}
		
void Cluster::setClusterName(std::string name)
{
	clusterName = name;
}

std::string Cluster::getClusterName()
{
	return clusterName;
}
		
void Cluster::setChildLeft(Cluster * l)
{
	this->left = l;
}

Cluster * Cluster::getChildLeft()
{
	return this->left;
}

void Cluster::setChildRight(Cluster * r)
{
	this->right = r;
}

Cluster * Cluster::getChildRight()
{
	return this->right;
}

void Cluster::setClusterAttributes(std::vector<int> attrs)
{
	clusterAttributes = attrs;
}

std::vector<int> & Cluster::getClusterAttributes()
{
	return clusterAttributes;
}

int Cluster::getClusterAttributesSize()
{
	return clusterAttributes.size();
}

void Cluster::setDistanceMatrixIndex(int index)
{
	distanceMatrixIndex = index;
}

int Cluster::getDistanceMatrixIndex()
{
	return distanceMatrixIndex;
}

void Cluster::setLeafIndexesList()
{
	//If this cluster is a LEAF node, set the vector to only include itself
	//else set the vector to be left's vector + the right's vector
	if(left == NULL && right == NULL) 
	{
		leafIndexesList.push_back(distanceMatrixIndex);
	}
	else
	{
		std::vector<int> & leftvector = left->getLeafIndexesList();
		std::vector<int> & rightvector = right->getLeafIndexesList();
		
		leafIndexesList.insert(leafIndexesList.end(), leftvector.begin(), leftvector.end());
		leafIndexesList.insert(leafIndexesList.end(), rightvector.begin(), rightvector.end());
	}
}

std::vector<int> & Cluster::getLeafIndexesList()
{
	return leafIndexesList;;
}