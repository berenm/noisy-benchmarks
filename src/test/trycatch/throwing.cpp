#include "driver/measure-trycatch.hpp"

namespace shootout {
  namespace test {

    void trycatch(bool const do_throw) throw (std::runtime_error) {
      if (do_throw) {
        throw std::runtime_error("");
      }
    }

  }
}
