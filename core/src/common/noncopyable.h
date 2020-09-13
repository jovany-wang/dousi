#ifndef _DOUSI_COMMON_NONCOPYABLE_H_
#define _DOUSI_COMMON_NONCOPYABLE_H_

namespace dousi {
namespace common {

class Noncopyable {
protected:
    Noncopyable() = default;

private:
    Noncopyable(const Noncopyable &) = delete;

    Noncopyable(const Noncopyable &&) = delete;

    Noncopyable &operator=(const Noncopyable &) = delete;
};

}
}
#endif
