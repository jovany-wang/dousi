package org.dousi.client;

import org.dousi.common.schema.SignatureUtils;
import org.testng.Assert;
import org.testng.annotations.Test;

@Test
public class CodecTest {

    public void testComputeEncodedValueOfParamTypeList1() {
        long encodedCode = SignatureUtils.computeEncodedValueOfParamTypeList(new Object[] {1, 2.0, "abc"});
        final long expectedEncodedCode = 0x0000000000030401;
        Assert.assertEquals(encodedCode, expectedEncodedCode);
    }

    public void testComputeEncodedValueOfParamTypeList2() {
        long encodedCode = SignatureUtils.computeEncodedValueOfParamTypeList(new Object[] {1, 1});
        final long expectedEncodedCode = 0x0000000000000101;
        Assert.assertEquals(encodedCode, expectedEncodedCode);
    }
}
