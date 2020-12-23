#ifndef TASK_7_PRIORITY_QUEUE_H
#define TASK_7_PRIORITY_QUEUE_H

typedef char *PQElm;
typedef const char *PQConstElm;
typedef struct priority_queue_t PriorityQueue;

typedef int (*PQComparator)(PQConstElm, PQConstElm);

PriorityQueue *pq_create(size_t capacity, PQComparator comparator);
void pq_free(PriorityQueue *pq);
size_t pq_size(PriorityQueue *pq);
int pq_offer(PriorityQueue *pq, PQElm elm);
PQConstElm pq_peek(PriorityQueue *pq);
PQElm pq_poll(PriorityQueue *pq);

#endif // TASK_7_PRIORITY_QUEUE_H
