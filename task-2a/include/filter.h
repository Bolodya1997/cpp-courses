#ifndef STREAM_FILTER_H
#define STREAM_FILTER_H

#include <iterator>
#include <memory>

#include "Utils.h"

template <class Predicate>
class filter {
public:
    using StreamTag = InfiniteStreamTag;

    template <class SAccessor>
    class Accessor {
    public:
        using Type = typename SAccessor::Type;

        Accessor() = delete;

        Accessor(const SAccessor & _sAccessor, const SAccessor & _end,
                 const Predicate & _predicate)
                : sAccessor(_sAccessor),
                  end(_end),
                  predicate(_predicate) {
        }

        Accessor(const Accessor &) = default;
        Accessor(Accessor &&) noexcept = default;

        ~Accessor() {
            if constexpr (!std::is_reference_v<Type>)
                delete valuePtr;
            valuePtr = nullptr;
        };

        Accessor & operator=(const Accessor &) = delete;
        Accessor & operator=(Accessor &&) noexcept = delete;

        bool operator!=(const Accessor & other) const {
            return sAccessor != other.sAccessor;
        }

        bool hasValue() {
            if (!sAccessor.hasValue())
                return false;

            if (valuePtr == nullptr) {
                if constexpr (std::is_reference_v<Type>)
                    valuePtr = &(*sAccessor);
                else if (std::is_move_constructible_v<Type>)
                    valuePtr = new Type(std::move(*sAccessor));
                else
                    valuePtr = new Type(*sAccessor);
            }

            return predicate(*valuePtr);
        }

        Type operator*() {
            if (!hasValue())
                throw std::runtime_error("filter::Accessor has no value");

            if constexpr (std::is_reference_v<Type>)
                return *valuePtr;
            else if (std::is_move_constructible_v<Type>)
                return std::move(*valuePtr);
            else
                return *valuePtr;
        }

        Accessor & operator++() {
            ++sAccessor;

            if constexpr (!std::is_reference_v<Type>)
                delete valuePtr;
            valuePtr = nullptr;

            return *this;
        }

    private:
        SAccessor sAccessor;
        SAccessor end;

        const Predicate & predicate;

        std::remove_reference<Type> * valuePtr = nullptr;
    };

    filter() = delete;

    explicit filter(const Predicate & _predicate)
            : predicate(_predicate) {
    }

    explicit filter(Predicate && _predicate)
            : predicate(std::move(_predicate)) {
    }

    template <class SAccessor>
    auto modify(SAccessor begin, SAccessor end) const {
        return std::pair{ Accessor<SAccessor>(begin, end, predicate),
                          Accessor<SAccessor>(end, end, predicate) };
    }

private:
    const Predicate predicate;
};

#endif //STREAM_FILTER_H
