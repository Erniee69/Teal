#pragma once

template <typename T, typename U>
concept SameType = __is_same(T, U);

template <typename T>
concept Copy = requires (T t) {
    { t.copy((T*) nullptr) } -> SameType<bool>;
};

template <typename T>
concept Drop = requires (T t) {
    { t.drop() } -> SameType<void>;
};