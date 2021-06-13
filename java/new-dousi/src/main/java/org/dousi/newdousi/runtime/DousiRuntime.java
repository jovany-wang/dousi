package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiRuntimeType;

import java.util.concurrent.CompletableFuture;

public class DousiRuntime {

    private DousiRuntimeType runtimeType = DousiRuntimeType.NONE;

    private DousiServerRuntime serverRuntime;

    private DousiClientRuntime clientRuntime;

    DousiRuntime(DousiRuntimeType type) throws InterruptedException {
        assert type != DousiRuntimeType.NONE;
        if (type == DousiRuntimeType.SERVER_RUNTIME) {
            serverRuntime = new DousiServerRuntime("127.0.0.1:10002");
            try {
                serverRuntime.loop();
            } catch (InterruptedException e) {

            }
        } else if (type == DousiRuntimeType.CLIENT_RUNTIME) {
            clientRuntime = new DousiClientRuntime("127.0.0.1:10002");
        }
    }



    <ReturnType> CompletableFuture<ReturnType> asyncInvoke(String serviceName, String methodName, Object[] args) {
        if (runtimeType == DousiRuntimeType.SERVER_RUNTIME) {
            // serverRuntime.asyncInvoke()
            return null;
        } else if (runtimeType == DousiRuntimeType.CLIENT_RUNTIME) {
            return clientRuntime.asyncInvoke(serviceName, methodName, args);
        }
        return null;
    }


}
