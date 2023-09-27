#include "HighwayExperiment.h"
/*
    File: WirelessTopology.cpp
    Author: Miguel Verdi
	Mail: verdi.resendiz.miguel@gmail.com
	Brief: This program takes a database in csv format that represents a series of vehicle-to-vehicle communication simulations. These simulations occur at a certain time T, where there are different clusters, different directions. 
	The program calculates different communication parameters for each of these cases, the complete code specification is shown below. 
*/


/*
--------------------------------------------------------------
Simulation time = 1s.

Comunication specifications: 
------------------------------------------------------------------
Application protocol: onoff
Sending rate: 80Mbps
Max packages: 10,000 packages
packet size= 200bytes

Channel
Txpower 23 dBm
Frequency 5.9GHz
Bandwidth: 20MHz



vehicle classification 
------------------------------------------------------------------
ID: 4-numbers ( type  X X X )
The type could be 
1 - slow vehicle
2 - Normal vehicle 
3 - Faste vehicle.
ex. (ID = 2012) is the vehicle 12 of the normal velocity vehicles 

There is one special vehicle with ID = 1 that is the source vehicle. 



IP classification 
------------------------------------------------------------------
Mask = 255.255.255.0

All the neworks between hops should be in the form 10.x.x.x
	
The second position represents the type (1,2,3) of the vehicle that is sending.
The third position represents the ID of the car without the class
The fourth position represents the sender or reciver, so if its 2 is the reciver and if it is 1 represents the sender.
General example: 
If the car 3031 is sending to car 2021.


					Net (10.3.31.0)               
other car  *  *  *  *  * # (3031)#  *  *  *  *  *  *  *  *  * # (2021)# * *  * * * * other car next iP (10.2.21.0)
				  *                            *                      
                                 *                            *
			IP(10.3.31.1)			  IP(10.3.32.2)					


if the source vehicle is sending IP(10.0.1.x)


Cars simulation 
------------------------------------------------------------------
Start position and start velocity:  As the routing eval files.

Highway especification: 5Km.
Track of highway: 3.5mts


Files highway*_PQGR.mat unpack
------------------------------------------------------------------
filename format: highway*_PQGR.mat
* = Dense, Sparse, Usparse, Vsparse

variables:
  var1 - Cluster_time: entire cluster information;
    first column: timestamp
    second column: cluster information of each time slot (id, x location, y location, speed)
	first column: complete cluster information
	second column: cluster head information
	
  var2 - routing_eval: entire routing information;
    first column: timestamp
    second column: cluster information of each time slot (id, x location, y location, speed)
    	first column: routing path of each cluster
     	  first row: forward routing path (from cluster head to the edge)
     	  second row: backward routing path 
     	second column: measured per-hop delay
     	third column: measured per-hop reliability
     	fourth column: measured per-hop channel capacity
     	fifth column: measured per-hop cost
 
  var3 - T_pred_con: prediction time interval;
    each value corresponds to the number seconds for mobility prediction
   
  var4 - T_update_con: time period to spot connection loss
    each value corresponds to the number seconds to update the global topology
        
     	
     	
Data stracting 
------------------------------------------------------------------
highway*_PQGR.mat -> var3 T_pred_con -> Column 1 is time slot, second column are cluster information in that time slot, there area two kinds (GPCA and LPCA)

* = Dense, Sparse, Usparse, Vsparse

GPCA: is presented each T_update_con time slots iterations.
 - 1. The column relays  'Relays' contains the number of clusters in the simulation (Iterate over the three clusters) 
 - 2. Iside each cluster the first row is Forward path and the second one is backward pack (Iterate over these two)
 - 3. Each row represents the (cluster ID, x position (m), y position (m) , velocity (m/s))
     	
LPCA: Is presented each time iteratin except when GPCA is presented. 
- 1.  Inside it It has the information in 'Relays' of GPCA (first row) and LPCA (second row), get the  second row. 
- 2. Inside the second row there is information of each cluster  (Iterate over each these two clusters) 
- 3. Iside each cluster the first row is Forward path and the second one is backward pack (Iterate over these two). 

- 4. Each row represents the (cluster ID, x position (m), y position (m) , velocity (m/s))
     	

Data formating for simulation
------------------------------------------------------------------
highway*_PQGR.mat -> ExperimentData.m -> highway*_PQGR.csv

* = Dense, Sparse, Usparse, Vsparse

 | Iteration (I)  | Time (Seconds) | GPCA | Cluster (C)   | node position | Direction (D)  | ID | X  | Y  |   V   | 
 |  5             |    303         | False|   1           |       1       |     0          |3020|384 |-4.8| 39.35 | 
 |  19            |    401         | True |   2           |       3       |     1          |2033|666.74|-8| 34.25 | 

In direction 0 means forward, 1 meand backward.



Data writing 
------------------------------------------------------------------
highway*_PQGR.mat -> ExperimentData.m -> highway*_PQGR.csv -> highway*_PQGR_simResults.csv

-minTh means to run all the clusters and both directions of a sime time slot and get the min Throughput
-minTh means to run all the clusters and both directions of a sime time slot and get the max Throughput
-meanTh means to run all the clusters and both directions of a sime time slot and get the sum Throughput(i)/Nclusters
- latency is the max latency of all the clusters and directions in each time slot. 
- PDR mieans the min pdr of all the clusters and directions in each time slot. 

 Time (Seconds) | GPCA | N of Clustes  | Th_statistics (minTh,maxTh,avgTh)| latency (ms) | PDR
-----------------------------------------------------------------
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
