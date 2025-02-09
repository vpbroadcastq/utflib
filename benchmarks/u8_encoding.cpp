#include <benchmark/benchmark.h>
#include "benchmark_data.h"
#include "utflib/utflib.h"
#include "utflib/encoders.h"
#include <vector>
#include <iterator>
#include <span>


// 
// utf8 generator
//
// These two benchmarks compare the two methods of the utf8_generator.  A user can either wrap the
// generator in a loop, calling .get() and .go_next() until .is_finished() (the number of iterations
// being == to the number of code units in the codepoint) OR, users can use .get_all(OIt), passing in
// an output iterator.  .get_all() looks at the number of code units and essentially just unrolls
// the loop that the user would manually write using .get().  Is get_all() the faster approach?

// Calling .get() in a loop manually written by the user
static void random_utf32_to_utf8_generator_in_loop_calling_get(benchmark::State& state) {
	// 2025/02/09:  602338 ns       530134 ns         1120
	std::vector<std::uint8_t> dest;
	dest.reserve(get_random_codepoints_dataset_2_utf8().size());
	std::span<const std::uint32_t> s = get_random_codepoints_dataset_2_utf32();
	for (auto _ : state) {
		dest.clear();
		std::back_insert_iterator oit(dest);
		for (std::uint32_t curr_val : s) {
			std::optional<codepoint> ocp = codepoint::to_codepoint(curr_val);
			utf8_generator g(*ocp);
			while (!g.is_finished()) {
				*oit++ = g.get();
				g.go_next();
			}
		}
		benchmark::DoNotOptimize(oit);
		benchmark::DoNotOptimize(dest);
	}
}
BENCHMARK(random_utf32_to_utf8_generator_in_loop_calling_get);

// Using .get_all()
static void random_utf32_to_utf8_generator_get_all_no_loop(benchmark::State& state) {
	// 2025/02/09:  507238 ns       498767 ns         1723
	std::vector<std::uint8_t> dest;
	dest.reserve(get_random_codepoints_dataset_2_utf8().size());
	std::span<const std::uint32_t> s = get_random_codepoints_dataset_2_utf32();
	for (auto _ : state) {
		dest.clear();
		std::back_insert_iterator oit(dest);
		for (std::uint32_t curr_val : s) {
			std::optional<codepoint> ocp = codepoint::to_codepoint(curr_val);
			utf8_generator g(*ocp);
			oit = g.get_all(oit);
		}
		benchmark::DoNotOptimize(oit);
		benchmark::DoNotOptimize(dest);
	}
}
BENCHMARK(random_utf32_to_utf8_generator_get_all_no_loop);
