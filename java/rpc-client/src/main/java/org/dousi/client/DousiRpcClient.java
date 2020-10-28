package org.dousi.client;

import java.util.concurrent.CompletableFuture;

public interface DousiRpcClient {

    DousiService getService(String serviceName);

    CompletableFuture<Object> invoke(Class<?> returnClz, String funcName, Object[] args);

    void close();

    <T> T newStub(Class<T> serviceClz);
}
