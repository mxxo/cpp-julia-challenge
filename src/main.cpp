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
#include <type_traits>
#include <vector>

// a simple user-defined function
inline auto user_defined(auto x) {
  return x + 1;
}

#ifndef BENCH

//scalar case
template<typename T> struct is_vector : public std::false_type {};
// array of arrays case
template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template <typename T>
class AbstractArray {
    T arrayType;
    // vector case
    void broadcast( std::true_type ) {
        std::cout << "ya vector" << std::endl;
    }
    // scalar case
    void broadcast( std::false_type ) {
      std::cout << "not vector" << std::endl;
    }
public:
    void broadcast() {
        broadcast( is_vector<T>{} );
    }
};

// materialize is variadic template
template<typename Func, typename... Args> void materialize();

void broadcast(auto x){
  x.broadcast();
}

// single array case

// scalar case

auto main() -> int {
  std::cout << user_defined(2) << std::endl;
  std::cout << user_defined(3.5) << std::endl;
  auto i = 1;
  std::cout << user_defined(i) << std::endl;
  std::cout << i << std::endl;

  // kind of clunky for scalars for sure
  AbstractArray<int> j;
  AbstractArray<std::vector<int>> vj;
  AbstractArray<std::vector<std::vector<int>>> vvj;

  j.broadcast();
  vj.broadcast();
  vvj.broadcast();
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
