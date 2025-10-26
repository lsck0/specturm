#pragma once

#if !defined(__clang__) || __STDC_VERSION__ <= 201703L
#error "nyangine requires clang and at least C23, add -std=c23 to your clang compiler flags."
#include <stdbool.h>
#define thread_local _Thread_local
#define typeof       __typeof__
#endif

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * DEBUG AND VERSION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(IS_DEBUG) && IS_DEBUG
#define NYA_IS_DEBUG 1
#else
#define NYA_IS_DEBUG 0
#endif

#ifdef VERSION
#define NYA_VERSION VERSION
#else
#define NYA_VERSION "unknown"
#endif

#ifdef GIT_COMMIT
#define NYA_GIT_COMMIT GIT_COMMIT
#else
#define NYA_GIT_COMMIT "unknown"
#endif

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * COMPILER DETECTION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else
#define COMPILER_UNKNOWN 1
#endif

typedef enum {
  NYA_COMPILER_NULL,
  NYA_COMPILER_MSVC,
  NYA_COMPILER_CLANG,
  NYA_COMPILER_GCC,
  NYA_COMPILER_COUNT,
#if COMPILER_MSVC
  NYA_COMPILER_CURRENT = NYA_COMPILER_MSVC,
#elif COMPILER_CLANG
  NYA_COMPILER_CURRENT = NYA_COMPILER_CLANG,
#elif COMPILER_GCC
  NYA_COMPILER_CURRENT = NYA_COMPILER_GCC,
#else
  NYA_COMPILER_CURRENT = NYA_COMPILER_NULL,
#endif
} NYA_Compiler;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PLATFORM DETECTION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(_WIN32) || defined(__CYGWIN__)
#define OS_WINDOWS 1
#elif defined(__linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MAC 1
#else
#define OS_UNKNOWN 1
#endif

typedef enum {
  NYA_OS_NULL,
  NYA_OS_WINDOWS,
  NYA_OS_LINUX,
  NYA_OS_MAC,
  NYA_OS_COUNT,
#if OS_WINDOWS
  NYA_OS_CURRENT = NYA_OS_WINDOWS,
#elif OS_LINUX
  NYA_OS_CURRENT = NYA_OS_LINUX,
#elif OS_MAC
  NYA_OS_CURRENT = NYA_OS_MAC,
#else
  NYA_OS_CURRENT = NYA_OS_NULL,
#endif
} NYA_OperatingSystem;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ARCHITECTURE DETECTION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(_M_IX86) || defined(__i386__)
#define ARCH_X86 1
#elif defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define ARCH_X86_64 1
#elif defined(_M_ARM) || defined(__arm__)
#define ARCH_ARM32 1
#elif defined(_M_ARM64) || defined(__aarch64__)
#define ARCH_ARM64 1
#else
#define ARCH_UNKNOWN 1
#endif

typedef enum {
  NYA_ARCH_NULL,
  NYA_ARCH_X86,
  NYA_ARCH_X86_64,
  NYA_ARCH_ARM32,
  NYA_ARCH_ARM64,
  NYA_ARCH_COUNT,
#if ARCH_X86
  NYA_ARCH_CURRENT = NYA_ARCH_X86,
#elif ARCH_X86_64
  NYA_ARCH_CURRENT = NYA_ARCH_X86_64,
#elif ARCH_ARM32
  NYA_ARCH_CURRENT = NYA_ARCH_ARM32,
#elif ARCH_ARM64
  NYA_ARCH_CURRENT = NYA_ARCH_ARM64,
#else
  NYA_ARCH_CURRENT = NYA_ARCH_NULL,
#endif
} NYA_Architecture;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * ADDRESS SANITIZATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#if defined(__has_feature) && __has_feature(address_sanitizer)
#include <sanitizer/asan_interface.h>
#define ASAN_ENABLED                            true
#define ASAN_PADDING                            64 // bytes
#define asan_poison_memory_region(addr, size)   __asan_poison_memory_region(addr, size)
#define asan_unpoison_memory_region(addr, size) __asan_unpoison_memory_region(addr, size)
static_assert(ASAN_PADDING >= 0);
#else
#define ASAN_ENABLED                            false
#define ASAN_PADDING                            0 // bytes
#define asan_poison_memory_region(addr, size)   ((void)(addr), (void)(size))
#define asan_unpoison_memory_region(addr, size) ((void)(addr), (void)(size))
static_assert(ASAN_PADDING >= 0);
#endif // defined(__has_feature) && __has_feature(address_sanitizer)

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * VISIBILITY AND LINKAGE
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#ifdef __cplusplus
#define NYA_EXTERN extern "C"
#else
#define NYA_EXTERN extern
#endif

#if COMPILER_CLANG || COMPILER_GCC
#define NYA_INTERNAL __attribute__((visibility("hidden"))) static
#else
#define NYA_INTERNAL static
#endif

#if OS_WINDOWS
#define NYA_API __declspec(dllexport)
#else
#define NYA_API __attribute__((visibility("default")))
#endif

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * OTHER CODEBASE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define OUT
