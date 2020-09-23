// TODO(qwang): Refine this.
#ifndef _DOUSI_RUNTIME_H_
#define _DOUSI_RUNTIME_H_

#include "abstract_service.h"

#include <common/logging.h>
#include "common/endpoint.h"
#include "core/submitter/service_handle.h"

#include <nameof/nameof.hpp>

#include "core/common/options.h"
#include "common/noncopyable.h"
#include "core/executor/dousi_request.h"
#include "core/common/tuple_traits.h"
#include "core/stream/stream.h"
#include "core/common/std_locked_queue.h"
#include <unordered_map>
#include <string>
#include <functional>

#include <boost/asio.hpp>

namespace dousi {

/**
 * A singleton executor runtime of dousi.
 */
class Executor : public std::enable_shared_from_this<Executor> {
public:
    Executor() : io_service_(), work_(io_service_), executor_options_() {
        if (executor_options_.work_thread_num_ > 0) {
            for (int i = 0; i < executor_options_.work_thread_num_; ++i) {
                std::thread th {[this]() { this->LoopToPerformRequest(); }};
                work_thread_pool_.emplace_back(std::move(th));
            }
        }

        const static auto write_thread_num = 8;
        for (int i = 0; i < write_thread_num; ++i) {
            std::thread th { [this]() { this->LoopToWriteResponse(); }};
            write_thread_pool_.emplace_back(std::move(th));
        }

        monitor_th_ = std::make_unique<std::thread>([this]() {
            while (true) {
                std::cout << "request queue size = " << request_queue_.GetSize() << std::endl
                            << "response queue size = " << response_queue_.GetSize() << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds {5});
            }
        });
    }

    ~Executor() {
        DOUSI_LOG(DEBUG) << "Joining thread pool.";
        for (auto &th : work_thread_pool_) {
            th.join();
        }
        for (auto &th : write_thread_pool_) {
            th.join();
        }
    }

    void Init(const std::string &listening_address) {
        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(
                io_service_, Endpoint(listening_address).GetTcpEndpoint());
    }

    void Shutdown() {
        acceptor_->close();
    }

    void Loop() {
        DoAccept();
        io_service_.run();
    }


    template<typename ServiceType>
    DousiService<ServiceType> CreateService(const std::string &service_name) {
//        constexpr auto service_name  = std::string(NAMEOF_TYPE(ServiceType));
        DOUSI_LOG(INFO) << "Registering service: " << service_name;
        auto service_ptr = std::make_shared<ServiceType>();
        created_services_[service_name] = service_ptr;
        return DousiService<ServiceType>(shared_from_this(), service_ptr);
    }

    template<typename ServiceOriginalType, typename MethodType>
    void RegisterMethod(DousiService<ServiceOriginalType> *service_instance, RemoteMethod<MethodType> remote_method) {
        std::lock_guard<std::mutex> lock {mutex_};
        registered_methods_[remote_method.GetName()] =  std::bind(
                &InvokeHelper<typename FunctionTraits<MethodType>::ReturnType>::template Invoke<MethodType, ServiceOriginalType>,
                remote_method.GetMethod(),
                service_instance,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
                );
    }

    void InvokeMethod(uint64_t stream_id, uint32_t object_id, const std::shared_ptr<char> &buffer_ptr, const size_t &buffer_size);

    uint64_t RequestStreamID() {
        return curr_stream_id_.fetch_add(1, std::memory_order_relaxed);
    }

private:
    void DoAccept();

    void LoopToPerformRequest();

    void PerformRequest(const DousiRequest &request);

    void LoopToWriteResponse();

private:
    std::atomic<uint64_t> curr_stream_id_ =  0;

    // Whether this is unused? If so, remove AbstractService.
    std::unordered_map<std::string, std::shared_ptr<AbstractService>> created_services_;

    // TODO(qwang): This can be refined as WRLock.
    // The mutex that protects registered_methods_.
    std::mutex mutex_;

    std::unordered_map<std::string, std::function<void(const std::shared_ptr<char>&, const size_t buffer_size, std::string &)>> registered_methods_;

    boost::asio::io_service io_service_;

    boost::asio::io_service::work work_;

    // acceptor
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;

    // TODO(qwang): This can be refined as WRLock.
    // The mutex that protects stream_;
    std::mutex streams_mutex_;

    // The stream id to the stream pointer.
    std::unordered_map<uint64_t , std::shared_ptr<AsioStream>> streams_;

    // The queue that queues the request closure.
    StdLockedQueue<DousiRequest> request_queue_;

    StdLockedQueue<DousiResponse> response_queue_;

    // The thread pool that fetch the requests and perform them, then push the result to the response queue.
    std::vector<std::thread> work_thread_pool_;

    std::vector<std::thread> write_thread_pool_;

    std::vector<std::thread> read_thread_pool_;

    std::unique_ptr<std::thread> monitor_th_;

    ExecutorOptions executor_options_;
};

}

#endif
