package org.dousi.examples.adder;

import org.dousi.server.DousiRpcServer;

public class ServerMain {

    public static void main(String[] args) {
        DousiRpcServer rpcServer = new DousiRpcServer("127.0.0.1:10001");
        rpcServer.registerService(AdderService.class, new AdderServiceImpl());
        rpcServer.loop();
    }
}
