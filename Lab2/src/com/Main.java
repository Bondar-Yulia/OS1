package com;

import java.util.logging.Level;
import java.util.logging.Logger;

public class Main {
    
    public static void main(String[] args) {
        Resource resource = new Resource();
        Thread thread1 = new Thread(new Changer(resource, 100, 2));
        Thread thread2 = new Thread(new Changer(resource, 100, -1));
        Thread thread3 = new Thread(new Changer(resource, 100, -1));
        
        Changer.beforeStart();
        
        thread1.start();
        thread2.start();
        thread3.start();
        
        try {
            thread1.join();
            thread2.join();
            thread3.join();
        } catch (InterruptedException ex) {
            Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
        }
        
        System.out.println(resource.getI());
    }
    
}
