package org.dousi.client;

import org.dousi.utils.JniUtils;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;

public class NativeRpcClient implements DousiRpcClient {

    static {
        JniUtils.loadLibrary("restrpc_jni");
    }

    private long rpcClientPointer = -1;

    private Codec codec;

    // The map to cache return type.
    private ConcurrentHashMap<Long, Class<?>> localFutureReturnTypenameCache = new ConcurrentHashMap<>();

    private ConcurrentHashMap<Long, CompletableFuture<Object>> localFutureCache = new ConcurrentHashMap<>();

    public NativeRpcClient() {
        rpcClientPointer = nativeNewRpcClient();
    }

    public void connect(String serverAddress) {
        if (rpcClientPointer == -1) {
            throw new RuntimeException("no init");
        }
        nativeConnect(rpcClientPointer, serverAddress);
        codec = new Codec();
    }

    public DousiService getService(String serviceName) {
        return new DousiServiceImpl(this, serviceName);
    }

    public CompletableFuture<Object> invoke(Class<?> returnClz, String funcName, Object[] args) {
        if (rpcClientPointer == -1) {
            throw new RuntimeException("no init");
        }

        byte[] encodedBytes = null;
        try {
            encodedBytes = codec.encodeFunc(funcName, args);
        } catch (IOException e) {
            throw new RuntimeException("...");
        }

        if (encodedBytes == null) {
            return null;
        }

        synchronized (this) {
            final long requestId = nativeInvoke(rpcClientPointer, encodedBytes);
            CompletableFuture<Object> futureToReturn = new CompletableFuture<>();
            localFutureReturnTypenameCache.put(requestId, returnClz);

            localFutureCache.put(requestId, futureToReturn);
            return futureToReturn;
        }
    }

    public void close() {
        if (rpcClientPointer == -1) {
            throw new RuntimeException("no init");
        }

        nativeDestroy(rpcClientPointer);
        this.rpcClientPointer = -1;
    }


    private native long nativeNewRpcClient();

    private native void nativeConnect(long rpcClientPointer, String serverAddress);

    private native long nativeInvoke(long rpcClientPointer, byte[] encodedFuncNameAndArgs);

    private native void nativeDestroy(long rpcClientPointer);
}
