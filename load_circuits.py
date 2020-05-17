import qiskit
import numpy as np

def tfc2qiskit(source):
    lines = source.splitlines()
    quantum_cost = int(lines[0])
    variables = {}
    inputs = []
    outputs = []
    intialize_with = []
    idx = 1
    # Parse the header
    v = set()
    while (True):
        line = lines[idx]
        idx += 1
        if (line.startswith('.v')):
            v = set(line[3:].split(','))
        elif (line.startswith('.i')):
            inputs = line[3:].split(',')
        elif (line.startswith('.o')):
            outputs = line[3:].split(',')
        elif (line.startswith('.c')):
            initialize_with = list(map(int, line[3:].split(',')))
        else:
            break
    vidx = 0
    for var in inputs:
        variables[var] = vidx
        v.remove(var)
        vidx += 1
    for var in v - set(outputs):
        variables[var] = vidx
        vidx += 1
    for var in outputs:
        if var not in inputs:
            variables[var] = vidx
            vidx += 1
    num_lines = len(variables)
    # Build the circuit
    circuit = qiskit.circuit.QuantumCircuit(num_lines, len(outputs))
    for i, init in enumerate(initialize_with):
        if init == 1:
            circuit.x(len(inputs)+i)
    while (lines[idx] != 'END'):
        line = lines[idx]
        idx += 1
        if (line[0] == 't' or line[0] == 'T'):
            args = line.split(' ')[1].split(',')
            invert_args = []
            for i, arg in enumerate(args):
                if arg[-1] == "'":
                    args[i] = arg[:-1]
                    invert_args.append(args[i])
            ctrl_bits = [variables[var] for var in args[:-1]]
            target_bit = variables[args[-1]]
            for arg in invert_args:
                circuit.x(variables[arg])
            circuit.mcx(ctrl_bits, target_bit)
            for arg in invert_args:
                circuit.x(variables[arg])
        elif (line[0] == 'f' or line[0] == 'F'):
            args = line.split(' ')[1].split(',')
            invert_args = []
            for i, arg in enumerate(args):
                if arg[-1] == "'":
                    args[i] = arg[:-1]
                    invert_args.append(args[i])
            ctrl_bits = [variables[var] for var in args[:-2]]
            target_bits = [variables[args[-2]], variables[args[-1]]]
            for arg in invert_args:
                circuit.x(variables[arg])
            circuit.cx(target_bits[0], target_bits[1])
            circuit.mcx(ctrl_bits+[target_bits[1]], target_bits[0])
            circuit.cx(target_bits[0], target_bits[1])
            for arg in invert_args:
                circuit.x(variables[arg])
        else:
            pass
    for i, otp in enumerate(outputs):
        circuit.measure([variables[otp]], [i])
    return quantum_cost, circuit


def ascii2qiskit(source):
    lines = source.splitlines()
    num_lines = int(lines[0].split()[0])
    circuit = qiskit.circuit.QuantumCircuit(num_lines, 1)
    for instruction in lines[1:]:
        gate = instruction.split()[0]
        args = list(map(int, instruction.split()[1:]))
        if gate == 'Id':
            circuit.i(args[0])
        elif gate == 'X':
            circuit.x(args[0])
        elif gate == 'cX':
            circuit.cx(control_qubit=args[1], target_qubit=args[0])
        elif gate == 'ccX':
            circuit.ccx(args[2], args[1], args[0])
        elif gate == 'Swap':
            circuit.swap(args[0], args[1])
        elif gate == 'cSwap':
            circuit.fredkin(args[2], args[0], args[1])
        else:
            pass
    circuit.barrier(range(num_lines))
    circuit.measure([num_lines-1], [0])
    return circuit
