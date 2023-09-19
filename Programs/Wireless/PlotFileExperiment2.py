import matplotlib.pyplot as plt
# Specify the path to your text file
file_path = 'Experiment_2.csv'

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


x = [row[0] for row in data]
y1 = [row[1] for row in data]
y2 = [row[2] for row in data]

fig, (ax1, ax2) = plt.subplots(2, 1, sharex=True)

# Plot data on the first subplot
ax1.plot(x, y1, label='Throughput')
ax1.scatter(x, y1, label='Throughput')
ax1.set_ylabel('Throughput')
ax1.legend()

# Plot data on the second subplot
ax2.plot(x, y2, label='PDR')
ax2.scatter(x, y2, label='PDR')
ax2.set_xlabel('gap')
ax2.set_ylabel('PDR')
ax2.legend()

# Show the plot
plt.tight_layout()  # Optional: Improve subplot spacing
plt.show()