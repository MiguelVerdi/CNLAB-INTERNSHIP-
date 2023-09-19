import matplotlib.pyplot as plt
# Specify the path to your text file
file_path = 'highwayDense_results_PQRG.csv'

# Initialize empty lists to store data
data = []

# Open and read the file
with open(file_path, 'r') as file:
    # Iterate over each line in the file
    for line in file:
        # Split each line into a list of values using ',' as the delimiter
        values = line.strip().split(',')
        # Convert the values to the desired data types (e.g., float)
        values = [float(val) for val in values]
        # Append the values to the data list
        data.append(values)

# Print the data (list of lists)


time = [row[0] for row in data]
nClusters = [row[1] for row in data]
GPCA = [row[2] for row in data]

maxTh= [row[3] for row in data]
avgTh = [row[4] for row in data]
minTh = [row[5] for row in data]

maxLat = [1000*row[6] for row in data]
avgLat = [1000*row[7] for row in data]
minLat= [1000*row[8] for row in data]

maxPDR = [100*row[9] for row in data]
avgPDR = [100*row[10] for row in data]
minPDR = [100*row[11] for row in data]

maxDist= [row[12] for row in data]
avgDist = [row[13] for row in data]
minDist = [row[14] for row in data]

maxHop  = [row[15] for row in data]
avgHop = [row[16] for row in data]
minHop = [row[17] for row in data]




fig,ax = plt.subplots(1, 1, sharex=True)

# Plot data on the first subplot




ax.plot(time, maxHop, label='Max number of hops' , color= 'red')
ax.plot(time, avgHop, label='Avg number of hops', color = 'black')
ax.plot(time, minHop, label='Min number of hops', color = 'blue' )
ax.set_ylabel('Number of hops')
ax.set_xlabel('Time (s)')
ax.legend()
ax.grid()
'''
ax[0].plot(time, maxTh, label='Max throughput' , color= 'red')
ax[0].plot(time, avgTh, label='Avg throughput', color = 'black')
ax[0].plot(time, minTh, label='Min throughput', color = 'blue' )
ax[0].set_xlabel('Time (s)')
ax[0].set_ylabel('Data Rate (Mbps)')
ax[0].legend()
ax[0].grid()


ax[1].plot(time, maxLat, label='Max latency' , color= 'red')
ax[1].plot(time, avgLat, label='Avg latency', color = 'black')
ax[1].plot(time, minLat, label='Min latency', color = 'blue' )
ax[1].set_ylabel('Latency (ms)')
ax[1].set_xlabel('Time (s)')
ax[1].legend()
ax[1].grid()


ax[2].plot(time, maxPDR, label='Max PDR' , color= 'red')
ax[2].plot(time, avgPDR, label='Avg PDR', color = 'black')
ax[2].plot(time, minPDR, label='Min PDR', color = 'blue' )
ax[2].set_ylabel('PDR (%)')
ax[2].set_xlabel('Time (s)')
ax[2].legend()
ax[2].grid()


ax[3].plot(time, maxDist, label='Max Distance' , color= 'red')
ax[3].plot(time, avgDist, label='Avg Distance', color = 'black')
ax[3].plot(time, minDist, label='Min Distance', color = 'blue' )
ax[3].set_ylabel('Distance (m)')
ax[3].set_xlabel('Time (s)')
ax[3].legend()
ax[3].grid()

ax[4].plot(time, maxHop, label='Max number of hops' , color= 'red')
ax[4].plot(time, avgHop, label='Avg number of hops', color = 'black')
ax[4].plot(time, minHop, label='Min number of hops', color = 'blue' )
ax[4].set_ylabel('Number of hops')
ax[4].set_xlabel('Time (s)')
ax[4].legend()
ax[4].grid()

'''
# Show the plot
plt.tight_layout()  # Optional: Improve subplot spacing
plt.show()