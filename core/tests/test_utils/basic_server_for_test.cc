#include "basic_services.h"
#include "core/dousi.h"
#include "core/executor/api.h"
#include "core/executor/dousi_service.h"

int main() {
    // This is the util server to test.
    dousi::executor::Init("127.0.0.1:10001");
    {
        // Echo service
        dousi::DousiService<Echoer> echoer = dousi::executor::CreateService<Echoer>();
        echoer.RegisterMethod(dousi::Remote(&Echoer::echo));
    }
    {
        // Calculator service
        auto calculator = dousi::executor::CreateService<Calculator>();
        calculator.RegisterMethod(dousi::Remote(&Calculator::add));
        calculator.RegisterMethod(dousi::Remote(&Calculator::sub));
    }

    dousi::executor::Loop();

    return 0;
}
