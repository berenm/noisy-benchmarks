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

    template< typename ItemType, std::size_t const DesiredItemCount = 2048 >
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
          ItemType      item;
          bool volatile valid;
        };

        record_type records[item_count];

        static position_t const position_mask = position_mask_t::sig_bits_fast;
        position_t              writer;
        position_t              reader;

      public:
        ringbuffer() : writer(0), reader(0) {
          std::memset(records, 0, sizeof(record_type) * item_count);
        }

        inline void push(ItemType const& item) {
          position_t const write_position = this->writer++ & position_mask;
          record_type&     write_record   = this->records[write_position];

          write_record.item  = item;
          write_record.valid = 1;
        }

        inline bool pop(ItemType& item) {
          position_t const read_position = this->reader & position_mask;
          record_type&     read_record   = this->records[read_position];

          if (read_record.valid) {
            item = read_record.item;

            read_record.valid = false;
            this->reader++;
            return true;
          } else {
            return false;
          }
        }

        inline std::size_t size() { return this->writer - this->reader; }

    };

  }
}

#endif // ifndef __SHOOTOUT_COMMON_RINGBUFFER_HPP__
