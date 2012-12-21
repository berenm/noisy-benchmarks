#include "driver/measure-hashtable.hpp"
#include "common/probe.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

std::string shootout::common::probe_data::name(std::size_t identifier) {
  switch (identifier) {
    case 0:
      return "insert";

    case 1:
      return "remove";

    default:
      return "";
  }
}

int main(int argc, char const* argv[]) {
  std::string const output_prefix = argv[1];
  std::size_t       iterations    = boost::lexical_cast< std::size_t >(argv[2]);

  shootout::common::probe_buffer::initialize(output_prefix);
  shootout::test::init();

  shootout::test::hash_data* hash_datas = new shootout::test::hash_data[iterations];

  boost::random::mt19937                     random_generator;
  boost::random::uniform_int_distribution< > key_distribution;

  for (std::size_t i = 0; i < iterations; ++i) {
    hash_datas[i].key = key_distribution(random_generator);
    memset(hash_datas[i].value, 1, sizeof(hash_datas[i].value));
  }

  for (shootout::test::hash_data* hash_data = hash_datas; hash_data != hash_datas + iterations; ++hash_data) {
    __builtin_prefetch(hash_data, 0);
    __builtin_prefetch(hash_data, 1);

    {
      shootout::common::scoped_probe probe(0);
      shootout::test::insert(hash_data);
    }
  }

  shootout::common::probe_buffer::instance().flush();

  for (shootout::test::hash_data* hash_data = hash_datas; hash_data != hash_datas + iterations; ++hash_data) {
    __builtin_prefetch(hash_data, 0);

    {
      shootout::common::scoped_probe probe(1);
      shootout::test::remove(hash_data);
    }
  }

  delete[] hash_datas;

  return 0;
} // main
