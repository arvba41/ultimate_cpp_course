# %%
# include necessary libraries
import numpy as np
import matplotlib.pyplot as plt
import json

import time

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
# Simulating the PMSM model in python


class RungeKutta4:
    def __init__(self, f):
        self.f = f

    def simulate(self, t, x, u):

        for i in range(1, len(t)):
            dt = t[i] - t[i - 1]
            k1 = self.f(x[:, i - 1], u[:, i - 1])
            k2 = self.f(x[:, i - 1] + 0.5 * dt * k1, u[:, i - 1])
            k3 = self.f(x[:, i - 1] + 0.5 * dt * k2, u[:, i - 1])
            k4 = self.f(x[:, i - 1] + dt * k3, u[:, i - 1])

            x[:, i] = x[:, i - 1] + (dt / 6) * (k1 + 2 * k2 + 2 * k3 + k4)

        return x


class PMSM:
    def __init__(self, Ld, Lq, Rs, psi_r, J, np):
        self.Ld = Ld
        self.Lq = Lq
        self.Rs = Rs
        self.psi_r = psi_r
        self.J = J
        self.np = np

    def set_time(self, start, end, delta):
        self.t = np.arange(start, end, delta)

    def set_inputs(self, input_idx, val, t0):
        id = np.zeros_like(self.t)
        iq = np.zeros_like(self.t)
        Tl = np.zeros_like(self.t)

        if input_idx == 0:
            id[self.t > t0] = val
        elif input_idx == 1:
            iq[self.t > t0] = val
        elif input_idx == 2:
            Tl[self.t > t0] = val

        self.u = np.array([id, iq, Tl])

    def fx(self, x, u):
        # extract the states and inputs
        id, iq, omega = x  # states
        vd, vq, Tl = u  # inputs

        # calculate the currents and speed
        id_dot = (1 / self.Ld) * (vd - self.Rs * id + self.np * omega * self.Lq * iq)
        iq_dot = (1 / self.Lq) * (
            vq - self.Rs * iq - self.np * omega * (self.Ld * id + self.psi_r)
        )
        omega_dot = (1 / self.J) * (
            3 * self.np / 2 * (self.psi_r * iq - (self.Ld - self.Lq) * id * iq) - Tl
        )

        xdot = np.array([id_dot, iq_dot, omega_dot])
        return xdot

    def simulate(self, x0):
        # initialize the states
        self.x = np.zeros((len(x0), len(self.t)))
        self.x[:, 0] = x0

        # simulate the model
        self.integrator = RungeKutta4(self.fx)
        self.integrator.simulate(self.t, self.x, self.u)


# %%
# pmsm simulation in python
pmsm = PMSM(Ld=2.85e-3, Lq=2.85e-3, Rs=0.054, psi_r=0.8603, J=0.25, np=3)

Vq_pk = 100
tset = 0.1
tstart = 0
tend = 1
dt = 10e-6

pmsm.set_time(tstart, tend, dt)
pmsm.set_inputs(1, Vq_pk, tset)
x0 = np.array([0, 0, 0])

tsim_start = time.time()
pmsm.simulate(x0)
tsim_end = time.time()

print("*" * 50)
print(f"Simulation time: {tsim_end - tsim_start:.2f} s")
print("*" * 50)

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

for ii, (uu, xx) in enumerate(zip(pmsm.u, pmsm.x)):
    ax[ii, 0].plot(pmsm.t, uu, label=unames[ii], linestyle="--")
    ax[ii, 1].plot(pmsm.t, xx, label=xnames[ii], linestyle="--")

ax[2, 0].set_xlabel("$t$ [s]")
ax[2, 1].set_xlabel("$t$ [s]")
fig.align_ylabels()

# %%
plt.show()
