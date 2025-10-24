#pragma once

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(cold)
#define __attr_cold __attribute__((cold))
#else
#define __attr_cold
#endif

#if __has_attribute(constructor)
#define __attr_constructor __attribute__((constructor))
#else
#define __attr_constructor
#endif

#if __has_attribute(destructor)
#define __attr_destructor __attribute__((destructor))
#else
#define __attr_destructor
#endif

#if __has_attribute(format)
#define __attr_fmt_printf(fmt_idx, varargs_idx) __attribute__((format(printf, fmt_idx, varargs_idx)))
#define __attr_fmt_scanf(fmt_idx, varargs_idx)  __attribute__((format(scanf, fmt_idx, varargs_idx)))
#else
#define __attr_fmt_printf(fmt_idx, varargs_idx)
#define __attr_fmt_scanf(fmt_idx, varargs_idx)
#endif

#if __has_attribute(hot)
#define __attr_hot __attribute__((hot))
#else
#define __attr_hot
#endif

#if __has_attribute(malloc)
#define __attr_malloc __attribute__((malloc))
#else
#define __attr_malloc
#endif

#if __has_attribute(matrix_type)
#define __attr_matrix(rows, cols) __attribute__((matrix_type(rows, cols)))
#else
#define __attr_matrix(rows, cols)
#endif

#if __has_attribute(unused)
#define __attr_maybe_unused __attribute__((unused))
#else
#define __attr_maybe_unused
#endif

#if __has_attribute(noreturn)
#define __attr_noreturn __attribute__((noreturn))
#else
#define __attr_noreturn
#endif

#if __has_attribute(overloadable)
#define __attr_overloaded __attribute__((overloadable))
#else
#define __attr_overloaded
#endif

#if __has_attribute(unused)
#define __attr_unused __attribute__((unused))
#else
#define __attr_unused
#endif

#if __has_attribute(ext_vector_type)
#define __attr_vector(x) __attribute__((ext_vector_type(x)))
#else
#define __attr_vector(x)
#endif
