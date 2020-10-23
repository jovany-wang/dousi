#include "basic_services.h"
#include "core/dousi.h"
#include "core/executor/dousi_service.h"
#include "runtime/direct/rpc_server.h"

int main() {

    // This is the util server to test.
    dousi::RpcServer rpc_server("127.0.0.1:10002");

    {
        // Echo service
        dousi::DousiService<Echoer> echoer = rpc_server.CreateService<Echoer>();
        echoer.RegisterMethod(dousi::Remote(&Echoer::echo));
    }
    {
        // Calculator service
        auto calculator = rpc_server.CreateService<Calculator>();
        calculator.RegisterMethod(dousi::Remote(&Calculator::add));
        calculator.RegisterMethod(dousi::Remote(&Calculator::sub));
    }

    {
        // VoidReturnService
        auto void_return = rpc_server.CreateService<VoidReturnService>();
        void_return.RegisterMethod(dousi::Remote(&VoidReturnService::Perform));
        void_return.RegisterMethod(dousi::Remote(&VoidReturnService::Get));
    }

    {
        // User-defined class as argument or return type.
        auto user_defined = rpc_server.CreateService<UserDefinedClass>();
        user_defined.RegisterMethod(dousi::Remote(&UserDefinedClass::IncrAge));
    }

    rpc_server.Loop();

    return 0;
}
