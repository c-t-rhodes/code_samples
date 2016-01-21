#include <iostream>
#include <algorithm>
#include <string>

#include "HierarchicalClustering.h"

//For use with switch statements on a string (c++ does not like this natively)
static enum StringValue { HELP, CUT, QUIT, EXIT };
static std::map<std::string, StringValue> mapped_enum;

int parseCommand(std::string command, HierarchicalClustering * hc);

int main(int argc, char* argv[]) 
{ 
	if(argc != 3)
	{
		std::cout << "Program expects only two arguments: Data File and Evaluation File." << std::endl;
		std::cout << "Usage: " << argv[0] << " datafile evalfile" << std::endl;
		return 1;
	}

	//Map enums
	mapped_enum["HELP"] = HELP;
	mapped_enum["CUT"] = CUT;
	mapped_enum["QUIT"] = QUIT;
	mapped_enum["EXIT"] = EXIT;
	
	//Inputfile Argument
	std::string datafile = argv[1];
	std::string datafile_eval = argv[2];
	//std::string datafile = argv[1] = "100_wine_attribute_data.dsv";
	//std::string datafile_eval = argv[2] = "100_wine_eval_data.dsv";
	//std::string datafile = argv[1] = "999_wines_input_data.dsv";
	//std::string datafile_eval = argv[2] = "999_wines_non_savory.dsv";
  
	//Create Hierarchical Clustering argument and start clustering (delete object when finished)
	HierarchicalClustering * hc = new HierarchicalClustering(datafile);
	hc->startClustering();
	hc->loadEvalAttrFile(datafile_eval);
	hc->printMostSimilarCluster();
	
	//START USER COMMAND LOOP
	int command_result = 0;

	while(command_result == 0)
	{
		std::string command = "";
		std::cout << "What would you like to do (type \"help\" for command list or \"quit\" to exit)?" << std::endl;
		std::cout << "Command: ";
		std::cin >> command;

		command_result = parseCommand(command, hc);
	}

	//CLEAN UP AND EXIT
	delete hc;

	return 0; 
}

//After the clustering is finished, allow for user to specify a cut point.
int parseCommand(std::string command, HierarchicalClustering * hc)
{
	int result = 0;
	
	//convert command to uppercase
	transform(command.begin(), command.end(), command.begin(), ::toupper);
	
	switch(mapped_enum[command])
	{
		case HELP:
			{
				std::cout << "---------------------------------------------------------------------------" << std::endl;
				std::cout << "|                    Hierarhical Clustering Command Help                  |" << std::endl;
				std::cout << "---------------------------------------------------------------------------" << std::endl;
				std::cout << "|        'help' or 'HELP' - Display commands and usage                    |" << std::endl;
				std::cout << "|        'quit' or 'QUIT' - Exit program                                  |" << std::endl;
				std::cout << "|                                                                         |" << std::endl;
				std::cout << "|          'cut' or 'CUT' - Prints to screen clusters below cutpoint      |" << std::endl;
				std::cout << "|        'ocut' or 'OCUT' - Prints to file (overwrites) clusters below    |" << std::endl;
				std::cout << "|                           a specified cutpoint                          |" << std::endl;
				std::cout << "|        'acut' or 'ACUT' - Prints to file (appends) clusters below       |" << std::endl;
				std::cout << "|                           a specified cutpoint                          |" << std::endl;
				std::cout << "|                                                                         |" << std::endl;
				std::cout << "|         'loadevalattrs' - Load Evaluation Attribute File                |" << std::endl;
				std::cout << "|        'eval' or 'EVAL' - Same as cut, but includes evaluation          |" << std::endl;
				std::cout << "|      'oeval' or 'OEVAL' - Same is ocut, but includes evaluation         |" << std::endl;
				std::cout << "|      'aeval' or 'AEVAL' - Same as acut, but includes evaluation         |" << std::endl;
				std::cout << "|                                                                         |" << std::endl;
				std::cout << "|    'newick' or 'NEWICK' - Prints Newick String for Hierarhical Tree     |" << std::endl;
				std::cout << "|                                                                         |" << std::endl;
				std::cout << "|'distance' or 'DISTANCE' - Prints distance matrix to file (overwrites)   |" << std::endl;
				std::cout << "---------------------------------------------------------------------------" << std::endl;
				std::cout << std::endl;
				break;
			}
		case CUT:
			{
				double cutpoint = -1;
				std::string noise = "";
				std::cout << "Similarity Cut Point: ";
				std::cin >> cutpoint;
				std::cout << "Include Noise (Single Node Clusters) [yes/no]: ";
				std::cin >> noise;
				hc->printLeavesFromCutPoint(cutpoint, noise);
				break;
			}
		case QUIT:
			result = 1;
			break;
		case EXIT:
			result = 1;
			break;
		default:
				std::cout << "Unknown Command (" << command << ")" << std::endl << std::endl;
	}

	return result;

	/*
	case LOADEVALATTRS:
	{
		//If file has already been loaded, ask to overwrite
		if(hc->getEvalAttrFilename().empty() == false)
		{
			std::string overwrite;
			std::cout << "Evaluation Attributes file already loaded.  Overwrite? [yes/no]: ";
			std::cin >> overwrite;

			if(overwrite.compare("no") == 0 || overwrite.compare("NO") == 0)
			{
				std::cout << std::endl;
				return result;
			}
		}
		
		std::string eval_filename;
		std::cout << "Please Input Evaluation Attributes Filepath: ";
		std::cin >> eval_filename;

		hc->loadEvalAttrFile(eval_filename);
		break;
	}
	*/
}