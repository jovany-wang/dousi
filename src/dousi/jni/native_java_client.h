#ifndef _DOUSI_JNI_NATIVE_CLIENT_FOR_JAVA_H_
#define _DOUSI_JNI_NATIVE_CLIENT_FOR_JAVA_H_

#include <string>

#include "core/submitter/submitter.h"
#include "core/submitter/service_handle.h"

namespace dousi {

class NativeClientForJava {
public:
    explicit NativeClientForJava(const RpcReturnedCallback& rpc_return_callback, const std::string &server_address) {
        dousi::DousiLog::StartDousiLog("/tmp/dousi/java-native-client.log",
                                       dousi::LOG_LEVEL::WARNING, 10, 3);
        submitter_ = std::make_shared<Submitter>(ClientLanguage::JAVA, rpc_return_callback);
        submitter_->Init(server_address);
    }

    long Invoke(const std::string &encoded_func_name_and_args) {
        const auto object_id = submitter_->RequestObjectID();
        submitter_->Submit(object_id, /*TODO(qwang) unused=*/"", encoded_func_name_and_args);
        return static_cast<long>(object_id);
    }

    void Close() {
        submitter_->Shutdown();
    }

    ~NativeClientForJava() {
        submitter_->Shutdown();
    }

private:
    std::shared_ptr<Submitter> submitter_ = nullptr;
};

}

#endif
