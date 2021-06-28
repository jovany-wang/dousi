package org.dousi.client;

import java.util.concurrent.CompletableFuture;

public interface DousiRpcClient {

    DousiService getService(String serviceName);

    CompletableFuture<Object> invoke(Class<?> returnClz, String serviceName, String funcName, Object[] args);

    void shutdown();

    <T> T newStub(Class<T> serviceClz);
}
