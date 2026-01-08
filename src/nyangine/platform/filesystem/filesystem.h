#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_string.h"

NYA_API NYA_EXTERN b8 nya_filesystem_file_exists(NYA_ConstCString path);
NYA_API NYA_EXTERN b8 nya_filesystem_file_last_modified(NYA_ConstCString path, OUT u64* out_timestamp);
NYA_API NYA_EXTERN b8 nya_filesystem_file_move(NYA_ConstCString source, NYA_ConstCString destination);
NYA_API NYA_EXTERN b8 nya_filesystem_file_copy(NYA_ConstCString source, NYA_ConstCString destination);
NYA_API NYA_EXTERN b8 nya_filesystem_file_delete(NYA_ConstCString path);
