package org.dousi.newdousi.runtime;

import io.netty.bootstrap.Bootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import java.io.IOException;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.nio.ByteOrder;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class DousiClientRuntime {

    private String ip;
    private int port;

    private ChannelFuture cf;

    private AtomicInteger ai = new AtomicInteger(0);

    private ConcurrentHashMap<Integer, Class<?>> returnTypes = new ConcurrentHashMap<>();

    private ConcurrentHashMap<Integer, CompletableFuture<Object>> returnFutures = new ConcurrentHashMap<>();

    private EventLoopGroup bossGroup;

    public DousiClientRuntime(String serverAddr) throws InterruptedException {
        try {
            String[] split = serverAddr.split(":");
            ip = split[0];
            port = Integer.parseInt(split[1]);
        } catch (Exception e) {
            throw new RuntimeException("Parsing serverAddr fail");
        }

        bossGroup = new NioEventLoopGroup();
        Bootstrap bs = new Bootstrap();
        bs.group(bossGroup)
                .channel(NioSocketChannel.class)
                .option(ChannelOption.SO_KEEPALIVE, true)
                .option(ChannelOption.TCP_NODELAY, true)
                .handler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel socketChannel) throws Exception {
                        socketChannel.pipeline().addLast(new DousiClientRuntimeChannelHandler(DousiClientRuntime.this));
                    }
                });

        cf = bs.connect(ip, port).sync();
    }

//    public DousiService getService(String serviceName) {
//        return new DousiServiceImpl(this, serviceName);
//    }

    public <ReturnType extends Object> CompletableFuture<ReturnType> asyncInvoke(String serviceName, String funcName, Object[] args) {
        final Class thisClazz = this.getClass();
        Type genType = thisClazz.getGenericSuperclass();
        Type[] params = ((ParameterizedType)genType).getActualTypeArguments();
        Class returnTypeClass = (Class) params[0];
        try {
            return submit(returnTypeClass, serviceName, funcName, args);
        } catch (IOException | InterruptedException e) {
            return null;
        }
    }

    private <ReturnType> CompletableFuture<ReturnType> submit(Class<?> returnType, String serviceName, String methodName, Object[] args) throws IOException, InterruptedException {
        final int objectId = ai.getAndIncrement();
        final byte[] encoded = new ClientCodec().encodeFunc(serviceName, methodName, args);
        CompletableFuture<ReturnType> returnedFuture = new CompletableFuture<>();
        returnTypes.put(objectId, returnType);
        returnFutures.put(objectId, (CompletableFuture<Object>) returnedFuture);

        ByteBuf byteBuf = cf.channel().alloc().heapBuffer();
        byteBuf.order(ByteOrder.LITTLE_ENDIAN);
        byteBuf.writeIntLE(objectId);
        byteBuf.writeIntLE(encoded.length);
        byteBuf.writeBytes(encoded);
        cf.channel().writeAndFlush(byteBuf).sync();
        return returnedFuture;

    }

//    @Override
//    public void shutdown() {
//        bossGroup.shutdown();
//    }

    Class<?> getReturnType(int objectId) {
        return returnTypes.get(objectId);
    }

    void putReturnValue(int objectId, Object result) {
        returnFutures.get(objectId).complete(result);
    }

}
