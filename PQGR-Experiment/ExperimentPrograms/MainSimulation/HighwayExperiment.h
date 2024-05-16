#ifndef HIGHWAY_EXPERIMENT_H
#define HIGHWAY_EXPERIMENT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/onoff-application.h"
#include "ns3/netanim-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/olsr-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/v4traceroute-helper.h"
#include <unistd.h>
using namespace ns3;


class HighwayExperiment 
{
public:
	/**
	 * @brief Configure the experiment based on command-line arguments.
	 * 
	 * This method parses command-line arguments and configures the experiment
	 * And specifies the constat values of the simulation parameters (physical layer, time of sim, etc.)
	 * 
	 * @param argc The number of command-line arguments.
	 * @param argv An array of command-line argument strings.
	 * @return true if configuration was successful, false otherwise.
	 */
	bool Configure(int argc, char **argv, const std::string& fileName);

	/**
	 * @brief Open the input CSV file for data.
	 * 
	 * This method opens the CSV file containing input data for the experiment and updates the dataMatrix attribute with the information of the file.
	 * The file should have the format 
	 *  | Iteration (I)  | Time (Seconds) | GPCA | Cluster (C)   | node position | Direction (D)  | ID | X  | Y  |   V   | 
	 *  Example 
	 *  |  5             |    303         | False|   1           |       1       |     0          |3020|384 |-4.8| 39.35 | 
 	 *	|  19            |    401         | True |   2           |       3       |     1          |2033|666.74|-8| 34.25 | 
	 */
	void openFile(); // Open the input CSV file

	/**
	 * @brief Run the entire experiment, iterating over clusters and collecting statistics.
	 * 
	 * This method runs the entire experiment, iterating over clusters of data and
	 * collecting statistics such as throughput, latency, and packet delivery ratio, etc.
	 */
	void runAll(); // Run the entire experiment
    

private:
    // Attributes
	bool printParametersTimeSlot;
	bool printSingleExperimentParameters;
	bool printConfigurationTimeSlot;
    // Count of received packets
    uint32_t g_packetCount = 0;
	float disconected = false; 
    // Server application port
    int SERVER;

    // Data rate (e.g., "90Mbps")
    std::string dataRate;

    // Packet size in bytes
    uint32_t packetSize;

    // Maximum bytes
    uint32_t maxBytes;

    // Maximum number of packages
    int maxPackages;

    // Transmission power in dBm
    double txPowerDbm;

    // Simulation time in seconds
    float simTime;

    // Start and stop times for sender and receiver
    float senderStart, receiverStart, senderStop, receiverStop;

    // Channel width in MHz
    int channelWidth;

    // Experiment variables of each particular simulation updated each (time, direction and cluster)
    float SingleExpThroughput; //Computed as the smallest throughput of each two hops in the simulation
    float SingleExpLatency; //Computed as the sum of the latency beween hops
    float SingleExpPDR; // Packet Delivery Ratio (initially set to 1), calculated as the product of each two hops comunication 
	float SingleExpDist; //Computed as the distance between two nodes.  
	float SingleExpHops; //The number of nodes in the simulation. 
	float timeValue,GPCAValue,clusterValue;
	float disconDistance; //Measures when there is an extreme distance 
    // This times are for the data reading, are independent of the actual simulation time 
    float totalTime, firstTime;
	float numberOfExperiments = 0;
	
    // Time slot variables
    int totalClusters, cluster;

    // Statistics for throughput
    float timeSlotMinTh, timeSlotMaxTh, timeSlotAvgTh, sumTh;

    // Statistics for PDR
    float timeSlotMaxPDR, timeSlotMinPDR, timeSlotAvgPDR, sumPDR;

    // Statistics for latency
    float timeSlotMaxLat, timeSlotMinLat, timeSlotAvgLat, sumLat;

	// Statistics for distances
    float timeSlotMaxDist, timeSlotMinDist, timeSlotAvgDist, sumDist;

	// Statistics for hops
    float timeSlotMaxHop, timeSlotMinHop, timeSlotAvgHop, sumHops;

    // Output CSV file
    std::ofstream csvFile;

    // PHY mode
    std::string phyMode;

    // Matrix to store input data
    std::vector<std::vector<double>> dataMatrix;

    // Input and output file names
    std::string fileNameInput;

    // Private methods
	/**
	 * @brief Iterate over clusters of data.
	 * 
	 * This method reads the matrix of input data and iterates over the clusters in each time
	 * it calls the runExperiment method.
	 * 
	 * @param input A matrix of input data.
	 */
	void iterateClusters(const std::vector<std::vector<double>>& input); // Iterate over clusters

	/**
	 * @brief Run an experiment based on cluster data.
	 * 
	 * This method takes a matrix of input data representing a cluster and performs
	 * an experiment, collecting relevant statistics and results.
	 * It runs for each time, cluster and direction
	 * 
	 * @param input A matrix of input data representing a cluster.
	 */
	bool runExperiment(const std::vector<std::vector<double>>& input); // Run an experiment

	/**
	 * @brief Create IP addresses based on input data.
	 * 
	 * This method takes a vector of numerical input data and generates corresponding
	 * IP addresses, returning them as a vector of strings.
	 * All the neworks between hops should be in the form 10.x.x.x
	 * The second position represents the type (1,2,3) of the vehicle that is sending.
     * The third position represents the ID of the car without the class
     *  The fourth position represents the sender or reciver, so if its 2 is the reciver and if it is 1 represents the sender.
	 *  General example: 
	 * 
		If the car 3031 is sending to car 2021.


				Net (10.3.31.0)               
		other car  *  *  *  *  * # (3031)#  *  *  *  *  *  *  *  *  * # (2021)# * *  * * * * other car next iP (10.2.21.0)
						*                            *                      
										*                            *
					IP(10.3.31.1)			  IP(10.3.32.2)					


		if the source vehicle is sending IP(10.0.1.x)

	 * 
	 * @param input A vector of numerical input data.
	 * @return A vector of string IP addresses.
	 */
	std::vector<std::string> createIP(const std::vector<double>& input); // Create IP addresses

	/**
	 * @brief Calculate and collect experiment parameters.
	 * 
	 * This method calculates the max,min of the thoughput, latency, pdr, latency, distance and hops.
	 */
	void getParameters(); // Calculate parameters

	/**
	 * @brief Calculate and collect average values.
	 * 
	 * This method calculates the average of the thoughput, latency, pdr, latency, distance and hops.s.
	 */
	void getAverages(); // Calculate averages

	/**
	 * @brief This function converst the input file highway*_PQGR.csv to output highway*_PQGR_result.csv  .
	 * 
	 * @param input A vector of numerical input data.
	 * @return A vector of string IP addresses.
	 */
	std::string addResultToFileName(const std::string& fileNameInput);

	void writeResults();


};




bool 
HighwayExperiment::Configure (int argc, char **argv, const std::string& fileName)
{
	
    CommandLine cmd (__FILE__);
    cmd.AddValue ("file", "Write file to open", fileNameInput);
 	cmd.Parse (argc, argv);
	fileNameInput = fileName;


    //Comunication settings
	SERVER = 9;  //Aplication port of the server.
	dataRate = "90Mbps"; //Data rate of sending in the on off aplication.
	phyMode = "VhtMcs6"; //For the physical layer configuration
	packetSize = 500; // In bytes. The packet size to be sent.  (500)
	txPowerDbm = 35.0; //In dBm. The power of transmition.
	channelWidth = 40; //In MHz.
	maxPackages = 10000; //The upper limit of packages the onoff aplication will send.  (20000)
	maxBytes = packetSize*maxPackages;
	disconDistance = 90; //Lower limit for too far vehicles.
	simTime = 1.0; //sec
	senderStart = 0.0;//sec
	receiverStart = 0.0;//sec
	senderStop = simTime;//sec
	receiverStop = simTime;//sec


	printParametersTimeSlot = false; 
	printSingleExperimentParameters = false;
	printConfigurationTimeSlot = false;



	
 	return true;
}

void 
HighwayExperiment::openFile(){  
    std::ifstream file(fileNameInput); //Name of the file to be input. 

	if (!file.is_open()) {std::cerr << "Failed to open the file." << std::endl;}

    std::string line;
    while (std::getline(file, line)) {

        std::vector<double> tokens; // Use double for both integers and floats
        std::istringstream tokenStream(line);
        std::string token;

        while (std::getline(tokenStream, token, ',')) {
            // Convert string token to double (integers and floats)
            tokens.push_back(std::stod(token));
        }

        dataMatrix.push_back(tokens); //Appends the data in a matrix with float values
    }


    file.close();
	  
	
}


std::string HighwayExperiment::addResultToFileName(const std::string& fileNameInput) {
    // Find the position of the last dot (.) in the string
    size_t lastDotPosition = fileNameInput.rfind('.');

    // Check if a dot was found and it's not the first character
    if (lastDotPosition != std::string::npos && lastDotPosition > 0) {
        // Extract the file name without extension
        std::string fileNameWithoutExtension = fileNameInput.substr(0, lastDotPosition);

        // Add "_Result" and then add the file extension
        std::string fileNameOutput = fileNameWithoutExtension + "_Result" + fileNameInput.substr(lastDotPosition);

        return fileNameOutput;
    } else {
        // If there's no file extension, return an empty string or handle the error as needed
        return "";
    }
}


void
HighwayExperiment::runAll(){

	
	double totalI;
	
	//Getting the last iteration element to iterate over it.
	for (const auto& row:dataMatrix) {totalI = row[0];}//Get the total number of iterations
	for (const auto& row:dataMatrix) {totalTime = row[1];}//Get the last time simulation

	firstTime = dataMatrix[0][1]; //Get the time value the simulation is starting

	std::string fileNameOutput = addResultToFileName(fileNameInput);
	csvFile.open(fileNameOutput, std::ios::app); 

	for (int I = 1; I <= totalI + 1; I++){	

		std::vector<std::vector<double>> iterationI;
		for (const auto& row:dataMatrix) {if(row[0] == I){iterationI.push_back(row);}}  //Creates an array with all the clusters in a time. 
		for (const auto& row:iterationI) { totalClusters = row[3]; } //Gets the number of clusters in this time  
		
		float simProgress = std::round(100*I/totalI); //For printing how many simulations have been ran. 
		if(simProgress < 100){ 
			std::cout<< "\n - Simulation progress: " << simProgress << "% " << std::endl; 
			std::cout<< "File: " << fileNameInput << std::endl;
		}
		else{
			std::cout<< "\n - End of the simulation" << std::endl; 
		}
		
		iterateClusters(iterationI);	
	}


	csvFile.close();


}

void 
HighwayExperiment::getParameters(){

	if (numberOfExperiments == 1) {
		// When ther first cluster, initialize the value with the first value.
		
		// Sum of throughput
		sumTh = SingleExpThroughput;

		// Sum of latency
		sumLat = SingleExpLatency;

		// Sum of Packet Delivery Ratio (PDR)
		sumPDR = SingleExpPDR;

		// Sum of distance
		sumDist = SingleExpDist;

		// Sum of hops
		sumHops = SingleExpHops;

		// Set initial values for max and min for each statistic within the time slot.
		
		// Maximum throughput
		timeSlotMaxTh = SingleExpThroughput;

		// Minimum throughput
		timeSlotMinTh = SingleExpThroughput;

		// Maximum latency
		timeSlotMaxLat = SingleExpLatency;

		// Minimum latency
		timeSlotMinLat = SingleExpLatency;

		// Maximum PDR
		timeSlotMaxPDR = SingleExpPDR;

		// Minimum PDR
		timeSlotMinPDR = SingleExpPDR;

		// Maximum distance
		timeSlotMaxDist = SingleExpDist;

		// Minimum distance
		timeSlotMinDist = SingleExpDist;

		// Maximum hops
		timeSlotMaxHop = SingleExpHops;

		// Minimum hops
		timeSlotMinHop = SingleExpHops;

	} else {
		// When the cluster is not the first one it computes the statistics.
		
		// Update the sum of throughput with the current value
		sumTh += SingleExpThroughput;

		// Update the sum of latency with the current value
		sumLat += SingleExpLatency;

		// Update the sum of Packet Delivery Ratio (PDR) with the current value
		sumPDR += SingleExpPDR;

		// Update the sum of distance with the current value
		sumDist += SingleExpDist;

		// Update the sum of hops with the current value
		sumHops += SingleExpHops;

		// Track the maximum values within the time slot for each statistic.
		
		// Update the maximum throughput if the current value is greater
		if (timeSlotMaxTh < SingleExpThroughput) {
			timeSlotMaxTh = SingleExpThroughput;
		}

		// Update the maximum latency if the current value is greater
		if (timeSlotMaxLat < SingleExpLatency) {
			timeSlotMaxLat = SingleExpLatency;
		}

		// Update the maximum PDR if the current value is greater
		if (timeSlotMaxPDR < SingleExpPDR) {
			timeSlotMaxPDR = SingleExpPDR;
		}

		// Update the maximum distance if the current value is greater
		if (timeSlotMaxDist < SingleExpDist) {
			timeSlotMaxDist = SingleExpDist;
		}

		// Update the maximum hops if the current value is greater
		if (timeSlotMaxHop < SingleExpHops) {
			timeSlotMaxHop = SingleExpHops;
		}

		// Track the minimum values within the time slot for each statistic.
		
		// Update the minimum throughput if the current value is smaller
		if (timeSlotMinTh > SingleExpThroughput) {
			timeSlotMinTh = SingleExpThroughput;
		}

		// Update the minimum latency if the current value is smaller
		if (timeSlotMinLat > SingleExpLatency) {
			timeSlotMinLat = SingleExpLatency;
		}

		// Update the minimum PDR if the current value is smaller
		if (timeSlotMinPDR > SingleExpPDR) {
			timeSlotMinPDR = SingleExpPDR;
		}

		// Update the minimum distance if the current value is smaller
		if (timeSlotMinDist > SingleExpDist) {
			timeSlotMinDist = SingleExpDist;
		}

		// Update the minimum hops if the current value is smaller
		if (timeSlotMinHop > SingleExpHops) {
			timeSlotMinHop = SingleExpHops;
		}
}

}

void 
HighwayExperiment::getAverages(){
	// Calculate the average latency within the time .
	timeSlotAvgLat = sumLat / (numberOfExperiments );

	// Calculate the average Packet Delivery Ratio (PDR) within the time .
	timeSlotAvgPDR = sumPDR / (numberOfExperiments );

	// Calculate the average throughput within the time .
	timeSlotAvgTh = sumTh / (numberOfExperiments );

	// Calculate the average distance within the time .
	timeSlotAvgDist = sumDist / (numberOfExperiments );

	// Calculate the average number of hops within the time.
	timeSlotAvgHop = sumHops / (numberOfExperiments );

}

void 
HighwayExperiment::writeResults(){

	
	if(printParametersTimeSlot){
		std::cout<< "\n=========== result =========== "  << std::endl;
		std::cout<< "Time: " << timeValue  << " s" << std::endl; 
		std::cout<< "Total clusters: " << clusterValue << std::endl; 
		std::cout<< "GPCA: " << GPCAValue << std::endl; 
		
		std::cout<< "Max throughput: " << timeSlotMaxTh << " Mbps" << std::endl;
		std::cout<< "Avg throughput: " << timeSlotAvgTh << " Mbps" <<std::endl;
		std::cout<< "Min throughput: " << timeSlotMinTh << " Mbps" << std::endl;

		std::cout<< "Max latency: " << timeSlotMaxLat*1000 <<  " ms" << std::endl;
		std::cout<< "Avg latency: " << timeSlotAvgLat*1000 << " ms" << std::endl;
		std::cout<< "Min latency: " << timeSlotMinLat*1000 << " ms" << std::endl;

		std::cout<< "Max PDR: " << timeSlotMaxPDR*100 << " %"<< std::endl;
		std::cout<< "Avg PDR: " << timeSlotAvgPDR*100 << " %" << std::endl;
		std::cout<< "Min PDR: " << timeSlotMinPDR*100 << " %" << std::endl;

		std::cout<< "Max Distance: " << timeSlotMaxDist<< " m"<< std::endl;
		std::cout<< "Avg Distance: " << timeSlotAvgDist << " m" << std::endl;
		std::cout<< "Min Distance: " << timeSlotMinDist << " m" << std::endl;

		std::cout<< "Max Hops: " << timeSlotMaxHop << std::endl;
		std::cout<< "Avg Hops: " << timeSlotAvgHop << std::endl;
		std::cout<< "Min Hops: " << timeSlotMinHop << std::endl;
	}

	if (csvFile.is_open()) {
		csvFile << timeValue << "," << clusterValue << "," << GPCAValue << ","
			<< timeSlotMaxTh << ", " << timeSlotAvgTh << ", " << timeSlotMinTh  << ","
			<< timeSlotMaxLat << "," << timeSlotAvgLat << "," << timeSlotMinLat << ","
			<< timeSlotMaxPDR << "," << timeSlotAvgPDR << "," << timeSlotMinPDR <<  ","
			<< timeSlotMaxDist << "," << timeSlotAvgDist << "," << timeSlotMinDist <<  "," 
			<< timeSlotMaxHop << "," << timeSlotAvgHop << "," << timeSlotMinHop<< "\n";
	} else {
		std::cerr << "Error opening CSV file for writing." << std::endl;
	}
	



}


void 
HighwayExperiment::iterateClusters (const std::vector<std::vector<double>>& arrayI){

	double totalC; //Number of clusters
	for (const auto& row:arrayI) {totalC = row[3];} //Get the total number of clusters in a time. 
	int C;
	for (C = 1; C <= totalC; C++){

		std::vector<std::vector<double>> iterationClusterF; //forward cluster matrix of vehicles
		std::vector<std::vector<double>> iterationClusterB; //Backward cluster matrix of vehicles
		
		
		for (const auto& row:arrayI) {

			if(row[3] == C && row[5] == 0){iterationClusterF.push_back(row);} //If forward creates the forward clusters matrix 
			if(row[3] == C && row[5] == 1){iterationClusterB.push_back(row);} //If backward creates the backward clusters matrix 

		} 

		//changes the order of the list, starting from the header to the las vehicle, since the direction is the oposite.
		std::reverse(iterationClusterB.begin(), iterationClusterB.end());  


		bool ranF = true; 
		ranF = runExperiment(iterationClusterF);
		if(ranF == true){getParameters(); }
	

		bool ranB = true;
		ranB = runExperiment(iterationClusterB);
		if(ranB == true){getParameters();}
		

		if(C == totalC){
			getAverages();
			writeResults();
			numberOfExperiments = 0; 
		}
		
	}
	
	

	
	
	

	SingleExpThroughput = 0; //Computed as the smallest throughput of each two hops in the simulation
	SingleExpLatency = 0; //Computed as the sum of the latency beween hops
	SingleExpPDR = 0; // Packet Delivery Ratio (initially set to 1), calculated as the product of each two hops comunication 
	SingleExpHops = 0 ; //The number of nodes in the simulation. 
	SingleExpDist = 0; 


}

bool 
HighwayExperiment::runExperiment (const std::vector<std::vector<double>>& arrayVehicles){
	//Integer value to save the number of nodes or total hops in a simulation
	int nNodes = 0;
	
	// An integer variable 'direction' which may be unused in the current context.
	int direction;

	// An integer variable that identifies what time iteration is running.
	int time;

	// A boolean variable 'GPCA' indicating if the courrent simulation is GPCA or not .
	bool GPCA;

	for(const auto& row : arrayVehicles){
		time = row[1]; 
		direction = row[5];
		cluster = row[3];
		GPCA = row[2];
		nNodes= nNodes + 1;
	}


	if (nNodes > 1){  
		SingleExpHops = nNodes;

		
		if(printConfigurationTimeSlot){
			std::cout<< "\n------------------------------ Data  ----------------------------------- \n";
			std::cout << "I \t T  \tGPCA \t C \t Node \t D  \t ID \t  x  \t y \t v" << std::endl;
			for (const auto& row : arrayVehicles){  
					for (const auto& value : row){std::cout << value  << "\t";}
					std::cout << std::endl; 
				}
			std::cout<< "----------------------------------------------------------------------------- \n ";
			std::cout<< "Time: " << time << std::endl;
			std::cout<< " Number of nodes: " << nNodes << std::endl;
			std::cout<< " Number of cluster: " << cluster << std::endl;
			std::cout<< " Direction: " << direction << std::endl;
			std::cout<< " GPCA: " << GPCA << std::endl;
		}
	
		
		timeValue  = time; 
		clusterValue = cluster; 
	    GPCAValue = GPCA;


		int sender = 0; 
		int receiver = nNodes - 1; 


		//double array with the ID of the vehicles.
		std::vector<double> IDarray; 
		
		//gets an array with all the ID's of the experiment 
		for (const auto& row : arrayVehicles) {IDarray.push_back(row[6]);} 

		//Creates the set of IP addresses in the experiment  
		std::vector<std::string> IPaddresses = createIP(IDarray); 

		
	    // Define the number of connections based on the number of nodes minus one.
		int CONECTIONS(nNodes - 1);

		// Create a container for all nodes.
		NodeContainer nodes;

		// Create an array of NodeContainer instances for connections.
		NodeContainer c[CONECTIONS];

		// Create 'nNodes' nodes.
		nodes.Create(nNodes);

		// Create containers for network devices for each connection.
		NetDeviceContainer devices[CONECTIONS];

		// Create helpers for configuring the wireless PHY, MAC, and channel for each connection.
		YansWifiPhyHelper wifiPhy[CONECTIONS];
		WifiHelper wifi[CONECTIONS];
		YansWifiChannelHelper wifiChannel[CONECTIONS];
		WifiMacHelper wifiMac;

		// Set the Wi-Fi MAC type to AdhocWifiMac.
		wifiMac.SetType("ns3::AdhocWifiMac");

		// Loop through each connection.
		for (int link = 0; link < CONECTIONS; link++) {
			// Add nodes to the NodeContainer for this connection.
			c[link].Add(nodes.Get(link));
			c[link].Add(nodes.Get(link + 1));

			// Configure the wireless channel properties for this connection.
			wifiChannel[link].SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
			wifiChannel[link].AddPropagationLoss("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue(3.0));

			// Set the Wi-Fi standard for this connection.
			wifi[link].SetStandard(WIFI_STANDARD_80211ac);

			// Set the remote station manager and data/control modes.
			wifi[link].SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue(phyMode), "ControlMode", StringValue(phyMode));

			// Configure the physical layer for this connection.
			wifiPhy[link].SetChannel(wifiChannel[link].Create());
			wifiPhy[link].SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
			wifiPhy[link].Set("TxPowerStart", DoubleValue(txPowerDbm));
			wifiPhy[link].Set("TxPowerEnd", DoubleValue(txPowerDbm));

			// Install the Wi-Fi devices for this connection.
			devices[link] = wifi[link].Install(wifiPhy[link], wifiMac, c[link]);
		}

		// Set the channel width for all Wi-Fi devices in the simulation.
		Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue(channelWidth));

		// Create an AODV routing helper.
		Ipv4StaticRoutingHelper staticRouting;
		Ipv4ListRoutingHelper list;

		// Create an InternetStackHelper for setting up the protocol stack.
		InternetStackHelper stack;

		list.Add(staticRouting, 10); // Static routing is given a lower priority (10).

		// Set the routing helper list in the InternetStackHelper.
		stack.SetRoutingHelper(list);

		// Install the protocol stack on all nodes.
		stack.InstallAll();
		// Create Ipv4StaticRoutingHelper and Ipv4ListRoutingHelper for configuring routing.
		
		// Create an array of Ipv4AddressHelper for addressing connections.
		Ipv4AddressHelper address[CONECTIONS];

		// Create an array of Ipv4InterfaceContainer for managing network interfaces.
		Ipv4InterfaceContainer interface[CONECTIONS];

		// Initialize an Ipv4Address variable.
		ns3::Ipv4Address ipv4Address;

		// Loop through each connection.
		for (int link = 0; link < CONECTIONS; link++) {
			// Convert an IP address from a string and assign it to ipv4Address.
			ipv4Address = IPaddresses[link].c_str();
			
			// Set the base IP address and subnet mask for this connection.
			address[link].SetBase(ipv4Address, "255.255.255.0"); // Addressing two nodes in the same network.
			
			// Assign the address to the devices for this connection.
			interface[link] = address[link].Assign(devices[link]);
		}
		
	

		for (int node = 0; node < nNodes - 1; node++) {
		 	ns3::Ptr<ns3::Ipv4StaticRouting> staticRoute = staticRouting.GetStaticRouting(nodes.Get(node)->GetObject<ns3::Ipv4>());
			Ipv4Address gatewayAddress = interface[node].GetAddress(1);
			
			if(node == 0){
				staticRoute  ->AddNetworkRouteTo(Ipv4Address(interface[receiver - 1].GetAddress(1)), Ipv4Mask("255.255.255.0"),gatewayAddress,1);
			}else{

				staticRoute  ->AddNetworkRouteTo(Ipv4Address(interface[receiver - 1].GetAddress(1)), Ipv4Mask("255.255.255.0"),gatewayAddress,2);
			}
			
		}

		
	
		// Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("RoutesStatic.routes", std::ios::out);
		// staticRouting.PrintRoutingTableAllAt(Seconds(0.5), routingStream);

		// Create a MobilityHelper for managing node mobility.
		MobilityHelper mobility;

		// Set the mobility model to ConstantVelocityMobilityModel.
		mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");

		// Create a ListPositionAllocator for node positions.
		Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();


		// Loop through each row in the arrayVehicles.
		for (const auto& row : arrayVehicles) {
			// Extract X and Y positions from the row.
			double posX = row[7];
			double posY = row[8];
			
			// Add the position to the position allocator.
			positionAlloc->Add(Vector(posX, posY, 0.0));
		}

		// Set the position allocator for node mobility.
		mobility.SetPositionAllocator(positionAlloc);

		// Install the mobility model on nodes.
		mobility.Install(nodes);

		// Initialize a node index.
		int node = 0;

		// Loop through each row in the arrayVehicles again.
		for (const auto& row : arrayVehicles) {
			// Extract velocity from the row.
			double vel = row[9];
			
			// Get the ConstantVelocityMobilityModel for the current node.
			Ptr<ConstantVelocityMobilityModel> mob = nodes.Get(node)->GetObject<ConstantVelocityMobilityModel>();
			
			// Set the velocity for the mobility model.
			mob->SetVelocity(Vector(vel, 0, 0));
			
			// Increment the node index.
			node = node + 1;
		}

		// Get the MobilityModel for the sender and receiver nodes.
		Ptr<MobilityModel> mobilityModelA = nodes.Get(sender)->GetObject<MobilityModel>();
		Ptr<MobilityModel> mobilityModelB = nodes.Get(receiver)->GetObject<MobilityModel>();

		// Calculate the constant distance between the sender and receiver.
		float x1 = mobilityModelA->GetPosition().x - mobilityModelB->GetPosition().x;
		float y1 = mobilityModelA->GetPosition().y - mobilityModelB->GetPosition().y;
		float d = sqrt(pow(x1, 2) + pow(y1, 2));

		for(int n = 0;  n < nNodes - 1; n++){

			// Get the MobilityModel for the sender and receiver nodes.
			Ptr<MobilityModel> nodeA = nodes.Get(n)->GetObject<MobilityModel>();
			Ptr<MobilityModel> nodeB = nodes.Get(n + 1)->GetObject<MobilityModel>();

			// Calculate the constant distance between the sender and receiver.
			float x = nodeA->GetPosition().x - nodeB->GetPosition().x;
			float y = nodeB->GetPosition().y - nodeA->GetPosition().y;
			float distTwoHops = sqrt(pow(x, 2) + pow(y, 2));	 
			if(distTwoHops > disconDistance){ 
				disconected = true;
			}
		}

		// Configure the OnOffHelper for sender.
		OnOffHelper onOffHelper("ns3::UdpSocketFactory", Address());
		onOffHelper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
		onOffHelper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
		onOffHelper.SetAttribute("DataRate", StringValue(dataRate));
		onOffHelper.SetAttribute("PacketSize", UintegerValue(packetSize));
		onOffHelper.SetAttribute("MaxBytes", UintegerValue(maxBytes));

		// Set the remote address for sender.
		AddressValue remoteAddress(InetSocketAddress(interface[receiver - 1].GetAddress(1), SERVER));
		onOffHelper.SetAttribute("Remote", remoteAddress);

		// Install the OnOff application on the sender node.
		ApplicationContainer senderApp = onOffHelper.Install(nodes.Get(sender));

		// Configure the start and stop times for senderApp.
		senderApp.Start(Seconds(senderStart));
		senderApp.Stop(Seconds(senderStop));

		// Configure the PacketSinkHelper for receiver.
		PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", Address());
		sinkHelper.SetAttribute("Local", AddressValue(InetSocketAddress(Ipv4Address::GetAny(), SERVER)));

		// Install the PacketSink application on the receiver node.
		ApplicationContainer receiverApp = sinkHelper.Install(nodes.Get(receiver));

		// Configure the start and stop times for receiverApp.
		receiverApp.Start(Seconds(receiverStart));
		receiverApp.Stop(Seconds(receiverStop));

	
		// Create a pointer to a FlowMonitor instance.
		Ptr<FlowMonitor> monitor;

		// Create a FlowMonitorHelper to set up flow monitoring.
		FlowMonitorHelper flowMonitor;

		// Install flow monitoring on all nodes.
		monitor = flowMonitor.InstallAll();

		// Stop the simulation at the specified simulation time.
		Simulator::Stop(Seconds(simTime));

		// Run the simulation until the specified stop time.
		Simulator::Run();

		// Serialize and save flow monitor data to an XML file for NetAnim use.
		//monitor->SerializeToXmlFile("MEASURMENTS.xml", true, true);

		// Create a pointer to an Ipv4FlowClassifier to access Ipv4 flow information.
		Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonitor.GetClassifier());

		// Get statistics for the flows monitored by the FlowMonitor.
		std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

		// Create an iterator to traverse the flow statistics map.
		std::map<FlowId, FlowMonitor::FlowStats>::const_iterator statsPointer;



		// Iterate through the flow statistics map.
		for (statsPointer = stats.begin(); statsPointer != stats.end(); ++statsPointer) {
			// Find the corresponding flow tuple using the Ipv4FlowClassifier.
			Ipv4FlowClassifier::FiveTuple monitor = classifier->FindFlow(statsPointer->first);

			// Check if the flow matches the sender and receiver addresses.
			if ((monitor.sourceAddress == interface[sender].GetAddress(0) && monitor.destinationAddress == interface[receiver - 1].GetAddress(1))) {
				// Calculate Packet Delivery Ratio (PDR) as the ratio of received packets to transmitted packets.
				float PDR = (static_cast<float>(statsPointer->second.rxPackets)) / (static_cast<float>(statsPointer->second.txPackets));

				// Calculate the transmission time as the difference between the time of the last received packet and the time of the first transmitted packet.
				float transmitionTime = statsPointer->second.timeLastRxPacket.GetSeconds() - statsPointer->second.timeFirstTxPacket.GetSeconds();

				// Calculate the total number of received packets.
				float totRecPackets = statsPointer->second.rxPackets;

				// Calculate throughput in Mbps.
				float throughput = (8 * totRecPackets * packetSize) / (transmitionTime * 1000000);

				// Calculate latency as the sum of delays divided by the number of received packets.
				ns3::Time delaySum = statsPointer->second.delaySum;
				float delaySumFloat = static_cast<float>(delaySum.GetSeconds());
				float latency = delaySumFloat / (statsPointer->second.rxPackets);

				// Add a constant delay to the latency.
				latency = latency + 0.002;


				if(disconected == true){
					//Default low boud values when the comunication is lost
					throughput = 60;
					PDR = .9;
					latency = 0.002*(nNodes -1); 
					disconected = false; 
				}

				// Uncomment this block to print detailed communication results for two hops simulation.
				if(printParametersTimeSlot){
					std::cout << "\n Flow " << " (" << monitor.sourceAddress << " -> " << monitor.destinationAddress << ")\n";
					std::cout << "\n --- Simulation results ----- \n";
					std::cout << "Throughput: " << throughput << " Mbps\n";
					std::cout << "Latency: " << latency*1000 <<  " ms\n"; // Divided by the number of packages because it's the mean value.
					std::cout << "PDR: " << PDR*100 << " %\n";
					std::cout << "Tx Packages:   " << statsPointer->second.txPackets << "\n";
					std::cout << "Rx Packages:   " << statsPointer->second.rxPackets << "\n";
					std::cout << "Lost packages:   " << statsPointer->second.lostPackets << "\n";
					std::cout << "distance:   " << d << "\n";
				}
				

				
				SingleExpThroughput = throughput; //Computed as the smallest throughput of each two hops in the simulation
				SingleExpLatency = latency; //Computed as the sum of the latency beween hops
				SingleExpPDR = PDR; // Packet Delivery Ratio (initially set to 1), calculated as the product of each two hops comunication 
				SingleExpHops = nNodes -1 ; //The number of nodes in the simulation. 
				SingleExpDist = d; 
				
			


				
			}
		}

		

		Simulator::Destroy();
		numberOfExperiments = numberOfExperiments + 1;
		return true;
	} else{
		return false;
	}



	
}


std::vector<std::string> HighwayExperiment::createIP(const std::vector<double>& arrayID) {
    std::vector<std::string> IPvector; // Vector to store generated IP addresses.
    double totalV; // Variable to compute the number of vehicles in the simulation.

    // Calculate the total number of vehicles by counting the elements in arrayID.
    for ([[maybe_unused]] const int& value : arrayID) {
        totalV = totalV + 1;
    }

    // Generate IP addresses for each vehicle.
    for (int V = 0; V <= totalV - 2; V++) {
        // Extract the type of vehicle (N1) and the vehicle ID (N2) from the arrayID.
        int N1 = static_cast<int>(arrayID[V] / 1000); // Get the type of vehicle.
        int N2 = static_cast<int>(arrayID[V]) % 1000;  // Get the ID of the vehicle.

        // Construct the IP address based on the vehicle type and ID.
        std::string ipAddress = "10." + std::to_string(N1) + "." + std::to_string(N2) + ".0";

        // Add the generated IP address to the IPvector.
        IPvector.push_back(ipAddress);
    }

    // Return the vector of generated IP addresses.
    return IPvector;
}
#endif // HIGHWAY_EXPERIMENT_H
