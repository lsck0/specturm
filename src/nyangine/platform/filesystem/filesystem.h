#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_string.h"

NYA_API NYA_EXTERN b8 nya_filesystem_exists(NYA_ConstCString path);
NYA_API NYA_EXTERN b8 nya_filesystem_last_modified(NYA_ConstCString path, OUT u64* out_timestamp);
NYA_API NYA_EXTERN b8 nya_filesystem_move(NYA_ConstCString source, NYA_ConstCString destination);
NYA_API NYA_EXTERN b8 nya_filesystem_copy(NYA_ConstCString source, NYA_ConstCString destination);
NYA_API NYA_EXTERN b8 nya_filesystem_delete(NYA_ConstCString path);
