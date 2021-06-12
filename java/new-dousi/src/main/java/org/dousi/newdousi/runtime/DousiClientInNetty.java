package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiClient;
import org.dousi.newdousi.api.DousiMethod2;

import java.util.concurrent.CompletableFuture;

public class DousiClientInNetty implements DousiClient {

    private DousiRuntime runtime = new DousiRuntime();

    public DousiClientInNetty(String addr) {

    }

    @Override
    public void shutdown() {

    }

    @Override
    public <ServiceType, Arg1Type, ReturnType> CompletableFuture<ReturnType> asyncInvoke(DousiMethod2<ServiceType, Arg1Type, ReturnType> method, Object arg1) {
        return runtime.asyncInvoke("service_class_name", "method_name", new Object[] {arg1});
    }
}
