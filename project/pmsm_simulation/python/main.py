# %% %matplotlib
from numba import njit
import pandas as pd
import json
import numpy as np
import matplotlib.pyplot as plt
from seaborn import despine

import time

# plt.rcParams["lines.linewidth"] = 0.75


# %% v
# function definitions for the PMSM model


# motor parameters
R_s = 0.56
L_d = 375e-6
L_q = 435e-6
psi_r = 0.0143
n_p = 2
J = 0.12e-4
b = 10e-6

# controller parameters
tri_c = 1e-3
alpha_c = np.log(9) / tri_c
Kpd = alpha_c * L_d
Kpq = alpha_c * L_q
Kid = alpha_c**2 * L_d
Kiq = alpha_c**2 * L_q
Rad = alpha_c * L_d - R_s
Raq = alpha_c * L_q - R_s
tri_s = 1e-2
alpha_s = np.log(9) / tri_s
psi = 3 * n_p * psi_r / 2
Kps = J * alpha_s / psi
Kis = J * alpha_s**2 / psi
Ba = (alpha_s * J - b) / psi

# controller sample time
Tctrl = 2e-4

# saturation parameters
Ibase = 2
Vbase = 12

v_d_prev = 0
v_q_prev = 0


# @njit
def pmsm_model(t, x):
    # R_s = 0.054
    # L_d = 2.85e-3
    # L_q = 2.85e-3
    # psi_r = 0.8603
    # n_p = 3
    # J = 0.25
    # b = 0.0

    T_l = 0 if t < 0.5 else 0.08

    i_d, i_q, w, _, _, _ = x

    # global v_d_prev, v_q_prev
    # if t % Tctrl < 1e-6:
    #     v_d, v_q, dI_ddt, dI_qdt, dI_sdt = controller(t, x)
    #     v_d_prev, v_q_prev = v_d, v_q
    # else:
    #     v_d, v_q = v_d_prev, v_q_prev
    #     dI_ddt, dI_qdt, dI_sdt = 0, 0, 0

    v_d, v_q, dI_ddt, dI_qdt, dI_sdt = controller(t, x)

    ## state update
    did_dt = 1 / L_d * (v_d - R_s * i_d + n_p * w * L_q * i_q)
    diq_dt = 1 / L_q * (v_q - R_s * i_q - n_p * w * (L_d * i_d + psi_r))
    dw_dt = (
        1 / J * (3 * n_p / 2 * (psi_r * i_q + (L_d - L_q) * i_d * i_q) - T_l - b * w)
    )

    return np.array([did_dt, diq_dt, dw_dt, dI_ddt, dI_qdt, dI_sdt])


# @njit
def controller(t, x):

    i_d, i_q, w, I_d, I_q, I_s = x

    w_ref = 0 if t < 0.1 else 4000 * 2 * np.pi / 60

    i_d_ref = 0
    i_q_ref = (w_ref - w) * Kps + I_s * Kis - Ba * w

    # i_q_ref = np.clip(i_q_ref, -Ibase, Ibase)
    i_q_ref = i_q_ref if abs(i_q_ref) < Ibase else np.sign(i_q_ref) * Ibase

    v_d_ref = (i_d_ref - i_d) * Kpd + I_d * Kid - Rad * i_d - n_p * w * L_q * i_q
    v_q_ref = (
        (i_q_ref - i_q) * Kpq + I_q * Kiq - Raq * i_q + n_p * w * (L_d * i_d + psi_r)
    )

    vdq = np.sqrt(v_d_ref**2 + v_q_ref**2)
    v_d = v_d_ref if vdq < Vbase else v_d_ref / vdq * Vbase
    v_q = v_q_ref if vdq < Vbase else v_q_ref / vdq * Vbase

    dI_ddt = ((i_d_ref - i_d) + (1 / Kpd) * (v_d - v_d_ref)) / 1e-6 * Tctrl
    dI_qdt = ((i_q_ref - i_q) + (1 / Kpq) * (v_q - v_q_ref)) / 1e-6 * Tctrl
    dI_sdt = ((w_ref - w) + (1 / Kps) * (i_q_ref - i_q)) / 1e-6 * Tctrl

    return np.array([v_d, v_q, dI_ddt, dI_qdt, dI_sdt])


# %% Fixed step RK
x0 = np.array([0, 0, 0, 0, 0, 0])
t = np.arange(0, 1 + 1e-6, 1e-6)


# @njit
def fixed_step_rk4(f, x0, t):
    N = len(t)
    n = len(x0)
    x = np.zeros((n, N))
    x[:, 0] = x0
    for i in range(1, N):
        h = t[i] - t[i - 1]
        k1 = f(t[i - 1], x[:, i - 1])
        k2 = f(t[i - 1] + h / 2, x[:, i - 1] + h / 2 * k1)
        k3 = f(t[i - 1] + h / 2, x[:, i - 1] + h / 2 * k2)
        k4 = f(t[i - 1] + h, x[:, i - 1] + h * k3)
        x[:, i] = x[:, i - 1] + h / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
    return x


# %%
# simulation
tstart = time.time()
x_fs = fixed_step_rk4(pmsm_model, x0, t)  # 13 s / 358 ms
tend = time.time()
print(f"Fixed step RK4 simulation time: {tend - tstart:.3f} s")

# %%
# plotting the results
fig, ax = plt.subplots(3, 1, num=20, clear=True, layout="constrained", sharex=True)

ax[0].plot(t, x_fs[0])
ax[0].set_ylabel(r"$i_d$ [A]")
# ax[0].set_xlabel("Time [s]")
despine(ax=ax[0])

ax[1].plot(t, x_fs[1])
ax[1].set_ylabel(r"$i_q$ [A]")
# ax[1].set_xlabel("Time [s]")
despine(ax=ax[1])

ax[2].plot(t, x_fs[2] * 60 / (2 * np.pi))
ax[2].set_ylabel(r"$w$ [rpm]")
ax[2].set_xlabel("Time [s]")
despine(ax=ax[2])

fig.suptitle("Python fixed step RK4")
fig.align_ylabels(ax)

# plotting the internal variables
fig, ax = plt.subplots(3, 1, num=21, clear=True, layout="constrained", sharex=True)

ax[0].plot(t, x_fs[3])
ax[0].set_ylabel(r"$I_d$ [A]")
# ax[0].set_xlabel("Time [s]")
despine(ax=ax[0])

ax[1].plot(t, x_fs[4])
ax[1].set_ylabel(r"$I_q$ [A]")
# ax[1].set_xlabel("Time [s]")
despine(ax=ax[1])

ax[2].plot(t, x_fs[5])
ax[2].set_ylabel(r"$I_s$ [A]")
ax[2].set_xlabel("Time [s]")
despine(ax=ax[2])

fig.suptitle("Python fixed step RK4 internal variables")
fig.align_ylabels(ax)


# %%
# simulating in the loop

Nsim = 100
tsim_time = []
for i in range(Nsim):
    tstart = time.time()
    x_fs = fixed_step_rk4(pmsm_model, x0, t)  # 13 s / 358 ms
    tend = time.time()
    tsim_time.append(tend - tstart)

print(
    f"{Nsim} simulations with {np.mean(tsim_time) * 1e3:.2f} ± {np.std(tsim_time) * 1e3:.2f} ms each"
)

print(f"first simulation: {tsim_time[0]:.2f} s")
# %%
plt.show()
