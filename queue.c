#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;
    element_t *n, *s;
    list_for_each_entry_safe (n, s, l, list)
        q_release_element(n);
    free(l);
}
/*
 * New an element for s,
 * It will allocate memory for s
 * Return null if allocation failed.
 */
element_t *part_ins(char *s)
{
    element_t *new_ele = malloc(sizeof(element_t));
    if (new_ele == NULL)
        return NULL;
    new_ele->value = strdup(s);
    if (new_ele->value == NULL) {
        free(new_ele);
        return NULL;
    }
    return new_ele;
}

bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *new_ele = part_ins(s);
    if (new_ele == NULL)
        return false;
    list_add(&new_ele->list, head);
    return true;
}

bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *new_ele = part_ins(s);
    if (new_ele == NULL)
        return false;
    list_add_tail(&new_ele->list, head);
    return true;
}


/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head) != 0)
        return NULL;
    // else if (list_empty(head) == 0)
    // puts("queue not empty");
    element_t *n = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp != NULL) {
        strncpy(sp, n->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return n;
}

// 修改一次兩處： (1)head 要做檢查。 (2)可以使用 q_remove_head
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !head->prev)
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

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)

{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }
    struct list_head *h = head->next;
    struct list_head *t = head->prev;
    while (true) {
        if (h == t) {
            list_del(h);
            q_release_element(list_entry(h, element_t, list));
            break;
        } else if (h->next == t) {
            list_del(t);
            q_release_element(list_entry(t, element_t, list));
            break;
        }
        h = h->next;
        t = t->prev;
    }
    return true;
}

/* Function of view list content */
void q_view(struct list_head *head)
{
    element_t *now;
    now = list_entry(head->next, element_t, list); /* Cppcheck init error */
    list_for_each_entry (now, head, list) {
        printf("%s ", now->value);
        if (now->list.next == head)
            printf("&&&!\n");
    }
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    element_t *cur, *safe;
    struct list_head *cut = head;
    LIST_HEAD(roll);
    list_for_each_entry_safe (cur, safe, head, list) {
        if (&safe->list != head && !strcmp(cur->value, safe->value))
            continue;

        if (cur->list.prev != cut) {
            LIST_HEAD(tmp);
            list_cut_position(&tmp, cut, &cur->list);
            list_splice(&tmp, &roll);
        }
        cut = safe->list.prev;
    }

    list_for_each_entry_safe (cur, safe, &roll, list)
        q_release_element(cur);

    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    /* https://leetcode.com/problems/swap-nodes-in-pairs/ */
    q_reverseK(head, 2);
}


/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *it, *safe;
    /* Iterate the list and move each item to the head */
    list_for_each_safe (it, safe, head)
        list_move(it, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    /* https://leetcode.com/problems/reverse-nodes-in-k-group/ */
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *tmp, *cut = head;
    int cnt_len = q_size(head), count = k;
    list_for_each_safe (node, safe, head) {
        if (count) {
            tmp = node->next;
            list_move(node, cut);
            /* count on moves */
            count--;
            cnt_len--;
            if (count == 0) {
                count = k;
                cut = tmp->prev;
                if (cnt_len < k)
                    break;
            }
        }
    }
}

static void merge_two(struct list_head *l1, struct list_head *l2, bool descend);


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    /* Try to use merge sort*/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find middle point */
    struct list_head *mid, *left, *right;
    left = right = head;
    do {
        left = left->next;
        right = right->prev;
    } while (left != right && left->next != right);
    mid = left;

    /* Divide into two part */
    LIST_HEAD(second);
    list_cut_position(&second, mid, head->prev);

    /* Conquer */
    q_sort(head, descend);
    q_sort(&second, descend);

    /* Merge */
    merge_two(head, &second, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

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


int q_descend(struct list_head *head)
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


/*merge two sorted lists, as one sorted list*/
static void merge_two(struct list_head *l1, struct list_head *l2, bool descend)
{
    if (!l1 || !l2)
        return;
    bool state;

    LIST_HEAD(newhd);
    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *nod1, *nod2, *ext; /* extreme value */
        nod1 = list_first_entry(l1, element_t, list);
        nod2 = list_first_entry(l2, element_t, list);
        if (!descend)
            state = strcmp(nod1->value, nod2->value) < 0;
        else
            state = strcmp(nod1->value, nod2->value) > 0;
        ext = state ? nod1 : nod2;
        /* add node to tail of newhd list */
        list_move_tail(&ext->list, &newhd);
    }
    if (list_empty(l2))
        list_splice_tail_init(l1, &newhd);
    else
        list_splice_tail_init(l2, &newhd);
    list_splice(&newhd, l1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *fir_qc = list_first_entry(head, queue_contex_t, chain);
    int cnt_len = q_size(fir_qc->q);
    if (list_is_singular(head))
        return cnt_len = 0;
    queue_contex_t *sec_qc =
        list_entry(fir_qc->chain.next, queue_contex_t, chain);
    queue_contex_t *end = NULL;
    while (sec_qc != end) {
        cnt_len += q_size(sec_qc->q);
        merge_two(fir_qc->q, sec_qc->q, descend);
        if (!end)
            end = sec_qc;
        list_move_tail(&sec_qc->chain, head);
        sec_qc = list_entry(fir_qc->chain.next, queue_contex_t, chain);
    }
    return cnt_len;
}
