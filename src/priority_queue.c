#include <malloc.h>
#include <string.h>
#include "priority_queue.h"
#include "logging.h"

struct priority_queue_t {
    PQElm *heap;
    size_t size;
    size_t capacity;
    PQComparator comparator;
};

static char *copy_elm(PQElm elm) {
    size_t size = (strlen(elm) + 1) * sizeof(char); // including '\0'
    PQElm copy = malloc(size);
    if (copy == NULL) return NULL;
    memcpy(copy, elm, size);
    return copy;
}

static void free_elm(PQElm elm) {
    free(elm);
}

PriorityQueue *pq_create(size_t capacity, PQComparator comparator) {
    if (capacity == 0) capacity = 1;
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        LOG_ERROR("Unable to allocate memory for priority queue\n");
        return NULL;
    }
    if ((pq->heap = malloc(capacity * sizeof(PQElm))) == NULL) {
        LOG_ERROR("Unable to allocate memory for heap array\n");
        free(pq);
        return NULL;
    }
    pq->size = 0;
    pq->capacity = capacity;
    pq->comparator = comparator;
    return pq;
}

void pq_free(PriorityQueue *pq) {
    if (pq == NULL) return;
    for (size_t i = 0; i < pq->size; i++)
        free_elm(pq->heap[i]);
    free(pq->heap);
    free(pq);
}

size_t pq_size(PriorityQueue *pq) {
    return pq->size;
}

int pq_offer(PriorityQueue *pq, char *elm) {
    if (pq->size == pq->capacity) {
        LOG_ERROR("Priority queue capacity exceeded\n");
        return -1;
    }
    if ((elm = copy_elm(elm)) == NULL) {
        LOG_ERROR("Unable to allocate memory for element copy\n");
        return -1;
    }
    size_t i = pq->size;
    pq->size++;
    while (i > 0) {
        size_t parent = (i - 1) >> 1;
        if (pq->comparator(elm, pq->heap[parent]) >= 0)
            break;
        pq->heap[i] = pq->heap[parent];
        i = parent;
    }
    pq->heap[i] = elm;
    return 0;
}

const char *pq_peek(PriorityQueue *pq) {
    return pq->size == 0 ? NULL : pq->heap[0];
}

char *pq_poll(PriorityQueue *pq) {
    if (pq->size == 0) return NULL;
    pq->size--;
    PQElm result = pq->heap[0];
    PQElm elm = pq->heap[pq->size];
    size_t i = 0;
    size_t half = pq->size >> 1;
    while (i < half) {
        size_t min_child = (i << 1) + 1;
        if (min_child + 1 < pq->size && pq->comparator(pq->heap[min_child + 1], pq->heap[min_child]) < 0)
            min_child++;
        if (pq->comparator(elm, pq->heap[min_child]) <= 0)
            break;
        pq->heap[i] = pq->heap[min_child];
        i = min_child;
    }
    pq->heap[i] = elm;
    return result;
}
