import numpy as np
from matplotlib.pyplot import *

import csv
import os

pwd = os.getcwd()

# ------------------------

trajectory = []
with open(pwd+'/data/trajectory.csv', 'r') as csvfile:
	trajectory_file = csv.reader(csvfile, delimiter=',')
	for row in trajectory_file:
		trajectory.append(row)		

trajectory.pop(0)
trajectory = [ [ float(x) for x in row ] for row in trajectory] 
trajectory = np.array(trajectory)

gains = []
with open(pwd+'/data/gains.csv', 'r') as csvfile:
	gains_file = csv.reader(csvfile, delimiter=',')
	for row in gains_file:
		gains.append(row)

gains.pop(0)
gains = [ [ float(x) for x in row ] for row in gains] 
gains = np.array(gains)

# ------------------

scatter(trajectory[:,0], trajectory[:, 1])
xlabel('X')
ylabel('Y')
title('Plot of the trajectory of the virtual sound source')
savefig("TrajectoryPlot.png")
grid(True)
clf()

# ------------------

nspeakers = len(gains.T)

for speaker in range(nspeakers):
	plot(gains[:,speaker], label = 'Speaker %d' % (speaker+1))

xlabel("Time Step")
ylabel("Gain")
title("Gains for 2D VBAP with %d Speakers" % nspeakers)
legend()
savefig("GainPlot_Combined.png")

clf()

for speaker in range(nspeakers):
	plot(gains[:,speaker])

	xlabel("Time Step")
	ylabel("Gain")

	title("Speaker %d Gain Plot" % (speaker+1))
	savefig("GainPlot_Speaker%d.png" % (speaker+1))

	clf()

# ------------------

sums = []

for i in range(len(gains)):
	s = np.sqrt(np.sum([x*x for x in gains[i,:]]))
	sums.append(s)

plot(sums)
title("Overall Gain plot")
xlabel("Overall Gain")
ylabel("Time Slice")
savefig("OverallGainPlot.png")

clf()



