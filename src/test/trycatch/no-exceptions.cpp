#include "driver/measure-trycatch.hpp"

namespace shootout {
  namespace test {

    int a(size_t depth, bool do_throw, int& calls) __attribute__((noinline));
    int b(size_t depth, bool do_throw, int& calls) __attribute__((noinline));
    int c(size_t depth, bool do_throw, int& calls) __attribute__((noinline));
    int d(size_t depth, bool do_throw, int& calls) __attribute__((noinline));

    int a(size_t depth, bool do_throw, int& calls) {
      calls++;

      if (do_throw && calls > 128)
        return -1;
      else
        return 0;
    }

    int b(size_t depth, bool do_throw, int& calls) {
      int r = 0;
      r = a(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = a(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = a(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = a(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = a(depth+1, do_throw, ++calls);
      if (r != 0) return r;
    }

    int c(size_t depth, bool do_throw, int& calls) {
      int r = 0;
      b(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      b(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      b(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      b(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      b(depth+1, do_throw, ++calls);
      if (r != 0) return r;
    }

    int d(size_t depth, bool do_throw, int& calls) {
      int r = 0;
      r = c(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = c(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = c(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = c(depth+1, do_throw, ++calls);
      if (r != 0) return r;
      r = c(depth+1, do_throw, ++calls);
      if (r != 0) return r;
    }

    void trycatch(bool const do_throw) throw (std::runtime_error) {
      int calls = 0;
      d(0, do_throw, calls);
    }

  }
}
