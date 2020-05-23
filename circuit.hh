#ifndef CIRCUIT_HH_
#define CIRCUIT_HH_

#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <sstream>
#include "instruction.hh"


template<typename Reg_t>
class Circuit {
public:
    Circuit(unsigned l, unsigned d) : inst_(d), l_(l) { assert(l <= CHAR_BIT*sizeof(Reg_t)); }
    Circuit() = default;
    Circuit(const Circuit&) = default;
    Circuit(Circuit&&) = default;
    Circuit& operator=(const Circuit&) = default;
    Circuit& operator=(Circuit&&) = default;

    Instruction<Reg_t>& operator[](unsigned idx) { return inst_[idx]; }
    const Instruction<Reg_t>& operator[](unsigned idx) const { return inst_[idx]; }

    unsigned l() const { return l_; }
    unsigned d() const { return inst_.size(); }

    unsigned quantum_cost() const {
	unsigned qc = 0;
	for (auto&& i : inst_)
	    qc += i.quantum_cost();
	return qc;
    }

    void extend(unsigned n) {
	for (unsigned i = 0 ; i < n ; ++i)
	    inst_.emplace_back(Gate::Id, 0, 0, 0);
    }

    template<typename Iterable>
    void run(Iterable& input) const {
	for (const auto& inst : inst_)
	    inst.apply(input);
    }

    template<typename Func_t>
    std::tuple<double, double, double> errors(const Func_t& func) const {
	// Test the circuit with every possible input
	const Reg_t input_count = Reg_t(1) << func.input_size;
	std::vector<Reg_t> inputs(input_count);
	std::iota(inputs.begin(), inputs.end(), Reg_t(0));
	// Run the circuit on the samples
	auto outputs = inputs;
	run(outputs);
	// Compute the errors
	size_t num_positive = 0;
	double e = 0;
	double fn = 0;
	double fp = 0;
	for (size_t i = 0 ; i < input_count ; ++i) {
	    const Reg_t in = inputs[i];
	    const Reg_t out = (outputs[i] >> (l_-Func_t::output_size)) & ((Reg_t(1) << Func_t::output_size)-1);
	    const Reg_t exact = func.func_eval(in);
	    for (int bit = 0 ; bit < Func_t::output_size ; ++bit) {
		if (((exact>>bit)&1) == 1)
		    ++num_positive;
		if (((out>>bit)&1) != ((exact>>bit)&1)) {
		    ++e;
		    if (((exact>>bit)&1) == 0 && ((out>>bit)&1) == 1)
			++fp;
		    else
			++fn;
		}
	    }
	}
	e /= Func_t::output_size * inputs.size();
	fn /= num_positive;
	fp /= Func_t::output_size*inputs.size() - num_positive;
	return {e, fn, fp};
    }

    void serialize(std::ostream& os) const {
	os << l() << ' ' << d() << '\n';
	for (auto&& i : inst_)
	    os << i << '\n';
    }

    static Circuit<Reg_t> deserialize(std::istream& is) {
	Circuit<Reg_t> circuit;
	unsigned d;
	is >> circuit.l_;
	is >> d;
	circuit.inst_.resize(d);
	for (unsigned i = 0 ; i < d ; ++i)
	    is >> circuit.inst_[i];
	return circuit;
    }

    Circuit<Reg_t> simplified(unsigned output_size=1) const {
	Circuit<Reg_t> simp = *this;
	simp.remove_identity();
	simp.remove_unnecessary_gates(output_size);
	return simp;
    }

private:
    std::vector<Instruction<Reg_t>> inst_;
    unsigned l_;

    void remove_identity() {
	inst_.erase(std::remove_if(begin(inst_),
				   end(inst_),
				   [](const Instruction<Reg_t>& i) {
				       return i.type() == Gate::Id;
				   }),
		    end(inst_));
    }

    int last_unnecessary_gate(unsigned output_size) const {
	Reg_t unused_bits = ((Reg_t(1)<<output_size)-1) << (l_-output_size);
	int idx;
	for (idx = d()-1 ; idx >= 0 ; --idx) {
	    const auto args = inst_[idx].args();
	    switch (inst_[idx].type()) {
		case Gate::Id:
		    return idx;
		case Gate::X:
		    if (!(args[0] & unused_bits))
			return idx;
		    break;
		case Gate::cX:
		    if (!(args[0] & unused_bits))
			return idx;
		    else
			unused_bits |= args[1];
		    break;
		case Gate::ccX:
		    if (!(args[0] & unused_bits))
			return idx;
		    else
			unused_bits |= args[1] | args[2];
		    break;
		case Gate::Swap:
		    if (!(args[0] & unused_bits) && !(args[1] & unused_bits))
			return idx;
		    else
			unused_bits |= args[0] | args[1];
		    break;
		case Gate::cSwap:
		    if (!(args[0] & unused_bits) && !(args[1] & unused_bits))
			return idx;
		    else
			unused_bits |= args[0] | args[1] | args[2];
		    break;
		default:
		    break;
	    }
	}
	return idx;
    }

    void remove_unnecessary_gates(unsigned output_size=1) {
	int idx = last_unnecessary_gate(output_size);
	while (idx >= 0) {
	    inst_.erase(begin(inst_)+idx);
	    idx = last_unnecessary_gate(output_size);
	}
    }
};


template<typename Reg_t>
std::ostream& operator<< (std::ostream& os, const Circuit<Reg_t>& circuit) {
    for (unsigned l = 0 ; l < circuit.l() ; ++l) {
	for (int row = -1 ; row <= 1 ; ++row) {
	    for (unsigned i = 0 ; i < circuit.d() ; ++i) {
		if (row == 0)
		    os << "\u2500";
		else
		    os << " ";
		circuit[i].print(os, l, row);
	    }
	    os << '\n';
	}
    }
    return os;
}


#endif // CIRCUIT_HH_
