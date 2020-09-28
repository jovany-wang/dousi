# dousi
A modern highly performance RPC framework with multiple-languages server.

Note that this is working on progress.

## Features
- Optimized Control flow and Data flow transfering.
- Optimize transfering the large size data to many remote ends.
- Mutilpe languages servers.
- Compiling safe both in C++ server and client.

## Usage
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

## Roadmap
- [x] Prototype of C++ implementation
- [ ] Optimize large data transforing
- [ ] Support mutilpe languages servers
- [ ] Optimize buffer-shipping in process
- [ ] Nested RPC call

## Multipe Advanced Models for Distributed IPC
### 1. Master-Slaves IPC Model
### 2. P2P IPC Model
### 3. Multiple Replicates
