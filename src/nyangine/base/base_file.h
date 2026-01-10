#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_string.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FILE DESCRIPTOR FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN b8 nya_fd_read(s32 fd, OUT NYA_String* out_content);
NYA_API NYA_EXTERN b8 nya_fd_write(s32 fd, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_fd_write(s32 fd, NYA_ConstCString content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_fd_append(s32 fd, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_fd_append(s32 fd, NYA_ConstCString content) __attr_overloaded;

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FILE FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN b8 nya_file_read(const char* path, OUT NYA_String* out_content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_read(const NYA_String* path, OUT NYA_String* out_content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_write(const char* path, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_write(const NYA_String* path, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_write(const char* path, NYA_ConstCString content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_write(const NYA_String* path, NYA_ConstCString content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_append(const char* path, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_append(const NYA_String* path, const NYA_String* content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_append(const char* path, NYA_ConstCString content) __attr_overloaded;
NYA_API NYA_EXTERN b8 nya_file_append(const NYA_String* path, NYA_ConstCString content) __attr_overloaded;
