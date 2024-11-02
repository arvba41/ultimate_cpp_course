# %%
# importing the required libraries
import numpy as np

# import pandas as pd
import matplotlib.pyplot as plt
import json


# %%
# loading the data

data = json.load(open("build/casadi_cpp_sim.json"))

# %%
# extracting the data
tvec = np.linspace(0, data["T"], data["Nsmp"] + 1)

x = np.array(data["x_pos"])
y = np.array(data["y_pos"])

vx = np.array(data["x_speed"])
vy = np.array(data["y_speed"])

u0 = np.array(data["U0"])
u1 = np.array(data["U1"])

# %%
# plotting the data

fig, ax = plt.subplots(3, 2, clear=True, layout="constrained")

gs = ax[0, 0].get_gridspec()
ax[0, 0].remove()
ax[0, 1].remove()
ax[0, 0] = fig.add_subplot(gs[0, :])

ax[0, 0].plot(x, y, label="path")
ax[0, 0].set_xlabel("x [m]")
ax[0, 0].set_ylabel("y [m]")
ax[0, 0].legend()

ax[1, 0].plot(tvec, vx * 3.6)
ax[1, 0].set_xlabel("$t$ [s]")
ax[1, 0].set_ylabel("$v_x$ [km/h]")
# despine(ax[1, 0])

ax[1, 1].plot(tvec, vy * 3.6)
ax[1, 1].set_xlabel("$t$ [s]")
ax[1, 1].set_ylabel("$v_y$ [km/h]")

ax[2, 0].plot(tvec[1:], u0, label="x")
ax[2, 0].plot(tvec[1:], u1, label="y")
ax[2, 0].set_xlabel("$t$ [s]")
ax[2, 0].set_ylabel("$u$ [N]")
ax[2, 0].legend()

ax[2, 1].plot(tvec[1:], np.sqrt(u0**2 + u1**2), label="u")
ax[2, 1].set_xlabel("$t$ [s]")
ax[2, 1].set_ylabel("$|u|$ [N]")

# fig.align_ylabels(ax)
# %%
plt.show()
