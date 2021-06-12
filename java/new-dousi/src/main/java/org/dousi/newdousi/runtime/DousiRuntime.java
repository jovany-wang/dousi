package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiRuntimeType;

import java.util.concurrent.CompletableFuture;

public class DousiRuntime {

    private DousiRuntimeType runtimeType = DousiRuntimeType.NONE;

    private DousiServerRuntime serverRuntime;

    private DousiClientRuntime clientRuntime;

    DousiRuntime(DousiRuntimeType type) {
        assert type != DousiRuntimeType.NONE;
        if (type == DousiRuntimeType.SERVER_RUNTIME) {
            serverRuntime = new DousiServerRuntime("127.0.0.1:10002");
        } else if (type == DousiRuntimeType.CLIENT_RUNTIME) {
            clientRuntime = new DousiClientRuntime();
        }
    }

    <ReturnType> CompletableFuture<ReturnType> asyncInvoke(String serviceName, String methodName, Object[] args) {
        return null;
    }


}
