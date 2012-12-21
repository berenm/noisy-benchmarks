#ifndef __SHOOTOUT_COMMON_RINGBUFFER_HPP__
#define __SHOOTOUT_COMMON_RINGBUFFER_HPP__

#include <cstdint>
#include <cstring>
#include <atomic>
#include <string>

#include <boost/integer/static_log2.hpp>
#include <boost/integer/integer_mask.hpp>

namespace shootout {
  namespace common {

    template< typename T, std::size_t const DesiredItemCount = 2048 >
    class ringbuffer {
      protected:
        static std::size_t const try_item_count_order = boost::static_log2< DesiredItemCount >::value;
        static std::size_t const try_item_count       = 1 << try_item_count_order;

        static std::size_t const item_count_order = try_item_count_order + (try_item_count == DesiredItemCount ? 0 : 1);
        static std::size_t const item_count       = 1 << item_count_order;

        typedef boost::low_bits_mask_t< item_count_order > position_mask_t;
        typedef typename position_mask_t::fast             position_t;
        typedef std::atomic< position_t >                  atomic_position_t;

        struct record_type {
          T             value;
          bool volatile valid;
        };

        record_type records[item_count];

        static position_t const position_mask = position_mask_t::sig_bits_fast;
        position_t volatile     writer;
        position_t volatile     reader;

      public:
        ringbuffer() : writer(0), reader(0) {
          std::memset(records, 0, sizeof(record_type) * item_count);
        }

        inline void push(T const& value) {
          position_t const position = __sync_fetch_and_add(&this->writer, 1) & ringbuffer::position_mask;
          record_type&     record   = this->records[position];

          // (x86) stores are not reordered with other stores, record WILL be written before valid.
          record.value = value;

          // however we have to prevent the compiler from reordering the instructions
          __asm__ __volatile__ ("" ::: "memory");

          record.valid = true;
        }

        inline bool pop(T& value) {
          position_t const position = this->reader & ringbuffer::position_mask;
          record_type&     record   = this->records[position];

          // (x86) loads are not reordered with other loads, valid WILL be read before record.
          if (!record.valid) {
            return false;
          }

          // however we have to prevent the compiler from reordering the instructions
          __asm__ __volatile__ ("" ::: "memory");

          value = record.value;

          // (x86) stores are not reordered with older loads, valid WILL be written after record is read.
          record.valid = false;

          this->reader++;
          return true;
        }

        inline std::size_t size() { return this->writer - this->reader; }

    };

  }
}

#endif // ifndef __SHOOTOUT_COMMON_RINGBUFFER_HPP__
