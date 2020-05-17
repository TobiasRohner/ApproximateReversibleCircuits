#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import qiskit
import functions
import qc_properties
import tqdm
import numpy as np
from load_circuits import *
from errors import *



known_circuit_filename = sys.argv[1]
optimized_circuit_filename = sys.argv[2]
func = getattr(functions, 'Func'+sys.argv[3])()
n_eval = int(sys.argv[4])
min_err = float(sys.argv[5])
max_err = float(sys.argv[6])
errors = np.exp(np.linspace(np.log(min_err), np.log(max_err), n_eval))

known_circuit = None
with open(known_circuit_filename, 'r') as f:
    source = f.read()
    qc, circuit = tfc2qiskit(source)
    known_circuit = circuit

optimized_circuit = None
with open(optimized_circuit_filename, 'r') as f:
    source = f.read()
    optimized_circuit = ascii2qiskit(source)

err_known = []
err_optim = []
for i in tqdm.tqdm(range(n_eval)):
    noise_model = reduce_noise(qc_properties.noise_model, 1./errors[i])
    e, fn, fp = compute_error_rates(known_circuit, func, noise_model)
    err_known.append(e)
    e, fn, fp = compute_error_rates(optimized_circuit, func, noise_model)
    err_optim.append(e)

# Find a point where the two error probabilities cross
px = None
py = None
for i in range(len(err_known)-1):
    if err_known[i] <= err_optim[i] and err_known[i+1] > err_optim[i+1]:
        px = errors[i] + (errors[i+1]-errors[i])*(err_optim[i]-err_known[i])/(err_known[i+1]-err_optim[i+1]+err_optim[i]-err_known[i])
        py = err_known[i] + (px-errors[i])/(errors[i+1]-errors[i])*(err_known[i+1]-err_known[i])

plt.semilogx(errors, err_known, label='Exact Circuit')
plt.semilogx(errors, err_optim, label='Approximate Circuit')
if px is not None:
    plt.axvline(x=px, linestyle='--', color='silver')
    plt.axhline(y=py, linestyle='--', color='silver')
plt.legend()
plt.xlabel('Noise Strength')
plt.ylabel('Error')
plt.savefig(sys.argv[3]+'_vs_noise.pdf')
