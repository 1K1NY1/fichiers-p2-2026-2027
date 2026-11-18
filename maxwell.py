import numpy as np
import matplotlib.pyplot as plt

v = np.loadtxt("speeds.txt")
kT = np.mean(v**2) / 2          # énergie cinétique moyenne (masse = 1)

x = np.linspace(0, v.max(), 500)
f = (x / kT) * np.exp(-x**2 / (2 * kT))

plt.hist(v, bins=25, density=True, alpha=0.6, label="Simulation(500 particules)")
plt.plot(x, f, "r", label="Maxwell-Boltzmann")
plt.xlabel("v")
plt.ylabel("Densité")
plt.legend()
plt.savefig("histogram.pdf")
