#include "common/probe.hpp"

#include <fstream>
#include <boost/thread.hpp>

namespace shootout {
  namespace common {

    std::size_t scoped_probe::sequence = 0;

    std::shared_ptr< probe_buffer > probe_buffer::instance_;

    struct probe_flushing_thread {
      void operator()() {
        cpu_set_t cpus;

        CPU_ZERO(&cpus);
        CPU_SET(0, &cpus);
        pthread_setaffinity_np(pthread_self(), 1, &cpus);

        probe_buffer& buffer = probe_buffer::instance();
        while (true) {
          buffer.flush();
        }
      }

    };

    void probe_buffer::initialize(std::string const& output_prefix) {
      probe_buffer::instance_ = std::make_shared< probe_buffer >(output_prefix);
    }

    probe_buffer::probe_buffer(std::string const& output_prefix) :
      cpuid_overhead(0),
      output_prefix(output_prefix) {
      this->warmup();

      // boost::thread thread = boost::thread(probe_flushing_thread());
    }

    probe_buffer::~probe_buffer() {
      while (this->size()) {
        this->flush();
      }
    }

    void probe_buffer::flush() {
      probe_data item;

      while (this->pop(item)) {
        std::string const& name = probe_data::name(item.identifier);
        if (this->streams.find(name) == this->streams.end()) {
          std::string file_name = this->output_prefix;
          if (name != "") {
            file_name += "-" + name;
          }

          this->streams[name] = std::make_shared< std::ofstream >(file_name + ".dat", std::ios_base::out);
        }

        std::size_t const nanoseconds_since_origin = (item.ticks_start - this->ticks_origin) / this->ticks_per_nanoseconds;
        std::size_t const nanoseconds_duration     = (item.ticks_end - item.ticks_start - this->cpuid_overhead) / this->ticks_per_nanoseconds;
        (*this->streams[name]) << item.sequence << " " << nanoseconds_since_origin << " " << nanoseconds_duration << std::endl;
      }
    }

    void probe_buffer::warmup() {
      std::clog << "warming up... ";

      cpu_set_t cpus;

      CPU_ZERO(&cpus);
      CPU_SET(1, &cpus);
      sched_setaffinity(0, 1, &cpus);
      sched_setscheduler(0, SCHED_FIFO, NULL);

      struct timespec time_start;

      clock_gettime(CLOCK_MONOTONIC, &time_start);
      std::size_t ticks_from = ticks_start< false >();

      std::size_t warmup_count = 1000000;
      this->cpuid_overhead = -1;
      while (--warmup_count) {
        scoped_probe probe;
        std::size_t  ticks = ticks_end< false >();
        this->cpuid_overhead = std::min(this->cpuid_overhead, ticks - probe.data.ticks_start);

        probe.discard();
      }

      struct timespec time_end;

      clock_gettime(CLOCK_MONOTONIC, &time_end);
      std::size_t ticks_to = ticks_end< false >();

      this->ticks_per_nanoseconds = static_cast< double >(ticks_to - ticks_from - this->cpuid_overhead) / (static_cast< double >(time_end.tv_sec - time_start.tv_sec) * 1000 * 1000 * 1000 + (time_end.tv_nsec - time_start.tv_nsec));
      this->ticks_origin          = ticks_to;
      this->cpuid_overhead        = this->cpuid_overhead * 1 / 2;
      std::clog << "done,"
                << " cpuid overhead: " << this->cpuid_overhead
                << " ticks origin: " << this->ticks_origin
                << " ticks per nanosec: " << this->ticks_per_nanoseconds
                << std::endl;
    }

  }
}
