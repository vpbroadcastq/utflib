#include <benchmark/benchmark.h>
#include "benchmark_data.h"
#include "utflib/utflib.h"
#include "utflib/iterators.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>


// Function to benchmark
static void u8it_valid_eqproblen_fwd(benchmark::State& state) {
	// 2025/02/01:  71607 ns        51618 ns
	std::span<const std::uint8_t> s = get_utf8_equal_probability_code_unit_seq_length_dataset_1();
	for (auto _ : state) {
		utf8_iterator it {s};
		while (!it.is_finished()) {
			it.go_next();
		}
		benchmark::DoNotOptimize(it);
	}
}
BENCHMARK(u8it_valid_eqproblen_fwd);

static void u8it_valid_eqproblen_rev(benchmark::State& state) {
	// 2025/02/01:  160902 ns       126596 ns
	std::span<const std::uint8_t> s = get_utf8_equal_probability_code_unit_seq_length_dataset_1();
	utf8_iterator it_end {s};
	while (!it_end.is_finished()) {
		it_end.go_next();
	}
	for (auto _ : state) {
		utf8_iterator it = it_end;
		while (!it.at_start()) {
			it.go_prev();
		}
		benchmark::DoNotOptimize(it);
	}
}
BENCHMARK(u8it_valid_eqproblen_rev);

/*
static void std_string(benchmark::State& state) {
	std::string s;
	for (auto _ : state) {
		benchmark::DoNotOptimize(s);
		s = "yaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaay";
		benchmark::DoNotOptimize(s);
	}
}
BENCHMARK(std_string);

static void std_vector(benchmark::State& state) {
	std::vector<int> v;
	for (auto _ : state) {
		benchmark::DoNotOptimize(v);
		v.push_back(7);
		v.push_back(7);
		v.push_back(7);
		v.push_back(7);
		benchmark::DoNotOptimize(v);
	}
}
BENCHMARK(std_vector);
*/
