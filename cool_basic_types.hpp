// cool_basic_types.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_BASIC_TYPES_HPP
#define xCOOL_BASIC_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <limits>
#include <type_traits>
#include <new>
#include <cstring>
#include <utility>
#include <tuple>
#include <cassert>


namespace cool
{
	// error type

	enum err_type_t
	{
		static_error,
		runtime_error
	};

	template <err_type_t err_type> class error_type
	{

	public:

		error_type() = delete;
		error_type(const cool::error_type<cool::static_error>&) = delete;
		cool::error_type<cool::static_error>& operator=(const cool::error_type<cool::static_error>&) = delete;
		error_type(cool::error_type<cool::static_error>&&) = delete;
		cool::error_type<cool::static_error>& operator=(cool::error_type<cool::static_error>&&) = delete;
		~error_type() = delete;
	};

	template <> class error_type<cool::runtime_error>
	{

	public:

		inline error_type() { assert(false); }
		inline error_type(const cool::error_type<cool::runtime_error>&) { assert(false); }
		inline cool::error_type<cool::runtime_error>& operator=(const cool::error_type<cool::runtime_error>&) { assert(false); return *this; }
		inline error_type(cool::error_type<cool::runtime_error>&&) { assert(false); }
		inline cool::error_type<cool::runtime_error>& operator=(cool::error_type<cool::runtime_error>&&) { assert(false); return *this; }
		inline ~error_type() { assert(false); }
	};


	// floating point numbers

#ifdef FLT_MAX
	using f32 = float;
#endif // FLT_MAX

#ifdef DBL_MAX
	using f64 = double;
#endif // DBL_MAX


	// integers

	template <class int_Ty> class integer;
	// has the same interface as integral types
	// also has the following members :
	// > using value_type = int_Ty;
	// > static constexpr std::size_t bit_count;
	// > static constexpr bool is_signed;
	// > static constexpr cool::integer<int_Ty> min() noexcept;
	// > static constexpr cool::integer<int_Ty> max() noexcept;
	// > inline constexpr int_Ty get_value() const noexcept;

	template <class integral_Ty> class is_integral : public std::false_type {};
	template <class int_Ty> class is_integral<cool::integer<int_Ty>> : public std::true_type {};

	using ubool = cool::integer<bool>;
	using ibyte = cool::integer<char>;
	using ubyte = cool::integer<unsigned char>;

#ifdef INT8_MAX
	using i8 = cool::integer<std::int8_t>;
#endif // INT8_MAX

#ifdef INT16_MAX
	using i16 = cool::integer<std::int16_t>;
#endif // INT16_MAX

#ifdef INT32_MAX
	using i32 = cool::integer<std::int32_t>;
#endif // INT32_MAX

#ifdef INT64_MAX
	using i64 = cool::integer<std::int64_t>;
#endif // INT64_MAX

#ifdef UINT8_MAX
	using u8 = cool::integer<std::uint8_t>;
#endif // UINT8_MAX

#ifdef UINT16_MAX
	using u16 = cool::integer<std::uint16_t>;
#endif // UINT16_MAX

#ifdef UINT32_MAX
	using u32 = cool::integer<std::uint32_t>;
#endif // UINT32_MAX

#ifdef UINT64_MAX
	using u64 = cool::integer<std::uint64_t>;
#endif // UINT64_MAX

	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator&(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator|(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator^(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;

	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator+(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator-(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator*(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator/(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator%(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;

	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator<<(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<int_Ty> operator>>(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept;

	template <class int_res_Ty, class int_Ty> inline constexpr int_res_Ty integral_cast(cool::integer<int_Ty> rhs) noexcept;

	template <class int_Ty> inline constexpr cool::integer<typename std::make_signed<int_Ty>::type> integral_signed_cast(cool::integer<int_Ty> rhs) noexcept;
	template <class int_Ty> inline constexpr cool::integer<typename std::make_unsigned<int_Ty>::type> integral_unsigned_cast(cool::integer<int_Ty> rhs) noexcept;

#if defined(INT8_MAX) && defined(INT16_MAX)
	inline constexpr cool::i16 integral_upcast(cool::i8 rhs) noexcept;
	inline constexpr cool::i8 integral_downcast(cool::i16 rhs) noexcept;
#endif // if defined(INT8_MAX) && defined(INT16_MAX)

#if defined(INT16_MAX) && defined(INT32_MAX)
	inline constexpr cool::i32 integral_upcast(cool::i16 rhs) noexcept;
	inline constexpr cool::i16 integral_downcast(cool::i32 rhs) noexcept;
#endif // if defined(INT16_MAX) && defined(INT32_MAX)

#if defined(INT32_MAX) && defined(INT64_MAX)
	inline constexpr cool::i64 integral_upcast(cool::i32 rhs) noexcept;
	inline constexpr cool::i32 integral_downcast(cool::i64 rhs) noexcept;
#endif // if defined(INT32_MAX) && defined(INT64_MAX)

#if defined(UINT8_MAX) && defined(UINT16_MAX)
	inline constexpr cool::u16 integral_upcast(cool::u8 rhs) noexcept;
	inline constexpr cool::u8 integral_downcast(cool::u16 rhs) noexcept;
#endif // if defined(UINT8_MAX) && defined(UINT16_MAX)

#if defined(UINT16_MAX) && defined(UINT32_MAX)
	inline constexpr cool::u32 integral_upcast(cool::u16 rhs) noexcept;
	inline constexpr cool::u16 integral_downcast(cool::u32 rhs) noexcept;
#endif // if defined(UINT16_MAX) && defined(UINT32_MAX)

#if defined(UINT32_MAX) && defined(UINT64_MAX)
	inline constexpr cool::u64 integral_upcast(cool::u32 rhs) noexcept;
	inline constexpr cool::u32 integral_downcast(cool::u64 rhs) noexcept;
#endif // if defined(UINT32_MAX) && defined(UINT64_MAX)


	// type if

	template <bool type_cond, class if_true_Ty, class if_false_Ty>
	using type_if = typename std::tuple_element<(type_cond ? static_cast<std::size_t>(0) : static_cast<std::size_t>(1)), std::tuple<if_true_Ty, if_false_Ty>>::type;


	// type switch

	template <std::size_t type_index, class ... Types>
	using type_switch = typename std::tuple_element<type_index, std::tuple<Types ...>>::type;


	// function pointers

	template <class function_Ty> class func_ptr : public cool::error_type<cool::static_error>
	{
		static_assert(std::is_function<function_Ty>::value, "cool::func_ptr only accepts function types as template argument");
	};

	template <class return_Ty, class ... arg_Ty> class func_ptr<return_Ty(arg_Ty ...)>
	{

	public:

		using value_type = return_Ty(*)(arg_Ty ...);
		using return_type = return_Ty;
		template <std::size_t arg_index> using arg_type = cool::type_switch<arg_index, arg_Ty ...>;

		static constexpr std::size_t arg_count = sizeof...(arg_Ty);

		func_ptr() noexcept = default;
		inline func_ptr(value_type _func_ptr) noexcept;
		inline func_ptr<return_Ty(arg_Ty ...)>& operator=(value_type _func_ptr) noexcept;
		template <class ... _arg_Ty> inline return_Ty operator()(_arg_Ty&& ... args) const;

		inline value_type& function() noexcept;
		inline const value_type& function() const noexcept;

		inline bool operator==(cool::func_ptr<return_Ty(arg_Ty ...)> rhs) const noexcept;
		inline bool operator!=(cool::func_ptr<return_Ty(arg_Ty ...)> rhs) const noexcept;

	private:

		value_type m_func_ptr;
	};

	template <class return_Ty, class ... arg_Ty>
	inline cool::func_ptr<return_Ty(arg_Ty ...)> make_func_ptr(return_Ty(*_func_ptr)(arg_Ty ...)) noexcept;

#if __cplusplus >= 201703L
	template <class return_Ty, class ... arg_Ty>
	inline cool::func_ptr<return_Ty(arg_Ty ...)> make_func_ptr(return_Ty(*_func_ptr)(arg_Ty ...) noexcept) noexcept;
#endif // __cplusplus >= 201703L

	// member function pointers

	template <class object_Ty, class function_Ty> class member_func_ptr : public cool::error_type<cool::static_error>
	{
		static_assert(std::is_class<object_Ty>::value&& std::is_function<function_Ty>::value,
			"cool::member_func_ptr only accepts classes and function types as first and second template arguments");
	};

	template <class object_Ty, class return_Ty, class ... arg_Ty> class member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>
	{

	private:

		static constexpr std::size_t _object_is_const = (std::is_const<object_Ty>::value) ? 1 : 0;
		static constexpr std::size_t _object_is_volatile = (std::is_volatile<object_Ty>::value) ? 2 : 0;
		using object_type_ncv = typename std::remove_cv<object_Ty>::type;

	public:

		using value_type = cool::type_switch<_object_is_const + _object_is_volatile,
			return_Ty(object_type_ncv::*)(arg_Ty ...),
			return_Ty(object_type_ncv::*)(arg_Ty ...) const,
			return_Ty(object_type_ncv::*)(arg_Ty ...) volatile,
			return_Ty(object_type_ncv::*)(arg_Ty ...) const volatile
		>;
		using object_type = object_Ty;
		using return_type = return_Ty;
		template <std::size_t arg_index> using arg_type = cool::type_switch<arg_index, arg_Ty ...>;

		static constexpr std::size_t arg_count = sizeof...(arg_Ty);
		static constexpr bool is_const = std::is_const<object_Ty>::value;
		static constexpr bool is_volatile = std::is_volatile<object_Ty>::value;

		member_func_ptr() noexcept = default;
		inline member_func_ptr(value_type _func_ptr) noexcept;
		inline member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>& operator=(value_type _func_ptr) noexcept;
		template <class ... _arg_Ty> inline return_Ty operator()(object_Ty* obj_ptr, _arg_Ty&& ... args) const;

		inline value_type& function() noexcept;
		inline const value_type& function() const noexcept;

		inline bool operator==(cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept;
		inline bool operator!=(cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept;

	private:

		value_type m_func_ptr;
	};

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...)) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile) noexcept;

#if __cplusplus >= 201703L
	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> make_member_func_ptr(return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile noexcept) noexcept;
#endif // __cplusplus >= 201703L

	// object member function pointers

	template <class object_Ty, class function_Ty> class object_member_func_ptr : public cool::error_type<cool::static_error>
	{
		static_assert(std::is_class<object_Ty>::value&& std::is_function<function_Ty>::value,
			"cool::object_member_func_ptr only accepts classes and function types as first and second template arguments");
	};

	template <class object_Ty, class return_Ty, class ... arg_Ty> class object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>
	{

	private:

		static constexpr std::size_t _object_is_const = (std::is_const<object_Ty>::value) ? 1 : 0;
		static constexpr std::size_t _object_is_volatile = (std::is_volatile<object_Ty>::value) ? 2 : 0;
		using object_type_ncv = typename std::remove_cv<object_Ty>::type;

	public:

		using value_type = cool::type_switch<_object_is_const + _object_is_volatile,
			return_Ty(object_type_ncv::*)(arg_Ty ...),
			return_Ty(object_type_ncv::*)(arg_Ty ...) const,
			return_Ty(object_type_ncv::*)(arg_Ty ...) volatile,
			return_Ty(object_type_ncv::*)(arg_Ty ...) const volatile
		>;
		using object_type = object_Ty;
		using object_pointer_type = object_Ty*;
		using return_type = return_Ty;
		template <std::size_t arg_index> using arg_type = cool::type_switch<arg_index, arg_Ty ...>;

		static constexpr std::size_t arg_count = sizeof...(arg_Ty);
		static constexpr bool is_const = std::is_const<object_Ty>::value;
		static constexpr bool is_volatile = std::is_volatile<object_Ty>::value;

		object_member_func_ptr() noexcept = default;
		inline object_member_func_ptr(object_Ty* obj_ptr, value_type _func_ptr) noexcept;
		template <class ... _arg_Ty> inline return_Ty operator()(_arg_Ty&& ... args) const;

		inline object_pointer_type& object() noexcept;
		inline const object_pointer_type& object() const noexcept;

		inline value_type& function() noexcept;
		inline const value_type& function() const noexcept;

		inline bool operator==(cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept;
		inline bool operator!=(cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept;

	private:

		object_Ty* m_obj_ptr;
		value_type m_func_ptr;
	};

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...)) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		const object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		const volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile) noexcept;

#if __cplusplus >= 201703L
	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		const object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile noexcept) noexcept;

	template <class object_Ty, class return_Ty, class ... arg_Ty>
	inline cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> make_object_member_func_ptr(
		const volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile noexcept) noexcept;
#endif // __cplusplus >= 201703L


	// aligned

	template <class Ty, std::size_t _align, std::size_t _alloc_align> class alignas(_alloc_align) _aligned_realign_impl;
	template <class Ty, std::size_t _align, std::size_t _alloc_align> class alignas(_alloc_align) _aligned_impl;

	template <class Ty, std::size_t _align = alignof(Ty), std::size_t _alloc_align = alignof(std::max_align_t)>
	using aligned = cool::type_if<(_align > _alloc_align),
		_aligned_realign_impl<Ty, _align, _alloc_align>,
		_aligned_impl<Ty, _align, _alloc_align>
	>;

	template <class Ty, std::size_t _align, std::size_t _alloc_align> class alignas(_alloc_align) _aligned_realign_impl
	{

	public:

		static_assert((_align & (_align - 1)) == 0, "cool::aligned<value_type, align, alloc_align> requirement : align must be a power of 2");
		static_assert((_alloc_align & (_alloc_align - 1)) == 0, "cool::aligned<value_type, align, alloc_align> requirement : alloc_align must be a power of 2");
		static_assert(_align >= alignof(Ty), "cool::aligned<value_type, align, alloc_align> requirement : align must not be smaller than alignof(value_type)");
		static_assert(_alloc_align >= alignof(std::max_align_t), "cool::aligned<value_type, align, alloc_align> requirement : alloc_align must not be smaller than alignof(std::max_align_t)");

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t align = _align;
		static constexpr std::size_t alloc_align = _alloc_align;

		inline _aligned_realign_impl();
		template <class ... arg_Ty> inline _aligned_realign_impl(arg_Ty&& ... args);
		inline _aligned_realign_impl(const cool::_aligned_realign_impl<Ty, _align, _alloc_align>& rhs);
		inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>& operator=(const cool::_aligned_realign_impl<Ty, _align, _alloc_align>& rhs);
		inline _aligned_realign_impl(cool::_aligned_realign_impl<Ty, _align, _alloc_align>&& rhs) noexcept;
		inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>& operator=(cool::_aligned_realign_impl<Ty, _align, _alloc_align>&& rhs) noexcept;
		inline ~_aligned_realign_impl();

		inline Ty& get_aligned_ref() noexcept;
		inline const Ty& get_aligned_ref() const noexcept;
		inline const Ty& get_aligned_const_ref() const noexcept;

		inline Ty* get_aligned_ptr() noexcept;
		inline const Ty* get_aligned_ptr() const noexcept;
		inline const Ty* get_aligned_const_ptr() const noexcept;

	private:

		static constexpr bool _need_realign = _align > _alloc_align;
		static constexpr std::size_t _byte_size = _need_realign ? sizeof(Ty) + _align - _alloc_align : sizeof(Ty);

		static inline Ty* _make_aligned_address_ptr(void* ptr) noexcept;

		unsigned char m_storage[_byte_size];
		Ty* m_ptr;
	};

	template <class Ty, std::size_t _align, std::size_t _alloc_align> class alignas(_alloc_align) _aligned_impl
	{

	public:

		static_assert((_align & (_align - 1)) == 0, "cool::aligned<value_type, align, alloc_align> requirement : align must be a power of 2");
		static_assert((_alloc_align & (_alloc_align - 1)) == 0, "cool::aligned<value_type, align, alloc_align> requirement : alloc_align must be a power of 2");
		static_assert(_align >= alignof(Ty), "cool::aligned<value_type, align, alloc_align> requirement : align must not be smaller than alignof(value_type)");
		static_assert(_alloc_align >= alignof(std::max_align_t), "cool::aligned<value_type, align, alloc_align> requirement : alloc_align must not be smaller than alignof(std::max_align_t)");

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t align = _align;
		static constexpr std::size_t alloc_align = _alloc_align;

		inline _aligned_impl() = default;
		template <class ... arg_Ty> inline _aligned_impl(arg_Ty&& ... args);
		inline _aligned_impl(const cool::_aligned_impl<Ty, _align, _alloc_align>& rhs) = default;
		inline cool::_aligned_impl<Ty, _align, _alloc_align>& operator=(const cool::_aligned_impl<Ty, _align, _alloc_align>& rhs) = default;
		inline _aligned_impl(cool::_aligned_impl<Ty, _align, _alloc_align>&& rhs) noexcept = default;
		inline cool::_aligned_impl<Ty, _align, _alloc_align>& operator=(cool::_aligned_impl<Ty, _align, _alloc_align>&& rhs) noexcept = default;
		inline ~_aligned_impl() = default;

		inline Ty& get_aligned_ref() noexcept;
		inline const Ty& get_aligned_ref() const noexcept;
		inline const Ty& get_aligned_const_ref() const noexcept;

		inline Ty* get_aligned_ptr() noexcept;
		inline const Ty* get_aligned_ptr() const noexcept;
		inline const Ty* get_aligned_const_ptr() const noexcept;

	private:

		Ty m_storage;
	};
}


// detail

// integers

namespace cool
{
	template <class int_Ty> class integer
	{

	public:

		static_assert(std::is_integral<int_Ty>::value, "value_type must be an integral type");

		using value_type = int_Ty;

		static constexpr std::size_t bit_count = sizeof(value_type) * CHAR_BIT;
		static constexpr bool is_signed = std::is_signed<value_type>::value;

		integer() noexcept = default;
		inline constexpr integer(value_type val) noexcept : m_value(val) {}

		static inline constexpr cool::integer<value_type> min() noexcept { return cool::integer<value_type>(std::numeric_limits<value_type>::min()); }
		static inline constexpr cool::integer<value_type> max() noexcept { return cool::integer<value_type>(std::numeric_limits<value_type>::max()); }

		inline cool::integer<value_type>& operator&=(cool::integer<value_type> rhs) noexcept { m_value &= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator|=(cool::integer<value_type> rhs) noexcept { m_value |= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator^=(cool::integer<value_type> rhs) noexcept { m_value ^= rhs.m_value; return *this; }

		inline cool::integer<value_type>& operator+=(cool::integer<value_type> rhs) noexcept { m_value += rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator-=(cool::integer<value_type> rhs) noexcept { m_value -= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator*=(cool::integer<value_type> rhs) noexcept { m_value *= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator/=(cool::integer<value_type> rhs) noexcept { m_value /= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator%=(cool::integer<value_type> rhs) noexcept { m_value %= rhs.m_value; return *this; }

		inline cool::integer<value_type>& operator<<=(cool::integer<value_type> rhs) noexcept { m_value <<= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator>>=(cool::integer<value_type> rhs) noexcept { m_value >>= rhs.m_value; return *this; }

		inline constexpr cool::integer<value_type> operator&(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value & rhs.m_value); }
		inline constexpr cool::integer<value_type> operator|(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value | rhs.m_value); }
		inline constexpr cool::integer<value_type> operator^(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value ^ rhs.m_value); }

		inline constexpr cool::integer<value_type> operator+(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value + rhs.m_value); }
		inline constexpr cool::integer<value_type> operator-(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value - rhs.m_value); }
		inline constexpr cool::integer<value_type> operator*(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value * rhs.m_value); }
		inline constexpr cool::integer<value_type> operator/(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value / rhs.m_value); }
		inline constexpr cool::integer<value_type> operator%(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value % rhs.m_value); }
		inline constexpr cool::integer<value_type> operator-() const noexcept { return cool::integer<value_type>(-m_value); }
		inline constexpr cool::integer<value_type> operator~() const noexcept { return cool::integer<value_type>(~m_value); }
		inline constexpr cool::integer<value_type> operator!() const noexcept { return cool::integer<value_type>(!m_value); }

		inline constexpr cool::integer<value_type> operator<<(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value << rhs.m_value); }
		inline constexpr cool::integer<value_type> operator>>(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value >> rhs.m_value); }

		inline constexpr bool operator==(cool::integer<value_type> rhs) const noexcept { return m_value == rhs.m_value; }
		inline constexpr bool operator!=(cool::integer<value_type> rhs) const noexcept { return m_value != rhs.m_value; }
		inline constexpr bool operator<=(cool::integer<value_type> rhs) const noexcept { return m_value <= rhs.m_value; }
		inline constexpr bool operator>=(cool::integer<value_type> rhs) const noexcept { return m_value >= rhs.m_value; }
		inline constexpr bool operator<(cool::integer<value_type> rhs) const noexcept { return m_value < rhs.m_value; }
		inline constexpr bool operator>(cool::integer<value_type> rhs) const noexcept { return m_value > rhs.m_value; }

		inline constexpr value_type get_value() const noexcept { return m_value; }

	private:

		value_type m_value;
	};

	template <> class integer<bool>
	{

	public:

		using value_type = bool;

		static constexpr std::size_t bit_count = sizeof(value_type) * CHAR_BIT;
		static constexpr bool is_signed = std::is_signed<value_type>::value;

		integer() noexcept = default;
		inline constexpr integer(value_type val) noexcept : m_value(val) {}

		static inline constexpr cool::integer<value_type> min() noexcept { return cool::integer<value_type>(std::numeric_limits<value_type>::min()); }
		static inline constexpr cool::integer<value_type> max() noexcept { return cool::integer<value_type>(std::numeric_limits<value_type>::max()); }

		inline cool::integer<value_type>& operator&=(cool::integer<value_type> rhs) noexcept { m_value &= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator|=(cool::integer<value_type> rhs) noexcept { m_value |= rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator^=(cool::integer<value_type> rhs) noexcept { m_value ^= rhs.m_value; return *this; }

		inline cool::integer<value_type>& operator+=(cool::integer<value_type> rhs) noexcept { m_value += rhs.m_value; return *this; }
		inline cool::integer<value_type>& operator*=(cool::integer<value_type> rhs) noexcept { m_value *= rhs.m_value; return *this; }

		inline constexpr cool::integer<value_type> operator&(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value & rhs.m_value); }
		inline constexpr cool::integer<value_type> operator|(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value | rhs.m_value); }
		inline constexpr cool::integer<value_type> operator^(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value ^ rhs.m_value); }

		inline constexpr cool::integer<value_type> operator+(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value + rhs.m_value); }
		inline constexpr cool::integer<value_type> operator*(cool::integer<value_type> rhs) const noexcept { return cool::integer<value_type>(m_value * rhs.m_value); }
		inline constexpr cool::integer<value_type> operator!() const noexcept { return cool::integer<value_type>(!m_value); }

		inline constexpr bool operator==(cool::integer<value_type> rhs) const noexcept { return m_value == rhs.m_value; }
		inline constexpr bool operator!=(cool::integer<value_type> rhs) const noexcept { return m_value != rhs.m_value; }
		inline constexpr bool operator<=(cool::integer<value_type> rhs) const noexcept { return m_value <= rhs.m_value; }
		inline constexpr bool operator>=(cool::integer<value_type> rhs) const noexcept { return m_value >= rhs.m_value; }
		inline constexpr bool operator<(cool::integer<value_type> rhs) const noexcept { return m_value < rhs.m_value; }
		inline constexpr bool operator>(cool::integer<value_type> rhs) const noexcept { return m_value > rhs.m_value; }

		inline constexpr value_type get_value() const noexcept { return m_value; }

	private:

		value_type m_value;
	};
}

template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator&(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs & rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator|(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs | rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator^(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs ^ rhs.get_value()); }

template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator+(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs + rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator-(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs - rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator*(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs * rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator/(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs / rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator%(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs % rhs.get_value()); }

template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator<<(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs << rhs.get_value()); }
template <class int_Ty> inline constexpr cool::integer<int_Ty> cool::operator>>(int_Ty lhs, cool::integer<int_Ty> rhs) noexcept { return cool::integer<int_Ty>(lhs >> rhs.get_value()); }

template <class int_res_Ty, class int_Ty> inline constexpr int_res_Ty cool::integral_cast(cool::integer<int_Ty> rhs) noexcept {
	static_assert(cool::is_integral<int_res_Ty>::value, "result type must be a cool integer type");
	using new_type = typename int_res_Ty::value_type;
	return int_res_Ty(static_cast<new_type>(rhs.get_value()));
}

template <class int_Ty> inline constexpr cool::integer<typename std::make_signed<int_Ty>::type> cool::integral_signed_cast(cool::integer<int_Ty> rhs) noexcept {
	using new_type = typename std::make_signed<int_Ty>::type;
	return cool::integer<new_type>(static_cast<new_type>(rhs.get_value()));
}
template <class int_Ty> inline constexpr cool::integer<typename std::make_unsigned<int_Ty>::type> cool::integral_unsigned_cast(cool::integer<int_Ty> rhs) noexcept {
	using new_type = typename std::make_unsigned<int_Ty>::type;
	return cool::integer<new_type>(static_cast<new_type>(rhs.get_value()));
}

#if defined(INT8_MAX) && defined(INT16_MAX)
inline constexpr cool::i16 cool::integral_upcast(cool::i8 rhs) noexcept { return cool::i16(static_cast<std::int16_t>(rhs.get_value())); }
inline constexpr cool::i8 cool::integral_downcast(cool::i16 rhs) noexcept { return cool::i8(static_cast<std::int8_t>(rhs.get_value())); }
#endif // if defined(INT8_MAX) && defined(INT16_MAX)

#if defined(INT16_MAX) && defined(INT32_MAX)
inline constexpr cool::i32 cool::integral_upcast(cool::i16 rhs) noexcept { return cool::i32(static_cast<std::int32_t>(rhs.get_value())); }
inline constexpr cool::i16 cool::integral_downcast(cool::i32 rhs) noexcept { return cool::i16(static_cast<std::int16_t>(rhs.get_value())); }
#endif // if defined(INT16_MAX) && defined(INT32_MAX)

#if defined(INT32_MAX) && defined(INT64_MAX)
inline constexpr cool::i64 cool::integral_upcast(cool::i32 rhs) noexcept { return cool::i64(static_cast<std::int64_t>(rhs.get_value())); }
inline constexpr cool::i32 cool::integral_downcast(cool::i64 rhs) noexcept { return cool::i32(static_cast<std::int32_t>(rhs.get_value())); }
#endif // if defined(INT32_MAX) && defined(INT64_MAX)

#if defined(UINT8_MAX) && defined(UINT16_MAX)
inline constexpr cool::u16 cool::integral_upcast(cool::u8 rhs) noexcept { return cool::u16(static_cast<std::uint16_t>(rhs.get_value())); }
inline constexpr cool::u8 cool::integral_downcast(cool::u16 rhs) noexcept { return cool::u8(static_cast<std::uint8_t>(rhs.get_value())); }
#endif // if defined(UINT8_MAX) && defined(UINT16_MAX)

#if defined(UINT16_MAX) && defined(UINT32_MAX)
inline constexpr cool::u32 cool::integral_upcast(cool::u16 rhs) noexcept { return cool::u32(static_cast<std::uint32_t>(rhs.get_value())); }
inline constexpr cool::u16 cool::integral_downcast(cool::u32 rhs) noexcept { return cool::u16(static_cast<std::uint16_t>(rhs.get_value())); }
#endif // if defined(UINT16_MAX) && defined(UINT32_MAX)

#if defined(UINT32_MAX) && defined(UINT64_MAX)
inline constexpr cool::u64 cool::integral_upcast(cool::u32 rhs) noexcept { return cool::u64(static_cast<std::uint64_t>(rhs.get_value())); }
inline constexpr cool::u32 cool::integral_downcast(cool::u64 rhs) noexcept { return cool::u32(static_cast<std::uint32_t>(rhs.get_value())); }
#endif // if defined(UINT32_MAX) && defined(UINT64_MAX)


// member function pointers

template <class return_Ty, class ... arg_Ty>
inline cool::func_ptr<return_Ty(arg_Ty ...)>::func_ptr(value_type _func_ptr) noexcept : m_func_ptr(_func_ptr) {}

template <class return_Ty, class ... arg_Ty>
inline cool::func_ptr<return_Ty(arg_Ty ...)>& cool::func_ptr<return_Ty(arg_Ty ...)>::operator=(value_type _func_ptr) noexcept {
	m_func_ptr = _func_ptr; return *this;
}

template <class return_Ty, class ... arg_Ty> template <class ... _arg_Ty>
inline return_Ty cool::func_ptr<return_Ty(arg_Ty ...)>::operator()(_arg_Ty&& ... args) const { return m_func_ptr(std::forward<_arg_Ty>(args)...); }

template <class return_Ty, class ... arg_Ty>
inline typename cool::func_ptr<return_Ty(arg_Ty ...)>::value_type& cool::func_ptr<return_Ty(arg_Ty ...)>::function() noexcept { return m_func_ptr; }

template <class return_Ty, class ... arg_Ty>
inline const typename cool::func_ptr<return_Ty(arg_Ty ...)>::value_type& cool::func_ptr<return_Ty(arg_Ty ...)>::function() const noexcept { return m_func_ptr; }

template <class return_Ty, class ... arg_Ty>
inline bool cool::func_ptr<return_Ty(arg_Ty ...)>::operator==(cool::func_ptr<return_Ty(arg_Ty ...)> rhs) const noexcept {
	return m_func_ptr == rhs.m_func_ptr;
}

template <class return_Ty, class ... arg_Ty>
inline bool cool::func_ptr<return_Ty(arg_Ty ...)>::operator!=(cool::func_ptr<return_Ty(arg_Ty ...)> rhs) const noexcept {
	return m_func_ptr != rhs.m_func_ptr;
}

template <class return_Ty, class ... arg_Ty>
inline cool::func_ptr<return_Ty(arg_Ty ...)> cool::make_func_ptr(return_Ty(*_func_ptr)(arg_Ty ...)) noexcept
{
	return cool::func_ptr<return_Ty(arg_Ty ...)>(_func_ptr);
}

#if __cplusplus >= 201703L
template <class return_Ty, class ... arg_Ty>
inline cool::func_ptr<return_Ty(arg_Ty ...)> cool::make_func_ptr(return_Ty(*_func_ptr)(arg_Ty ...) noexcept) noexcept
{
	return cool::func_ptr<return_Ty(arg_Ty ...)>(_func_ptr);
}
#endif // __cplusplus >= 201703L

// member function pointers

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::member_func_ptr(value_type _func_ptr) noexcept : m_func_ptr(_func_ptr) {}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>& cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator=(value_type _func_ptr) noexcept {
	m_func_ptr = _func_ptr; return *this;
}

template <class object_Ty, class return_Ty, class ... arg_Ty> template <class ... _arg_Ty>
inline return_Ty cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator()(object_Ty* obj_ptr, _arg_Ty&& ... args) const {
	return ((*obj_ptr).*m_func_ptr)(std::forward<_arg_Ty>(args)...);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline typename cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::value_type& cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::function() noexcept { return m_func_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline const typename cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::value_type& cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::function() const noexcept { return m_func_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline bool cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator==(cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept {
	return m_func_ptr == rhs.m_func_ptr;
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline bool cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator!=(cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept {
	return m_func_ptr != rhs.m_func_ptr;
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...)) noexcept
{
	return cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const) noexcept
{
	return cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile) noexcept
{
	return cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile) noexcept
{
	return cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

#if __cplusplus >= 201703L
template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) noexcept) noexcept
{
	return cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const noexcept) noexcept
{
	return cool::member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile noexcept) noexcept
{
	return cool::member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_member_func_ptr(
	return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile noexcept) noexcept
{
	return cool::member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)>(_func_ptr);
}
#endif // __cplusplus >= 201703L

// object member function pointers

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::object_member_func_ptr(object_Ty* obj_ptr, value_type _func_ptr) noexcept
	: m_obj_ptr(obj_ptr), m_func_ptr(_func_ptr) {
}

template <class object_Ty, class return_Ty, class ... arg_Ty> template <class ... _arg_Ty>
inline return_Ty cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator()(_arg_Ty&& ... args) const {
	return ((*m_obj_ptr).*m_func_ptr)(std::forward<_arg_Ty>(args)...);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline typename cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::object_pointer_type& cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::object() noexcept { return m_obj_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline const typename cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::object_pointer_type& cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::object() const noexcept { return m_obj_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline typename cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::value_type& cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::function() noexcept { return m_func_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline const typename cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::value_type& cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::function() const noexcept { return m_func_ptr; }

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline bool cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator==(cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept {
	return (m_obj_ptr == rhs.m_obj_ptr) && (m_func_ptr == rhs.m_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline bool cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>::operator!=(cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs) const noexcept {
	return (m_obj_ptr != rhs.m_obj_ptr) || (m_func_ptr != rhs.m_func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...)) noexcept
{
	return cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	const object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const) noexcept
{
	return cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile) noexcept
{
	return cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	const volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile) noexcept
{
	return cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

#if __cplusplus >= 201703L
template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) noexcept) noexcept
{
	return cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	const object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const noexcept) noexcept
{
	return cool::object_member_func_ptr<const object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) volatile noexcept) noexcept
{
	return cool::object_member_func_ptr<volatile object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}

template <class object_Ty, class return_Ty, class ... arg_Ty>
inline cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)> cool::make_object_member_func_ptr(
	const volatile object_Ty* obj_ptr, return_Ty(object_Ty::* _func_ptr)(arg_Ty ...) const volatile noexcept) noexcept
{
	return cool::object_member_func_ptr<const volatile object_Ty, return_Ty(arg_Ty ...)>(obj_ptr, _func_ptr);
}
#endif // __cplusplus >= 201703L

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>::_aligned_realign_impl()
{
	m_ptr = _make_aligned_address_ptr(static_cast<void*>(m_storage));
	new (m_ptr) Ty();
}

template <class Ty, std::size_t _align, std::size_t _alloc_align> template <class ... arg_Ty>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>::_aligned_realign_impl(arg_Ty&& ... args)
{
	m_ptr = _make_aligned_address_ptr(static_cast<void*>(m_storage));
	new (m_ptr) Ty(std::forward<arg_Ty>(args)...);
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>::_aligned_realign_impl(const cool::_aligned_realign_impl<Ty, _align, _alloc_align>& rhs)
{
	m_ptr = _make_aligned_address_ptr(static_cast<void*>(m_storage));
	new (m_ptr) Ty(*rhs.m_ptr);
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>& cool::_aligned_realign_impl<Ty, _align, _alloc_align>::operator=(const cool::_aligned_realign_impl<Ty, _align, _alloc_align>& rhs)
{
	*m_ptr = *rhs.m_ptr;
	return *this;
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>::_aligned_realign_impl(cool::_aligned_realign_impl<Ty, _align, _alloc_align>&& rhs) noexcept
{
	m_ptr = _make_aligned_address_ptr(static_cast<void*>(m_storage));
	new (m_ptr) Ty(std::move(*rhs.m_ptr));
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>& cool::_aligned_realign_impl<Ty, _align, _alloc_align>::operator=(cool::_aligned_realign_impl<Ty, _align, _alloc_align>&& rhs) noexcept
{
	*m_ptr = std::move(*rhs.m_ptr);
	return *this;
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline cool::_aligned_realign_impl<Ty, _align, _alloc_align>::~_aligned_realign_impl()
{
	m_ptr->~Ty();
}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline Ty& cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_ref() noexcept { return *m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty& cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_ref() const noexcept { return *m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty& cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_const_ref() const noexcept { return *m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline Ty* cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_ptr() noexcept { return m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty* cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_ptr() const noexcept { return m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty* cool::_aligned_realign_impl<Ty, _align, _alloc_align>::get_aligned_const_ptr() const noexcept { return m_ptr; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline Ty* cool::_aligned_realign_impl<Ty, _align, _alloc_align>::_make_aligned_address_ptr(void* ptr) noexcept
{
	if (_need_realign)
	{
		std::uintptr_t ptr_address = reinterpret_cast<std::uintptr_t>(ptr);
		std::uintptr_t remainder = ptr_address % _align;
		if (remainder != 0)
		{
			ptr_address += (_align - remainder);
		}

		return reinterpret_cast<Ty*>(ptr_address);
	}
	else
	{
		return static_cast<Ty*>(ptr);
	}
}

template <class Ty, std::size_t _align, std::size_t _alloc_align> template <class ... arg_Ty>
inline cool::_aligned_impl<Ty, _align, _alloc_align>::_aligned_impl(arg_Ty&& ... args) : m_storage(std::forward<arg_Ty>(args)...) {}

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline Ty& cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_ref() noexcept { return m_storage; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty& cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_ref() const noexcept { return m_storage; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty& cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_const_ref() const noexcept { return m_storage; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline Ty* cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_ptr() noexcept { return &m_storage; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty* cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_ptr() const noexcept { return &m_storage; }

template <class Ty, std::size_t _align, std::size_t _alloc_align>
inline const Ty* cool::_aligned_impl<Ty, _align, _alloc_align>::get_aligned_const_ptr() const noexcept { return &m_storage; }

#endif // xCOOL_BASIC_TYPES_HPP

#if defined(xCOOL_BASIC_TYPES_HPP) && (defined(_LIBCPP_OSTREAM) || defined(_GLIBCXX_OSTREAM) || defined(_OSTREAM_))
#ifndef xCOOL_BASIC_TYPES_HPP_OSTREAM
#define xCOOL_BASIC_TYPES_HPP_OSTREAM
namespace cool
{
	template <class int_Ty, class char_Ty>
	inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
		cool::integer<int_Ty> rhs);

	template <class return_Ty, class ... arg_Ty, class char_Ty>
	inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
		cool::func_ptr<return_Ty(arg_Ty ...)> rhs);

	template <class object_Ty, class return_Ty, class ... arg_Ty, class char_Ty>
	inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
		cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs);

	template <class object_Ty, class return_Ty, class ... arg_Ty, class char_Ty>
	inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
		cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs);
}

template <class int_Ty, class char_Ty>
inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& cool::operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
	cool::integer<int_Ty> rhs)
{
	out_stream << rhs.get_value();
	return out_stream;
}

template <class return_Ty, class ... arg_Ty, class char_Ty>
inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& cool:: operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
	cool::func_ptr<return_Ty(arg_Ty ...)> rhs)
{
	out_stream << rhs.function();
	return out_stream;
}

template <class object_Ty, class return_Ty, class ... arg_Ty, class char_Ty>
inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& cool:: operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
	cool::member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs)
{
	if (sizeof(void(*)(void)) == sizeof(decltype(rhs.function())))
	{
		void(*function_ptr)(void);
		std::memcpy(&function_ptr, &(rhs.function()), sizeof(void(*)(void)));
		out_stream << function_ptr;
	}
	else
	{
		out_stream << rhs.function();
	}

	return out_stream;
}

template <class object_Ty, class return_Ty, class ... arg_Ty, class char_Ty>
inline std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& cool:: operator<<(std::basic_ostream<char_Ty, std::char_traits<char_Ty>>& out_stream,
	cool::object_member_func_ptr<object_Ty, return_Ty(arg_Ty ...)> rhs)
{
	out_stream << rhs.object() << '-' << '>';

	if (sizeof(void(*)(void)) == sizeof(decltype(rhs.function())))
	{
		void(*function_ptr)(void);
		std::memcpy(&function_ptr, &(rhs.function()), sizeof(void(*)(void)));
		out_stream << function_ptr;
	}
	else
	{
		out_stream << rhs.function();
	}

	return out_stream;
}
#endif // xCOOL_BASIC_TYPES_HPP_OSTREAM
#endif // defined(xCOOL_BASIC_TYPES_HPP) && (defined(_LIBCPP_OSTREAM) || defined(_GLIBCXX_OSTREAM) || defined(_OSTREAM_))


// cool_basic_types.hpp
// 
// This is free software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.
