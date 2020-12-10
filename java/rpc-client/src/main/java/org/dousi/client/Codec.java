package org.dousi.client;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.msgpack.core.MessageBufferPacker;
import org.msgpack.core.MessagePack;
import org.msgpack.core.MessageUnpacker;
import org.msgpack.jackson.dataformat.JsonArrayFormat;
import org.msgpack.jackson.dataformat.MessagePackFactory;

import java.io.IOException;

public class Codec {

    private final static String INT_TYPE_NAME = "java.lang.Integer";

    private final static String LONG_TYPE_NAME = "java.lang.Long";

    private final static String DOUBLE_TYPE_NAME = "java.lang.Double";

    private final static String STRING_TYPE_NAME = "java.lang.String";

    public byte[] encodeFunc(String serviceName, String funcName, Object[] args) throws IOException {
        // assert args != nullptr.
        if (args.length > 8) {
            throw new RuntimeException("We don't support more than 8 arguments now.");
        }

        MessageBufferPacker messagePacker = MessagePack.newDefaultBufferPacker();
        // [serviceName, funcName, paramTypeList(A 64-bits Long), args...]
        messagePacker.packArrayHeader(3 + args.length);
        messagePacker.packString(serviceName);
        messagePacker.packString(funcName);

        final long encodedParamTypes = computeEncodedValueOfParamTypeList(args);
        messagePacker.packLong(encodedParamTypes);

        for (Object arg : args) {
            if (arg == null) {
                messagePacker.packNil();
                continue;
            }

            final String argTypeName = arg.getClass().getName();
            switch (argTypeName) {
                case INT_TYPE_NAME:
                    messagePacker.packInt((int) arg);
                    break;
                case LONG_TYPE_NAME:
                    messagePacker.packLong((long) arg);
                    break;
                case STRING_TYPE_NAME:
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

        if (Integer.class.equals(returnClz)) {
            return messageUnpacker.unpackInt();
        } else if (Long.class.equals(returnClz)) {
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

    /**
     * The method that compute a encoded code for the list of parameters. The encoded code
     * is a 64-bits value, and the lowest 8 bits indicates the first parameter type. For
     * example, if the signature of the method is:
     *      void f(int, long, long);
     * the encoded code should be a 64-bits value: 0x00000221.
     */
     static long computeEncodedValueOfParamTypeList(Object[] args) {
        // check args.length > 8
        long result = 0x0000000000000000;
        for (int i = args.length - 1; i >= 0; --i) {
            switch (args[i].getClass().getName()) {
                case INT_TYPE_NAME:
                    result |= 0x0000000000000001;
                    break;
                case LONG_TYPE_NAME:
                    result |= 0x0000000000000002;
                    break;
                case STRING_TYPE_NAME:
                    result |= 0x0000000000000003;
                    break;
                case DOUBLE_TYPE_NAME:
                    result |= 0x0000000000000004;
                    break;
                default:
                    throw new RuntimeException("We now don't support the type: " + args[i].getClass());
            }
            if (i > 0) {
                result <<= 8;
            }
        }
        return result;
    }
}
