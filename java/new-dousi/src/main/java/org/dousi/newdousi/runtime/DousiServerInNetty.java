package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiRuntimeType;
import org.dousi.newdousi.api.DousiServer;

public class DousiServerInNetty implements DousiServer {

    private DousiRuntime runtime = new DousiRuntime(DousiRuntimeType.SERVER_RUNTIME);

    public DousiServerInNetty(String addr) throws InterruptedException {

    }

    @Override
    public void loop() {

    }

    @Override
    public void registerService(Class<?> serviceClass, Object serviceInstance) {

    }
}
