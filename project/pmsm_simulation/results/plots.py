# %%
# include necessary libraries
import numpy as np
import matplotlib.pyplot as plt
import json

# %%
# load the data
with open("pmsm_simulation.json", "r") as file:
    data = json.load(file)

# %%
# extract the data
t = np.array(data["t"])  # time

u = {}  # input
for uu in data["U"]:
    u[uu] = np.array(data["U"][uu])

x = {}  # state
for xx in data["X"]:
    x[xx] = np.array(data["X"][xx])

# %%
# load the simulation results from python
with open("python_results.npy", "rb") as file:
    data = np.load(file, allow_pickle=True).item()

t_python = data["t"]  # time
u_python = data["u"]  # input
x_python = data["x"]  # state

# %%
# loading the simulation results from matlab
with open("matlab_results.json", "r") as file:
    data_matlab = json.load(file)

t_matlab = np.array(data_matlab["t"])  # time
u_matlab = np.array(data_matlab["u"])  # input
x_matlab = np.array(data_matlab["x"])  # state

# %%
# plot the simulation results

fig, ax = plt.subplots(
    3, 2, clear=True, num="Simulation Results", layout="constrained", sharex=True
)

unames = ["$v_d$", "$v_q$", "$T_l$"]
xnames = ["$i_d$", "$i_q$", "$\\omega$"]

for ii, (uu, xx) in enumerate(zip(u, x)):
    ax[ii, 0].plot(t, u[uu])
    ax[ii, 0].set_ylabel(unames[ii])
    ax[ii, 0].grid(True)
    # ax[ii, 0].legend()

    ax[ii, 1].plot(t, x[xx])
    ax[ii, 1].set_ylabel(xnames[ii])
    ax[ii, 1].grid(True)
    # ax[ii, 1].legend()

for ii, (uu, xx) in enumerate(zip(u_python, x_python)):
    ax[ii, 0].plot(t_python, uu, linestyle="--")
    ax[ii, 1].plot(t_python, xx, linestyle="--")

for ii, (uu, xx) in enumerate(zip(u_matlab, x_matlab)):
    ax[ii, 0].plot(t_matlab, uu, linestyle=":")
    ax[ii, 1].plot(t_matlab, xx, linestyle=":")

ax[2, 0].set_xlabel("$t$ [s]")
ax[2, 1].set_xlabel("$t$ [s]")
fig.align_ylabels()

# %%
plt.show()
