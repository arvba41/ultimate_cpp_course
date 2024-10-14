# %%
# include necessary libraries
import numpy as np
import matplotlib.pyplot as plt
import json

# %%
# load the data
with open("build/pmsm_simulation.json", "r") as file:
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
# plot the simulation results

fig, ax = plt.subplots(
    3, 2, clear=True, num="Simulation Results", layout="constrained", sharex=True
)

unames = ["$v_d$", "$v_q$", "$T_l$"]
xnames = ["$i_d$", "$i_q$", "$\\omega$"]

for ii, (uu, xx) in enumerate(zip(u, x)):
    ax[ii, 0].plot(t, u[uu], label=unames[ii])
    ax[ii, 0].set_ylabel(unames[ii])
    ax[ii, 0].grid(True)
    # ax[ii, 0].legend()

    ax[ii, 1].plot(t, x[xx], label=xnames[ii])
    ax[ii, 1].set_ylabel(xnames[ii])
    ax[ii, 1].grid(True)
    # ax[ii, 1].legend()

ax[2, 0].set_xlabel("$t$ [s]")
ax[2, 1].set_xlabel("$t$ [s]")
fig.align_ylabels()

# %%
