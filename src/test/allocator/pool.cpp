#include "driver/measure-allocator.hpp"

#include <boost/pool/pool_alloc.hpp>

namespace shootout {
  namespace test {

    typedef char block_type[64];
    typedef boost::fast_pool_allocator< block_type,
                                        boost::default_user_allocator_malloc_free,
                                        boost::details::pool::null_mutex,
                                        1024*1024 > pool_t;

    void alloc(alloc_data* alloc_data) {
      alloc_data->pointer = pool_t::allocate(alloc_data->size / sizeof(block_type) + 1);
    }

    void dealloc(alloc_data const* alloc_data) {
      pool_t::deallocate(reinterpret_cast< block_type* >(alloc_data->pointer), alloc_data->size / sizeof(block_type) + 1);
    }

  }
}
