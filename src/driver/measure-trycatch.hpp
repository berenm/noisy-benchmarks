#ifndef __SHOOTOUT_MEASURE_TRYCATCH_HPP__
#define __SHOOTOUT_MEASURE_TRYCATCH_HPP__

#include <stdexcept>

namespace shootout {
  namespace test {

    void trycatch(bool const do_throw) throw (std::runtime_error);

  }
}

#endif // ifndef __SHOOTOUT_MEASURE_TRYCATCH_HPP__
