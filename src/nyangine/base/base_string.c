#include "nyangine/nyangine.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API DECLARATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// clang-format off
NYA_INTERNAL NYA_CString _nya_strstrn(NYA_ConstCString haystack, NYA_ConstCString needle, u64 haystack_len, u64 needle_len);
// clang-format on

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PUBLIC API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

b8 nya_string_contains(const NYA_String* str, NYA_ConstCString substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  return _nya_strstrn((NYA_CString)str->items, substr, str->length, strlen(substr)) != nullptr;
}

b8 nya_string_contains(const NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  return _nya_strstrn((NYA_CString)str->items, (NYA_CString)substr->items, str->length, substr->length) != nullptr;
}

b8 nya_string_ends_with(const NYA_String* str, NYA_ConstCString suffix) {
  nya_assert(str);
  nya_assert(suffix);

  u64 str_length    = str->length;
  u64 suffix_length = strlen(suffix);

  if (str_length < suffix_length) return false;

  return nya_memcmp(str->items + str_length - suffix_length, suffix, suffix_length) == 0;
}

b8 nya_string_equals(NYA_ConstCString str1, NYA_ConstCString str2) __attr_overloaded {
  nya_assert(str1);
  nya_assert(str2);

  u64 str1_length = strlen(str1);
  u64 str2_length = strlen(str2);
  if (str1_length != str2_length) return false;

  return nya_memcmp(str1, str2, str1_length) == 0;
}

b8 nya_string_equals(const NYA_String* str1, NYA_ConstCString str2) __attr_overloaded {
  nya_assert(str1);
  nya_assert(str2);

  u64 str1_length = str1->length;
  u64 str2_length = strlen(str2);
  if (str1_length != str2_length) return false;

  return nya_memcmp(str1->items, str2, str1_length) == 0;
}

b8 nya_string_equals(const NYA_String* str1, const NYA_String* str2) __attr_overloaded {
  nya_assert(str1);
  nya_assert(str2);

  if (str1->length != str2->length) return false;

  return nya_memcmp(str1->items, str2->items, str1->length) == 0;
}

b8 nya_string_is_empty(const NYA_String* str) {
  nya_assert(str);

  return str->length == 0;
}

b8 nya_string_starts_with(const NYA_String* str, NYA_ConstCString prefix) {
  nya_assert(str);
  nya_assert(prefix);

  u64 str_length    = str->length;
  u64 prefix_length = strlen(prefix);

  if (str_length < prefix_length) return false;

  return nya_memcmp(str->items, prefix, prefix_length) == 0;
}

NYA_String nya_string_clone(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  NYA_String result = nya_string_new_with_capacity(arena, str->length);
  nya_memcpy(result.items, str->items, str->length);
  result.length = str->length;

  return result;
}

NYA_String nya_string_concat(NYA_Arena* arena, const NYA_String* str1, const NYA_String* str2) {
  nya_assert(arena);
  nya_assert(str1);
  nya_assert(str2);

  NYA_String result = nya_string_new_with_capacity(arena, str1->length + str2->length);
  nya_memcpy(result.items, str1->items, str1->length);
  nya_memcpy(result.items + str1->length, str2->items, str2->length);
  result.length = str1->length + str2->length;

  return result;
}

NYA_String nya_string_from(NYA_Arena* arena, NYA_ConstCString cstr) __attr_overloaded {
  nya_assert(arena);
  nya_assert(cstr);

  u64        length = strlen(cstr);
  NYA_String result = nya_string_new_with_capacity(arena, length);
  nya_memcpy(result.items, cstr, length);
  result.length = length;

  return result;
}

NYA_String nya_string_join(NYA_Arena* arena, const NYA_StringArray* arr, NYA_ConstCString separator) __attr_overloaded {
  nya_assert(arena);
  nya_assert(arr);
  nya_assert(separator);

  u64 separator_length = strlen(separator);
  u64 total_length     = 0;

  for (u64 i = 0; i < arr->length; i++) total_length += arr->items[i].length;
  total_length += (arr->length - 1) * separator_length;

  NYA_String result = nya_string_new_with_capacity(arena, total_length);
  for (u64 i = 0; i < arr->length; i++) {
    nya_memmove(result.items + result.length, arr->items[i].items, arr->items[i].length);
    result.length += arr->items[i].length;

    if (i < arr->length - 1) {
      nya_memmove(result.items + result.length, separator, separator_length);
      result.length += separator_length;
    }
  }

  return result;
}

NYA_String nya_string_join(NYA_Arena* arena, const NYA_StringArray* arr, const NYA_String* separator)
    __attr_overloaded {
  nya_assert(arena);
  nya_assert(arr);
  nya_assert(separator);

  NYA_CString separator_cstr = nya_alloca(separator->length + 1);
  nya_memmove(separator_cstr, separator->items, separator->length);
  separator_cstr[separator->length] = '\0';

  return nya_string_join(arena, arr, separator_cstr);
}

NYA_String nya_string_new(NYA_Arena* arena) {
  nya_assert(arena);

  return nya_array_new(arena, u8);
}

NYA_String nya_string_new_with_capacity(NYA_Arena* arena, u64 capacity) {
  nya_assert(arena);

  return nya_array_new_with_capacity(arena, u8, capacity);
}

NYA_String nya_string_sprintf(NYA_Arena* arena, NYA_ConstCString fmt, ...) __attr_fmt_printf(2, 3) {
  nya_assert(arena);
  nya_assert(fmt);

  va_list args;
  va_start(args, fmt);

  u64 length = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  NYA_String result = nya_string_new_with_capacity(arena, length);

  va_start(args, fmt);
  (void)vsnprintf((NYA_CString)result.items, length + 1, fmt, args);
  va_end(args);

  result.length = length;

  return result;
}

NYA_String nya_string_substring_excld(NYA_Arena* arena, const NYA_String* str, u64 start, u64 end) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(start <= end);
  nya_assert(end <= str->length);

  u64        length = end - start;
  NYA_String substr = nya_string_new_with_capacity(arena, length);
  nya_memmove(substr.items, str->items + start, length);
  substr.length = length;

  return substr;
}

NYA_String nya_string_substring_incld(NYA_Arena* arena, const NYA_String* str, u64 start, u64 end) {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(start <= end);
  nya_assert(end < str->length);

  return nya_string_substring_excld(arena, str, start, end + 1);
}

NYA_StringArray nya_string_split(NYA_Arena* arena, const NYA_String* str, NYA_ConstCString separator)
    __attr_overloaded {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(separator);

  NYA_StringArray result     = nya_array_new(arena, NYA_String);
  u64             sep_length = strlen(separator);
  u64             start      = 0;
  u64             end        = 0;

  while (end < str->length) {
    if (nya_memcmp(str->items + end, separator, sep_length) == 0) {
      NYA_String substr = nya_string_substring_excld(arena, str, start, end);
      nya_array_push_back(&result, substr);

      start = end + sep_length;
      end   = start;
    } else {
      end++;
    }
  }

  if (start < str->length) {
    NYA_String substr = nya_string_substring_excld(arena, str, start, end);
    nya_array_push_back(&result, substr);
  }

  return result;
}

NYA_StringArray nya_string_split(NYA_Arena* arena, const NYA_String* str, const NYA_String* separator)
    __attr_overloaded {
  nya_assert(arena);
  nya_assert(str);
  nya_assert(separator);

  NYA_CString separator_cstr = nya_alloca(separator->length + 1);
  nya_memmove(separator_cstr, separator->items, separator->length);
  separator_cstr[separator->length] = '\0';

  return nya_string_split(arena, str, separator_cstr);
}

NYA_StringArray nya_string_split_lines(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  return nya_string_split(arena, str, "\n");
}

NYA_StringArray nya_string_split_words(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(arena);
  nya_assert(str);

  NYA_StringArray arr    = nya_array_new(arena, NYA_String);
  NYA_String      buffer = nya_string_new(arena);

  nya_array_foreach (str, ch) {
    if (isspace(*ch)) {
      if (!nya_string_is_empty(&buffer)) {
        nya_array_push_back(&arr, nya_string_clone(arena, &buffer));
        nya_string_clear(&buffer);
      }
      continue;
    }

    nya_array_push_back(&buffer, *ch);
  }

  if (!nya_string_is_empty(&buffer)) nya_array_push_back(&arr, buffer);

  return arr;
}

u64 nya_string_count(const NYA_String* str, NYA_ConstCString substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  u64 count  = 0;
  u64 length = strlen(substr);

  if (nya_unlikely(length == 0 || length > str->length)) return 0;

  for (u64 i = 0; i < str->length; i++) {
    if (i + length > str->length) break;
    if (nya_memcmp(str->items + i, substr, length) == 0) {
      count++;
      i += length - 1;
    }
  }

  return count;
}

u64 nya_string_count(const NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  NYA_CString substr_cstr = nya_alloca(substr->length + 1);
  nya_memmove(substr_cstr, substr->items, substr->length);
  substr_cstr[substr->length] = '\0';

  return nya_string_count(str, substr_cstr);
}

void nya_string_clear(NYA_String* str) {
  nya_assert(str);

  str->length = 0;
}

void nya_string_extend(NYA_String* str, NYA_ConstCString extension) __attr_overloaded {
  nya_assert(str);
  nya_assert(extension);

  u64 extension_length = strlen(extension);
  u64 new_length       = str->length + extension_length;

  nya_array_reserve(str, new_length);
  nya_memmove(str->items + str->length, extension, extension_length);
  str->length = new_length;
}

void nya_string_extend(NYA_String* str, const NYA_String* extension) __attr_overloaded {
  nya_assert(str);
  nya_assert(extension);

  u64 new_length = str->length + extension->length;

  nya_array_reserve(str, new_length);
  nya_memmove(str->items + str->length, extension->items, extension->length);
  str->length = new_length;
}

void nya_string_destroy(NYA_String* str) {
  nya_array_destroy(str);
}

void nya_string_print(const NYA_String* str) {
  nya_assert(str);

  printf(NYA_FMT_STRING, NYA_FMT_STRING_ARG(*str));
}

void nya_string_println(const NYA_String* str) {
  nya_assert(str);

  printf(NYA_FMT_STRING "\n", NYA_FMT_STRING_ARG(*str));
}

void nya_string_remove(NYA_String* str, NYA_ConstCString substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  u64 length = strlen(substr);

  for (u64 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, substr, length) == 0) {
      nya_memmove(str->items + i, str->items + i + length, str->length - i - length);
      str->length -= length;
      i           -= length;
    }
  }
}

void nya_string_remove(NYA_String* str, const NYA_String* substr) __attr_overloaded {
  nya_assert(str);
  nya_assert(substr);

  for (u64 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, substr->items, substr->length) == 0) {
      nya_memmove(str->items + i, str->items + i + substr->length, str->length - i - substr->length);
      str->length -= substr->length;
      i           -= substr->length;
    }
  }
}

void nya_string_replace(NYA_String* str, NYA_ConstCString old, NYA_ConstCString new) __attr_overloaded {
  nya_assert(str);
  nya_assert(old);
  nya_assert(new);

  u64 old_length = strlen(old);
  u64 new_length = strlen(new);

  for (u64 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, old, old_length) == 0) {
      if (old_length != new_length) nya_array_reserve(str, str->length + new_length - old_length);

      nya_memmove(str->items + i + new_length, str->items + i + old_length, str->length - i - old_length);
      nya_memmove(str->items + i, new, new_length);
      str->length += new_length - old_length;
      i           += new_length - 1;
    }
  }
}

void nya_string_replace(NYA_String* str, const NYA_String* old, const NYA_String* new) __attr_overloaded {
  nya_assert(str);
  nya_assert(old);
  nya_assert(new);

  for (u64 i = 0; i < str->length; i++) {
    if (nya_memcmp(str->items + i, old->items, old->length) == 0) {
      if (old->length != new->length) nya_array_reserve(str, str->length + new->length - old->length);

      nya_memmove(str->items + i + new->length, str->items + i + old->length, str->length - i - old->length);
      nya_memmove(str->items + i, new->items, new->length);
      str->length += new->length - old->length;
      i           += new->length - 1;
    }
  }
}

void nya_string_reserve(NYA_String* str, u64 capacity) {
  nya_assert(str);

  nya_array_reserve(str, capacity);
}

void nya_string_reverse(NYA_String* str) {
  nya_assert(str);

  nya_array_reverse(str);
}

s32 nya_string_sscanf(NYA_String* str, NYA_ConstCString fmt, ...) __attr_fmt_scanf(2, 3) {
  nya_assert(str);
  nya_assert(fmt);

  va_list args;
  va_start(args, fmt);

  s32 ret = vsscanf((NYA_ConstCString)str->items, fmt, args);

  va_end(args);
  return ret;
}

void nya_string_strip_prefix(NYA_String* str, NYA_ConstCString prefix) {
  nya_assert(str);
  nya_assert(prefix);

  u64 prefix_length = strlen(prefix);

  if (nya_memcmp(str->items, prefix, prefix_length) == 0) {
    nya_memmove(str->items, str->items + prefix_length, str->length - prefix_length);
    str->length -= prefix_length;
  }
}

void nya_string_strip_suffix(NYA_String* str, NYA_ConstCString suffix) {
  nya_assert(str);
  nya_assert(suffix);

  u64 suffix_length = strlen(suffix);

  if (nya_memcmp(str->items + str->length - suffix_length, suffix, suffix_length) == 0) {
    str->length -= suffix_length;
  }
}

NYA_CString nya_string_to_cstring(NYA_Arena* arena, const NYA_String* str) {
  nya_assert(str);

  NYA_CString cstr = nya_arena_alloc(arena, str->length + 1);
  nya_memmove(cstr, str->items, str->length);
  cstr[str->length] = '\0';

  return cstr;
}

void nya_string_to_lower(NYA_String* str) {
  nya_assert(str);

  for (u64 i = 0; i < str->length; i++) str->items[i] = tolower(str->items[i]);
}

void nya_string_to_upper(NYA_String* str) {
  nya_assert(str);

  for (u64 i = 0; i < str->length; i++) str->items[i] = toupper(str->items[i]);
}

void nya_string_trim_whitespace(NYA_String* str) {
  nya_assert(str);

  u64 start = 0;
  u64 end   = str->length;

  while (start < str->length && isspace(str->items[start])) start++;
  while (end > start && isspace(str->items[end - 1])) end--;

  nya_memmove(str->items, str->items + start, end - start);
  str->length = end - start;
}

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * PRIVATE API IMPLEMENTATION
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

static NYA_CString _nya_strstrn(NYA_ConstCString haystack, NYA_ConstCString needle, u64 haystack_len, u64 needle_len) {
  if (needle_len == 0 || needle_len > haystack_len) return nullptr;

  for (u64 i = 0; i <= haystack_len - needle_len; i++) {
    if (nya_memcmp(haystack + i, needle, needle_len) == 0) return (NYA_CString)(haystack + i);
  }

  return nullptr;
}
