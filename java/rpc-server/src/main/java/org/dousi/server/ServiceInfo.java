package org.dousi.server;

import java.lang.reflect.Method;
import java.util.concurrent.ConcurrentHashMap;

public class ServiceInfo {
    // We now not support the overloading method.
    // We can use method descriptor to do the overloading.

    private String serviceName;

    private Object serviceObject;

    // Method name to method instance.
    private ConcurrentHashMap<String, Method> allMethods = new ConcurrentHashMap<>();

    ServiceInfo(String serviceName, Object serviceObject) {
        this.serviceName = serviceName;
        this.serviceObject = serviceObject;

        // register methods of this service.
        Method[] declaredMethods = serviceObject.getClass().getDeclaredMethods();
        for (Method m : declaredMethods) {
            allMethods.put(m.getName(), m);
        }

    }

    public Object getServiceObject() {
        return serviceObject;
    }
    public Method getMethod(String methodName) {
        return allMethods.get(methodName);
    }

}
