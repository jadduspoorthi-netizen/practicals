#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t resourceA;
pthread_mutex_t resourceB;

void *thread1(void *arg)
{
    (void)arg;

    printf("Thread 1: Trying to acquire Resource A\n");
    pthread_mutex_lock(&resourceA);
    printf("Thread 1: Acquired Resource A\n");

    sleep(1);

    printf("Thread 1: Trying to acquire Resource B\n");
    pthread_mutex_lock(&resourceB);

    printf("Thread 1: Acquired Resource B\n");
    printf("Thread 1: Using both resources\n");

    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);

    printf("Thread 1: Released resources\n");
    return NULL;
}

void *thread2(void *arg)
{
    (void)arg;

    printf("Thread 2: Trying to acquire Resource B\n");
    pthread_mutex_lock(&resourceB);
    printf("Thread 2: Acquired Resource B\n");

    sleep(1);

    printf("Thread 2: Trying to acquire Resource A\n");
    pthread_mutex_lock(&resourceA);

    printf("Thread 2: Acquired Resource A\n");
    printf("Thread 2: Using both resources\n");

    pthread_mutex_unlock(&resourceA);
    pthread_mutex_unlock(&resourceB);

    printf("Thread 2: Released resources\n");
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;

    setbuf(stdout, NULL);

    if (pthread_mutex_init(&resourceA, NULL) != 0 ||
        pthread_mutex_init(&resourceB, NULL) != 0)
    {
        fprintf(stderr, "Mutex initialization failed\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&t1, NULL, thread1, NULL) != 0)
    {
        fprintf(stderr, "Failed to create Thread 1\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&t2, NULL, thread2, NULL) != 0)
    {
        fprintf(stderr, "Failed to create Thread 2\n");
        return EXIT_FAILURE;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&resourceA);
    pthread_mutex_destroy(&resourceB);

    printf("Program completed\n");
    return EXIT_SUCCESS;
}
