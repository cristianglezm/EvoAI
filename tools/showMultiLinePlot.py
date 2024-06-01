import pandas as pd
import matplotlib.pyplot as plt
import argparse

# Parse command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('file', help='CSV file to read')
parser.add_argument('-w', '--window', type=int, default=5, help='Rolling window size (default: 5)')
args = parser.parse_args()

# Load the CSV file into a Pandas DataFrame
df = pd.read_csv(args.file, header=None)
df = df.T
df.columns = df.iloc[0]
df = df.drop(0)
df = df.apply(pd.to_numeric, errors='ignore')

# Get the column names and remove the specified column from the list
cols = list(df.columns)

# Create a multi-line plot with the specified column as the legend
fig, ax = plt.subplots()

# Smooth the lines using a rolling mean with the specified window size
rolling_mean = df[cols].rolling(window=args.window).mean()
for col in cols:
    ax.plot(rolling_mean[col], label=col + ' (smoothed)', alpha=0.5)

# Add the specified column as the legend and display the plot
ax.legend(cols)
plt.ylim(0, 100)
plt.yscale('log')
plt.autoscale()
plt.ylabel("accuracy")
plt.xlabel("Epoch")
plt.twinx()
plt.ylim(0, 10)
plt.autoscale()
plt.title("MultiLine Plot")
plt.xlabel("Epoch")
plt.ylabel("loss")
plt.show()
