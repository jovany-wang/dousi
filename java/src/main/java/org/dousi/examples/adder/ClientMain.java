package org.dousi.examples.adder;

import org.dousi.client.DousiRpcClient;
import org.dousi.client.NettyRpcClient;
import org.dousi.common.exception.ParserAddrException;

public class ClientMain {

    public static void main(String[] args) throws InterruptedException, ParserAddrException {
        DousiRpcClient rpcClient = new NettyRpcClient("127.0.0.1:10002");
        AdderService serviceStub = rpcClient.newStub(AdderService.class);
        int i = serviceStub.add(3, 10);
        int j = serviceStub.add(23, 43);
        System.out.println("i=" + i);
        System.out.println("j=" + j);
    }
}
