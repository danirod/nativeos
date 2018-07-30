/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2018 The NativeOS contributors
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

/**
 * \file module.h
 * \brief Module definitions
 *
 * Modules are additional code or data objects that are loaded by the NativeOS
 * Kernel Loader and handled by NativeOS as if they were additional kernel code
 * routines. Modules are useful for platform dependent code.
 */
#pragma once

#include <stdint.h>

/** Kernel module. */
struct kext_module {
	/** Initial memory address of the given module. */
	uintptr_t addr;

	/** Size of the given module. */
	uintptr_t size;
};

/**
 * \brief Process a Kernel Extension.
 * \param kext the kernel extension to process.
 * \return zero if not handled, non-zero if handled.
 */
int kext_process (struct kext_module * kext);
