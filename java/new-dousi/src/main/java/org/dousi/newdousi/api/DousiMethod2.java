package org.dousi.newdousi.api;

import java.io.Serializable;

/** Functional interface for a remote function that has 1 parameter. */
@FunctionalInterface
public interface DousiMethod2<T0, T1, R> extends Serializable {

    R apply(T0 t0, T1 t1) throws Exception;
}