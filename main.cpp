#include <iostream>
#include <cstddef>
#include <typeinfo>
/*
using namespace std;

struct Test
{
    wchar_t t;
};

int main(int argc, char const *argv[])
{
    Test * test;
    int nsize = sizeof(*test);
    cout<<"Size of "<<typeid(*test).name()<<" :"<<nsize;
    getwchar();
    return 0;
}
*/

/*

#include <pthread.h>
#include <stdio.h>
#include <mutex>

pthread_mutex_t mutex;
pthread_cond_t underflow;

pthread_cond_t overflow;

const int max_size = 100;
int buffer[max_size];

int loops = 50000;
int length = 0;

void * producer(void *arg) {
    int i;
   
    for(i = 0; i < loops; i++) {
        int * id = (int *)arg;
        pthread_mutex_lock(&mutex);
        while(length == max_size)
        {
            printf("producer is waiting...\n");
            pthread_cond_wait(&overflow, &mutex);
        }
        printf("producer %d : %d : %d\n", *id, length, i);
        buffer[length++] = i;
        pthread_cond_broadcast(&underflow);
        pthread_mutex_unlock(&mutex);
    }
}

void * consumer(void *arg) {
    while(1) {
        int * id = (int *)arg;

        pthread_mutex_lock(&mutex);
        while(length == 0) {
            printf("\tconsumer %d is waiting...\n", *id);
            pthread_cond_wait(&underflow, &mutex);
        }
        int item = buffer[--length];
        printf("\tconsumer %d : %d : %d\n", *id, length, item);
        pthread_cond_broadcast(&overflow);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&overflow, 0);
    pthread_cond_init(&underflow, 0);

    pthread_t pThread[2], cThread[2];
    int threadIds[4] = {1, 2, 3, 4};
    pthread_create(&pThread[0], 0, producer, &threadIds[0]);
    pthread_create(&pThread[1], 0, producer, &threadIds[1]);
    pthread_create(&cThread[0], 0, consumer, &threadIds[2]);
    pthread_create(&cThread[2], 0, consumer, &threadIds[3]);

    while(!getchar())
    {

    }

    pthread_join(pThread[0], NULL);
    pthread_join(pThread[1], NULL);
    pthread_join(cThread[0], NULL);
    pthread_join(cThread[1], NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&overflow);
    pthread_cond_destroy(&underflow);

    getchar();
    return 0;
}
*/

/*

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

void *functionCount1(void *);
void *functionCount2(void *);
int  count = 0;
#define COUNT_DONE  100
#define COUNT_HALT1  3
#define COUNT_HALT2  6

int main()
{
    pthread_t thread1, thread2;

    pthread_create( &thread1, NULL, functionCount1, NULL);
    pthread_create( &thread2, NULL, functionCount2, NULL);
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);

    getchar();
    exit(0);
}

void *functionCount1(void * arg)
{
    for(;;)
    {
        pthread_mutex_lock( &condition_mutex );
        while( count >= COUNT_HALT1 && count <= COUNT_HALT2 )
        {
            printf("Go to Fuction 2\n");
            pthread_cond_wait( &condition_cond, &condition_mutex );
        }
        pthread_mutex_unlock( &condition_mutex );

        pthread_mutex_lock( &count_mutex );
        count++;
        printf("Counter value functionCount1: %d\n",count);
        pthread_mutex_unlock( &count_mutex );

        if(count >= COUNT_DONE) return(NULL);
    }
}

void *functionCount2(void * arg)
{
    for(;;)
    {
        pthread_mutex_lock( &condition_mutex );
        if( count < COUNT_HALT1 || count > COUNT_HALT2 )
        {
            printf("Go to Fuction 1\n");
            pthread_cond_signal( &condition_cond );
        }
        pthread_mutex_unlock( &condition_mutex );

        pthread_mutex_lock( &count_mutex );
        count++;
        printf("Counter value functionCount2: %d\n",count);
        pthread_mutex_unlock( &count_mutex );

        if(count >= COUNT_DONE) return(NULL);
    }
}

*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 20

#define RAND_DIVISOR 100000000
#define TRUE 1

/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full, empty;

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

/* buffer counter */
int counter;

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

void initializeData() {

   /* Create the mutex lock */
   pthread_mutex_init(&mutex, NULL);

   /* Create the full semaphore and initialize to 0 */
   sem_init(&full, 0, 0);

   /* Create the empty semaphore and initialize to BUFFER_SIZE */
   sem_init(&empty, 0, BUFFER_SIZE);

   /* Get the default attributes */
   pthread_attr_init(&attr);

   /* init buffer */
   counter = 0;
}

/* Add an item to the buffer */
int insert_item(buffer_item item) {
   /* When the buffer is not full add the item
      and increment the counter*/
   if(counter < BUFFER_SIZE) {
      buffer[counter] = item;
      counter++;
      return 0;
   }
   else { /* Error the buffer is full */
      return -1;
   }
}

/* Remove an item from the buffer */
int remove_item(buffer_item *item) {
   /* When the buffer is not empty remove the item
      and decrement the counter */
   if(counter > 0) {
      *item = buffer[(counter-1)];
      counter--;
      return 0;
   }
   else { /* Error buffer empty */
      return -1;
   }
}

/* Producer Thread */
void *producer(void *param) {
   buffer_item item;

    int * id = (int *)param;
   while(TRUE) {
      /* sleep for a random period of time */
      int rNum = rand() / RAND_DIVISOR;
      sleep(0);

      /* generate a random number */
      item = rand();

      /* acquire the empty lock */
      sem_wait(&empty);
      /* acquire the mutex lock */
      pthread_mutex_lock(&mutex);

      if(insert_item(item) == -1) {
         printf(" Producer report error condition\n");
      }
      else {
         printf("producer %d produced %d : %d\n", *id, item, counter);
      }
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);
      /* signal full */
      sem_post(&full);
   }
}

/* Consumer Thread */
void *consumer(void *param) {
   buffer_item item;

    int * id = (int *)param;
   while(TRUE) {
      /* sleep for a random period of time */
      int rNum = rand() / RAND_DIVISOR;
      sleep(0);

      /* aquire the full lock */
      sem_wait(&full);
      /* aquire the mutex lock */
      pthread_mutex_lock(&mutex);
      if(remove_item(&item) == -1) {
         printf(" Consumer report error condition\n");
      }
      else {
         printf("\tconsumer %d consumed %d : %d\n", *id, item, counter);
      }
      /* release the mutex lock */
      pthread_mutex_unlock(&mutex);
      /* signal empty */
      sem_post(&empty);
   }
}

int main(int argc, char *argv[]) {
   /* Loop counter */
   int i;

   /* Verify the correct number of arguments were passed in */
   if(argc != 4) {
      fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT>\n");
   }

   int mainSleepTime = atoi(argv[1]); /* Time in seconds for main to sleep */
   int numProd = atoi(argv[2]); /* Number of producer threads */
   int numCons = atoi(argv[3]); /* Number of consumer threads */

   /* Initialize the app */
   initializeData();

    int aIDP[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
   /* Create the producer threads */
   for(i = 0; i < numProd; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,producer,&(aIDP[i]));
    }

    int aIDC[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
   /* Create the consumer threads */
   for(i = 0; i < numCons; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,consumer,&(aIDC[i]));
   }

   /* Sleep for the specified amount of time in milliseconds */
   sleep(mainSleepTime);

   /* Exit the program */
   printf("Exit the program\n");
   getchar();
   exit(0);
}