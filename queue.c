#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */
/* Create an empty queue  */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    else {
        INIT_LIST_HEAD(q);
        return q;
    }
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *go_node, *go_next;
    /* go_node */
    list_for_each_entry_safe(go_node, go_next, head, list) {
        list_del(&(go_node->list));
        q_release_element(go_node);
    }
    list_del_init(head);
    free(head);
    return;
}

/*
 * New an element for s,
 * It will allocate memory for s
 * Return null if allocation failed.
 * Assert malloc and value to node
 */
element_t *create_new_element(char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    if (!new_ele)
        return NULL;

    new_ele->value = malloc(sizeof(char) * (strlen(s) + 1));

    if (!new_ele->value) {
        free(new_ele);
        return NULL;
    }

    strlcpy(new_ele->value, s, strlen(s) + 1);
    return new_ele;
}
/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_ele = create_new_element(s);

    if (!new_ele)
        return false;
    list_add(&new_ele->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_ele = create_new_element(s);
    if (!new_ele)
        return false;
    list_add_tail(&new_ele->list, head);

    return true;
}



/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    // else if (list_empty(head) == 0)
    // puts("queue not empty");
    element_t *node = list_first_entry(head, element_t, list);
    // remove_element
    list_del_init(head->next);
    if (sp) {
        // cut the word if too long
        size_t len;
        if (strlen(node->value) < bufsize - 1)
            len = strlen(node->value);
        else
            len = bufsize - 1;
        memcpy(sp, node->value, len);
        sp[len] = '\0';
    }
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *safe;
    list_for_each_safe(curr, safe, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k == 1)
        return;
    if (k == q_size(head))
        q_reverse(head);
    struct list_head *curr = head->next, *safe, *dummy = head;
    int count = k;
    int group_count = q_size(head) / k;
    while (curr != dummy) {
        while (count > 0) {
            safe = curr->next;
            list_move(curr, dummy);
            curr = safe;
            count--;
        }
        group_count--;
        if (group_count == 0)
            break;
        dummy = curr->prev;
        curr = curr->next;
        count = k - 1;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value
 * anywhere to the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value
 * anywhere to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
