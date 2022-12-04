CC = gcc
CFLAGS = -fPIC -Wall -g
LDFLAGS = -g

.PHONY: build
build: libscheduler.so

libscheduler.so: scheduler.o pqueue.o llist.o
	$(CC) $(LDFLAGS) -shared -o $@ $^

scheduler.o: so_scheduler.c
	$(CC) $(CFLAGS) -o $@ -c $^

pqueue.o: tcb_pqueue/tcb_pqueue.c
	$(CC) $(CFLAGS) -o $@ -c $<

llist.o: tcb_pqueue/tcb_llist.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	-rm -f scheduler.o pqueue.o llist.o libscheduler.so
