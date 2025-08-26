#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define NUM_ITEMS 20

static int buffer[BUFFER_SIZE];
static int in_idx = 0;
static int out_idx = 0;
static int count = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    for (int i = 1; i <= NUM_ITEMS; ++i) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&not_full, &mutex);
        }
        buffer[in_idx] = i;
        in_idx = (in_idx + 1) % BUFFER_SIZE;
        count++;
        printf("Produced: %d (count=%d)\n", i, count);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        usleep(20000);
    }
    return NULL;
}

void *consumer(void *arg) {
    int consumed = 0;
    while (consumed < NUM_ITEMS) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            pthread_cond_wait(&not_empty, &mutex);
        }
        int item = buffer[out_idx];
        out_idx = (out_idx + 1) % BUFFER_SIZE;
        count--;
        consumed++;
        printf("  Consumed: %d (count=%d)\n", item, count);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        usleep(30000);
    }
    return NULL;
}

int main(void) {
    pthread_t prod, cons;

    if (pthread_create(&prod, NULL, producer, NULL) != 0) {
        perror("pthread_create prod");
        return 1;
    }
    if (pthread_create(&cons, NULL, consumer, NULL) != 0) {
        perror("pthread_create cons");
        return 1;
    }

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("Done.\n");
    return 0;
}
