package org.dousi.common.schema;

import org.dousi.common.DousiConstants;

public class SignatureUtils {

    /**
     * The method that compute a encoded code for the list of parameters. The encoded code
     * is a 64-bits value, and the lowest 8 bits indicates the first parameter type. For
     * example, if the signature of the method is:
     *      void f(int, long, long);
     * the encoded code should be a 64-bits value: 0x0000000000020201.
     */
    public static long computeEncodedValueOfParamTypeList(Object[] args) {
        // check args.length > 8
        long result = 0x0000000000000000;
        for (int i = args.length - 1; i >= 0; --i) {
            switch (((Class<?>) args[i]).getName()) {
                case DousiConstants.PRIMITIVE_INT_TYPE_NAME:
                case DousiConstants.INT_TYPE_NAME:
                    result |= 0x0000000000000001;
                    break;
                case DousiConstants.LONG_TYPE_NAME:
                case DousiConstants.PRIMITIVE_LONG_TYPE_NAME:
                    result |= 0x0000000000000002;
                    break;
                case DousiConstants.STRING_TYPE_NAME:
                    result |= 0x0000000000000003;
                    break;
                case DousiConstants.PRIMITIVE_DOUBLE_TYPE_NAME:
                case DousiConstants.DOUBLE_TYPE_NAME:
                    result |= 0x0000000000000004;
                    break;
                default:
                    throw new RuntimeException("We now don't support the type: " + args[i]);
            }
            if (i > 0) {
                result <<= 8;
            }
        }
        return result;
    }
}
