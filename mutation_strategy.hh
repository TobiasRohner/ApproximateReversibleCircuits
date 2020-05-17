#ifndef MUTATION_STRATEGY_HH_
#define MUTATION_STRATEGY_HH_

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>
#include <map>
#include "circuit.hh"
#include "instruction.hh"


template<typename Reg_t>
class BaseMutationStrategy {
public:
    BaseMutationStrategy() = default;
    BaseMutationStrategy(const BaseMutationStrategy&) = default;
    BaseMutationStrategy(BaseMutationStrategy&&) = default;
    BaseMutationStrategy& operator=(const BaseMutationStrategy&) = default;
    BaseMutationStrategy& operator=(BaseMutationStrategy&&) = default;

    template<typename Rng_t>
    void mutate(Rng_t& rng, Circuit<Reg_t>& circuit) const {
	std::uniform_int_distribution<unsigned> dist(0, circuit.d()-1);
	const unsigned idx = dist(rng);
	circuit[idx] = random_gate(rng);
    }

    template<typename Rng_t>
    void randomize(Rng_t& rng, Circuit<Reg_t>& circuit) const {
	for (unsigned i = 0 ; i < circuit.d() ; ++i) {
	    circuit[i] = random_gate(rng);
	}
    }

protected:
    std::vector<Instruction<Reg_t>> instruction_set_;
    std::vector<double> cdf_;

private:
    template<typename Rng_t>
    const Instruction<Reg_t>& random_gate(Rng_t& rng) const {
	std::uniform_real_distribution<double> dist(0, 1);
	const double p = dist(rng);
	for (unsigned i = 0 ; i < cdf_.size() ; ++i) {
	    if (p <= cdf_[i])
		return instruction_set_[i];
	}
	return instruction_set_.back();
    }
};


template<typename Reg_t>
class FullyConnectedMutationStrategy : public BaseMutationStrategy<Reg_t> {
public:
    FullyConnectedMutationStrategy(unsigned l) {
	std::vector<unsigned> bits(l);
	std::iota(bits.begin(), bits.end(), 0);
	const auto connections1 = permutations(bits, 1);
	const auto connections2 = permutations(bits, 1, connections1);
	const auto connections3 = permutations(bits, 1, connections2);
	std::map<Gate, unsigned> instructions_per_gate;
	// Add the 1-bit gates
	if (l >= 1) {
	    // Id
	    for (auto&& conn : connections1)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::Id, conn[0], 0, 0);
	    instructions_per_gate[Gate::Id] = connections1.size();
	    // X
	    for (auto&& conn : connections1)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::X, conn[0], 0, 0);
	    instructions_per_gate[Gate::X] = connections1.size();
	}
	// Add the 2-bit gates
	if (l >= 2) {
	    // cX
	    for (auto&& conn : connections2)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::cX, conn[0], conn[1], 0);
	    instructions_per_gate[Gate::cX] = connections2.size();
	    // Swap
	    for (auto&& conn : connections2)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::Swap, conn[0], conn[1], 0);
	    instructions_per_gate[Gate::Swap] = connections2.size();
	}
	// Add the 3-bit gates
	if (l >= 3) {
	    // ccX
	    for (auto&& conn : connections3)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::ccX, conn[0], conn[1], conn[2]);
	    instructions_per_gate[Gate::ccX] = connections3.size();
	    // cSwap
	    for (auto&& conn : connections3)
		BaseMutationStrategy<Reg_t>::instruction_set_.emplace_back(Gate::cSwap, conn[0], conn[1], conn[2]);
	    instructions_per_gate[Gate::cSwap] = connections3.size();
	}
	// Compute the CDF for the instruction probabilities
	const unsigned num_inst = BaseMutationStrategy<Reg_t>::instruction_set_.size();
	BaseMutationStrategy<Reg_t>::cdf_.resize(num_inst);
	Gate g = BaseMutationStrategy<Reg_t>::instruction_set_[0].type();
	BaseMutationStrategy<Reg_t>::cdf_[0] = 1.0 / instructions_per_gate[g];
	for (unsigned i = 1 ; i < BaseMutationStrategy<Reg_t>::instruction_set_.size() ; ++i) {
	    g = BaseMutationStrategy<Reg_t>::instruction_set_[i].type();
	    BaseMutationStrategy<Reg_t>::cdf_[i] = BaseMutationStrategy<Reg_t>::cdf_[i-1] + 1.0/instructions_per_gate[g];
	}
	const auto cdf_max = BaseMutationStrategy<Reg_t>::cdf_.back();
	for (auto& p : BaseMutationStrategy<Reg_t>::cdf_)
	    p /= cdf_max;
    }

    FullyConnectedMutationStrategy() = default;
    FullyConnectedMutationStrategy(const FullyConnectedMutationStrategy&) = default;
    FullyConnectedMutationStrategy(FullyConnectedMutationStrategy&&) = default;
    FullyConnectedMutationStrategy& operator=(const FullyConnectedMutationStrategy&) = default;
    FullyConnectedMutationStrategy& operator=(FullyConnectedMutationStrategy&&) = default;

    using BaseMutationStrategy<Reg_t>::mutate;
    using BaseMutationStrategy<Reg_t>::randomize;

private:
    template<typename T>
    static std::vector<std::vector<T>> permutations(const std::vector<T>& elements, unsigned k, const std::vector<std::vector<T>>& pkm1) {
	if (k == 0)
	    return pkm1;
	std::vector<std::vector<T>> pk;
	for (auto&& p : pkm1) {
	    for (auto&& elem : elements) {
		if (std::count(p.begin(), p.end(), elem) == 0) {
		    pk.push_back(p);
		    pk.back().push_back(elem);
		}
	    }
	}
	return permutations(elements, k-1, pk);
    }

    template<typename T>
    static std::vector<std::vector<T>> permutations(const std::vector<T>& elements, unsigned k) {
	std::vector<std::vector<T>> pk0;
	pk0.emplace_back();
	return permutations(elements, k, pk0);
    }
};


#endif // MUTATION_STRATEGY_HH_
