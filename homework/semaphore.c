#include <semaphore.h> 
sem_t s; // declare a semaphore s
sem_init(&s, 0, 1); // initialize the semaphore with its address
										// 0 indicates shared process by all the thread
										// 1 is its initial value


										
int sem_wait(sem_t *s) { 
	// decrement the value of semaphore s by one 
    s->value--;
	// wait if value of semaphore s is negative 
	if (s->value < 0) {
		// block the thread
        
	}
} 
	
int sem_post(sem_t *s) { 
	// increment the value of semaphore s by one 
	s->value++;
	// if there are one or more threads waiting, wake one 
	if (s->value <= 0) {
        
	}
}