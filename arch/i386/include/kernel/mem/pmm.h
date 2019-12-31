/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2020 The NativeOS contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

typedef unsigned int physaddr_t;

/**
 * \brief Initialize the physical memory manager subsystem.
 */
void pmm_init();

/**
 * \brief Allocate a 4 KB page in the physical memory of the computer.
 * \return The allocated physical memory address, 0 if no free page is found.
 */
physaddr_t pmm_alloc_page();

/**
 * \brief Free a 4 KB page from the physical memory of the computer.
 * \param page the memory address of the page to free up from memory.
 */
void pmm_free_page(physaddr_t page);
