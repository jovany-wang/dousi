package org.dousi.client;


import io.netty.bootstrap.Bootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;

import java.io.IOException;
import java.lang.reflect.Proxy;
import java.nio.ByteOrder;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class NettyRpcClient implements DousiRpcClient {

    private String serverAddr;

    private ChannelFuture cf;

    private AtomicInteger ai = new AtomicInteger(0);

    private ConcurrentHashMap<Integer, Class<?>> returnTypes = new ConcurrentHashMap<>();

    private ConcurrentHashMap<Integer, CompletableFuture<Object>> returnFutures = new ConcurrentHashMap<>();

    public NettyRpcClient(String serverAddr) throws InterruptedException {
        this.serverAddr = serverAddr;

        EventLoopGroup bossGroup = new NioEventLoopGroup();

        Bootstrap bs = new Bootstrap();

        bs.group(bossGroup)
                .channel(NioSocketChannel.class)
                .option(ChannelOption.SO_KEEPALIVE, true)
                .option(ChannelOption.TCP_NODELAY, true)
                .handler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel socketChannel) throws Exception {
//                  socketChannel.pipeline().addLast(MarshallingCodefactory.buildDecoder());
//                  socketChannel.pipeline().addLast(MarshallingCodefactory.buildEncoder());
                        socketChannel.pipeline().addLast(new DousiRpcClientChannelHandler(NettyRpcClient.this));
                    }
                });

        // 客户端开启
        cf = bs.connect("127.0.0.1", 10001).sync();
//        // 发送客户端的请求
//        cf.channel().writeAndFlush(Unpooled.copiedBuffer(reqStr.getBytes(Constant.charset)));
//        // 等待直到连接中断
//        cf.channel().closeFuture().sync();
    }

    @Override
    public DousiService getService(String serviceName) {
        return null;
    }

    @Override
    public CompletableFuture<Object> invoke(Class<?> returnClz, String funcName, Object[] args) {
        return null;
    }

    CompletableFuture<Object> submit(Class<?> returnType, String methodName, Object[] args) throws IOException, InterruptedException {
        synchronized (this) {
            final int objectId = ai.getAndIncrement();
            final byte[] encoded = new Codec().encodeFunc(methodName, args);

            CompletableFuture<Object> returnedFuture = new CompletableFuture<>();
            returnTypes.put(objectId, returnType);
            returnFutures.put(objectId, returnedFuture);

            ByteBuf byteBuf = cf.channel().alloc().heapBuffer();
            byteBuf.order(ByteOrder.LITTLE_ENDIAN);
            byteBuf.writeIntLE(objectId);
            byteBuf.writeIntLE(encoded.length);
            byteBuf.writeBytes(encoded);
            cf.channel().writeAndFlush(byteBuf).sync();
            return returnedFuture;
        }
    }

    @Override
    public void close() {

    }

    Class<?> getReturnType(int objectId) {
       return returnTypes.get(objectId);
    }

    void putReturnValue(int objectId, Object result) {
        synchronized (this) {
            returnFutures.get(objectId).complete(result);
        }
    }

    @Override
    public <T> T newStub(Class<T> serviceClz) {
        return (T) Proxy.newProxyInstance(
                serviceClz.getClassLoader(), new Class[]{serviceClz}, new ProxyHandler<T>(this, serviceClz/*, invoker*/, null));
    }
}
