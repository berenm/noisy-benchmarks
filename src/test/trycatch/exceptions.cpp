#include "driver/measure-trycatch.hpp"

namespace shootout {
  namespace test {

    void a(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) __attribute__((noinline));
    void b(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) __attribute__((noinline));
    void c(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) __attribute__((noinline));
    void d(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) __attribute__((noinline));

    void a(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) {
      calls++;

      if (do_throw && calls > 128)
        throw std::runtime_error("");
      else
        return;
    }

    void b(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) {
      a(depth+1, do_throw, ++calls);
      a(depth+1, do_throw, ++calls);
      a(depth+1, do_throw, ++calls);
      a(depth+1, do_throw, ++calls);
      a(depth+1, do_throw, ++calls);
    }

    void c(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) {
      b(depth+1, do_throw, ++calls);
      b(depth+1, do_throw, ++calls);
      b(depth+1, do_throw, ++calls);
      b(depth+1, do_throw, ++calls);
      b(depth+1, do_throw, ++calls);
    }

    void d(size_t depth, bool do_throw, int& calls) throw (std::runtime_error) {
      c(depth+1, do_throw, ++calls);
      c(depth+1, do_throw, ++calls);
      c(depth+1, do_throw, ++calls);
      c(depth+1, do_throw, ++calls);
      c(depth+1, do_throw, ++calls);
    }

    void trycatch(bool const do_throw) throw (std::runtime_error) {
      int calls = 0;
      d(0, do_throw, calls);
    }

  }
}
