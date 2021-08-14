#ifndef _DOUSI_CORE_EXECUTOR_IO_THREAD_POOL_H_
#define _DOUSI_CORE_EXECUTOR_IO_THREAD_POOL_H_

#include <thread>
#include <array>
#include <thread>
#include <boost/asio.hpp>

namespace dousi {

class IOThreadPool final {
public:
    /// The triple tuple that indicates <thread, io_service, work>.
    using ThreadAndServiceTuple = std::tuple<
            std::shared_ptr<std::thread>,
            std::shared_ptr<boost::asio::io_service>,
            std::shared_ptr<boost::asio::io_service::work>>;


    explicit IOThreadPool(const uint16_t pool_size) : pool_size_(pool_size) {
        all_threads_.clear();
        for (int i = 0; i < pool_size; ++i) {
            auto io_service = std::make_shared<boost::asio::io_service>();
            auto work = std::make_shared<boost::asio::io_service::work>(*io_service);
            auto th = std::make_shared<std::thread>([io_service, work]() {
                io_service->run();
            });
            all_threads_.emplace_back(std::move(th), std::move(io_service), std::move(work));
        }
    }

    /// Note that this method is not thread safe.
    std::shared_ptr<boost::asio::io_service> GetNextIOService() {
        auto ret = std::get<1>(all_threads_[next_index_]);
        next_index_ = (++next_index_) % pool_size_;
        return ret;
    }

private:
    uint16_t pool_size_;

    uint16_t next_index_ = 0;

    std::vector<ThreadAndServiceTuple> all_threads_;

};

}

#endif
