#include "common/endpoint.h"

namespace dousi {
asio_tcp::resolver::results_type Endpoint::Resolve(boost::asio::io_context &io_context) const {
    asio_tcp::resolver resolver(io_context);
    return resolver.resolve(GetHost(), std::to_string(GetPort()));
}
}
