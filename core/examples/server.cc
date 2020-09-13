#include "services.h"

#include "core/dousi.h"

#include "core/executor/dousi_service.h"
#include "core/executor/api.h"


int main(int argc, char *argv[]) {
    using namespace dousi;

    dousi::executor::Init(/*master_address=*/"127.0.0.1:10001");

    DousiService<Adder> adder_service = dousi::executor::CreateService<Adder>();
    auto add_rm = dousi::Remote(&Adder::add);
    adder_service.RegisterMethod(add_rm);
    adder_service.RegisterMethod(dousi::Remote(&Adder::sub));

    DousiService<Echoer> echo_service = dousi::executor::CreateService<Echoer>();
    echo_service.RegisterMethod(dousi::Remote(&Echoer::echo));

    dousi::executor::Loop();

    return 0;
}
