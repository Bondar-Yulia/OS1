package com;

import java.util.concurrent.atomic.AtomicIntegerArray;

public class Lamport {
    private int n; //number of threads
    AtomicIntegerArray ticket; 
    AtomicIntegerArray choosing;
    
    
    Lamport(int n) {
        ticket = new AtomicIntegerArray(n);
        choosing =  new AtomicIntegerArray(n);
        for (int i = 0; i < n; i++) {
            ticket.set(i, 0);
            choosing.set(i, 0);
        }
        this.n = n;
    }
    
    public void lock(int tid) {
        choosing.set(tid, 1);
        ticket.set(tid, maxInTicket() + 1);
        choosing.set(tid, 0);
        for (int i = 0; i < n; i++) {
            if (i!= tid) {
                while (choosing.get(i) != 0) {
                }
                while (((ticket.get(i) != 0) && (ticket.get(i) < ticket.get(tid)))
                        || ((ticket.get(i) == ticket.get(tid)) && (i < tid))) {
                }
            }
        }
    }
    
    public void unlock(int tid) {
        ticket.set(tid, 0);
    }
    
    
    private int maxInTicket() {
        int x = 0;
        for (int i = 0; i < ticket.length(); i++) {
            if (ticket.get(i) >= x) {
                x = ticket.get(i);
            }
        }
        return x;
    }
}
