package org.dousi.client;


import io.netty.bootstrap.Bootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.channel.*;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioSocketChannel;
import org.dousi.common.exception.ParserAddrException;
import java.io.IOException;
import java.lang.reflect.Proxy;
import java.nio.ByteOrder;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

public class NettyRpcClient implements DousiRpcClient {

    private String ip;
    private int port;

    private ChannelFuture cf;

    private AtomicInteger ai = new AtomicInteger(0);

    private ConcurrentHashMap<Integer, Class<?>> returnTypes = new ConcurrentHashMap<>();

    private ConcurrentHashMap<Integer, CompletableFuture<Object>> returnFutures = new ConcurrentHashMap<>();

    private EventLoopGroup bossGroup;

    public NettyRpcClient(String serverAddr) throws InterruptedException, ParserAddrException {
        try {
            String[] split = serverAddr.split(":");
            ip = split[0];
            port = Integer.parseInt(split[1]);
        } catch (Exception e) {
            throw new ParserAddrException("Parsing serverAddr fail");
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
                        socketChannel.pipeline().addLast(new DousiRpcClientChannelHandler(NettyRpcClient.this));
                    }
                });

        cf = bs.connect(ip, port).sync();
    }

    @Override
    public DousiService getService(String serviceName) {
        return new DousiServiceImpl(this, serviceName);
    }

    @Override
    public CompletableFuture<Object> invoke(Class<?> returnClz, String serviceName, String funcName, Object[] args) {
        try {
            return submit(returnClz, serviceName, funcName, args);
        } catch (IOException | InterruptedException e) {
            return null;
        }
    }

    CompletableFuture<Object> submit(Class<?> returnType, String serviceName, String methodName, Object[] args) throws IOException, InterruptedException {
        final int objectId = ai.getAndIncrement();
        final byte[] encoded = new Codec().encodeFunc(serviceName, methodName, args);
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

    @Override
    public void shutdown() {
        bossGroup.shutdown();
    }

    Class<?> getReturnType(int objectId) {
       return returnTypes.get(objectId);
    }

    void putReturnValue(int objectId, Object result) {
        returnFutures.get(objectId).complete(result);
    }

    @Override
    public <T> T newStub(Class<T> serviceClz) {
        return (T) Proxy.newProxyInstance(
                serviceClz.getClassLoader(), new Class[]{serviceClz}, new ProxyHandler<T>(this, serviceClz));
    }
}
