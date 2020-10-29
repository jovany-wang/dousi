package org.dousi.server;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.PooledByteBufAllocator;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOption;
import io.netty.channel.ChannelPipeline;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import org.dousi.common.exception.ParserAddrException;

import java.util.concurrent.ConcurrentHashMap;

public class DousiRpcServer {

    private io.netty.channel.Channel serverChannel;

    private NioEventLoopGroup bossGroup;

    private NioEventLoopGroup workerGroup;

    private final ConcurrentHashMap<String, ServiceInfo> serviceInfo = new ConcurrentHashMap<>();

    private final String ip;

    private final int port;

    public DousiRpcServer(String listeningAddr) throws ParserAddrException {
        try {
            String[] split = listeningAddr.split(":");
            ip = split[0];
            port = Integer.parseInt(split[1]);
        } catch (Exception e) {
            throw new ParserAddrException("Parsing listeningAddr fail");
        }
        bossGroup = new NioEventLoopGroup(1);
        workerGroup = new NioEventLoopGroup();
    }

    public ServiceInfo getService() {
        return serviceInfo.get("org.dousi.examples.adder.AdderService");
    }

    public void registerService(Class<?> service, Object instance) {
        serviceInfo.put(service.getName(), new ServiceInfo(service.getName(), instance));
    }

    public void loop() throws InterruptedException {
        ServerBootstrap serverBootstrap = new ServerBootstrap();
        serverBootstrap.group(bossGroup, workerGroup)
                .channel(NioServerSocketChannel.class)
                .childHandler(new ChannelInitializer<SocketChannel>() {
                    @Override
                    protected void initChannel(SocketChannel ch) {
                        ChannelPipeline pipeline = ch.pipeline();
//                        pipeline.addLast("decoder", new ProtobufVarint32FrameDecoder());
//                        pipeline.addLast("encoder", new ProtobufVarint32LengthFieldPrepender());
                        pipeline.addLast("handler", new DousiRpcServerChannelHandler(DousiRpcServer.this));
                    }
                });
        serverBootstrap.childOption(ChannelOption.TCP_NODELAY, true);
        serverBootstrap.childOption(ChannelOption.SO_KEEPALIVE, true);
        serverBootstrap.childOption(ChannelOption.ALLOCATOR, PooledByteBufAllocator.DEFAULT);


        ChannelFuture f = serverBootstrap.bind(ip,port).syncUninterruptibly();
        serverChannel = f.channel();
        serverChannel.closeFuture().sync();
    }

    public void close() {
        if (serverChannel != null) {
            serverChannel.close();
        }
        if (bossGroup != null) {
            bossGroup.shutdownGracefully();
            bossGroup = null;
        }
        if (workerGroup != null) {
            workerGroup.shutdownGracefully();
            workerGroup = null;
        }
    }
}
