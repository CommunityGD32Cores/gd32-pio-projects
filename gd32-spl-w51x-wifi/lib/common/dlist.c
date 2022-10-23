/*!
    \file    dlist.c
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

#include "dlist.h"

/*!
    \brief      insert a new entry after the specified head
    \param[in]  new: the new entry
    \param[in]  head: the specified head
    \param[out] none
    \retval     none
*/
void list_add(struct list_head *new, struct list_head *head)
{
    struct list_head *prev = head;
    struct list_head *next = head->next;

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/*!
    \brief      insert a new entry before the specified head
    \param[in]  new: the new entry
    \param[in]  head: the specified head
    \param[out] none
    \retval     none
*/
void list_add_tail(struct list_head *new, struct list_head *head)
{
    struct list_head *prev = head->prev;
    struct list_head *next = head;

    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

/*!
    \brief      delete entry from list
    \param[in]  entry: the entry of a list
    \param[out] none
    \retval     none
*/
void list_del(struct list_head *entry)
{
    struct list_head *prev = entry->prev;
    struct list_head *next = entry->next;

    next->prev = prev;
    prev->next = next;
}

/*!
    \brief      delete entry from list and reinitialize it
    \param[in]  entry: the entry to be deleted
    \param[out] none
    \retval     none
*/
void list_del_init(struct list_head *entry)
{
    list_del(entry);
    INIT_DLIST_HEAD(entry);
}

/*!
    \brief      whether a list is empty
    \param[in]  head: the list head entry
    \param[out] none
    \retval     the result
*/
int list_empty(struct list_head *head)
{
    if(head->next == head)
        return 1;
    else
        return 0;
}

/*!
    \brief      splice two lists
    \param[in]  list: the new list to add
    \param[in]  head: the place to add it in the first list
    \param[out] none
    \retval     none
*/
void list_splice(struct list_head *list, struct list_head *head)
{
    struct list_head *first = list->next;

    if (first != list)
    {
        struct list_head *last = list->prev;
        struct list_head *at = head->next;
        first->prev = head;
        head->next = first;
        last->next = at;
        at->prev = last;
    }
}
