/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <sys/list.h>
#include <sys/stdkern.h>

static inline void
chain(listnode_t *car, listnode_t *cdr)
{
	if (car)
		car->next = cdr;
	if (cdr)
		cdr->prev = car;
}

static listnode_t *
listnode_alloc(void *ptr)
{
	listnode_t *node = (listnode_t *) malloc(sizeof(listnode_t));
	if (node) {
		node->prev = NULL;
		node->next = NULL;
		node->data = ptr;
	}
	return node;
}

static void *
listnode_free(listnode_t *node)
{
	void *ptr = node->data;
	node->prev = NULL;
	node->next = NULL;
	node->data = NULL;
	free(node);
	return ptr;
}

list_t *
list_alloc()
{
	list_t *list = (list_t *) malloc(sizeof(list_t));
	if (list) {
		list->count = 0;
		list->head = NULL;
		list->tail = NULL;
	}
	return list;
}

unsigned int
list_count(list_t *list)
{
	return list->count;
}

listnode_t *
list_append(list_t *list, void *ptr)
{
	listnode_t *node = listnode_alloc(ptr);
	if (node) {
		chain(list->tail, node);
		list->tail = node;
		if (!list->head)
			list->head = node;
		list->count++;
	}
	return node;
}

listnode_t *
list_prepend(list_t *list, void *ptr)
{
	listnode_t *node = listnode_alloc(ptr);
	if (node) {
		chain(node, list->head);
		list->head = node;
		if (!list->tail)
			list->tail = node;
		list->count++;
	}
	return node;
}

listnode_t *
list_insert(list_t *list, void *ptr, unsigned int before)
{
	listnode_t *cur = list->head, *newnode, *prevnode;

	/*
	 * Shortcircuit head and tail of the list. I dislike having multiple
	 * algorithms in the same function, but it is useful to filter out
	 * all the "prev is null", "next is null", "assign node to head" and
	 * "assign node to tail" stuff.
	 */
	if (!before) /* head */
		return list_prepend(list, ptr);
	if (before >= list->count) /* tail */
		return list_append(list, ptr);

	/* Inserts the new node before cur. */
	while (before--)
		cur = cur->next;
	newnode = listnode_alloc(ptr);
	if (newnode) {
		prevnode = cur->prev;
		chain(prevnode, newnode);
		chain(newnode, cur);
		list->count++;
	}
	return newnode;
}

void *
list_first(list_t *list)
{
	if (list->head)
		return list->head->data;
	else
		return NULL;
}

void *
list_last(list_t *list)
{
	if (list->tail)
		return list->tail->data;
	else
		return NULL;
}

void *
list_at(list_t *list, unsigned int idx)
{
	listnode_t *cur = list->head;
	if (idx >= list->count)
		return NULL;
	while (idx--) {
		cur = cur->next;
	}
	return cur->data;
}

unsigned int
list_index(list_t *list, void *ptr)
{
	listnode_t *cur = list->head;
	unsigned int i = 0;
	while (cur) {
		if (cur->data == ptr)
			return i;
		cur = cur->next;
		i++;
	}
	return (unsigned int) -1;
}

void
list_empty(list_t *list)
{
	listnode_t *cur = list->head;
	if (list->count) {
		while (cur) {
			listnode_free(cur);
			cur = cur->next;
		}
		list->head = NULL;
		list->tail = NULL;
		list->count = 0;
	}
}

static void *
list_unchain(list_t *list, listnode_t *node)
{
	listnode_t *prev = node->prev, *next = node->next;
	if (prev)
		prev->next = next;
	else
		list->head = node->next;
	if (next)
		next->prev = prev;
	else
		list->tail = node->prev;
	list->count--;
	return listnode_free(node);
}

void *
list_remove(list_t *list, unsigned int at)
{
	listnode_t *cur = list->head;
	if (!list->head || at >= list->count)
		return NULL;
	while (at--) {
		cur = cur->next;
		if (!cur) {
			return NULL;
		}
	}
	return list_unchain(list, cur);
}

void *
list_delete(list_t *list, void *ptr)
{
	listnode_t *cur = list->head;
	while (cur) {
		if (cur->data == ptr) {
			return list_unchain(list, cur);
		}
		cur = cur->next;
	}
	return NULL;
}

void *
list_pop_head(list_t *list)
{
	if (list->head)
		return list_unchain(list, list->head);
	else
		return NULL;
}

void *
list_pop_tail(list_t *list)
{
	if (list->tail)
		return list_unchain(list, list->tail);
	else
		return NULL;
}

void
list_free(list_t *list)
{
	list_empty(list);
	free(list);
}
