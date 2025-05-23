///
/// \file
/// \copyright Copyright (C) 2020-2022 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
///
/// References to http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/n4808.pdf are of the form
/// [parallel] chapter paragraph
///

// IWYU pragma: private

#ifndef SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_DATA_TYPES_HPP
#define SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_DATA_TYPES_HPP

#include <score/assert.hpp>
#include <score/bit.hpp>
#include <score/type_traits.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

// usable on QNX for ASIL B software. covered by requirement broken_link_c/issue/4049789
#define SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE __attribute__((__always_inline__))

namespace score::cpp
{

template <typename T, typename Abi>
class simd;

template <typename T, typename Abi>
class simd_mask;

namespace simd_abi
{

namespace detail
{
template <std::int32_t N>
struct fixed_size;

template <typename T>
struct compatible;

template <typename T, typename Abi>
struct deduce;
} // namespace detail

/// \brief An ABI tag is a type that indicates a choice of size and binary representation for objects of data-parallel
/// type.
///
/// [parallel] 9.3 ff
/// \{
template <std::int32_t N>
using fixed_size = detail::fixed_size<N>;
template <typename T>
using compatible = detail::compatible<T>;
template <typename T, typename Abi>
using deduce = detail::deduce<T, Abi>;
/// \}

} // namespace simd_abi

/// \brief If `T` is a standard or extended ABI tag returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 1 and 2
/// \{
template <typename T>
struct is_abi_tag : std::false_type
{
};
template <>
struct is_abi_tag<simd_abi::detail::fixed_size<2>> : std::true_type
{
};
template <>
struct is_abi_tag<simd_abi::detail::fixed_size<4>> : std::true_type
{
};
template <>
struct is_abi_tag<simd_abi::detail::compatible<std::int32_t>> : std::true_type
{
};
template <>
struct is_abi_tag<simd_abi::detail::compatible<float>> : std::true_type
{
};
template <>
struct is_abi_tag<simd_abi::detail::compatible<double>> : std::true_type
{
};
template <typename T>
constexpr bool is_abi_tag_v{is_abi_tag<T>::value};
/// \}

/// \brief If `T` is a specialization of the `simd` class template returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 3 and 4
/// \{
template <typename T>
struct is_simd : std::false_type
{
};
template <>
struct is_simd<simd<std::int32_t, simd_abi::detail::fixed_size<4>>> : std::true_type
{
};
template <>
struct is_simd<simd<std::int32_t, simd_abi::detail::compatible<std::int32_t>>> : std::true_type
{
};
template <>
struct is_simd<simd<float, simd_abi::detail::fixed_size<4>>> : std::true_type
{
};
template <>
struct is_simd<simd<float, simd_abi::detail::compatible<float>>> : std::true_type
{
};
template <>
struct is_simd<simd<double, simd_abi::detail::fixed_size<2>>> : std::true_type
{
};
template <>
struct is_simd<simd<double, simd_abi::detail::compatible<double>>> : std::true_type
{
};
template <typename T>
constexpr bool is_simd_v{is_simd<T>::value};
/// \}

/// \brief If `T` is a specialization of the `simd_mask` class template returns `true_type`, and `false_type` otherwise.
///
/// [parallel] 9.4 5 and 6
/// \{
template <typename T>
struct is_simd_mask : std::false_type
{
};
template <>
struct is_simd_mask<simd_mask<std::int32_t, simd_abi::detail::fixed_size<4>>> : std::true_type
{
};
template <>
struct is_simd_mask<simd_mask<std::int32_t, simd_abi::detail::compatible<std::int32_t>>> : std::true_type
{
};
template <>
struct is_simd_mask<simd_mask<float, simd_abi::detail::fixed_size<4>>> : std::true_type
{
};
template <>
struct is_simd_mask<simd_mask<float, simd_abi::detail::compatible<float>>> : std::true_type
{
};
template <>
struct is_simd_mask<simd_mask<double, simd_abi::detail::fixed_size<2>>> : std::true_type
{
};
template <>
struct is_simd_mask<simd_mask<double, simd_abi::detail::compatible<double>>> : std::true_type
{
};
template <typename T>
constexpr bool is_simd_mask_v{is_simd_mask<T>::value};
/// \}

struct element_aligned_tag
{
};
struct vector_aligned_tag
{
};
constexpr element_aligned_tag element_aligned{};
constexpr vector_aligned_tag vector_aligned{};

/// \brief If `T` is one of `element_aligned_tag` or `vector_aligned_tag` returns `true_type`, and `false_type`
/// otherwise.
///
/// [parallel] 9.4 7 and 8
/// \{
template <typename T>
struct is_simd_flag_type : std::false_type
{
};
template <>
struct is_simd_flag_type<element_aligned_tag> : std::true_type
{
};
template <>
struct is_simd_flag_type<vector_aligned_tag> : std::true_type
{
};
template <typename T>
constexpr bool is_simd_flag_type_v{is_simd_flag_type<T>::value};
/// \}

/// \brief If `value` is present returns `integral_constant<size_t, N>` with `N` equal to the number of elements
/// in a `simd< T, Abi>` object
///
/// - `T` shall be a vectorizable type
/// - `is_abi_tag<Abi>` is true
///
/// [parallel] 9.4 9, 10 and 11
/// \{
template <typename T, typename Abi>
struct simd_size : std::integral_constant<std::size_t, simd_abi::deduce<T, Abi>::impl::width>
{
};
template <typename T, typename Abi>
constexpr std::size_t simd_size_v{simd_size<T, Abi>::value};
/// \}

/// \brief If `value` is present returns `integral_constant<size_t, N>` with `N` identifing the alignment restrictions
/// on pointers used for (converting) loads and stores for the give type `T` on arrays of type `U`
///
/// - `T` shall be a vectorizable type
///
/// [parallel] 9.4 12, 13 and 14
/// \{
template <typename T, typename U = typename T::value_type>
struct memory_alignment
    : std::integral_constant<std::size_t, alignof(typename simd_abi::deduce<U, typename T::abi_type>::impl::type)>
{
};
template <typename T, typename U = typename T::value_type>
constexpr std::size_t memory_alignment_v{memory_alignment<T, U>::value};
/// \}

/// \brief The class template `simd_mask` is a data-parallel type with the element type bool.
///
/// `T` must be either an integral type, `std::is_integral_v<T> == true`, or a floating point type,
/// `std::is_floating_point_v<T> == true`.
///
/// A data-parallel type consists of elements of an underlying arithmetic type, called the element type. The number of
/// elements is a constant for each data-parallel type and called the width of that type.
///
/// An element-wise operation applies a specified operation to the elements of one or more data-parallel objects. Each
/// such application is unsequenced with respect to the others. A unary element-wise operation is an element-wise
/// operation that applies a unary operation to each element of a data-parallel object. A binary element-wise operation
/// is an element-wise operation that applies a binary operation to corresponding elements of two data-parallel objects.
///
/// [parallel] 9.8 ff
template <typename T, typename Abi = simd_abi::compatible<T>>
class simd_mask
{
    static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T not a data-parallel type");
    static_assert(is_abi_tag_v<Abi>, "Abi must be an abi tag");
    static_assert(is_simd_mask_v<simd_mask>, "simd_mask<T, Abi> not a valid mask type");

    using impl = typename simd_abi::deduce<T, Abi>::mask_impl;

    template <typename U>
    using is_forwarding_ref_overload = std::is_same<simd_mask, score::cpp::remove_cvref_t<U>>;

    template <typename G, typename std::size_t... Is>
    static constexpr bool is_generator_invocable(std::index_sequence<Is...>)
    {
        return conjunction_v<score::cpp::is_invocable<G, std::integral_constant<std::size_t, Is>>...> &&
               conjunction_v<std::is_same<bool, score::cpp::invoke_result_t<G, std::integral_constant<std::size_t, Is>>>...>;
    }

public:
    using value_type = bool;
    using simd_type = simd<T, Abi>;
    using abi_type = Abi;

    /// \brief The number of elements, i.e., the width, of `score::cpp::simd<T, Abi>`.
    ///
    /// [parallel] 9.8.2 ff
    static constexpr std::size_t size() noexcept { return simd_size_v<T, Abi>; }

    /// \brief Default initialize.
    ///
    /// Performs no initialization of the elements. Thus, leaves the elements in an indeterminate state.
    ///
    /// [parallel] 9.8.1 3
    simd_mask() noexcept = default;

    /// \brief Broadcast argument to all elements.
    ///
    /// [parallel] 9.8.3 1
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd_mask(const value_type v) noexcept : v_{impl::broadcast(v)} {}

    /// \brief Constructs an object where the ith element is initialized to `gen(integral_constant<size_t, i>())`.
    ///
    /// [parallel] none
    template <typename G,
              typename = std::enable_if_t<!is_forwarding_ref_overload<G>::value &&
                                          is_generator_invocable<G>(std::make_index_sequence<simd_size_v<T, Abi>>{})>>
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd_mask(G&& gen) noexcept
        : v_{impl::init(std::forward<G>(gen), std::make_index_sequence<size()>{})}
    {
    }

    /// \brief Convert from platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.8.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd_mask(const typename impl::type v) : v_{v} {}

    /// \brief Convert to platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.8.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator typename impl::type() const { return v_; }

    /// \brief The value of the ith element.
    ///
    /// @pre i < size()
    ///
    /// [parallel] 9.8.3 4, 5 and 6
    value_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator[](const std::size_t i) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i < size());
        return impl::extract(v_, i);
    }

    /// \brief Applies "logical not" to each element.
    ///
    /// [parallel] 9.8.6 1
    simd_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator!() const noexcept { return simd_mask{impl::logical_not(v_)}; }

    /// \brief Applies "logical and" to each element.
    ///
    /// [parallel] 9.9.1 1
    friend simd_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator&&(const simd_mask<T, Abi>& lhs,
                                                       const simd_mask<T, Abi>& rhs) noexcept
    {
        using type = typename impl::type;
        return simd_mask{impl::logical_and(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Applies "logical or" to each element.
    ///
    /// [parallel] 9.9.1 1
    friend simd_mask SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator||(const simd_mask& lhs, const simd_mask& rhs) noexcept
    {
        using type = typename impl::type;
        return simd_mask{impl::logical_or(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

private:
    typename impl::type v_;
};

/// \brief Returns true if all boolean elements in v are true, false otherwise.
///
/// [parallel] 9.9.4 1
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE all_of(const simd_mask<T, Abi>& v) noexcept
{
    using impl = typename simd_abi::deduce<T, Abi>::mask_impl;
    return impl::all_of(static_cast<typename impl::type>(v));
}

/// \brief Returns true if at least one boolean element in v is true, false otherwise.
///
/// [parallel] 9.9.4 2
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE any_of(const simd_mask<T, Abi>& v) noexcept
{
    using impl = typename simd_abi::deduce<T, Abi>::mask_impl;
    return impl::any_of(static_cast<typename impl::type>(v));
}

/// \brief Returns true if none of the boolean elements in v is true, false otherwise.
///
/// [parallel] 9.9.4 3
template <typename T, typename Abi>
inline bool SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE none_of(const simd_mask<T, Abi>& v) noexcept
{
    using impl = typename simd_abi::deduce<T, Abi>::mask_impl;
    return impl::none_of(static_cast<typename impl::type>(v));
}

/// \brief The class template `simd` is a data-parallel type with the element type `T`.
///
/// `T` must be either an integral type, `std::is_integral_v<T> == true`, or a floating point type,
/// `std::is_floating_point_v<T> == true`.
///
/// A data-parallel type consists of elements of an underlying arithmetic type, called the element type. The number of
/// elements is a constant for each data-parallel type and called the width of that type.
///
/// An element-wise operation applies a specified operation to the elements of one or more data-parallel objects. Each
/// such application is unsequenced with respect to the others. A unary element-wise operation is an element-wise
/// operation that applies a unary operation to each element of a data-parallel object. A binary element-wise operation
/// is an element-wise operation that applies a binary operation to corresponding elements of two data-parallel objects.
///
/// [parallel] 9.6 ff
template <typename T, typename Abi = simd_abi::compatible<T>>
class simd
{
    static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "T not a data-parallel type");
    static_assert(is_abi_tag_v<Abi>, "Abi must be an abi tag");
    static_assert(is_simd_v<simd>, "simd<T, Abi> not a valid simd type");

    using impl = typename simd_abi::deduce<T, Abi>::impl;

    template <typename U>
    using is_forwarding_ref_overload = std::is_same<simd, score::cpp::remove_cvref_t<U>>;

    template <typename G, typename std::size_t... Is>
    static constexpr bool is_generator_invocable(std::index_sequence<Is...>)
    {
        return conjunction_v<score::cpp::is_invocable<G, std::integral_constant<std::size_t, Is>>...> &&
               conjunction_v<std::is_same<T, score::cpp::invoke_result_t<G, std::integral_constant<std::size_t, Is>>>...>;
    }

public:
    using value_type = T;
    using mask_type = simd_mask<T, Abi>;
    using abi_type = Abi;

    /// \brief The number of elements, i.e., the width, of score::cpp::simd<T, Abi>.
    ///
    /// [parallel] 9.6.2 1
    static constexpr std::size_t size() noexcept { return simd_size_v<T, Abi>; }

    /// \brief Default initialize.
    ///
    /// Performs no initialization of the elements. Thus, leaves the elements in an indeterminate state.
    ///
    /// [parallel] 9.6.1 3
    simd() noexcept = default;

    /// \brief Broadcast argument to all elements.
    ///
    /// \note This constructor does not allow conversion to `value_type` as opposed to standard.
    ///
    /// [parallel] 9.6.4 1 and 2
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd(const value_type v) noexcept : v_{impl::broadcast(v)} {}

    /// \brief Constructs an object where the ith element is initialized to `gen(integral_constant<size_t, i>())`.
    ///
    /// [parallel] 9.6.4 5, 6 and 7
    template <typename G,
              typename = std::enable_if_t<!is_forwarding_ref_overload<G>::value &&
                                          is_generator_invocable<G>(std::make_index_sequence<simd_size_v<T, Abi>>{})>>
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd(G&& gen) noexcept
        : v_{impl::init(std::forward<G>(gen), std::make_index_sequence<size()>{})}
    {
    }

    /// \brief Constructs the elements of the simd object from an aligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to score::cpp::memory_alignment_v<simd>.
    ///
    /// [parallel] 9.6.4 8, 9 and 10
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd(const value_type* v, vector_aligned_tag) : v_{impl::load_aligned(v)} {}

    /// \brief Constructs the elements of the simd object from an unaligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to alignof(value_type).
    ///
    /// [parallel] 9.6.4 8, 9 and 10
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd(const value_type* v, element_aligned_tag) : v_{impl::load(v)} {}

    /// \brief Convert from platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.6.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd(const typename impl::type& v) noexcept : v_{v} {}

    /// \brief Convert to platform specific type, e.g., _m128 for SSE4.2.
    ///
    ///  \note Use this function with caution as it makes the code platform dependent.
    ///
    /// [parallel] 9.6.1 4
    explicit SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator typename impl::type() const { return v_; }

    /// \brief Replaces the elements of the simd object from an aligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to score::cpp::memory_alignment_v<simd>.
    ///
    /// [parallel] 9.6.5 1, 2 and 3
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE copy_from(const value_type* const v, vector_aligned_tag)
    {
        static_assert(is_simd_flag_type_v<vector_aligned_tag>, "vector_aligned_tag not a simd flag type tag");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((score::cpp::bit_cast<std::uintptr_t>(v) % memory_alignment_v<simd>) == 0U);
        v_ = impl::load_aligned(v);
    }

    /// \brief Replaces the elements of the simd object from an unaligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to alignof(value_type).
    ///
    /// [parallel] 9.6.5 1, 2 and 3
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE copy_from(const value_type* const v, element_aligned_tag)
    {
        static_assert(is_simd_flag_type_v<element_aligned_tag>, "element_aligned_tag not a simd flag type tag");
        v_ = impl::load(v);
    }

    /// \brief Replaces the elements of the simd object from an aligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to score::cpp::memory_alignment_v<simd>.
    ///
    /// [parallel] 9.6.5 4, 5 and 6
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE copy_to(value_type* const v, vector_aligned_tag) const
    {
        static_assert(is_simd_flag_type_v<vector_aligned_tag>, "vector_aligned_tag not a simd flag type tag");
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG((score::cpp::bit_cast<std::uintptr_t>(v) % memory_alignment_v<simd>) == 0U);
        impl::store_aligned(v, v_);
    }

    /// \brief Replaces the elements of the simd object from an unaligned memory address.
    ///
    /// @pre [v, v + size()) is a valid range.
    /// @pre v shall point to storage aligned to alignof(value_type).
    ///
    /// [parallel] 9.6.5 4, 5 and 6
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE copy_to(value_type* const v, element_aligned_tag) const
    {
        static_assert(is_simd_flag_type_v<element_aligned_tag>, "element_aligned_tag not a simd flag type tag");
        impl::store(v, v_);
    }

    /// \brief The value of the ith element.
    ///
    /// @pre i < size()
    ///
    /// [parallel] 9.6.6 5 4, 5 and 6
    value_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator[](const std::size_t i) const
    {
        SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(i < size());
        return impl::extract(v_, i);
    }

    /// \brief Same as -1 * *this.
    ///
    /// [parallel] 9.6.7 14
    simd SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-() const noexcept { return simd{impl::negate(v_)}; }

    /// \brief Addition operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend simd SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator+(const simd& lhs, const simd& rhs) noexcept
    {
        simd tmp{lhs};
        return tmp += rhs;
    }

    /// \brief Subtraction operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend simd SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-(const simd& lhs, const simd& rhs) noexcept
    {
        simd tmp{lhs};
        return tmp -= rhs;
    }

    /// \brief Multiplication operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend simd SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*(const simd& lhs, const simd& rhs) noexcept
    {
        simd<T, Abi> tmp{lhs};
        return tmp *= rhs;
    }

    /// \brief Division operator.
    ///
    /// [parallel] 9.7.1 1 and 2
    friend simd SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/(const simd& lhs, const simd& rhs) noexcept
    {
        simd<T, Abi> tmp{lhs};
        return tmp /= rhs;
    }

    /// \brief Addition assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE simd& operator+=(simd& lhs, const simd& rhs) noexcept
    {
        lhs.v_ = impl::add(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Subtraction assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend simd& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-=(simd& lhs, const simd& rhs) noexcept
    {
        lhs.v_ = impl::subtract(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Multiplication assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend simd& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*=(simd& lhs, const simd& rhs) noexcept
    {
        lhs.v_ = impl::multiply(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Division assignment operator.
    ///
    /// [parallel] 9.7.2 1, 2 and 3
    friend simd& SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/=(simd& lhs, const simd& rhs) noexcept
    {
        lhs.v_ = impl::divide(lhs.v_, rhs.v_);
        return lhs;
    }

    /// \brief Returns true if lhs is equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator==(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is not equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator!=(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::not_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is less than rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator<(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::less_than(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is less than or equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator<=(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::less_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is greater than rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator>(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::greater_than(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

    /// \brief Returns true if lhs is greater than or equal to rhs, false otherwise.
    ///
    /// [parallel] 9.7.3 1
    friend mask_type SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator>=(const simd& lhs, const simd& rhs) noexcept
    {
        using type = typename impl::type;
        return mask_type{impl::greater_equal(static_cast<type>(lhs), static_cast<type>(rhs))};
    }

private:
    typename impl::type v_;
};

/// \brief Returns the smaller of a and b. Returns a if one operand is NaN.
///
/// [parallel] 9.7.6 1
template <typename T, typename Abi>
inline simd<T, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE min(const simd<T, Abi>& a, const simd<T, Abi>& b) noexcept
{
    using impl = typename simd_abi::deduce<T, Abi>::impl;
    using type = typename impl::type;
    return simd<T, Abi>{impl::min(static_cast<type>(a), static_cast<type>(b))};
}

/// \brief Returns the greater of a and b. Returns a if one operand is NaN.
///
/// [parallel] 9.7.6 2
template <typename T, typename Abi>
inline simd<T, Abi> SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE max(const simd<T, Abi>& a, const simd<T, Abi>& b) noexcept
{
    using impl = typename simd_abi::deduce<T, Abi>::impl;
    using type = typename impl::type;
    return simd<T, Abi>{impl::max(static_cast<type>(a), static_cast<type>(b))};
}

/// \brief Returns low if v is less than low, high if high is less than v, otherwise v.
///
/// @pre low <= high
///
/// [parallel] 9.7.6 4 and 5
template <typename T, typename Abi>
inline simd<T, Abi>
    SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE clamp(const simd<T, Abi>& v, const simd<T, Abi>& low, const simd<T, Abi>& high)
{
    SCORE_LANGUAGE_FUTURECPP_PRECONDITION_DBG(all_of(low <= high));
    return score::cpp::min(score::cpp::max(v, low), high);
}

/// \brief The class abstracts the notion of selecting elements of a given object of a data-parallel type.
///
/// [parallel] 9.5 ff
// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions) Follows literaly the C++ standard
template <typename M, typename T>
class where_expression
{
    static_assert(is_simd_mask_v<M>, "M not a simd_mask type");
    static_assert(is_simd_v<T>, "T not a simd type");
    static_assert(std::is_same<T, typename M::simd_type>::value, "simd and simd_mask are incompatible");

    using impl = typename simd_abi::deduce<typename T::value_type, typename T::abi_type>::impl;
    using mask_impl = typename simd_abi::deduce<typename T::value_type, typename T::abi_type>::mask_impl;

public:
    /// \brief Use `where()` function to get automatic type deduction for `M` and `T`.
    where_expression(const M& mask, T& value) : m_{mask}, v_{value} {}
    where_expression(const where_expression&) = delete;
    where_expression& operator=(const where_expression&) = delete;

    /// \brief Replace the elements of value with the elements of x for elements where mask is true.
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value + x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator+=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ + std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value - x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator-=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ - std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value * x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator*=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ * std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

    /// \brief Replace the elements of value with the elements of value / x for elements where mask is true.
    ///
    /// [parallel] 9.5 13 and 14
    template <typename U>
    void SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE operator/=(U&& x) && noexcept
    {
        static_assert(std::is_same<const T, const std::remove_reference_t<U>>::value, "x must be of type T");
        v_ = T{impl::blend(static_cast<typename impl::type>(v_),
                           static_cast<typename impl::type>(v_ / std::forward<U>(x)),
                           static_cast<typename mask_impl::type>(m_))};
    }

private:
    const M m_;
    T& v_;
};

/// \brief Select elements of v where the corresponding elements of m are true.
///
/// Usage: `where(mask, value) @ other;`.
///
/// Where `@` denotes one of the operators of `where_expression<>`.
///
/// [parallel] 9.9.5 ff
template <typename T, typename Abi>
inline where_expression<simd_mask<T, Abi>, simd<T, Abi>> where(const typename simd<T, Abi>::mask_type& m,
                                                               simd<T, Abi>& v) noexcept
{
    return {m, v};
}

/// \brief Returns a simd object with each element cast to `T::value_type`.
///
/// \pre `T` and `simd<U>` must have the same number of elements
///
/// [parallel] 9.7.5 5, 6, 7ff and 8ff
template <typename T,
          typename U,
          typename Abi,
          typename = std::enable_if_t<is_simd_v<T> && T::size() == simd<U, Abi>::size()>>
inline T SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE static_simd_cast(const simd<U, Abi>& v) noexcept
{
    using impl = typename simd_abi::deduce<U, Abi>::impl;
    using type = typename impl::type;
    return T{impl::template convert<typename T::value_type>(static_cast<type>(v))};
}

} // namespace score::cpp

#undef SCORE_LANGUAGE_FUTURECPP_SIMD_ALWAYS_INLINE

#endif // SCORE_LANGUAGE_FUTURECPP_PRIVATE_SIMD_SIMD_DATA_TYPES_HPP
