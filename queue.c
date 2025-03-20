#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "list_sort.h"
#include "queue.h"

#ifndef strlcpy
#define strlcpy(dst, src, sz) snprintf((dst), (sz), "%s", (src))
#endif

/* Compare two elements based on their string values. */
int cmp(const struct list_head *a, const struct list_head *b)
{
    const element_t *ela = list_entry(a, element_t, list);
    const element_t *elb = list_entry(b, element_t, list);

    return strcmp(ela->value, elb->value);
}

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
    if (!head || list_empty(head))
        return false;
    struct list_head *access_next = head->next;
    struct list_head *access_prev = head->prev;
    while ((access_next != access_prev) && (access_prev->next != access_next)) {
        access_next = access_next->next;
        access_prev = access_prev->prev;
    }
    list_del(access_prev);
    q_release_element(list_entry(access_prev, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    LIST_HEAD(pending);
    element_t *it, *safe;
    struct list_head *cut = head;

    list_for_each_entry_safe(it, safe, head, list) {
        if (&safe->list != head && !strcmp(safe->value, it->value))
            continue;
        /* Detect duplicated elements */
        if (it->list.prev != cut) {
            LIST_HEAD(tmp);
            list_cut_position(&tmp, cut, &it->list);
            list_splice(&tmp, &pending);
        }
        cut = safe->list.prev;
    }

    /* Process pending list */
    list_for_each_entry_safe(it, safe, &pending, list)
        q_release_element(it);
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
void q_sort(struct list_head *head, bool descend)
{
    list_sort(head, cmp);
    if (descend)
        q_reverse(head);
}

/* Remove every node which has a node with a strictly less value
 * anywhere to the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    /**
     * Traverse from the last entry and remove the element that is
     * smaller or equal to its right. Also count the number of elements.
     */
    int cnt = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) < 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cnt++;
            cur = prev;
        }
    }

    return cnt;
}

/* Remove every node which has a node with a strictly greater value
 * anywhere to the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    /**
     * Traverse from the last entry and remove the element that is
     * smaller or equal to its right. Also count the number of elements.
     */
    int cnt = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) > 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cnt++;
            cur = prev;
        }
    }

    return cnt;
}

/* Merge all the queues into one sorted queue, which is in
 * ascending/descending order */
int __merge(struct list_head *l1, struct list_head *l2)
{
    if (!l1 || !l2)
        return 0;
    LIST_HEAD(tmp_head);
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *ele_1 = list_first_entry(l1, element_t, list);
        element_t *ele_2 = list_first_entry(l2, element_t, list);
        element_t *ele_min =
            strcmp(ele_1->value, ele_2->value) < 0 ? ele_1 : ele_2;
        list_move_tail(&ele_min->list, &tmp_head);
    }
    list_splice_tail_init(l1, &tmp_head);
    list_splice_tail_init(l2, &tmp_head);
    list_splice(&tmp_head, l1);
    return q_size(l1);
}
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    int size = q_size(head);
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;
    for (int i = 0; i < count; i++) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);
        while (!list_empty(first->q) && !list_empty(second->q)) {
            queue_size = __merge(first->q, second->q);
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }
    return queue_size;
}
