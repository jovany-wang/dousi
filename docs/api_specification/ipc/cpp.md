## P2P Model
The P2P model makes that one service holds all of the others service stubs as clients. Those service instances forms a P2P network graph.
Those services must be the homogeneous.

```c++
class MyService {
    public void heartbeat(std::string &info) {
    
    }
    
    public void ack_heartbeat() {
    
    }   
};
```
TODO


## Master-Slaves Model(Leader-Followers)
1. A master service:
```c++
class MasterCallSlavesService {
public:
    virtual std::string Heartbeat(const std::string &master_info) = 0;   
};
class SlavesCallMasterService {
public:
    virtual std::string ReportSlaveInfo(const std::string &slave_info) = 0;
};

class MasterService : public MasterCallSlavesService {
public:
    std::string ReportSlaveInfo(const SlaveInfo &slave_info) override {
        // some business code.
        alived_slaves_.insert({slave_info.GetId(), slave_info}); 
    }

private:
    std::unordered_map<SlaveID, SlaveInfo> alived_slaves_;
};
```
TODO

## Streaming Model
```c++
// 模版参数std::string是产生的record的类型.
class MockSource : public SourceService<std::string> {
public:
    MockSource() : SourceSerive<std::string>(/*this_endpoint=*/"127.0.0.1:10000/MockSource") {}
    
    void Start() override {
        std::string text = "A redirect is a page which automatically sends visitors to another page,"
                           " usually an article or section of an article. For example, if you type "
                           "UK in the search box or click on the wikilink UK, you will be taken to"
                           " the article United Kingdom with a note at the top of the page (or on "
                           "mobile, in a black message bar at the bottom). This is because the page"
                           " UK contains special wikitext which defines it as a redirect page and "
                           "indicates the target article. It is also possible to redirect to a specific"
                           " section of the target page, using more advanced syntax."; 
    }
    
    std::vector<std::string> lines = MyUtils::SplitByLines(text);
    auto &downsteams = GetDownstreamsRefByRandRobin();
    for (auto &line : lines) {
        Done(downsteams.Next(), line);
    }
};

// 第一个模版参数是上游传过来的数据类型，第二个模版参数是当前节点产生的传到下游的数据类型.
class MapService : public StreamingService<std::string, std::unordered_map<std::string, size_t>> {
public:
    MapService() : StreamingService<std::string, std::pair<std::string, size_t>>(
        /*upstream_endpoint=*/"127.0.0.1:10000/MockSource",
        /*this_endpoint=*/"127.0.0.1:10001/Map") {}
    
    void OnRecord(const std::string &line) override {
        std::unordered_map<std::string, size_t> count = MyUtils::Count(line);
        for (auto &pair : count) {
            // These can be batched.
            Done(GetDownstreamsRef().First(), count); 
        }
    }

};

class ReduceAndSinkService : public StreamingSink<std::unordered_map<std::string, size_t>> {
public:
    ReduceAndSinkService() : StreamingService<std::string, std::pair<std::string, size_t>>(
        /*upstream_endpoint=*/"127.0.0.1:10001/Map") {}
    
       void OnRecord(const std::pair<std::string, size_t> &pair) override {
            UpdateAndSinkResult(pair);
       } 
};
```

## Actor Model
TODO
