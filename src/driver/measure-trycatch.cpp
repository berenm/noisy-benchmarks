#include "driver/measure-trycatch.hpp"
#include "common/probe.hpp"

#include <boost/lexical_cast.hpp>

std::string shootout::common::probe_data::name(std::size_t identifier) {
  switch (identifier) {
    case 0:
      return "no-throw";

    case 1:
      return "no-throw-global-try";

    case 2:
      return "no-throw-local-try";

    case 3:
      return "throw-local-try";

    case 4:
      return "throw-half-local-try";

    case 5:
      return "throw-1pct-local-try";

    default:
      return "";
  }
}

int main(int argc, char const* argv[]) {
  std::string const output_prefix = argv[1];
  std::size_t       iterations    = boost::lexical_cast< std::size_t >(argv[2]);

  shootout::common::probe_buffer::initialize(output_prefix);

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(0);
    shootout::test::trycatch(false);
  }

  shootout::common::probe_buffer::instance().flush();

  try {
    for (std::size_t i = 0; i < iterations; ++i) {
      shootout::common::scoped_probe probe(1);
      shootout::test::trycatch(false);
    }
  } catch (std::runtime_error const& e) {}

  shootout::common::probe_buffer::instance().flush();

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(2);
    try {
      shootout::test::trycatch(false);
    } catch (std::runtime_error const& e) {}
  }

  shootout::common::probe_buffer::instance().flush();

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(3);
    try {
      shootout::test::trycatch(true);
    } catch (std::runtime_error const& e) {}
  }

  shootout::common::probe_buffer::instance().flush();

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(4);
    try {
      shootout::test::trycatch(i > (iterations / 2));
    } catch (std::runtime_error const& e) {}
  }

  shootout::common::probe_buffer::instance().flush();

  for (std::size_t i = 0; i < iterations; ++i) {
    shootout::common::scoped_probe probe(5);
    try {
      shootout::test::trycatch(i % 100 == 0);
    } catch (std::runtime_error const& e) {}
  }

  shootout::common::probe_buffer::instance().flush();

  return 0;
} // main
