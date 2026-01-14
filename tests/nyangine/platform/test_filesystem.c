#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

s32 main(void) {
  NYA_ConstCString test_file_path       = "test_file.txt";
  NYA_ConstCString test_file_copy_path  = "test_file_copy.txt";
  NYA_ConstCString test_file_moved_path = "test_file_moved.txt";
  NYA_String       file_content         = nya_string_new(&nya_global_arena);
  NYA_String       copied_file_content  = nya_string_new(&nya_global_arena);
  NYA_String       moved_file_content   = nya_string_new(&nya_global_arena);

  nya_assert(!nya_filesystem_exists(test_file_path));
  nya_assert(!nya_filesystem_exists(test_file_copy_path));
  nya_assert(!nya_filesystem_exists(test_file_moved_path));

  nya_assert(nya_file_write(test_file_path, "Hello, Nyangine!"));
  nya_assert(nya_file_read(test_file_path, &file_content));
  nya_assert(nya_string_equals(&file_content, "Hello, Nyangine!"));
  nya_string_clear(&file_content);

  nya_file_append(test_file_path, " Appended text.");
  nya_assert(nya_file_read(test_file_path, &file_content));
  nya_assert(nya_string_equals(&file_content, "Hello, Nyangine! Appended text."));

  nya_filesystem_copy(test_file_path, "test_file_copy.txt");
  nya_assert(nya_file_read(test_file_copy_path, &copied_file_content));
  nya_assert(nya_string_equals(&copied_file_content, "Hello, Nyangine! Appended text."));

  nya_filesystem_move(test_file_copy_path, test_file_moved_path);
  nya_assert(!nya_filesystem_exists(test_file_copy_path));
  nya_assert(nya_filesystem_exists(test_file_moved_path));
  nya_assert(nya_file_read(test_file_moved_path, &moved_file_content));
  nya_assert(nya_string_equals(&moved_file_content, "Hello, Nyangine! Appended text."));

  nya_assert(!nya_filesystem_exists(test_file_copy_path));
  nya_assert(nya_filesystem_delete(test_file_path));
  nya_assert(nya_filesystem_delete(test_file_moved_path));

  return 0;
}
