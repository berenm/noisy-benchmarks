#include "driver/measure-allocator.hpp"
#include "common/probe.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

std::string shootout::common::probe_data::name(std::size_t identifier) {
  switch (identifier) {
    case 0:
      return "alloc";

    case 1:
      return "dealloc";

    default:
      return "";
  }
}

int main(int argc, char const* argv[]) {
  std::string const output_prefix = argv[1];
  std::size_t       iterations    = boost::lexical_cast< std::size_t >(argv[2]);

  std::string scenario = "128";

  if (argc > 3) {
    scenario = argv[3];
  }

  shootout::test::alloc_data* alloc_datas = new shootout::test::alloc_data[iterations];

  shootout::common::probe_buffer::initialize(output_prefix);

  if (scenario == "random") {
    boost::random::mt19937                     random_generator;
    boost::random::uniform_int_distribution< > size_distribution(1, 512);
    boost::random::uniform_int_distribution< > action_distribution(0, 1);

    bool*       actions   = new bool[iterations];
    std::size_t allocated = 0;

    for (std::size_t i = 0; i < iterations; ++i) {
      alloc_datas[i].size = size_distribution(random_generator);

      do {
        actions[i] = action_distribution(random_generator);
      } while ((actions[i] && allocated == 0) ||
               (!actions[i] && allocated == iterations - 1));

      allocated += (actions[i] ? -1 : 1);
    }

    shootout::test::alloc_data* alloc_data   = alloc_datas;
    shootout::test::alloc_data* dealloc_data = alloc_datas;
    for (bool* action = actions; action != actions + iterations; ++action) {
      if (*action) {
        __builtin_prefetch(dealloc_data, 0);

        {
          shootout::common::scoped_probe probe(1);
          shootout::test::dealloc(dealloc_data);
        }

        ++dealloc_data;

      } else {
        __builtin_prefetch(alloc_data, 0);
        __builtin_prefetch(alloc_data, 1);

        {
          shootout::common::scoped_probe probe(0);
          shootout::test::alloc(alloc_data);
        }

        ++alloc_data;
      }
    }

    delete[] actions;

  } else {
    std::size_t allocated_size = boost::lexical_cast< std::size_t >(scenario);
    for (std::size_t i = 0; i < iterations; ++i) {
      alloc_datas[i].size = allocated_size;
    }

    for (shootout::test::alloc_data* alloc_data = alloc_datas; alloc_data != alloc_datas + iterations; ++alloc_data) {
      __builtin_prefetch(alloc_data, 0);
      __builtin_prefetch(alloc_data, 1);

      {
        shootout::common::scoped_probe probe(0);
        shootout::test::alloc(alloc_data);
      }
    }

    shootout::common::probe_buffer::instance().flush();

    for (shootout::test::alloc_data* alloc_data = alloc_datas; alloc_data != alloc_datas + iterations; ++alloc_data) {
      __builtin_prefetch(alloc_data, 0);

      {
        shootout::common::scoped_probe probe(1);
        shootout::test::dealloc(alloc_data);
      }
    }

  }

  delete[] alloc_datas;

  return 0;
} // main
