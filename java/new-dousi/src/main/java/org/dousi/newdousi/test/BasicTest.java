package org.dousi.newdousi.test;

import org.dousi.newdousi.api.DousiClient;
import org.dousi.newdousi.api.DousiServer;
import org.dousi.newdousi.runtime.DousiClientInNetty;
import org.dousi.newdousi.runtime.DousiServerInNetty;
import org.testng.Assert;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

interface AdderService {
    int add(int x, int y);

    int sub(int x, int y);

    int incr(int x);

    int get0();
}

class AdderServiceImpl implements AdderService {

    @Override
    public int add(int x, int y) {
        return x + y;
    }

    @Override
    public int sub(int x, int y) {
        return x - y;
    }

    @Override
    public int incr(int x) {
        return 0;
    }

    @Override
    public int get0() {
        return 0;
    }
}

class ClientService {

}

public class BasicTest {

    private Thread serverThread;

    @Test
    public void testAdd() throws InterruptedException, ExecutionException {
        DousiClient client = new DousiClientInNetty("127.0.0.1:10001");
        CompletableFuture<Integer> result = client.asyncInvoke(AdderService::incr, 2);
        Assert.assertEquals((int) result.get(), 5);
        client.shutdown();
    }

    @BeforeClass
    private void startServerThread() throws InterruptedException {
        serverThread = new Thread(() -> {
            DousiServer server = new DousiServerInNetty("127.0.0.1:10001");
            server.registerService(AdderService.class, new AdderServiceImpl());
            server.loop();
        });
        serverThread.start();
        // Sleep to wait server getting ready.
        TimeUnit.SECONDS.sleep(1);
    }

    @AfterClass
    private void  shutdownServerThread() throws InterruptedException {
        if (serverThread == null) {
            return;
        }
        serverThread.interrupt();
    }
}
