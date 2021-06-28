package org.dousi.client;

import org.dousi.common.schema.SignatureUtils;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.stream.Collectors;

@Test
public class CodecTest {

    public void testComputeEncodedValueOfParamTypeList1() {
        final Object[] typeList = Arrays.stream(new Object[] {1, 2.0, "abc"}).map(Object::getClass).collect(Collectors.toList()).toArray();
        long encodedCode = SignatureUtils.computeEncodedValueOfParamTypeList(typeList);
        final long expectedEncodedCode = 0x0000000000030401;
        Assert.assertEquals(encodedCode, expectedEncodedCode);
    }

    public void testComputeEncodedValueOfParamTypeList2() {
        final Object[] typeList = Arrays.stream(new Object[] {1, 1}).map(Object::getClass).collect(Collectors.toList()).toArray();
        long encodedCode = SignatureUtils.computeEncodedValueOfParamTypeList(typeList);
        final long expectedEncodedCode = 0x0000000000000101;
        Assert.assertEquals(encodedCode, expectedEncodedCode);
    }
}
