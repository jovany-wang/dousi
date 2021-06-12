package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiRuntimeType;

import java.util.concurrent.CompletableFuture;

public class DousiRuntime {

    private DousiRuntimeType runtimeType = DousiRuntimeType.NONE;


    <ReturnType> CompletableFuture<ReturnType> asyncInvoke(String serviceName, String methodName, Object[] args) {
        return null;
    }
}
