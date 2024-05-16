%======================================================================
% MATLAB SCRIPT
%======================================================================
% File:       ExperimentmToCSV.m
% Author:     Miguel Verdi 
% Date:       September, 2023
% Version:    1.0
%======================================================================
% Description:
% This code takes and specific set of .mat data and unpacks it to get a
% .cvs file with specific headers to run a ns-3.33 experiment
% highway*_PQGR.mat -> var3 T_pred_con -> Column 1 is time slot, second column are cluster information in that time slot, there area two kinds (GPCA and LPCA)
% 
% * = Dense, Sparse, Usparse, Vsparse
% 
% GPCA: is presented each T_update_con time slots iterations.
%  - 1. The column relays  'Relays' contains the number of clusters in the simulation (Iterate over the three clusters) 
%  - 2. Iside each cluster the first row is Forward path and the second one is backward pack (Iterate over these two)
%  - 3. Each row represents the (cluster ID, x position (m), y position (m) , velocity (m/s))
% 
% LPC: Is presented each time iteratin except when GPCA is presented. 
% - 1.  Inside it It has the information in 'Relays' of GPCA (first row) and LPCA (second row), get the  second row. 
% - 2. Inside the second row there is information of each cluster  (Iterate over each these two clusters) 
% - 3. Iside each cluster the first row is Forward path and the second one is backward pack (Iterate over these two). 
% 
% - 4. Each row represents the (cluster ID, x position (m), y position (m) , velocity (m/s))
% 
% 
% Data formating for simulation
% ------------------------------------------------------------------
% highway*_PQGR.mat -> ExperimentData.m -> highway*_PQGR.csv
% 
% * = Dense, Sparse, Usparse, Vsparse
% 
%  | Iteration (I)  | Time (Seconds) | GPCA | Cluster (C)   | node position | Direction (D)  | ID | X  | Y  |   V   | 
%  |  5             |    303         | False|   1           |       1       |     0          |3020|384 |-4.8| 39.35 | 
%  |  19            |    401         | True |   2           |       3       |     1          |2033|666.74|-8| 34.25 | 
% 
% In direction 0 means forward, 1 meand backward.
%======================================================================

clc

% Selects the .mat file 
[fileName, filePath] = uigetfile('*.mat', 'Select a MAT file');

% Check if a file was selected
if fileName == 0
    error('No file selected.');
end

%Loads the selected file into 
fprintf('Selected file: %s\n', fileName);
data = load(fullfile(filePath, fileName));



% Specify the .csv file name
matBaseFileName = strrep(fileName, '.mat', '');
csvFileName = [matBaseFileName, '.csv'];


%Simulation variables
matrix = []; %Matrix for writing in the csv, contains all the information in a row each iteration.
GPCA = [];

%Variables from data
simulationIterations = length(data.routing_eval); %Total rows in the simulation (each row is an iteration) 

%vector = ["Iteration","Time", "GPCA","Cluster", "Node_position", "Direction", "ID","X","Y","V"]; %C-1 because that variable has a header, and 1 because backward

for I = 1:simulationIterations  %Iterates over all the time segments, it uses the variable I so the time segments are not just of 1 sec gap.
    
    if I == 1 | mod((I - 1), 11) == 0 
        isGPCA = true 
    else
        isGPCA = false
    
    end

    time = data.routing_eval(I,1);

    
    if isGPCA == true 

        ClusterCell = data.routing_eval(I,2); 
        [clusterRows, cols] = size(ClusterCell{1}); %Gets the total number of clusters
        ClusterCell = ClusterCell{1,1}; %Unpacks the cluster information .
        
        for C = 2:clusterRows
            
            cluster = ClusterCell{C,1}; %iterates over Relays C starts in 2. 
            forward = cluster{1,1}; %Clusters forward unpack and identified with 0. 
            backward = cluster{2,1}; %Clusters backwatd unpack and identified with 1. 

            %Checks if there is not an empty array of the backward cluster
            if size(backward) ~= 0 
                
                [forwardRows,cols] = size(forward); 
                %Unpacking of the forward data segment
                for forwardRow = 1:forwardRows 
                    nodeInfo = forward(forwardRow,1:end-1);
                    vector = [I,time{1},isGPCA,C-1,forwardRow,0]; %C-1 because that variable has a header, and 0 because forward
                    %Writes the matrix in the csv file
                    vector = [vector,nodeInfo]; 
                    matrix = vertcat(matrix, vector);
                 end %Iteration in forward rows

                 [backwarRows,cols] = size(backward);
                 %Unpacking of the backward data segment
                 for backwardRow = 1:backwarRows
                    nodeInfo = backward(backwardRow,1:end-1);
                    vector = [I,time{1},isGPCA,C-1,backwardRow,1]; %C-1 because that variable has a header, and 1 because backward
                    %Writes the matrix in the csv file
                    vector = [vector,nodeInfo];
                    matrix = vertcat(matrix, vector);
                 end %Iteration in forward rows

                
            end %Not empty cluster

            
    

        end %end of C loop


    else %sGPCA == False (the format is diferent) 
        
        ClusterCell = data.routing_eval(I,2);
        ClusterCell = ClusterCell{1}(3,1);
        [clusterRows, cols] = size(ClusterCell{1});

        for C = 1:clusterRows
            cluster = ClusterCell{1}(C); %iterates over Relays C starts in 2. 
 
            if size(cluster{1}) ~= 0 
                forward = cluster{1}(1);
                backward = cluster{1}(2);
    
                backward = backward{1}; %Data set for backward clusters
                forward = forward{1}; %Data set for foward clusters
    
                if size(backward) ~= 0
                    [forwardRows,cols] = size(forward);
                    
                    %Since the format is diferent in each direction, it first
                    %gets the forward data
                    for forwardRow = 1:forwardRows
                        nodeInfo = forward(forwardRow,:);%Becase ut has 4 elements
                        vector = [I,time{1},isGPCA,C,forwardRow,0]; %C-1 because that variable has a header, and 0 because forward
                        %Writing in the matrix of the cvs file 
                        vector = [vector,nodeInfo];
                        matrix = vertcat(matrix, vector);
                     end %Iteration in forward rows 
    
                     [backwarRows,cols] = size(backward);
                     for backwardRow = 1:backwarRows
                        nodeInfo = backward(backwardRow,:);
                        vector = [I,time{1},isGPCA,C,backwardRow,1]; %C-1 because that variable has a header, and 1 because backward
                        %Writing in the cvs file. 
                        vector = [vector,nodeInfo];
                        matrix = vertcat(matrix, vector);
                     end %Iteration in forward rows
    
    
                end %Not empty cluster
            end%Not empty backward
            

        end %end of C loop
 
    end %sGPCA == True or False


end %end of I loop;

%Writes int the cvsfile
writematrix(matrix,csvFileName);

%======================================================================
% End of script
%======================================================================
