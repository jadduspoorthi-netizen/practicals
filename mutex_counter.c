
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define INCREMENTS 1000000

long long counter = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *increment_counter(void *arg)
{
    (void)arg;

    for (long long i = 0; i < INCREMENTS; i++)
    {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];
    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int result = pthread_create(
            &threads[i], NULL, increment_counter, NULL);

        if (result != 0)
        {
            fprintf(stderr, "pthread_create failed: %d\n", result);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int result = pthread_join(threads[i], NULL);

        if (result != 0)
        {
            fprintf(stderr, "pthread_join failed: %d\n", result);
            return EXIT_FAILURE;
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    double elapsed =
        (double)(end.tv_sec - start.tv_sec) +
        (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Expected counter value: %lld\n",
           (long long)NUM_THREADS * INCREMENTS);
    printf("Actual counter value:   %lld\n", counter);
    printf("Execution time: %.6f seconds\n", elapsed);

    pthread_mutex_destroy(&lock);
    return EXIT_SUCCESS;
}
