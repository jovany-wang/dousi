#ifndef _DOUSI_FUTURE_H_
#define _DOUSI_FUTURE_H_

#include "core/submitter/submitter_runtime.h"

namespace dousi {


template <typename T>
class DousiFuture final {

public:
    explicit DousiFuture(uint64_t object_id): object_id_(object_id) {
    }

    std::shared_ptr<T> Get() const {
        if (has_data_) {
            return data_ptr_;
        }

        data_ptr_ = SubmitterRuntime::GetInstance().GetObject<T>(object_id_);
        has_data_ = true;
        return data_ptr_;
    }

    [[nodiscard]] bool HasData() const {
        return has_data_;
    }

private:
//    std::optional instead.
    mutable bool has_data_ = false;

    mutable std::shared_ptr<T> data_ptr_ = nullptr;

    uint64_t object_id_ = -1;
};

}

#endif
