package org.dousi.examples.adder;

public class AdderServiceImpl implements AdderService {

    @Override
    public int add(int x, int y) {
        return x + y;
    }

    @Override
    public int sub(int x, int y) {
        return x - y;
    }
}
