#ifndef OPTIMIZER_HH_
#define OPTIMIZER_HH_

#include <cstdint>
#include <vector>
#include <iostream>
#include <iomanip>
#include <array>
#include <algorithm>
#include <random>
#include "circuit.hh"


template<typename Reg_t, typename Func_t, typename MutStrat_t>
class Optimizer : public Func_t {
public:
    using Func_t::func_eval;

    template<typename Rng_t>
    Optimizer(Rng_t& rng, unsigned l, unsigned d, unsigned S, unsigned F, MutStrat_t& mut_strat) : l_(l), d_(d), S_(S), F_(F), fails_(), population_(S_*F_, Circuit<Reg_t>(l, d)), mut_strat_(mut_strat) {
	for (auto& c : population_)
	    mut_strat_.randomize(rng, c);
    }

    template<typename Rng_t>
    void optimize(Rng_t& rng, unsigned generations, double ds, unsigned b) {
	for (unsigned g = 0 ; g < generations ; ++g)
	    run_generation(rng, ds, b);
    }

    const std::vector<Circuit<Reg_t>>& population() const { return population_; }
    std::vector<Circuit<Reg_t>>& population() { return population_; }

    Circuit<Reg_t> compute_best() const {
	Circuit<Reg_t> best = population_[0];
	double best_e = 1;
	unsigned best_qc = best.simplified().quantum_cost();
	for (auto&& circuit : population_) {
	    const auto simp = circuit.simplified();
	    auto [e, fn, fp] = simp.errors(Func_t{});
	    if ((e < best_e) || (e == best_e && best_qc > simp.quantum_cost())) {
		best = circuit;
		best_e = e;
		best_qc = simp.quantum_cost();
	    }
	}
	return best;
    }

private:
    const unsigned l_;
    const unsigned d_;
    const unsigned S_;
    const unsigned F_;
    std::vector<Reg_t> fails_;
    std::vector<Circuit<Reg_t>> population_;
    MutStrat_t& mut_strat_;

    template<typename Rng_t>
    std::pair<std::vector<double>, std::vector<Reg_t>> estimate_fitness(Rng_t& rng, const Circuit<Reg_t>* circuits, unsigned n, double ds, unsigned b) {
	const unsigned num_fails = std::min(static_cast<unsigned>(fails_.size()), static_cast<unsigned>((1.-ds)*b));
	// Sample num_fails fails without replacement
	std::vector<Reg_t> inputs(b);
	for (unsigned i = 0 ; i < num_fails ; ++i) {
	    std::uniform_int_distribution<unsigned> dist(0, fails_.size()-i-1);
	    const unsigned idx = dist(rng);
	    inputs[i] = fails_[idx];
	    std::swap(fails_[idx], fails_[fails_.size()-i-1]);
	}
	// Sample the rest of the inputs randomly
	const Reg_t max_input = (Reg_t(1) << Func_t::input_size) - 1;
	std::uniform_int_distribution<Reg_t> dist(0, max_input);
	for (unsigned i = num_fails ; i < b ; ++i)
	    inputs[i] = dist(rng);
	// Simulate every circuit
	std::vector<Reg_t> new_fails;
	std::vector<double> fitness(n, 0);
	std::vector<Reg_t> outputs;
	for (unsigned i = 0 ; i < n ; ++i) {
	    outputs = inputs;
	    circuits[i].run(outputs);
	    for (unsigned k = 0 ; k < b ; ++k) {
		if (((outputs[k] >> (l_-1)) & 1) == func_eval(inputs[k]))
		    fitness[i] += 1;
		else
		    new_fails.push_back(inputs[k]);
	    }
	    fitness[i] /= b;
	}
	return {std::move(fitness), std::move(new_fails)};
    }

    template<typename Rng_t>
    void run_generation(Rng_t& rng, double ds, unsigned b) {
	std::vector<Circuit<Reg_t>> new_population;
	std::vector<Reg_t> new_fails;
	new_population.reserve(S_*F_);
	for (unsigned i = 0 ; i < S_ ; ++i) {
	    auto [fit, fail] = estimate_fitness(rng, population_.data()+F_*i, F_, ds, b);
	    new_fails.insert(new_fails.end(), fail.begin(), fail.end());
	    unsigned best_fitness = fit[0];
	    unsigned best_quantum_cost = population_[F_*i].simplified().quantum_cost();
	    size_t best_idx = 0;
	    for (unsigned idx = 1 ; idx < F_ ; ++idx) {
		const auto simp = population_[F_*i+idx].simplified();
		if ((fit[idx] > best_fitness) || (fit[idx] == best_fitness && simp.quantum_cost() < best_quantum_cost)) {
		    best_idx = idx;
		    best_fitness = fit[idx];
		    best_quantum_cost = simp.quantum_cost();
		}
	    }
	    best_idx += F_*i;
	    new_population.push_back(population_[best_idx]);
	}
	fails_ = new_fails;
	population_.clear();
	population_.reserve(S_*F_);
	for (const auto& circuit : new_population) {
	    population_.push_back(circuit);
	    for (unsigned i = 0 ; i < F_-1 ; ++i) {
		auto temp_circuit = circuit;
		mut_strat_.mutate(rng, temp_circuit);
		population_.push_back(temp_circuit);
	    }
	}
	std::shuffle(population_.begin(), population_.end(), rng);
    }
};


#endif // OPTIMIZER_HH_
