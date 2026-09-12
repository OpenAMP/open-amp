/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2024, STMicroelectronics
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * @internal
 *
 * @brief Copies a string to a destination buffer with size limitation and returns the length of
 *        the destination string.
 *
 * This function copies up to `s_size - 1` characters from the source string `src`
 * to the destination buffer `dst`, ensuring that the destination buffer is
 * null-terminated. The function returns the length of the `dst` string.
 * If the length of `src` string is greater than or equal to `d_size`, the destination
 * buffer will be truncated.
 *
 * @param dst    Destination buffer where the string will be copied.
 * @param d_size Size of the destination buffer.
 * @param src    Source string to be copied.
 * @param s_size Size of the source buffer.
 * @return       The length of the string contained in the `dst` buffer.
 *
 * @note If the size of the destination buffer is 0, the function does not copy any characters and
 *       the destination buffer is not null-terminated.
 * @note The function ensures that the destination buffer is always null-terminated if `size` is
 *       greater than 0.
 * @note The function ensures that no data is read past the end of the 'src' buffer.
 */
size_t safe_strcpy(char *dst, size_t d_size, const char *src, size_t s_size);

/*
 * Convert an arithmetic operand to uint64_t, rejecting negative values and
 * values which cannot be represented. This also makes the builtin and
 * fallback implementations below apply the same input rules.
 */
#if defined(__has_builtin)
#if __has_builtin(__builtin_add_overflow) && \
	__has_builtin(__builtin_sub_overflow) && \
	__has_builtin(__builtin_mul_overflow)
#define OPENAMP_HAVE_BUILTIN_OVERFLOW 1
#endif
#endif

#if !defined(OPENAMP_HAVE_BUILTIN_OVERFLOW) && defined(__GNUC__) && \
	(__GNUC__ >= 5)
#define OPENAMP_HAVE_BUILTIN_OVERFLOW 1
#endif

#if defined(OPENAMP_HAVE_BUILTIN_OVERFLOW)
#define OPENAMP_TO_U64_SAFE(value, out) (__extension__({ \
	__typeof__(value) __openamp_value = (value); \
	__builtin_add_overflow(__openamp_value, 0, (out)); \
}))
#else
#define OPENAMP_TO_U64_SAFE(value, out) (__extension__({ \
	__typeof__(value) __openamp_value = (value); \
	int __openamp_overflow = 1; \
	if (__openamp_value == 0 || __openamp_value > 0) { \
		*(out) = (uint64_t)__openamp_value; \
		__openamp_overflow = 0; \
	} \
	__openamp_overflow; \
}))
#endif

#if defined(OPENAMP_HAVE_BUILTIN_OVERFLOW)
#define OPENAMP_ADD_U64_SAFE(a, b, out) \
	__builtin_add_overflow((a), (b), (out))
#define OPENAMP_SUB_U64_SAFE(a, b, out) \
	__builtin_sub_overflow((a), (b), (out))
#define OPENAMP_MUL_U64_SAFE(a, b, out) \
	__builtin_mul_overflow((a), (b), (out))
#else
#define OPENAMP_ADD_U64_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	__typeof__(a) __openamp_a = (a); \
	__typeof__(b) __openamp_b = (b); \
	uint64_t __openamp_umax = (uint64_t)(__typeof__(*__openamp_out)) \
		~(__typeof__(*__openamp_out))0; \
	int __openamp_overflow = __openamp_a > __openamp_umax || \
		__openamp_b > __openamp_umax - __openamp_a; \
	if (!__openamp_overflow) \
		*__openamp_out = (__typeof__(*__openamp_out)) \
			(__openamp_a + __openamp_b); \
	__openamp_overflow; \
}))
#define OPENAMP_SUB_U64_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	__typeof__(a) __openamp_a = (a); \
	__typeof__(b) __openamp_b = (b); \
	uint64_t __openamp_umax = (uint64_t)(__typeof__(*__openamp_out)) \
		~(__typeof__(*__openamp_out))0; \
	int __openamp_overflow = __openamp_a > __openamp_umax || \
		__openamp_b > __openamp_umax || __openamp_a < __openamp_b; \
	if (!__openamp_overflow) \
		*__openamp_out = (__typeof__(*__openamp_out)) \
			(__openamp_a - __openamp_b); \
	__openamp_overflow; \
}))
#define OPENAMP_MUL_U64_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	__typeof__(a) __openamp_a = (a); \
	__typeof__(b) __openamp_b = (b); \
	uint64_t __openamp_umax = (uint64_t)(__typeof__(*__openamp_out)) \
		~(__typeof__(*__openamp_out))0; \
	int __openamp_overflow = __openamp_a != 0 && \
		__openamp_b > __openamp_umax / __openamp_a; \
	if (!__openamp_overflow) \
		*__openamp_out = (__typeof__(*__openamp_out)) \
			(__openamp_a * __openamp_b); \
	__openamp_overflow; \
}))
#endif

/**
 * @internal
 *
 * @brief Safely add two non-negative values with overflow detection.
 *
 * @return 0 on success, otherwise non-zero.
 */
#define OPENAMP_ADD_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	uint64_t __openamp_a; \
	uint64_t __openamp_b; \
	int __openamp_overflow = !__openamp_out || \
		OPENAMP_TO_U64_SAFE((a), &__openamp_a) || \
		OPENAMP_TO_U64_SAFE((b), &__openamp_b); \
	if (!__openamp_overflow) \
		__openamp_overflow = OPENAMP_ADD_U64_SAFE( \
			__openamp_a, __openamp_b, __openamp_out); \
	__openamp_overflow; \
}))

/**
 * @internal
 *
 * @brief Safely subtract two non-negative values with underflow detection.
 *
 * @return 0 on success, otherwise non-zero.
 */
#define OPENAMP_SUB_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	uint64_t __openamp_a; \
	uint64_t __openamp_b; \
	int __openamp_overflow = !__openamp_out || \
		OPENAMP_TO_U64_SAFE((a), &__openamp_a) || \
		OPENAMP_TO_U64_SAFE((b), &__openamp_b); \
	if (!__openamp_overflow) \
		__openamp_overflow = OPENAMP_SUB_U64_SAFE( \
			__openamp_a, __openamp_b, __openamp_out); \
	__openamp_overflow; \
}))

/**
 * @internal
 *
 * @brief Safely multiply two non-negative values with overflow detection.
 *
 * @return 0 on success, otherwise non-zero.
 */
#define OPENAMP_MUL_SAFE(a, b, out) (__extension__({ \
	__typeof__(out) __openamp_out = (out); \
	uint64_t __openamp_a; \
	uint64_t __openamp_b; \
	int __openamp_overflow = !__openamp_out || \
		OPENAMP_TO_U64_SAFE((a), &__openamp_a) || \
		OPENAMP_TO_U64_SAFE((b), &__openamp_b); \
	if (!__openamp_overflow) \
		__openamp_overflow = OPENAMP_MUL_U64_SAFE( \
			__openamp_a, __openamp_b, __openamp_out); \
	__openamp_overflow; \
}))

/**
 * @internal
 *
 * @brief Check whether one half-open size range is contained in another.
 *
 * @param base      Start of the containing range.
 * @param base_len  Length of the containing range.
 * @param offset    Start of the range to check.
 * @param len       Length of the range to check.
 * @param contained Non-null pointer to store the containment result.
 *
 * @return 0 on success, otherwise non-zero on arithmetic overflow or an
 *         invalid output pointer.
 */
#define OPENAMP_RANGE_CONTAINS(base, base_len, offset, len, contained) \
	(__extension__({ \
		__typeof__(contained) __openamp_contained = (contained); \
		__typeof__(base) __openamp_base = (base); \
		__typeof__(base_len) __openamp_base_len = (base_len); \
		__typeof__(offset) __openamp_offset = (offset); \
		__typeof__(len) __openamp_len = (len); \
		size_t __openamp_base_end; \
		size_t __openamp_end; \
		int __openamp_range_overflow = !__openamp_contained || \
			OPENAMP_ADD_SAFE(__openamp_base, __openamp_base_len, \
					 &__openamp_base_end) || \
			OPENAMP_ADD_SAFE(__openamp_offset, __openamp_len, \
					 &__openamp_end); \
		if (!__openamp_range_overflow) \
			*__openamp_contained = __openamp_base <= __openamp_offset && \
				__openamp_base_end >= __openamp_end; \
		__openamp_range_overflow; \
	}))
