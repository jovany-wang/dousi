#ifndef _DOUSI_RUNTIME_H_
#define _DOUSI_RUNTIME_H_

#include "abstract_service.h"

#include <common/logging.h>
#include "common/endpoint.h"
#include "core/submitter/service_handle.h"

#include <nameof/nameof.hpp>

#include "common/noncopyable.h"
//#include "dousi_service.h"

#include "core/common/tuple_traits.h"
#include "core/executor/connection_session.h"

#include <unordered_map>
#include <string>
#include <functional>

#include <boost/asio.hpp>

namespace dousi {


template <typename ServiceType>
class DousiService;


/**
 * A singleton executor runtime of dousi.
 */
class ExecutorRuntime : public common::Noncopyable {
public:
    static ExecutorRuntime &GetInstance() {
        static ExecutorRuntime runtime;
        return runtime;
    }

    void Init(const std::string &listening_address) {
        this->listening_address_ = listening_address;
        // TODO(qwang): Do not hardcode this.
        acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(io_service_, Endpoint("127.0.0.1", 10001).GetTcpEndpoint());
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
        return DousiService<ServiceType>(service_ptr);
    }

    // This could be refined as a lambda.
    template<typename MethodType, typename ServiceOriginalType>
    static void InvokeHelper(
            const MethodType &method,
            DousiService<ServiceOriginalType> *service_instance,
            const char *data,
            size_t size,
            std::string &result) {
        msgpack::unpacked unpacked;
        msgpack::unpack(unpacked, data, size);
        using MethodNameWithArgsTupleTypes = typename FunctionTraits<MethodType>::MethodNameWithArgsTuple;
        auto method_name_and_args_tuple = unpacked.get().as<MethodNameWithArgsTupleTypes>();

        auto ret = TraitAndCall(method, service_instance->GetServiceObjectRef(), method_name_and_args_tuple);
        using ReturnType = typename FunctionTraits<MethodType>::ReturnType;
        DOUSI_LOG(INFO) << "The type of the result is " << NAMEOF_TYPE(ReturnType) << ", and the result = " << ret;

        msgpack::sbuffer buffer(1024);
        msgpack::pack(buffer, ret);
        result = {buffer.data(), buffer.size()};

        DOUSI_LOG(INFO) << "Invoke the user method: " << std::string(data, size);
    }

    template<typename ServiceOriginalType, typename MethodType>
    void RegisterMethod(DousiService<ServiceOriginalType> *service_instance, RemoteMethod<MethodType> remote_method) {
        registered_methods_[remote_method.GetName()] =  std::bind(
                &ExecutorRuntime::InvokeHelper<MethodType, ServiceOriginalType>,
                remote_method.GetMethod(),
                service_instance,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
                );
    }

    void InvokeMethod(uint64_t conn_id, const std::string &data, std::string &result) {
        msgpack::unpacked unpacked;
        msgpack::unpack(unpacked, data.data(), data.size());
        auto tuple = unpacked.get().as<std::tuple<std::string>>();
        const auto method_name = std::get<0>(tuple);
        registered_methods_[method_name](data.data(), data.size(), result);

        auto it = conn_sessions_.find(conn_id);
        assert(it != conn_sessions_.end());
        std::shared_ptr<executor::ConnectionSession> conn = it->second;

        // serialize.
        msgpack::sbuffer buffer(1024);
        msgpack::pack(buffer, result);
        std::string result_str {buffer.data(), buffer.size()};
        // Since we send the result in connection_session when after invoking the callback, there is no need to use this Write().
//        conn->Write(result_str);
        DOUSI_LOG(INFO) << "Method invoked, method name is " << method_name << ", result is \"" << result << "\".";
    }

    uint64_t RequestConnectionSessionID() {
        return curr_conn_id_.fetch_add(1, std::memory_order_relaxed);
    }

private:
    void DoAccept();

private:
    ExecutorRuntime() :io_service_(), work_(io_service_) {
    };

private:
    std::atomic<uint64_t> curr_conn_id_ =  0;

    std::unordered_map<std::string, std::shared_ptr<AbstractService>> created_services_;

    std::unordered_map<std::string, std::function<void(const char *, size_t, std::string &)>> registered_methods_;

    boost::asio::io_service io_service_;

    boost::asio::io_service::work work_;

    // The listening address that this RPC server listening on.
    std::string listening_address_;

    // acceptor
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;

    // The connection session id to the session.
    std::unordered_map<uint64_t , std::shared_ptr<executor::ConnectionSession>> conn_sessions_;
};

}

#endif
