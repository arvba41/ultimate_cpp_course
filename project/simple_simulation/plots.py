# %%
# include the necessary libraries
import matplotlib.pyplot as plt
import numpy as np
import json

# %%
# load the data
with open("build/pmsm_simulation.json", "r") as file:
    data = json.load(file)

# %%
# data processing
# x1_ode1 = np.array(data["ode1"]["X1"])
# x2_ode1 = np.array(data["ode1"]["X2"])
x1_ode4 = np.array(data["ode4"]["X1"])
x2_ode4 = np.array(data["ode4"]["X2"])
ref = np.array(data["ref"])
# f_cos = np.array(data["f(y)"]["cos"])
tsim = np.array(data["time"])

#   json_obj["time"] = t;
#   json_obj["ref"] = ref;
#   json_obj["f(y)"]["ode1"] = x;

# %%
fig, ax = plt.subplots(2, 1, clear=True, layout="tight")

# ax[0].plot(tsim, x1_ode1, label="i(t)")
# ax[0].plot(tsim, x2_ode1, label="v(t)")
ax[0].set_title("Ode 1")
ax[0].set_xlabel("Time [s]")
ax[0].set_ylabel("Amplitude")
ax[0].legend()

ax[1].plot(tsim, x1_ode4, label="i(t)")
ax[1].plot(tsim, x2_ode4, label="v(t)")
ax[1].set_title("Ode 4")
ax[1].set_xlabel("Time [s]")
ax[1].set_ylabel("Amplitude")

plt.show()

# %%
