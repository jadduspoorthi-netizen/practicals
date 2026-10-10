#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define ITEMS_PER_PRODUCER 500000
#define BUFFER_SIZE 10
#define STOP_ITEM -1

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t empty, full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

long long produced = 0;
long long consumed = 0;

void wait_sem(sem_t *sem)
{
    while (sem_wait(sem) == -1)
    {
        if (errno != EINTR)
        {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }
    }
}

void *producer(void *arg)
{
    long id = (long)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++)
    {
        int item = (int)(id * ITEMS_PER_PRODUCER + i);

        wait_sem(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        produced++;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    return NULL;
}

void *consumer(void *arg)
{
    (void)arg;

    while (1)
    {
        int item;

        wait_sem(&full);
        pthread_mutex_lock(&mutex);

        item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        if (item != STOP_ITEM)
            consumed++;

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if (item == STOP_ITEM)
            break;
    }

    return NULL;
}

int main(void)
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];
    struct timespec start, end;

    long long total_items =
        (long long)NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    if (sem_init(&empty, 0, BUFFER_SIZE) == -1)
    {
        perror("sem_init empty");
        return EXIT_FAILURE;
    }

    if (sem_init(&full, 0, 0) == -1)
    {
        perror("sem_init full");
        sem_destroy(&empty);
        return EXIT_FAILURE;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        perror("clock_gettime");
        sem_destroy(&empty);
        sem_destroy(&full);
        return EXIT_FAILURE;
    }

    int producers_created = 0;
    int consumers_created = 0;

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        if (pthread_create(&consumers[i], NULL, consumer, NULL) != 0)
        {
            fprintf(stderr, "Failed to create consumer thread\n");
            return EXIT_FAILURE;
        }
        consumers_created++;
    }

    for (long i = 0; i < NUM_PRODUCERS; i++)
    {
        if (pthread_create(&producers[i], NULL, producer, (void *)i) != 0)
        {
            fprintf(stderr, "Failed to create producer thread\n");
            return EXIT_FAILURE;
        }
        producers_created++;
    }

    for (int i = 0; i < producers_created; i++)
        pthread_join(producers[i], NULL);

    for (int i = 0; i < NUM_CONSUMERS; i++)
    {
        wait_sem(&empty);
        pthread_mutex_lock(&mutex);

        buffer[in] = STOP_ITEM;
        in = (in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }

    for (int i = 0; i < consumers_created; i++)
        pthread_join(consumers[i], NULL);

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    double elapsed =
        (double)(end.tv_sec - start.tv_sec) +
        (double)(end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("\nProducer-Consumer Experiment\n");
    printf("============================\n");
    printf("Buffer size      : %d\n", BUFFER_SIZE);
    printf("Producers        : %d\n", NUM_PRODUCERS);
    printf("Consumers        : %d\n", NUM_CONSUMERS);
    printf("Expected items   : %lld\n", total_items);
    printf("Produced items   : %lld\n", produced);
    printf("Consumed items   : %lld\n", consumed);
    printf("Execution time   : %.6f seconds\n", elapsed);

    if (produced == total_items && consumed == total_items)
        printf("Synchronization  : CORRECT\n");
    else
        printf("Synchronization  : ERROR\n");

    printf("Throughput       : %.2f items/second\n",
           elapsed > 0 ? consumed / elapsed : 0.0);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return (produced == total_items && consumed == total_items)
               ? EXIT_SUCCESS
               : EXIT_FAILURE;
}
