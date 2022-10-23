/*!
    \file    dlist.h
    \brief   Dlist function for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef _DLIST_H_
#define _DLIST_H_

/**
 * @brief list entry type
 */
typedef struct list_head {
    struct list_head *next, *prev;
} dlist_t;

#define DLIST_HEAD_INIT(name) { &(name), &(name) }

#define DLIST_HEAD(name) \
    struct list_head name = DLIST_HEAD_INIT(name)

#define INIT_DLIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/**
 * list_entry - get the struct for this entry
 * ptr: the &struct list_head pointer.
 * type: the type of the struct this is embedded in.
 * member: the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((uint8_t *)(ptr)-(uint32_t)(&((type *)0)->member)))

/**
 * list_first - get the first list element
 * head: the &struct list_head pointer of the list head
 */
#define list_first(head) \
    (list_empty((head)) ? NULL : (head)->next)

/**
 * list_last - get the last list element
 * head: the &struct list_head pointer of the list head
 */
#define list_last(head, type, member) \
    (list_empty((head)) ? NULL : (head)->prev)

/**
 * list_first_entry - get the first entry of the given list
 * head: the &struct list_head pointer of the list head
 * type: the type of the struct this is embedded in.
 * member: the name of the list_struct within the struct.
 */
#define list_first_entry(head, type, member) \
    (list_empty((head)) ? NULL : \
     list_entry((head)->next, type, member))

/**
 * list_last_entry - get the last entry of the given list
 * head: the &struct list_head pointer of the list head
 * type: the type of the struct this is embedded in.
 * member: the name of the list_struct within the struct.
 */
#define list_last_entry(head, type, member) \
    (list_empty((head)) ? NULL : \
     list_entry((head)->prev, type, member))

 /**
 * list_for_each - iterate over a list
 * pos: the &struct list_head to use as a loop counter.
 * head: the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the &struct list_head to use as a loop cursor.
 * @n:      another &struct list_head to use as temporary storage
 * @head:   the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

 /**
 * list_for_each_entry - iterate over a list
 * pos: the entry pointer to use as a loop counter.
 * head: the head for your list.
 * type: type of the list entry.
 * member: struct list_head member name of the list entry.
 */
#define list_for_each_entry(pos, head, type, member) \
    for (pos = list_entry((head)->next, type, member); \
         &pos->member != (head); \
         pos = list_entry(pos->member.next, type, member))

/**
 * list_for_each_entry_safe - iterate over a list safe against removal of list entry
 * @pos:    the entry pointer to use as a loop cursor.
 * @n:      another entry pointer to use as temporary storage
 * @head:   the head for your list.
 * type:    type of the list entry.
 * member   struct list_head member name of the list entry.
 */
#define list_for_each_entry_safe(pos, n, head, type, member) \
    for (pos = list_entry((head)->next, type, member), \
           n = list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = list_entry(n->member.next, type, member))

 /**
 * @brief This function insert a new entry after the specified head
 * @param new the new entry
 * @param head the specified head
 * @return none
 */
void list_add(struct list_head *new, struct list_head *head);

 /**
 * @brief This function insert a new entry before the specified head
 * @param new the new entry
 * @param head the specified head
 * @return none
 */
void list_add_tail(struct list_head *new, struct list_head *head);

 /**
 * @brief This function deletes entry from list
 * @param entry the entry to be deleted
 * @return none
 */
void list_del(struct list_head *entry);

 /**
 * @brief This function deletes entry from list and reinitialize it
 * @param entry the entry to be deleted
 * @return none
 */
void list_del_init(struct list_head *entry);

 /**
 * @brief This function tests whether a list is empty
 * @param head the list head entry
 * @return the result
 */
int list_empty(struct list_head *head);

 /**
 * @brief This function join two lists
 * @param list the new list to add
 * @param head the place to add it in the first list
 * @return none
 */
void list_splice(struct list_head *list, struct list_head *head);

#endif //#ifndef _DLIST_H_
