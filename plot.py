#!/usr/bin/env python3

import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import dill


known_circuits = None
parsed = None
with open(sys.argv[1]+'.dat', 'rb') as f:
    data = dill.load(f)
    known_circuits = data['known_circuits']
    parsed = data['parsed']


# Find the median, min and max of the errors for each quantum cost
errors = {}
for p in parsed:
    if not p['qc'] in errors:
        errors[p['qc']] = {'with_noise':[], 'with_noise_melbourne':[], 'without_noise':[]}
    errors[p['qc']]['with_noise'].append(p['e_noise'])
    errors[p['qc']]['with_noise_melbourne'].append(p['e_noise_melbourne'])
    errors[p['qc']]['without_noise'].append(p['e'])
for qc in errors:
    errors[qc]['median_with_noise'] = np.median(errors[qc]['with_noise'])
    errors[qc]['median_with_noise_melbourne'] = np.median(errors[qc]['with_noise_melbourne'])
    errors[qc]['median_without_noise'] = np.median(errors[qc]['without_noise'])
    errors[qc]['min_with_noise'] = min(errors[qc]['with_noise'])
    errors[qc]['min_with_noise_melbourne'] = min(errors[qc]['with_noise_melbourne'])
    errors[qc]['min_without_noise'] = min(errors[qc]['without_noise'])
    errors[qc]['max_with_noise'] = max(errors[qc]['with_noise'])
    errors[qc]['max_with_noise_melbourne'] = max(errors[qc]['with_noise_melbourne'])
    errors[qc]['max_without_noise'] = max(errors[qc]['without_noise'])


plt.errorbar([qc-0.05 for qc in errors],
             [errors[qc]['median_without_noise'] for qc in errors],
             [[errors[qc]['median_without_noise']-errors[qc]['min_without_noise'] for qc in errors], [errors[qc]['max_without_noise']-errors[qc]['median_without_noise'] for qc in errors]],
             capsize=5, elinewidth=0.5, fmt='x', label='Errors without noise')
plt.errorbar([qc+0.05 for qc in errors],
             [errors[qc]['median_with_noise'] for qc in errors],
             [[errors[qc]['median_with_noise']-errors[qc]['min_with_noise'] for qc in errors], [errors[qc]['max_with_noise']-errors[qc]['median_with_noise'] for qc in errors]],
             capsize=5, elinewidth=0.5, fmt='x', label='Errors with noise')
plt.errorbar([d['qc'] for d in known_circuits if d['qc'] <= max(errors)],
             [d['e_noise'] for d in known_circuits if d['qc'] <= max(errors)],
             [0 for d in known_circuits if d['qc'] <= max(errors)],
             elinewidth=0.001, fmt='x', label='Known circuits')
plt.xlabel('Quantum Cost')
plt.ylabel('Error rate')
plt.legend()
plt.savefig(sys.argv[1]+'.pdf')

plt.gcf().clear()

plt.plot([qc for qc in errors], [errors[qc]['median_without_noise'] for qc in errors], label='Errors without noise')
plt.fill_between([qc for qc in errors], [errors[qc]['min_without_noise'] for qc in errors], [errors[qc]['max_without_noise'] for qc in errors], alpha=0.2)
plt.plot([qc for qc in errors], [errors[qc]['median_with_noise'] for qc in errors], label='Errors with noise')
plt.fill_between([qc for qc in errors], [errors[qc]['min_with_noise'] for qc in errors], [errors[qc]['max_with_noise'] for qc in errors], alpha=0.2)
plt.scatter([d['qc'] for d in known_circuits if d['qc'] <= max(errors)], [d['e_noise'] for d in known_circuits if d['qc'] <= max(errors)], label='Known circuits')
plt.xlabel('Quantum Cost')
plt.ylabel('Error rate')
plt.legend()
plt.savefig(sys.argv[1]+'_alternative.pdf')

plt.gcf().clear()

plt.errorbar([qc-0.05 for qc in errors],
             [errors[qc]['median_without_noise'] for qc in errors],
             [[errors[qc]['median_without_noise']-errors[qc]['min_without_noise'] for qc in errors], [errors[qc]['max_without_noise']-errors[qc]['median_without_noise'] for qc in errors]],
             capsize=5, elinewidth=0.5, fmt='x', label='Errors without noise')
plt.errorbar([qc+0.05 for qc in errors],
             [errors[qc]['median_with_noise_melbourne'] for qc in errors],
             [[errors[qc]['median_with_noise_melbourne']-errors[qc]['min_with_noise_melbourne'] for qc in errors], [errors[qc]['max_with_noise_melbourne']-errors[qc]['median_with_noise_melbourne'] for qc in errors]],
             capsize=5, elinewidth=0.5, fmt='x', label='Errors with noise')
plt.errorbar([d['qc'] for d in known_circuits if d['qc'] <= max(errors)],
             [d['e_noise_melbourne'] for d in known_circuits if d['qc'] <= max(errors)],
             [0 for d in known_circuits if d['qc'] <= max(errors)],
             elinewidth=0.001, fmt='x', label='Known circuits')
plt.xlabel('Quantum Cost')
plt.ylabel('Error rate')
plt.legend()
plt.savefig(sys.argv[1]+'_melbourne.pdf')

plt.gcf().clear()

plt.plot([qc for qc in errors], [errors[qc]['median_without_noise'] for qc in errors], label='Errors without noise')
plt.fill_between([qc for qc in errors], [errors[qc]['min_without_noise'] for qc in errors], [errors[qc]['max_without_noise'] for qc in errors], alpha=0.2)
plt.plot([qc for qc in errors], [errors[qc]['median_with_noise_melbourne'] for qc in errors], label='Errors with noise')
plt.fill_between([qc for qc in errors], [errors[qc]['min_with_noise_melbourne'] for qc in errors], [errors[qc]['max_with_noise_melbourne'] for qc in errors], alpha=0.2)
plt.scatter([d['qc'] for d in known_circuits if d['qc'] <= max(errors)], [d['e_noise_melbourne'] for d in known_circuits if d['qc'] <= max(errors)], label='Known circuits')
plt.xlabel('Quantum Cost')
plt.ylabel('Error rate')
plt.legend()
plt.savefig(sys.argv[1]+'_alternative_melbourne.pdf')
