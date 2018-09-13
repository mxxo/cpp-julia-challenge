// Loose spec from https://nextjournal.com/sdanisch/the-julia-challenge
// Needs to work for n arguments
// Needs to work for any combinations of scalars and n-dimensional arrays
// Inside the implementation, one needs to have access to the whole call tree
// and it should be possible to rewrite that tree or flatten it into one loop
// User defined functions and types need to be inlined and SIMD acceleration
// should be used whenever possible

// Code must not be much longer while still being readable.
// Exemptions for languages with tiny standard libraries.
// -> variadic templates are readable right ;)

#include <algorithm>
#include <iostream>
#include <omp.h> // ;)
#include <random>
#include <vector>

// a simple user-defined function
inline auto user_defined(auto x) {
  return x + 1;
}

#ifndef BENCH

auto main() -> int {
  std::cout << user_defined(2) << std::endl;
  std::cout << user_defined(3.5) << std::endl;
  auto i = 1;
  std::cout << user_defined(i) << std::endl;
  std::cout << i << std::endl;
}

#else
#include "benchmark.h"
// time to beat = 10.171ms
void BM_materialize(benchmark::State& state){
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<> dist(1,20);

  std::vector<int> data;
  for (auto _ : state){
    // materialize();
    // dummy for now
    std::generate_n(std::back_inserter(data),
                    dist(rng),
                    [&](){ return dist(rng);}
                   );
  }
}

// google benchmark calls
BENCHMARK(BM_materialize)->Repetitions(5)->ReportAggregatesOnly(true);
BENCHMARK_MAIN();

#endif
