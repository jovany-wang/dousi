#include "basic_services.h"
#include "core/dousi.h"

int main() {
    // This is the util server to test.
    dousi::Init("127.0.0.1:10001");
    {
        // Echo service
        dousi::DousiService<Echoer> echoer = dousi::CreateService<Echoer>();
        echoer.RegisterMethod(dousi::Remote(&Echoer::echo));
    }
    {
        // Calculator service
        auto calculator = dousi::CreateService<Calculator>();
        calculator.RegisterMethod(dousi::Remote(&Calculator::add));
        calculator.RegisterMethod(dousi::Remote(&Calculator::sub));
    }

    dousi::Loop();

    return 0;
}
