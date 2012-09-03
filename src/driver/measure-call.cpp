#include "driver/measure-call.hpp"
#include "common/probe.hpp"

#include <boost/lexical_cast.hpp>

std::string shootout::common::probe_data::name(std::size_t identifier) {
  return "";
}

int main(int argc, char const* argv[]) {
  std::string const output_prefix = argv[1];
  std::size_t       iterations    = boost::lexical_cast< std::size_t >(argv[2]);

  shootout::common::probe_buffer::initialize(output_prefix);

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(0);
    shootout::test::call();
  }

  return 0;
}
