/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, STMicroelectronics
 *
 */

#include <internal/utilities.h>
#include <metal/io.h>
#include <metal/utilities.h>

size_t safe_strcpy(char *dst, size_t d_size, const char *src, size_t s_size)
{
	size_t size = metal_min(s_size, d_size);
	size_t nleft = size + 1;
	char *d = dst;

	if (!d_size)
		return 0;

	/* Copy as many bytes as will fit. */
	while (--nleft != 0) {
		*dst = *src++;
		if (*dst++ == '\0')
			break;
	}

	/* Fill last characters with '\0' */
	if (size < d_size)
		memset(dst, '\0',  d_size - size + nleft);
	else
		d[d_size - 1] = '\0';

	return size - nleft;
}
