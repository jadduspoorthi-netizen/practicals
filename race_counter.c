
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define INCREMENTS 1000000

long long counter = 0;

void *increment_counter(void *arg)
{
    (void)arg;

    for (long long i = 0; i < INCREMENTS; i++)
    {
        counter++;
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_THREADS];

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

    printf("Expected counter value: %lld\n",
           (long long)NUM_THREADS * INCREMENTS);
    printf("Actual counter value:   %lld\n", counter);

    return EXIT_SUCCESS;
}
