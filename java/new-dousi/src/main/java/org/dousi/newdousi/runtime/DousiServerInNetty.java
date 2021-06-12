package org.dousi.newdousi.runtime;

import org.dousi.newdousi.api.DousiServer;

public class DousiServerInNetty implements DousiServer {

    private DousiRuntime runtime = new DousiRuntime();

    public DousiServerInNetty(String addr) {

    }

    @Override
    public void loop() {

    }

    @Override
    public void registerService(Class<?> serviceClass, Object serviceInstance) {

    }
}
