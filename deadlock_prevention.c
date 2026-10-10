#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t resourceA;
pthread_mutex_t resourceB;

void *worker(void *arg)
{
    int id = *(int *)arg;

    printf("Thread %d: Trying to acquire Resource A\n", id);
    pthread_mutex_lock(&resourceA);
    printf("Thread %d: Acquired Resource A\n", id);

    sleep(1);

    printf("Thread %d: Trying to acquire Resource B\n", id);
    pthread_mutex_lock(&resourceB);

    printf("Thread %d: Acquired Resource B\n", id);
    printf("Thread %d: Using both resources\n", id);

    pthread_mutex_unlock(&resourceB);
    pthread_mutex_unlock(&resourceA);

    printf("Thread %d: Released both resources\n", id);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    setbuf(stdout, NULL);

    if (pthread_mutex_init(&resourceA, NULL) != 0)
    {
        fprintf(stderr, "Failed to initialize Resource A\n");
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&resourceB, NULL) != 0)
    {
        fprintf(stderr, "Failed to initialize Resource B\n");
        pthread_mutex_destroy(&resourceA);
        return EXIT_FAILURE;
    }

    if (pthread_create(&t1, NULL, worker, &id1) != 0)
    {
        fprintf(stderr, "Failed to create Thread 1\n");
        return EXIT_FAILURE;
    }

    if (pthread_create(&t2, NULL, worker, &id2) != 0)
    {
        fprintf(stderr, "Failed to create Thread 2\n");
        return EXIT_FAILURE;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&resourceA);
    pthread_mutex_destroy(&resourceB);

    printf("Program completed without deadlock.\n");
    return EXIT_SUCCESS;
}
