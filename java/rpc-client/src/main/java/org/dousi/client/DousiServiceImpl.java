package org.dousi.client;

public class DousiServiceImpl implements DousiService {

    private DousiRpcClient rpcClient;

    private String serviceName;

    public DousiServiceImpl(DousiRpcClient rpcClient, String serviceName) {
        this.rpcClient = rpcClient;
        this.serviceName = serviceName;
    }

    public DousiAsyncFunction asyncFunction(String funcName) {
        return new DousiAsyncFunction(rpcClient, serviceName, funcName);
    }

}
