# CNLAB-INTERNSHIP Simulation Readme

## Simulation Time
Simulation time = 1s.

## Communication Specifications
- **Application Protocol:** onoff
- **Sending Rate:** 90Mbps
- **Max Packages:** 20,000 packages
- **Packet Size:** 500 bytes

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

