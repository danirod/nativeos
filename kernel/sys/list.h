/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#pragma once

typedef struct listnode listnode_t;
typedef struct list list_t;

struct listnode {
	void *data;
	listnode_t *prev;
	listnode_t *next;
};

struct list {
	listnode_t *head;
	listnode_t *tail;
	int count;
};

list_t *list_alloc();

unsigned int list_count(list_t *list);

listnode_t *list_append(list_t *list, void *ptr);

listnode_t *list_prepend(list_t *list, void *ptr);

listnode_t *list_insert(list_t *list, void *ptr, unsigned int before);

void *list_first(list_t *list);

void *list_last(list_t *list);

void *list_at(list_t *list, unsigned int idx);

unsigned int list_index(list_t *list, void *ptr);

void list_empty(list_t *list);

void *list_remove(list_t *list, unsigned int at);

void *list_delete(list_t *list, void *ptr);

void *list_pop_head(list_t *list);

void *list_pop_tail(list_t *list);

void list_free(list_t *list);

#define list_foreach(list, var) for (var = list->head; var; var = var->next)
