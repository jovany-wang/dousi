package org.dousi.newdousi.api;

import java.util.concurrent.CompletableFuture;

public interface DousiClient {

    void shutdown();

    <ServiceType, Arg1Type, ReturnType> CompletableFuture<ReturnType> asyncInvoke(DousiMethod2<ServiceType, Arg1Type, ReturnType> method, Object arg1);
}
