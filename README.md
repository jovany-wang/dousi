# dousi
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/jovany-wang/dousi/CI)
![Github Star Number](https://badgen.net/github/stars/jovany-wang/dousi)  
A Modern Multiple Languages RPC Framework.


Note that this is working on progress.

## Features
- Multiple languages servers.
- Dynamic schema and dynamic signification validation. 
- Optimize transferring the large size data to many remote ends.
- Compiling safe both in C++ server and client.

## Pure C++ Usage
### C++ Server
```c++
class Calculator {
public:
    int add(int x, int y) {
        return x + y;
    }

    int sub(int x, int y) {
        return x - y;
    }
};

int main() {

    dousi::Init(/*master=*/"127.0.0.1:100001");

    auto my_service = dousi::CreateService<Calculator>();
    const auto add_rm = dousi::Remote(&Calculator::add);
    adder_service.RegisterMethod(add_rm);
    adder_service.RegisterMethod(dousi::Remote(&Calculator::sub));

    dousi::Loop();

    return 0;
}

```

### C++ Client
```c++
int main() {
    dousi::Init(/*master=*/"127.0.0.1:100001");

    ServiceHandle calc_service = dousi::GetService<Calculator>();
    
    DousiFuture<int> sum_future = calc_service.Call(dousi::Remote(&Calculator::add), 2, 3);
    DousiFuture<int> sub_future = calc_service.Call(dousi::Remote(&Calculator::sub), 10, 3);
    
    std::cout << "2 + 3 = " << *sum_future.Get() << std::endl;
    std::cout << "10 - 3 = " << *sub_future.Get() << std::endl;
    return 0;
}
```

## Cross-Languaged RPC
We now supported that using a Java client to connect and invoke the C++ RPC server.  
More information can be refered in [Java Part of Dousi](https://github.com/jovany-wang/dousi/blob/master/java/README.md)


## Roadmap
- [x] Prototype of C++ implementation
- [x] Prototype of Java async server and client
- [ ] Optimize large data transferring
- [ ] Support multiple languages servers
- [ ] Optimize buffer-shipping in process
