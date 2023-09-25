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
        
     	

