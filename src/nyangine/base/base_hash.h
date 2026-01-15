#pragma once

#include "nyangine/base/base_attributes.h"
#include "nyangine/base/base_string.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * FUNCTIONS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

NYA_API NYA_EXTERN u64 nya_hash_fnv1a(const void* data, u64 size) __attr_overloaded;
NYA_API NYA_EXTERN u64 nya_hash_fnv1a(NYA_ConstCString string) __attr_overloaded;
NYA_API NYA_EXTERN u64 nya_hash_fnv1a(NYA_String string) __attr_overloaded;
