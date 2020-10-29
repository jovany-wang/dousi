package org.dousi.examples.adder;

import org.dousi.server.DousiRpcServer;
import org.dousi.common.exception.ParserAddrException;

public class ServerMain {

    public static void main(String[] args) throws ParserAddrException, InterruptedException {
        DousiRpcServer rpcServer = new DousiRpcServer("127.0.0.1:10002");
        rpcServer.registerService(AdderService.class, new AdderServiceImpl());
        rpcServer.loop();
    }
}
