#ifndef FUNCTIONS_HH_
#define FUNCTIONS_HH_

#include <bit>


struct Func2of5 {
    static constexpr unsigned input_size = 5;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) { return std::popcount(reg) == 2 ? 1 : 0; }
};


struct Func4mod5 {
    static constexpr unsigned input_size = 4;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) { return reg % 5 == 0 ? 1 : 0; }
};


struct Func5mod5 {
    static constexpr unsigned input_size = 5;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) { return reg % 5 == 0 ? 1 : 0; }
};


struct Func6sym {
    static constexpr unsigned input_size = 6;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) {
	const auto pcnt = std::popcount(reg);
	return (pcnt >= 2 && pcnt <= 4) ? 1 : 0;
    }
};


struct Func9sym {
    static constexpr unsigned input_size = 9;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) {
	const auto pcnt = std::popcount(reg);
	return (pcnt >= 3 && pcnt <= 6) ? 1 : 0;
    }
};


struct FuncId {
    static constexpr unsigned input_size = 1;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) { return reg; }
};


struct FuncXor5 {
    static constexpr unsigned input_size = 5;
    static constexpr unsigned output_size = 1;

    template<typename Reg_t>
    static Reg_t func_eval(Reg_t reg) { return std::popcount(reg) % 2; }
};


#endif // FUNCTIONS_HH_
