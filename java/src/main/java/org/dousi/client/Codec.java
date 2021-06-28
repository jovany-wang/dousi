package org.dousi.client;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.dousi.common.schema.SignatureUtils;
import org.msgpack.core.MessageBufferPacker;
import org.msgpack.core.MessagePack;
import org.msgpack.core.MessageUnpacker;
import org.msgpack.jackson.dataformat.JsonArrayFormat;
import org.msgpack.jackson.dataformat.MessagePackFactory;
import org.dousi.common.DousiConstants;

import java.io.IOException;
import java.util.Arrays;
import java.util.stream.Collectors;

public class Codec {

    public byte[] encodeFunc(String serviceName, String funcName, Object[] args) throws IOException {
        final int argsLength = args == null ? 0 : args.length;
        if (argsLength > 8) {
            throw new RuntimeException("We don't support more than 8 arguments now.");
        }

        MessageBufferPacker messagePacker = MessagePack.newDefaultBufferPacker();
        // [serviceName, funcName, paramTypeList(A 64-bits Long), args...]
        messagePacker.packArrayHeader(3 + argsLength);
        messagePacker.packString(serviceName);
        messagePacker.packString(funcName);
        Object[] paramArray;
        if (argsLength != 0) {
            paramArray = Arrays.stream(args).map(Object::getClass).collect(Collectors.toList()).toArray();
        } else {
            paramArray = new Object[0];
        }
        final long encodedParamTypes = SignatureUtils.computeEncodedValueOfParamTypeList(paramArray);
        messagePacker.packLong(encodedParamTypes);

        if (argsLength == 0) {
            return messagePacker.toByteArray();
        }

        /// Pack arguments.
        for (Object arg : args) {
            if (arg == null) {
                messagePacker.packNil();
                continue;
            }

            final String argTypeName = arg.getClass().getName();
            switch (argTypeName) {
                case DousiConstants.INT_TYPE_NAME:
                    messagePacker.packInt((int) arg);
                    break;
                case DousiConstants.LONG_TYPE_NAME:
                    messagePacker.packLong((long) arg);
                    break;
                case DousiConstants.STRING_TYPE_NAME:
                    messagePacker.packString((String) arg);
                    break;
                default:
                    // We treat it as a user-defined class.
                    ObjectMapper objectMapper = new ObjectMapper(new MessagePackFactory());
                    objectMapper.setAnnotationIntrospector(new JsonArrayFormat());
                    messagePacker.writePayload(objectMapper.writeValueAsBytes(arg));
//                    throw new RuntimeException("Unknown type: " + argTypeName);
            }
        }
        return messagePacker.toByteArray();
    }

    public Object decodeReturnValue(Class<?> returnClz, byte[] encodedBytes) throws IOException {
        if (returnClz == null) {
            throw new RuntimeException("Internal bug.");
        }

        if (encodedBytes == null) {
            return null;
        }

        // TODO(qwang): unpack nil.
        MessageUnpacker messageUnpacker = MessagePack.newDefaultUnpacker(encodedBytes);
//        // Unpack unnecessary fields.
//        messageUnpacker.unpackArrayHeader();
//        messageUnpacker.unpackInt();
        final int errorCode =  messageUnpacker.unpackInt();
        if (errorCode != 0) {
            throw new RuntimeException("The signature is not matched.");
        }

        if (Integer.class.equals(returnClz) || int.class.equals(returnClz)) {
            return messageUnpacker.unpackInt();
        } else if (Long.class.equals(returnClz) || long.class.equals(returnClz)) {
            return messageUnpacker.unpackLong();
        } else if (String.class.equals(returnClz)) {
            return messageUnpacker.unpackString();
        } else {
            // Handle as user-defined class.
            ObjectMapper objectMapper = new ObjectMapper(new MessagePackFactory());
            objectMapper.setAnnotationIntrospector(new JsonArrayFormat());
            return objectMapper.readValue(encodedBytes, returnClz);
        }
    }
}
