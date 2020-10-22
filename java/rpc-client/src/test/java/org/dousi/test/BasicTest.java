package org.dousi.test;

import org.dousi.client.DousiRpcClient;
import org.dousi.client.DousiService;
import org.dousi.client.NativeRpcClient;
import org.testng.Assert;
import org.testng.annotations.Test;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

public class BasicTest {

    @Test
    public void testBasicCall() throws InterruptedException, ExecutionException {
        DousiRpcClient rpcClient = new NativeRpcClient("127.0.0.1:10001");
        DousiService service = rpcClient.getService("Adder");
        CompletableFuture<Object> future = service.asyncFunction("add").invoke(Integer.class, 2, 3);
        Assert.assertEquals(future.get(), 5);
    }
}
