package org.dousi.newdousi.runtime;

import org.dousi.newdousi.common.SignatureUtils;
import org.msgpack.core.MessageBufferPacker;
import org.msgpack.core.MessagePack;
import org.msgpack.core.MessageUnpacker;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class ServerCodec {

    private final static Logger LOG = LoggerFactory.getLogger(ServerCodec.class);

    // Decode the bytes to the method name and arguments to perform.
    public byte[] decode(DousiServerRuntime serverRuntime, byte[] bytes) throws IOException, InvocationTargetException, IllegalAccessException {
        MessageUnpacker messageUnpacker = MessagePack.newDefaultUnpacker(bytes);

        final int argsNum = messageUnpacker.unpackArrayHeader();
        // The argsNum can be used to find the overloading methods.

        final String serviceName = messageUnpacker.unpackString();
        final String methodName = messageUnpacker.unpackString();
        final long encodedCodeOfParamListOfRequest = messageUnpacker.unpackLong();

        ServiceInfo serviceInfo = serverRuntime.getService(serviceName);
        Method method = serviceInfo.getMethod(methodName);

        Object[] paramTypes = method.getParameterTypes();
        final long encodedCodeOfParamListOnServer  = SignatureUtils.computeEncodedValueOfParamTypeList(paramTypes);
        if (encodedCodeOfParamListOnServer != encodedCodeOfParamListOfRequest) {
            // -1 indicates the parameter type list is not matched.
            return encodeException(-1);
        }

        if (paramTypes.length == 0) {
            // no params.
            LOG.debug("no params.");
//            return new Object[0];
        }

        // Check tuple size from bytes and assert equals to paramTypes.
        Object[] ret = new Object[paramTypes.length];
        for (int i = 0; i  < paramTypes.length; ++i) {
            final Class<?> paramType = (Class<?>) paramTypes[i];
            Object arg = null;
            if (paramType.equals(Integer.class) || paramType.equals(int.class)) {
                arg = messageUnpacker.unpackInt();
            } else if (paramType.equals(Double.class) || paramType.equals(double.class)) {
                arg = messageUnpacker.unpackDouble();
            } else if (paramType.equals(String.class)) {
                arg = messageUnpacker.unpackString();
            } else {
                // user-defined class.
                // TODO
            }
            ret[i] = arg;
        }

        // Note that if we do not set this attr, the un-public service will be failed.
        method.setAccessible(true);
        Object result = method.invoke(serviceInfo.getServiceObject(), ret);
        return encode(result);
    }

    private byte[] encode(Object o) throws IOException {
        MessageBufferPacker messagePacker = MessagePack.newDefaultBufferPacker();
        // packing 0 to the 1st item indicates there is no error.
        messagePacker.packInt(0);
        if (o.getClass().equals(Integer.class) || o.getClass().equals(int.class)) {
            messagePacker.packInt((Integer) o);
        }
        return messagePacker.toByteArray();
    }

    private byte[] encodeException(int errorCode) throws IOException {
        // check errorCode != 0.
        MessageBufferPacker messagePacker = MessagePack.newDefaultBufferPacker();
        messagePacker.packInt(errorCode);
        return messagePacker.toByteArray();
    }
}
