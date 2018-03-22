#ifndef STREAM_MAP_H
#define STREAM_MAP_H

#include <iterator>

#include "Utils.h"

template <class Transform>
class map {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using SuperValueType = typename std::iterator_traits<SAccessor>::value_type;

        using difference_type = std::ptrdiff_t;
        using value_type = decltype(std::declval<Transform>()(std::declval<SuperValueType>()));
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor, const Transform & _transform)
                : sAccessor(_sAccessor),
                  transform(_transform) {
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() = default;

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator==(const Accessor & other) {
            return sAccessor == other.sAccessor;
        }

        bool operator!=(const Accessor & other) {
            return sAccessor != other.sAccessor;
        }

        value_type operator*() {
            return transform(*sAccessor);
        }

        Accessor & operator++() {
            ++sAccessor;

            return *this;
        }

    private:
        SAccessor sAccessor;

        const Transform & transform;
    };

    map() = delete;

    explicit map(const Transform & _transform)
            : transform(_transform) {
    }

    explicit map(Transform && _transform)
            : transform(std::move(_transform)) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor<SAccessor>(begin, transform),
                          Accessor<SAccessor>(end, transform) };
    }

private:
    const Transform transform;
};

#endif //STREAM_MAP_H
