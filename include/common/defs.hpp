#pragma once
#include <stdint.h>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#if UINTPTR_MAX == 0xFFFFFFFF
    using usize = uint32_t;
    using isize = int32_t;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
    using usize = uint64_t;
    using isize = int64_t;
#endif
