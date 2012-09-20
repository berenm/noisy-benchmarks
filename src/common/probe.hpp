#ifndef __SHOOTOUT_COMMON_PROBE_HPP__
#define __SHOOTOUT_COMMON_PROBE_HPP__

#include <cstdint>
#include <string>
#include <iostream>
#include <map>
#include <memory>

#include "common/ringbuffer.hpp"

namespace shootout {
  namespace common {

    template< bool const HasRdtscp >
    static inline std::uint64_t ticks_start() {
      std::uint32_t cycles_high;
      std::uint32_t cycles_low;

      __asm__ __volatile__ ("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
      __asm__ __volatile__ ("rdtsc" : "=&d" (cycles_high), "=&a" (cycles_low));

      return (static_cast< std::uint64_t >(cycles_high) << 32) | cycles_low;
    }

    template< bool const HasRdtscp >
    static inline std::uint64_t ticks_end();

    template< >
    inline std::uint64_t ticks_end< true >() {
      std::uint32_t cycles_high;
      std::uint32_t cycles_low;

      __asm__ __volatile__ ("rdtscp" : "=&d" (cycles_high), "=&a" (cycles_low) ::"%rcx");
      __asm__ __volatile__ ("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");

      return (static_cast< std::uint64_t >(cycles_high) << 32) | cycles_low;
    }

    template< >
    inline std::uint64_t ticks_end< false >() {
      std::uint32_t cycles_high;
      std::uint32_t cycles_low;

      __asm__ __volatile__ ("cpuid" ::: "%rax", "%rbx", "%rcx", "%rdx");
      __asm__ __volatile__ ("rdtsc" : "=&d" (cycles_high), "=&a" (cycles_low));

      return (static_cast< std::uint64_t >(cycles_high) << 32) | cycles_low;
    }

    struct probe_data {
      std::size_t   identifier;
      std::size_t   sequence;
      std::uint64_t ticks_start;
      std::uint64_t ticks_end;

      inline probe_data const& operator=(probe_data const& other) {
        this->identifier  = other.identifier;
        this->sequence    = other.sequence;
        this->ticks_start = other.ticks_start;
        this->ticks_end   = other.ticks_end;
        return *this;
      }

      static std::string name(std::size_t identifier);

    };

    struct probe_buffer : public ringbuffer< probe_data, 10000000 > {
      static probe_buffer& instance() { return *instance_; }

      static void initialize(std::string const& output_prefix);

      void warmup();
      void flush();

      probe_buffer(std::string const& output_prefix);
      ~probe_buffer();

      private:
        static std::shared_ptr< probe_buffer > instance_;

        double      ticks_per_nanoseconds;
        std::size_t ticks_origin;
        std::size_t cpuid_overhead;

        typedef std::map< std::string, std::shared_ptr< std::ostream > > stream_map_t;

        stream_map_t streams;
        std::string  output_prefix;
    };

    struct scoped_probe {
      static std::size_t sequence;

      inline scoped_probe(std::size_t const identifier=0) {
        this->data.identifier  = identifier;
        this->data.ticks_start = ticks_start< false >();
      }

      inline ~scoped_probe() {
        if (!this->discarded) {
          this->data.sequence  = ++scoped_probe::sequence;
          this->data.ticks_end = ticks_end< false >();
          probe_buffer::instance().push(this->data);
        }
      }

      void discard() { this->discarded = true; }

      protected:
        friend struct probe_buffer;

        bool       discarded = false;
        probe_data data;
    };

  }
}

#endif // ifndef __SHOOTOUT_COMMON_PROBE_HPP__
