#ifndef _DOUSI_CORE_OPTIONS_H_
#define _DOUSI_CORE_OPTIONS_H_

#include <string>

namespace dousi {

struct ExecutorOptions final {
    uint16_t work_thread_num_ = 4;
};

}

#endif
