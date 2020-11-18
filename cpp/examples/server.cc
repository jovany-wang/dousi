#include "services.h"

#include "core/dousi.h"

#include "core/executor/dousi_service.h"
#include "rpc/rpc_server.h"

int main(int argc, char *argv[]) {

    dousi::RpcServer rpc_server {/*master_address=*/"127.0.0.1:10001"};

    dousi::DousiService<Adder> adder_service = rpc_server.CreateService<Adder>();
    auto add_rm = dousi::Remote(&Adder::add);
    adder_service.RegisterMethod(add_rm);
    adder_service.RegisterMethod(dousi::Remote(&Adder::sub));

    dousi::DousiService<Echoer> echo_service = rpc_server.CreateService<Echoer>();
    echo_service.RegisterMethod(dousi::Remote(&Echoer::echo));

    rpc_server.Loop();

    return 0;
}
