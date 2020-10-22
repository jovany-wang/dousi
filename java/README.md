# Java Part of Dousi

## Install
First, you should build the native libraries:
```shell script
./build_native_libraries.sh
```

Then,
```shell script
mvn clean install -DskipTests
```
Now you can use it.

## Quick Example
1. Write your C++ RPC server and run it.
```c++
class Adder : public dousi::AbstractService {
public:
    int add(int x, int y) {
        return x + y;
    }
};

int main(int argc, char *argv[]) {
    dousi::RpcServer rpc_server {"127.0.0.1:10001"};
    dousi::DousiService<Adder> adder_service = rpc_server.CreateService<Adder>();
    auto add_rm = dousi::Remote(&Adder::add);
    adder_service.RegisterMethod(add_rm);
    rpc_server.Loop();

    return 0;
}
```

2. Write you Java client and connect to the C++ RPC server.
```java
public class Example {
    public static void main(String[] args) throws InterruptedException, ExecutionException {
        DousiRpcClient rpcClient = new NativeRpcClient("127.0.0.1:10001");
        DousiService service = rpcClient.getService("Adder");
        CompletableFuture<Object> future = service.asyncFunction("add").invoke(Integer.class, 2, 3);
        System.out.println("The result of add(2, 3) is " + future.get());
    }
}
```
