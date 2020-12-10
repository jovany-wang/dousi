package innerlanguage;


import org.dousi.client.DousiRpcClient;
import org.dousi.client.NettyRpcClient;
import org.dousi.common.exception.ParserAddrException;
import org.dousi.server.DousiRpcServer;
import org.testng.Assert;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.concurrent.TimeUnit;

interface AdderService {
    int add(int x, int y);

    int sub(int x, int y);
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

}


public class BasicTest {

    private Thread serverThread;

    @Test
    public void testAdd() throws ParserAddrException, InterruptedException {
        DousiRpcClient client = new NettyRpcClient("127.0.0.1:10001");
        AdderService service = client.newStub(AdderService.class);
        int result = service.add(2, 3);
        Assert.assertEquals(result, 5);
        client.shutdown();
    }

    @Test
    public void testSub() throws ParserAddrException, InterruptedException {
        DousiRpcClient client = new NettyRpcClient("127.0.0.1:10001");
        AdderService service = client.newStub(AdderService.class);
        int result = service.sub(200, 3);
        Assert.assertEquals(197, result);
        client.shutdown();
    }

    @Test
    public void testMultiCalls() throws ParserAddrException, InterruptedException {
        DousiRpcClient client = new NettyRpcClient("127.0.0.1:10001");
        AdderService service = client.newStub(AdderService.class);
        int result1 = service.add(3, 4);
        int result2 = service.add(1, 2);
        Assert.assertEquals(7, result1);
        Assert.assertEquals(3, result2);
        client.shutdown();
    }

    @BeforeClass
    private void startServerThread() throws InterruptedException {
        serverThread = new Thread(() -> {
            try {
                DousiRpcServer server = new DousiRpcServer("127.0.0.1:10001");
                server.registerService(AdderService.class, new AdderServiceImpl());
                server.loop();
            } catch (ParserAddrException | InterruptedException e) {
                throw new RuntimeException(e);
            }
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
