package org.dousi.server;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.SplittableRandom;

public class DousiRpcServerChannelHandler extends ChannelInboundHandlerAdapter {

    private final static Logger LOG = LoggerFactory.getLogger(DousiRpcServerChannelHandler.class);

    private DousiRpcServer rpcServer;

    public DousiRpcServerChannelHandler(DousiRpcServer rpcServer) {
        this.rpcServer = rpcServer;
    }
    @Override
    public void channelActive(ChannelHandlerContext ctx) {
        System.out.println("客户端连上了");
    }

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
        ByteBuf buf = (ByteBuf) msg;
        if (buf.readableBytes() < 8) {
            LOG.debug("Buffer is less than 8 bytes as header.");
            // The buffer is not enough to handle, so do not decode it now.
            // It will be triggered once the following bytes arrived.
            return;
        }
        buf.order(ByteOrder.LITTLE_ENDIAN);
        final int objectId =buf.readIntLE();

        byte[] bs = new byte[4];
        buf.readBytes(bs, 0, 4);
        ByteBuffer bff = ByteBuffer.wrap(bs);
        bff.order(ByteOrder.LITTLE_ENDIAN);
        final int bodySize = bff.getInt();

        if (bodySize > buf.readableBytes()) {
            LOG.debug("Body size is not enough. Expected is " +
                    bodySize + ", but number of readable bytes is " + buf.readableBytes());
            // The buffer is not enough to handle, so do not decode it now.
            // It will be triggered once the following bytes arrived.
            return;
        }

        // parse.
        byte[] req = new byte[bodySize];
        buf.readBytes(req);

        try {
            byte[] result = new ServerCodec().decode(rpcServer, req);
            ByteBuf byteBuf = ctx.channel().alloc().heapBuffer();
            byteBuf.order(ByteOrder.LITTLE_ENDIAN);
            byteBuf.writeIntLE(objectId);
            byteBuf.writeIntLE(result.length);
            byteBuf.writeBytes(result);
            if (ctx.channel().isActive()) {
//                return ctx.channel().writeAndFlush(byteBuf).sync();
            }
            ctx.channel().writeAndFlush(byteBuf).sync();
            channelRead(ctx, msg);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void channelReadComplete(ChannelHandlerContext ctx) {
        ctx.flush();
    }

    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
        ctx.close();
    }
}
