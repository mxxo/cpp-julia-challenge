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

// playing around with auto templates
// template<auto...> struct auto_struct {};

// vector overload - scalar case
template<typename T> struct is_vector : public std::false_type {};
// vector overload - vector case
template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template <typename Type>
class AbstractArray {
  Type AbstractValue;
    constexpr bool vec_check( std::true_type, auto vec ) {
        std::cout << " found vector " << std::endl;
        for (auto val : vec){
          vec_check(is_vector<decltype(val)>{}, val);
        }
        return true;
    }
    // scalar
    constexpr bool vec_check( std::false_type, auto val ) {
      std::cout << " base case: " << val << std::endl;
      return false;
    }
public:
    AbstractArray(Type value) : AbstractValue(std::move(value)) {}
    constexpr void broadcast() {
      vec_check(is_vector<Type>{}, AbstractValue);
    }
};

// materialize is variadic template
template<typename Func, typename... Args> void materialize();

void broadcast(auto x){
  x.broadcast();
}

auto main() -> int {
  AbstractArray<int> j (2);
  AbstractArray<std::vector<double>> vj ({1.2, 2.3});
  AbstractArray<std::vector<std::vector<int>>> vvj ({{1, 2}, {3, 4}});
  AbstractArray<std::vector<
                std::vector<
                std::vector<int>>>> vvvj ({{{1, 2}, {3, 4}}, {{1, 2}, {3, 4}}});

  std::cout << "scalar : "; j.broadcast(); std::cout << std::endl;
  std::cout << "vector : "; vj.broadcast(); std::cout << std::endl;
  std::cout << "2D-array : "; vvj.broadcast(); std::cout << std::endl;
  std::cout << "3D-array : "; vvvj.broadcast(); std::cout << std::endl;

  std::cout << "what hath god wrought" << std::endl;
}
////////////////////////////////////////////////////////////////////////////////
// to compare impl's let's stop counting sloc here
////////////////////////////////////////////////////////////////////////////////
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
