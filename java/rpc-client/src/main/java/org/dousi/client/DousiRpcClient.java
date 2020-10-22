package org.dousi.client;

import java.util.concurrent.CompletableFuture;

public interface DousiRpcClient {

    void connect(String serverAddress);

    DousiService getService(String serviceName);

    CompletableFuture<Object> invoke(Class<?> returnClz, String funcName, Object[] args);

    void close();
}
