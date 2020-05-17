#ifndef INSTRUCTION_HH_
#define INSTRUCTION_HH_

#include <cstdint>
#include <array>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cassert>


enum class Gate { Id, X, cX, ccX, Swap, cSwap };


template<typename Reg_t>
class Instruction {
public:
    Instruction(Gate type, unsigned arg0, unsigned arg1=0, unsigned arg2=0)
	: type_(type),
	  args_{Reg_t(1)<<arg0, Reg_t(1)<<arg1, Reg_t(1)<<arg2} {}
    Instruction() = default;
    Instruction(const Instruction&) = default;
    Instruction(Instruction&&) = default;
    Instruction& operator=(const Instruction&) = default;
    Instruction& operator=(Instruction&&) = default;

    Gate type() const { return type_; }
    const std::array<Reg_t, 3>& args() const { return args_; }

    template<typename Iterable>
    void apply(Iterable& regs) const {
	switch (type_) {
	    case Gate::Id:
		for (auto& reg : regs) apply_Id(reg);
		break;
	    case Gate::X:
		for (auto& reg : regs) apply_X(reg);
		break;
	    case Gate::cX:
		for (auto& reg : regs) apply_cX(reg);
		break;
	    case Gate::ccX:
		for (auto& reg : regs) apply_ccX(reg);
		break;
	    case Gate::Swap:
		for (auto& reg : regs) apply_Swap(reg);
		break;
	    case Gate::cSwap:
		for (auto& reg : regs) apply_cSwap(reg);
		break;
	    default:
		break;
	}
    }

    unsigned quantum_cost() const {
	switch(type_) {
	    case Gate::Id:
		return 0;
	    case Gate::X:
	    case Gate::cX:
		return 1;
	    case Gate::ccX:
		return 5;
	    case Gate::Swap:
		return 3;
	    case Gate::cSwap:
		return 7;
	    default:
		return 0;
	}
    }

    void print(std::ostream& os, unsigned bit, int line) const {
	const Reg_t bitmask = Reg_t(1) << bit;
	switch (type_) {
	    case Gate::Id:
		if (bitmask == args_[0]) {
		    if (line == -1)
			os << "\u250C\u2500\u2500\u2500\u2500\u2510";
		    else if (line == 0)
			os << "\u2524 Id \u251C";
		    else if (line == 1)
			os << "\u2514\u2500\u2500\u2500\u2500\u2518";
		}
		else {
		    if (line == 0)
			os << "\u2500\u2500\u2500\u2500\u2500\u2500";
		    else
			os << "      ";
		}
		break;
	    case Gate::X:
		if (bitmask == args_[0]) {
		    if (line == -1)
			os << "\u250C\u2500\u2500\u2500\u2510";
		    else if (line == 0)
			os << "\u2524 X \u251C";
		    else if (line == 1)
			os << "\u2514\u2500\u2500\u2500\u2518";
		}
		else {
		    if (line == 0)
			os << "\u2500\u2500\u2500\u2500\u2500";
		    else
			os << "     ";
		}
		break;
	    case Gate::cX:
		if (bitmask == args_[0]) {
		    if (line == -1)
			os << "\u250C\u2500\u2500\u2500\u2510";
		    else if (line == 0)
			os << "\u2524 X \u251C";
		    else if (line == 1)
			os << "\u2514\u2500\u2500\u2500\u2518";
		}
		else if (bitmask > std::min(args_[0], args_[1]) && bitmask < std::max(args_[0], args_[1])) {
		    if (line == -1)
			os << "  \u2502  ";
		    else if (line == 0)
			os << "\u2500\u2500\u253C\u2500\u2500";
		    else if (line == 1)
			os << "  \u2502  ";
		}
		else if (bitmask == args_[1]) {
		    if (line == -1) {
			if (args_[0] < args_[1])
			    os << "  \u2502  ";
			else
			    os << "     ";
		    }
		    else if (line == 0) {
			os << "\u2500\u2500o\u2500\u2500";
		    }
		    else if (line == 1) {
			if (args_[0] < args_[1])
			    os << "     ";
			else
			    os << "  \u2502  ";
		    }
		}
		else {
		    if (line == -1)
			os << "     ";
		    else if (line == 0)
			os << "\u2500\u2500\u2500\u2500\u2500";
		    else if (line == 1)
			os << "     ";
		}
		break;
	    case Gate::ccX:
		if (bitmask == args_[0]) {
		    if (line == -1)
			os << "\u250C\u2500\u2500\u2500\u2510";
		    else if (line == 0)
			os << "\u2524 X \u251C";
		    else if (line == 1)
			os << "\u2514\u2500\u2500\u2500\u2518";
		}
		else if (bitmask == args_[1] || bitmask == args_[2]) {
		    if (line == -1)
			os << "     ";
		    else if (line == 0)
			os << "\u2500\u2500o\u2500\u2500";
		    else if (line == 1)
			os << "     ";
		}
		else {
		    if (line == -1)
			os << "     ";
		    else if (line == 0)
			os << "\u2500\u2500\u2500\u2500\u2500";
		    else if (line == 1)
			os << "     ";
		}
		break;
	    case Gate::Swap:
		if (bitmask == std::min(args_[0], args_[1])) {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500\u2573\u2500";
		    else if (line == 1)
			os << " \u2502 ";
		}
		else if (bitmask == std::max(args_[0], args_[1])) {
		    if (line == -1)
			os << " \u2502 ";
		    else if (line == 0)
			os << "\u2500\u2573\u2500";
		    else if (line == 1)
			os << "   ";
		}
		else if (bitmask > std::min(args_[0], args_[1]) && bitmask < std::max(args_[0], args_[1])) {
		    if (line == -1)
			os << " \u2502 ";
		    else if (line == 0)
			os << "\u2500\u253C\u2500";
		    else if (line == 1)
			os << " \u2502 ";
		}
		else {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500\u2500\u2500";
		    else if (line == 1)
			os << "   ";
		}
		break;
	    case Gate::cSwap:
		if (bitmask == args_[0]) {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500\u2573\u2500";
		    else if (line == 1)
			os << "   ";
		}
		else if (bitmask == args_[1]) {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500\u2573\u2500";
		    else if (line == 1)
			os << "   ";
		}
		else if (bitmask == args_[2]) {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500o\u2500";
		    else if (line == 1)
			os << "   ";
		}
		else {
		    if (line == -1)
			os << "   ";
		    else if (line == 0)
			os << "\u2500\u2500\u2500";
		    else if (line == 1)
			os << "   ";
		}
		break;
	    default:
		if (line == -1)
		    os << "\u2502";
		else if (line == 0)
		    os << "\u253C";
		else if (line == 1)
		    os << "\u2502";
		break;
	}
    }

    void serialize(std::ostream& os) const {
	os << *this;
    }

    static Instruction<Reg_t> deserialize(std::istream& is) {
	Instruction<Reg_t> inst;
	is >> inst;
	return inst;
    }

private:
    Gate type_;
    std::array<Reg_t, 3> args_;

    void apply_Id (Reg_t& reg) const { }
    void apply_X  (Reg_t& reg) const { reg ^= args_[0]; }
    void apply_cX (Reg_t& reg) const { if (reg & args_[1]) apply_X(reg); }
    void apply_ccX(Reg_t& reg) const { if (reg & args_[2]) apply_cX(reg); }
    void apply_Swap (Reg_t& reg) const { Reg_t tmp{reg}; reg &= ~(args_[0]|args_[1]); if (tmp & args_[0]) reg |= args_[1]; if (tmp & args_[1]) reg |= args_[0]; }
    void apply_cSwap(Reg_t& reg) const { if (reg & args_[2]) apply_Swap(reg); }
};


template<typename Reg_t>
std::ostream& operator<<(std::ostream& os, const Instruction<Reg_t>& inst) {
    const auto reg2idx = [](Reg_t reg) {
	unsigned idx = 0;
	for (; !(reg & (Reg_t(1) << idx)) ; ++idx);
	return idx;
    };
    switch(inst.type()) {
	case Gate::Id:
	    os << "Id   ";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    break;
	case Gate::X:
	    os << "X    ";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    break;
	case Gate::cX:
	    os << "cX   ";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[1]);
	    break;
	case Gate::ccX:
	    os << "ccX  ";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[1]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[2]);
	    break;
	case Gate::Swap:
	    os << "Swap ";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[1]);
	    break;
	case Gate::cSwap:
	    os << "cSwap";
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[0]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[1]);
	    os << ' '<< std::setw(2) << std::setfill(' ') << reg2idx(inst.args()[2]);
	    break;
	default:
	    break;
    }
    return os;
}


template<typename Reg_t>
std::istream& operator>>(std::istream& is, Instruction<Reg_t>& inst) {
    std::string name;
    unsigned arg0, arg1, arg2;
    is >> name;
    if (name == "Id") {
	is >> arg0;
	inst = Instruction<Reg_t>(Gate::Id, arg0, arg1, arg2);
    }
    else if (name == "X") {
	is >> arg0;
	inst = Instruction<Reg_t>(Gate::X, arg0, arg1, arg2);
    }
    else if (name == "cX") {
	is >> arg0;
	is >> arg1;
	inst = Instruction<Reg_t>(Gate::cX, arg0, arg1, arg2);
    }
    else if (name == "ccX") {
	is >> arg0;
	is >> arg1;
	is >> arg2;
	inst = Instruction<Reg_t>(Gate::ccX, arg0, arg1, arg2);
    }
    else if (name == "Swap") {
	is >> arg0;
	is >> arg1;
	inst = Instruction<Reg_t>(Gate::Swap, arg0, arg1, arg2);
    }
    else if (name == "cSwap") {
	is >> arg0;
	is >> arg1;
	is >> arg2;
	inst = Instruction<Reg_t>(Gate::cSwap, arg0, arg1, arg2);
    }
    else {
	assert(false && "Unknown Gate!");
    }
    return is;
}


#endif // INSTRUCTION_HH_
