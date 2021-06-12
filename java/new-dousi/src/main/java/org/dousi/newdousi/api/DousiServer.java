package org.dousi.newdousi.api;

public interface DousiServer {

    void loop();

    void registerService(Class<?> serviceClass, Object serviceInstance);
}
