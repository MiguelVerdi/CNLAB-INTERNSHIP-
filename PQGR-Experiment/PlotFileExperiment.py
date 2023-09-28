import matplotlib.pyplot as plt
import os

# Specify the path to your text file
file_path = r'ExperimentFiles\NPCA\highwayUsparse_NPCA_Result.csv'
subfolder = 'NPCA'

# Extract the base name of the CSV file without the extension
base_name = os.path.splitext(os.path.basename(file_path))[0]

# Create the name for the plot PNG
plot_png_name = f'{base_name}_AllFigures.png'
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

#Time in seconds
time = [row[0] for row in data]

nClusters = [row[1] for row in data]
GPCA = [row[2] for row in data]

#Throughput in Mbps
maxTh= [row[3] for row in data]
avgTh = [row[4] for row in data]
minTh = [row[5] for row in data]

#Change the Lattency values to ms. 
maxLat = [1000*row[6] for row in data]
avgLat = [1000*row[7] for row in data]
minLat= [1000*row[8] for row in data]

#Change the PDR to 0% to 100% values
maxPDR = [100*row[9] for row in data]
avgPDR = [100*row[10] for row in data]
minPDR = [100*row[11] for row in data]

#Distances in meters
maxDist= [row[12] for row in data]
avgDist = [row[13] for row in data]
minDist = [row[14] for row in data]

#Number of hops or nodes
maxHop  = [row[15] for row in data]
avgHop = [row[16] for row in data]
minHop = [row[17] for row in data]



# Create a figure with subplots
fig, ax = plt.subplots(5, 1, sharex=True, figsize=(8, 12))  # Adjust the figure size as needed
# Plot and save each subplot
for i, subplot_data in enumerate([(maxHop, avgHop, minHop, 'Number of hops'),
                                   (maxTh, avgTh, minTh, 'Data Rate (Mbps)'),
                                   (maxLat, avgLat, minLat, 'Latency (ms)'),
                                   (maxPDR, avgPDR, minPDR, 'PDR (%)'),
                                   (maxDist, avgDist, minDist, 'Distance (m)')]):
    max_data, avg_data, min_data, ylabel = subplot_data
    ax[i].plot(time, max_data, label='Max', color='red')
    ax[i].plot(time, avg_data, label='Avg', color='black')
    ax[i].plot(time, min_data, label='Min', color='blue')
    ax[i].set_ylabel(ylabel)
    ax[i].legend()
    ax[i].grid()
# Set the common X-axis label
ax[-1].set_xlabel('Time (s)')
plt.tight_layout()  # Optional: Improve subplot spacing

save_directory = "ExperimentResultPlots"
save_directory = os.path.join(save_directory, subfolder)

#plt.savefig(plot_png_name, dpi=300, format="png", bbox_inches="tight")
plt.savefig(os.path.join(save_directory, plot_png_name), dpi=300, format="png", bbox_inches="tight")
plt.clf()  # Clear the current figure for the next iteration


fig_size = (8, 6)

for i, metric in enumerate([(maxHop, avgHop, minHop, 'Number of hops','Hops'),
                                   (maxTh, avgTh, minTh, 'Data Rate','DataRate'),
                                   (maxLat, avgLat, minLat, 'Latency (ms)', 'Latency'),
                                   (maxPDR, avgPDR, minPDR, 'PDR (%)','PDR'),
                                   (maxDist, avgDist, minDist, 'Distance (m)','Distance')]):
    
    max_data, avg_data, min_data, ylabel, title = metric
    plt.figure(figsize=fig_size)
    plt.plot(time, max_data, label=f'{ylabel} Max', color='red')
    plt.plot(time, avg_data, label=f'{ylabel} Avg', color='black')
    plt.plot(time, min_data, label=f'{ylabel} Min', color='blue')
    plt.xlabel('Time (s)')
    plt.ylabel(ylabel)
    plt.title(f'{title}')
    plt.grid()
    save_directory = "ExperimentResultPlots"
    save_directory = os.path.join(save_directory, subfolder)

    plt.savefig(os.path.join(save_directory, f"{base_name}_{title}_plot.png"))
    #plt.savefig(f"{base_name}_{title}_plot.png")  # Save each plot with a unique name
    plt.clf()  # Clear the current figure for the next iteration






