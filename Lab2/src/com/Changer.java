package com;

public class Changer implements Runnable {
    protected Resource resource;
    protected int count;
    static protected Lamport lamport;
    static protected int number = 0;
    protected int id;
    protected int x;
    
    public Changer(Resource res, int count, int x) {
        resource = res;
        id = number++;
        this.count = count;
        this.x = x;
    }
    
    static public void beforeStart() {
        lamport = new Lamport(number);
    }
    
    @Override
    public void run() {
        for (int i = 0; i < count; i++) {
            lamport.lock(id);
            
            int tmp = resource.getI() + x;
            resource.setI(tmp);
            
            lamport.unlock(id);
        }
    }

}
