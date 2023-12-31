# CNLAB-INTERNSHIP 
This repository shows the work done during the internship at UVic's CNLAB, in BC, Canada. 
Different communication test programs are shown, the final objective was to perform a simulation of a V2X communication algorithm.

##Programs
It contains a folder with simulations called `P2P` and `Wireless`. In P2P, there are different examples of ns3 simulations with basic and complex topologies. In the Wireless folder, there are several examples of communication where the hops have different speeds, initial positions, or there are multiple hops.

##Netanim simulations
Contains .xml simulations for netAnim software showing communication between devices. 

##PQGR-Experiment
This folder contains the general procedure to simulate the experiment, the same was tested in a virtual machine with Ubuntu `20.04.6` with a `ns3.33` software, the procedure is described in detail as follows.

- There are 4 types of scenarios `*` = Dense, Sparse, Usparse, Vsparse

- Each scenario is described by a file with `.mat` data that was generated by another external program. 
These are located in the `InputFiles` file and are described as `highway*_PQGR.mat`, these will be the files you start working with. 

##Proposed algorithm
- There are 4 types of scenarios `*` = AODV,NPCA,PQGR,DGGR

- Each of the `InputFiles/Proposed_routing/newResults-0822/highway*_PQGR.mat` files is processed in Matlab Online using the file named `ExperimentmToCSVComparison.m`, you only have to copy paste the code in matlab and charge the file after running the program, it would ask for the file, this generates the files found in `ExerimentFiles` which are `ExperimentFiles/**/highway*_**.csv` files, containing the data from the .mat file but unpacked so that C++ can read them in a more orderly.

##Comparision with other algorithms
- There are 4 types of scenarios `*` = AODV,NPCA,PQGR,DGGR

- Each of the `InputFiles/Ruting_compare_0922/highway*_**.mat` files is processed in Matlab Online using the file named `ExperimentmToCSVProposed.m`, you only have to copy paste the code in matlab and charge the file after running the program, it would ask for the file, this generates the files found in `ExerimentFiles` which are `ExperimentFiles/**/highway*_**.csv` files, containing the data from the .mat file but unpacked so that C++ can read them in a more orderly.

- There is an special kind of experiment called DGGR, the files are found in `InputFiles/DGGR/highway*_DGGR.mat` this file is processed in `ExperimentmToCSVDGGR.m` you only have to copy paste the code in matlab and charge the file after running the program, it would ask for the file, this generates the files found in `ExerimentFiles` which are `ExperimentFiles/**/highway*_**.csv` files, containing the data from the .mat file but unpacked so that C++ can read them in a more orderly.

- These `MatlabProcessingResult/highway*_PQGR.csv` files are the input for the main C++ simulation, to run this file you must have installed the simulation software `ns3.33` and in the `ns-allinone-3.33/ns3-. 33` folder paste the `.csv` file, save the `ExperimentMain.cc` and `HighwayExperiment.h` program in `ns-allinone-3.33/ns3-.33/scratch` and to modify the input file you must specify it in the main function of `ExperimentMain.cc`.
  
- This experiment takes approximately 4 hours to run and generates the files `ExperimentResultFiles/highway*_PQGR_Result.csv`. 

- Finally this file is run in the python3 program `PlotFileExperiment.py` where you must specify in the file_path variable the file you want to display.
This generates the `ExperimentResultPlots/*/highway*_PQGR_Result_**_plot.png` files.

- Where ** = DataRate, Distance, Hops,Latency,PDR 

- In addition a final plot is generated with all these graphs together `ExperimentResultPlots/**/highway*_PQGR_Result_AllFigures.png`.

# General specifications of the simulation
- All the parameters can be changes in the `HighwayExperiment.h` for configurtion `HighwayExperiment::Configure()` 
## Simulation Time
Simulation time = 1s.

## Communication Specifications
- **Application Protocol:** onoff
- **Sending Rate:** 90Mbps
- **Max Packages:** 10,000 packages
- **Packet Size:** 500 bytes
- There is an special parameter called disconDistance that gives a lower bound for the cases the cars are to far, this case is 90 mts.
- It generates a multi-hop ad-hoc network in each experiment with static routing.

### Channel
- **Tx Power:** 35 dBm
- **Frequency:** 5.9GHz
- **Bandwidth:** 40MHz
- **Phy mode:** VhtMcs6

## Vehicle Classification
- **ID Format:** 4-numbers (type XXX)
- **Types:** 
  - 1 - Slow Vehicle
  - 2 - Normal Vehicle
  - 3 - Fast Vehicle
- Example: ID = 2012 represents vehicle 12 of normal velocity vehicles
- Special Vehicle with ID = 1 is the source vehicle

## IP Classification
- **Mask:** 255.255.255.0
- Networks between hops should be in the form 10.x.x.x
- Second position represents the sender's type (1,2,3)
- Third position represents the ID of the car without the class
- Fourth position represents the sender or receiver (2 for receiver, 1 for sender)
- Example: Car `3031` sending to car `2021`
In that case  the newtor io would be 10.3.31.0 and the sender would be IP(10.3.31.1) sending to receiver IP(10.3.32.2)
- If the source vehicle is sending, IP(10.0.1.x)

## Cars Simulation
- **Start Position and Start Velocity:** As the routing eval files.
- **Highway Specification:** 5Km.
- **Track of Highway:** 3.5mts

## Files `highway*_PQGR.mat` Unpack
- **Filename Format:** `highway*_PQGR.mat`
  - `*` = Dense, Sparse, Usparse, Vsparse

**Variables:**
1. **`Cluster_time.mat`:** Entire cluster information
   - First column: Timestamp
   - Second column: Cluster information of each time slot (ID, x location, y location, speed)
   - First row: Complete cluster information
   - Second row: Cluster head information

2. **`Routing_eval.mat`:** Entire routing information
   - First column: Timestamp
   - Second column: Cluster information of each time slot (ID, x location, y location, speed)
   - First row: Routing path of each cluster
   - Second row: Measured per-hop delay
   - Third column: Measured per-hop reliability
   - Fourth column: Measured per-hop channel capacity
   - Fifth column: Measured per-hop cost

3. **`T_pred_con.mat`:** Prediction time interval
   - Each value corresponds to the number of seconds for mobility prediction

4. **`T_update_con.mat`:** Time period to spot connection loss
   - Each value corresponds to the number of seconds to update the global topology

## Data Stacking
- `highway*_PQGR.mat` -> `var3 T_pred_con.mat`
- Column 1 is the time slot, the second column has cluster information (GPCA and LPCA)
- `*` = Dense, Sparse, Usparse, Vsparse

**GPCA:** Presented every T_update_con time slots iterations.
- 1. The column relays `Relays` contains the number of clusters in the simulation (Iterate over the three clusters)
- 2. Inside each cluster, the first row is the Forward path, and the second one is the backward pack (Iterate over these two)
- 3. Each row represents the (cluster ID, x position (m), y position (m), velocity (m/s))

**LPCA:** Presented every time iteration except when GPCA is presented.
- 1. Inside it, It has the information in `Relays` of GPCA (first row) and LPCA (second row), get the second row.
- 2. Inside the second row, there is information for each cluster (Iterate over each of these two clusters)
- 3. Inside each cluster, the first row is the Forward path, and the second one is the backward pack (Iterate over these two).
- 4. Each row represents the (cluster ID, x position (m), y position (m), velocity (m/s))

## Data Formatting for Simulation
- `highway*_PQGR.mat` -> `ExperimentData.m` -> `highway*_PQGR.csv`
- For the comparing is the same
- `*` = Dense, Sparse, Usparse, Vsparse

| Iteration (I) | Time (Seconds) | GPCA | Cluster (C) | Node Position | Direction (D) | ID  | X    | Y    | V     |
| --------------| --------------- | ---- | ----------- | ------------- | ------------- | --- | ---- | ---- | ----- |
| 5             | 303             | False| 1           | 1             | 0             | 3020| 384  | -4.8 | 39.35 |
| 19            | 401             | True | 2           | 3             | 1             | 2033| 666.74 | -8  | 34.25 |

- In direction, `0` means forward, `1` means backward.

## Data Writing
When running the simulation, the program generates a file `highway*_PQGR_simResults.csv` with the following headers:

| Time | Total Clusters | GPCA | Max Th | Avg Th | Min Th | Max Lat | Avg Lat | Min Lat | Max PDR | Avg PDR | Min PDR | Max Dist | Avg Dist | Min Dist | Max Hop | Avg Hop | Min Hop |
|------|----------------|------|--------|--------|--------|---------|---------|---------|---------|---------|---------|----------|----------|----------|---------|---------|---------|
| 271  | 3              | 1    | 89.0279| 88.941 | 88.809 | 0.0097876| 0.0078271| 0.0033623| 0.9879  | 0.9817  | 0.9771  | 95.5058  | 77.051   | 61.9015  | 4       | 3.4     | 2       |
| 279  | 3              | 0    | 89.3051| 89.0168| 88.9182| 0.0227674| 0.0130128| 0.0065037| 0.9881  | 0.9780  | 0.9549  | 60.8249  | 46.8088  | 39.3827  | 8       | 5       | 3       |

