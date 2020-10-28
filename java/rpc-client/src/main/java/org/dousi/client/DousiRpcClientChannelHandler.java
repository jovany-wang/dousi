package org.dousi.client;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Type;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class DousiRpcClientChannelHandler extends ChannelInboundHandlerAdapter {

    private final static Logger LOG = LoggerFactory.getLogger(DousiRpcClientChannelHandler.class);

    private NettyRpcClient nettyRpcClient;

    public DousiRpcClientChannelHandler(NettyRpcClient nettyRpcClient) {
        this.nettyRpcClient = nettyRpcClient;
    }

    @Override
    public void channelActive(ChannelHandlerContext ctx) {}

    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) throws InvocationTargetException, NoSuchMethodException, IllegalAccessException, IOException, InterruptedException {
        ByteBuf buf = (ByteBuf) msg;
        if (buf.readableBytes() < 8) {
            LOG.debug("buf is less than 8 bytes as header.");
            // Not enough to a header.
            throw new RuntimeException("Header is not ready.");
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
            // bytes are not enough to parse a msg.
            return;
        }

        // parse.
        byte[] req = new byte[bodySize];
        buf.readBytes(req);

        Class<?> returnType = nettyRpcClient.getReturnType(objectId);
        Object result = new Codec().decodeReturnValue(returnType, req);
        nettyRpcClient.putReturnValue(objectId, result);
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
