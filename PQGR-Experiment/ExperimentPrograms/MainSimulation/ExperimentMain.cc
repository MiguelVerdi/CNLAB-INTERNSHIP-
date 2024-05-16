#include "HighwayExperiment.h"
/*
    File: WirelessTopology.cpp
    Author: Miguel Verdi
	Mail: verdi.resendiz.miguel@gmail.com
	Brief: This program takes a database in csv format that represents a series of vehicle-to-vehicle communication simulations. These simulations occur at a certain time T, where there are different clusters, different directions. 
	The program calculates different communication parameters for each of these cases, the complete code specification is shown below. 
*/





int
main(int argc, char **argv)
{
    
	// HighwayExperiment highwayExperiment1;
	HighwayExperiment highwayExperiment2;
	HighwayExperiment highwayExperiment3;
	HighwayExperiment highwayExperiment4;

	// std::string fileName1 = "PQGR/highwayVsparse_PQGR.csv";
	std::string fileName2 = "PQGR/highwaySparse_PQGR.csv";
	std::string fileName3 = "PQGR/highwayUsparse_PQGR.csv";
	std::string fileName4 = "PQGR/highwayDense_PQGR.csv";

	// highwayExperiment1.Configure (argc, argv, fileName1);
	// highwayExperiment1.openFile();
	// highwayExperiment1.runAll();

	highwayExperiment2.Configure (argc, argv, fileName2);
	highwayExperiment2.openFile();
	highwayExperiment2.runAll();

	highwayExperiment3.Configure (argc, argv, fileName3);
	highwayExperiment3.openFile();
	highwayExperiment3.runAll();

	highwayExperiment4.Configure (argc, argv, fileName4);
	highwayExperiment4.openFile();
	highwayExperiment4.runAll();

	// HighwayExperiment highwayExperimentDebug;
	// std::string fileName1 = "DataFordebug.csv";
	// // std::string fileName1 = "highwayVsparse_DGGR.csv";
	// highwayExperimentDebug.Configure (argc, argv, fileName1);
	// highwayExperimentDebug.openFile();
	// highwayExperimentDebug.runAll();
	return(0);
}
