#include "instruction.hh"
#include "circuit.hh"
#include "optimizer.hh"
#include "functions.hh"
#include "mutation_strategy.hh"

#include <sstream>
#include <random>
#include <fstream>
#include <string>
#include <cstdlib>
#include <boost/program_options.hpp>
#include <omp.h>


namespace po = boost::program_options;


int main(int argc, char *argv[]) {
    using Reg_t = uint16_t;

    po::options_description desc("Allowed options");
    desc.add_options()
	("output,o", po::value<std::string>(), "Name of output file")
	("function,f", po::value<std::string>(), "Function to optimize")
	("num_lines,l", po::value<unsigned>(), "Number of lines of the circuit")
	("min_num_gates,d", po::value<unsigned>(), "Minumum number of gates")
	("max_num_gates,D", po::value<unsigned>(), "Maximum number of gates")
	("num_gates_increment,i", po::value<unsigned>(), "Increment in the number of gates")
	("num_survivors,S", po::value<unsigned>(), "Number of survivors per generation")
	("num_offspring,F", po::value<unsigned>(), "Number of offspring per survivor")
	("batch_size,b", po::value<unsigned>(), "Number of inputs to test each circuit with")
	("optimizations_per_circuit,n", po::value<int>(), "Number of optimization passes per circuit")
	("seed,s", po::value<int>()->default_value(0), "Seed to initialize the RNG with");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("output") == 0 ||
	    vm.count("function") == 0 ||
	    vm.count("num_lines") == 0 ||
	    vm.count("min_num_gates") == 0 ||
	    vm.count("max_num_gates") == 0 ||
	    vm.count("num_gates_increment") == 0 ||
	    vm.count("num_survivors") == 0 ||
	    vm.count("num_offspring") == 0 ||
	    vm.count("batch_size") == 0 ||
	    vm.count("optimizations_per_circuit") == 0) {
	std::cout << desc << std::endl;
	exit(1);
    }

    const unsigned l = vm["num_lines"].as<unsigned>();
    const unsigned d_min = vm["min_num_gates"].as<unsigned>();
    const unsigned d_max = vm["max_num_gates"].as<unsigned>();
    const unsigned d_inc = vm["num_gates_increment"].as<unsigned>();
    const unsigned S = vm["num_survivors"].as<unsigned>();
    const unsigned F = vm["num_offspring"].as<unsigned>();
    const unsigned b = vm["batch_size"].as<unsigned>();
    const int optimizations_per_circuit = vm["optimizations_per_circuit"].as<int>();
    const int seed = vm["seed"].as<int>();
    
    unsigned num_threads;
    #pragma omp parallel
    {
	#pragma omp master
	num_threads = omp_get_num_threads();
    }
    std::vector<std::mt19937> rngs(num_threads);
    std::vector<FullyConnectedMutationStrategy<Reg_t>> mut_strats(num_threads);
    #pragma omp parallel
    {
	const int tidx = omp_get_thread_num();
	rngs[tidx] = std::mt19937(seed+tidx);
	mut_strats[tidx] = FullyConnectedMutationStrategy<Reg_t>(l);
    }

    std::ofstream output_file;
    output_file.open(vm["output"].as<std::string>());
    unsigned output_size;
    for (unsigned d = d_min ; d <= d_max ; d += d_inc) {
	std::vector<Circuit<Reg_t>> best_per_optim(optimizations_per_circuit);
	std::vector<std::tuple<double, double, double>> e_per_optim(optimizations_per_circuit);
	#pragma omp parallel for
	for (int i = 0 ; i < optimizations_per_circuit ; ++i) {
	    const int tidx = omp_get_thread_num();
	    using MS_t = FullyConnectedMutationStrategy<Reg_t>;
	    #define DO_OPTIMIZATION(fn) Optimizer<Reg_t, fn, MS_t> optimizer(rngs[tidx], l, d, S, F, mut_strats[tidx]);		\
					optimizer.optimize(rngs[tidx], 100*d, 0.5, b);						\
					best_per_optim[i] = optimizer.compute_best();						\
					e_per_optim[i] = best_per_optim[i].errors(fn{});					\
					output_size = fn::output_size;
	    const std::string function_name = vm["function"].as<std::string>();
	    if (function_name == "2of5") {
		DO_OPTIMIZATION(Func2of5);
	    }
	    else if (function_name == "4mod5") {
		DO_OPTIMIZATION(Func4mod5);
	    }
	    else if (function_name == "5mod5") {
		DO_OPTIMIZATION(Func5mod5);
	    }
	    else if (function_name == "6sym") {
		DO_OPTIMIZATION(Func6sym);
	    }
	    else if (function_name == "9sym") {
		DO_OPTIMIZATION(Func9sym);
	    }
	    else if (function_name == "Id") {
		DO_OPTIMIZATION(FuncId);
	    }
	    else if (function_name == "Xor5") {
		DO_OPTIMIZATION(FuncXor5);
	    }
	    else if (function_name == "NthPrime3") {
		DO_OPTIMIZATION(FuncNthPrime3);
	    }
	    else if (function_name == "NthPrime4") {
		DO_OPTIMIZATION(FuncNthPrime4);
	    }
	    else {
		std::cout << "Unknown function: '" << function_name << "'" << std::endl;
		exit(1);
	    }
	    #undef DO_OPTIMIZATION
	}
	Circuit<Reg_t> best;
	double best_e = 1;
	double best_fn;
	double best_fp;
	for (int i = 0 ; i < optimizations_per_circuit ; ++i) {
	    auto [e, fn, fp] = e_per_optim[i];
	    if (e < best_e) {
		best = best_per_optim[i];
		best_e = e;
		best_fn = fn;
		best_fp = fp;
	    }
	}

	std::cout << best << std::endl;
	std::cout << best.simplified(output_size) << std::endl;
	std::cout << l << ' ' << d << ' ' << best_e << ' ' << best_fn << ' ' << best_fp << std::endl;
	// Write the best circuit to the output file
	best.serialize(output_file);
	output_file << l << ' ' << d << ' ' << best_e << ' ' << best_fn << ' ' << best_fp << ' ' << best.simplified(output_size).quantum_cost() << '\n';
	best.extend(d_inc);
    }
    
    return 0;
}
