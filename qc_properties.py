from qiskit.providers.aer.noise import NoiseModel
from qiskit.test.mock import FakeMelbourne

backend = FakeMelbourne()
coupling_map = backend.configuration().coupling_map
basis_gates = backend.configuration().basis_gates
noise_model = NoiseModel.from_backend(backend)
