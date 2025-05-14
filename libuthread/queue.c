#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue_node {
    void *data;
    struct queue_node *next;
};

struct queue {
	/* TODO Phase 1 */
	struct queue_node *head;
    struct queue_node *tail;
    int length;
};

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	queue_t q = malloc(sizeof *q);
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->length = 0;
    return q;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (!queue || queue->length != 0)
        return -1;
    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if (!queue || !data)
        return -1;
    struct queue_node *node = malloc(sizeof *node);
    if (!node)
        return -1;
    node->data = data;
    node->next = NULL;
    if (queue->tail)
        queue->tail->next = node;
    else
        queue->head = node;
    queue->tail = node;
    queue->length++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
	if (!queue || !data || !queue->head)
        return -1;
    struct queue_node *node = queue->head;
    *data = node->data;
    queue->head = node->next;
    if (!queue->head)
        queue->tail = NULL;
    free(node);
    queue->length--;
    return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if (!queue || !data)
        return -1;
    struct queue_node *prev = NULL;
    struct queue_node *curr = queue->head;
    while (curr) {
        if (curr->data == data) {
            if (prev)
                prev->next = curr->next;
            else
                queue->head = curr->next;
            if (!curr->next)
                queue->tail = prev;
            free(curr);
            queue->length--;
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
	if (!queue || !func)
        return -1;
    struct queue_node *curr = queue->head;
    while (curr) {
        struct queue_node *next = curr->next;
        func(queue, curr->data);
        curr = next;
    }
    return 0;
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
	if (!queue)
        return -1;
    return queue->length;
}

