// cool_matrix.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef _COOL_MATRIX_HPP
#define _COOL_MATRIX_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <complex>

#ifndef COOL_MATRIX_RESTRICT
#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
#define COOL_MATRIX_RESTRICT __restrict
#else // compiler choice
#define COOL_MATRIX_RESTRICT
#endif // compiler choice
#endif // COOL_MATRIX_RESTRICT


#ifndef COOL_MATRIX_MM_DEFAULT_KERNEL_SPEC_PS
#define COOL_MATRIX_MM_DEFAULT_KERNEL_SPEC_PS { 8, 2, 8 }
#endif // COOL_MATRIX_MM_DEFAULT_KERNEL_SPEC_PS

#ifndef COOL_MATRIX_DEFAULT_ALIGN_SPEC
#define COOL_MATRIX_DEFAULT_ALIGN_SPEC { 8, 16, 32 }
#endif // COOL_MATRIX_DEFAULT_ALIGN_SPEC


// matrix multiply kernel specifications
// values : { packed, broadcasted, repeat_threshold }
//
// packed : number of floating point numbers packed contiguously
// in vector registers,
// should be a multiple of <elements per vector register>,
// divided by 2 in ***default*** kernel when double precision is used
//
// broadcasted : number of floating point numbers broadcasted
// in vector registers
//
// repeat_threshold : minimum number of consecutive repeats
// of the kernel considered to warrant setting up a loop
//
// minimum number of vector registers used in the kernel
// when rhs column count > 1 and if no spill occurs  :
// (packed / (elements per vector register)) * broadcasted
// + (packed / (elements per vector register)) + 1
//
// minimum number of vector registers used in the kernel
// when rhs column count == 1 and if no spill occurs :
// (packed / <elements per vector register>) * max(broadcasted / 2, 1)
// + (packed / <elements per vector register>) * max(broadcasted / 2, 1) + 1
// because new packed value is adusted to 
// (packed / <elements per vector register>) * max(broadcasted / 2, 1)
// unless ***matrix_multiply_kernel_spec*** is specialized
//
// number of multiply add scalar operations in kernel :
// packed * broadcasted


// matrix align specifications
// values in byte alignments : { a0, a1, ... , aN }
//
// required : a0 < a1 < ... < aN and a1, a2, ... , aN are all powers of 2
//
// in a matrix of element type Ty and rows padded to _rows_padded elements,
// default alignment of matrix will be aj for greatest the j such that
// _rows_padded * sizeof(Ty) is a multiple of aj and aj is a multiple
// of alignof(Ty)


namespace cool
{
	// template specializable kernel dimensions
	// overrides COOL_MATRIX_MM_DEFAULT_KERNEL_SPEC_PS
	template <class Ty> class matrix_type_multiply_kernel_spec
	{

	public:

		// specializable parameters
		static constexpr std::size_t packed = 0;
		static constexpr std::size_t broadcasted = 0;
		static constexpr std::size_t repeat_threshold = 0;
	};

	// template specializable kernel dimensions for matrices with specific dimensions
	// overrides matrix_type_multiply_kernel_spec
	template <class Ty, std::size_t _lhs_rows, std::size_t _rhs_cols, std::size_t _lhs_cols> class matrix_multiply_kernel_spec
	{

	private:

		static constexpr std::size_t _default_kernel_ps[3] = COOL_MATRIX_MM_DEFAULT_KERNEL_SPEC_PS;
		inline static constexpr std::size_t compute_packed_param() noexcept;
		inline static constexpr std::size_t compute_broadcasted_param() noexcept;
		inline static constexpr std::size_t compute_repeat_threshold_param() noexcept;

	public:

		// specializable parameters
		static constexpr std::size_t packed = compute_packed_param();
		static constexpr std::size_t broadcasted = compute_broadcasted_param();
		static constexpr std::size_t repeat_threshold = compute_repeat_threshold_param();
	};
}


namespace cool
{
	// template specializable byte alignment for matrices with specific dimensions
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded = _rows> class matrix_align_spec
	{

	private:

		static constexpr std::size_t _default_multiples[] = COOL_MATRIX_DEFAULT_ALIGN_SPEC;
		inline static constexpr std::size_t compute_value() noexcept;

	public:

		// specializable align
		static constexpr std::size_t value = compute_value();
	};
}


namespace cool
{
#ifndef _COOL_NO_INIT_ENUM
#define _COOL_NO_INIT_ENUM
	enum no_init_t { no_init };
#endif // _COOL_NO_INIT_ENUM

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class _matrix_ptr;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class _const_matrix_ptr;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class _matrix_array;


	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> class const_matrix_interface;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> class matrix_interface;


	template <class Ty, std::size_t _rows, std::size_t _cols = 1, std::size_t _rows_padded = _rows,
		std::size_t _align = cool::matrix_align_spec<Ty, _rows, _cols, _rows_padded>::value
	>
	using matrix = cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>>;

	template <class Ty, std::size_t _rows, std::size_t _cols = 1, std::size_t _rows_padded = _rows, std::size_t _align = 0>
	using matrix_span = cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>>;

	template <class Ty, std::size_t _rows, std::size_t _cols = 1, std::size_t _rows_padded = _rows, std::size_t _align = 0>
	using const_matrix_span = cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>>;


	template <std::size_t _optional_dim, std::size_t _default_dim> class _opt_dim {
	public:
		static constexpr std::size_t value = _optional_dim != 0 ? _optional_dim : _default_dim;
	};

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _opt_rows_padded, std::size_t _opt_align>
	using matrix_result = cool::matrix<Ty, _rows, _cols, cool::_opt_dim<_opt_rows_padded, _rows>::value,
		cool::_opt_dim<_opt_align, cool::matrix_align_spec<Ty, _rows, _cols, cool::_opt_dim<_opt_rows_padded, _rows>::value>::value>::value>;


	template <class Ty> class cm;
	template <class Ty> class rm;


	namespace matrix_scalar_subroutine
	{
		// template specializable functions
		template <class Ty> static inline Ty conj(const Ty& val) noexcept; // return type must be same type as val
		template <class Ty> static inline Ty abs_sq(const Ty& val) noexcept; // return type must be comparable with operator<

		// template specializations
		template <class Ty> static inline std::complex<Ty> conj(const std::complex<Ty>& val) noexcept;
		template <class Ty> static inline Ty abs_sq(const std::complex<Ty>& val) noexcept;
	}


	template <class Ty> class cm
	{

	public:

		inline cm() noexcept { m_begin_ptr = nullptr; }
		inline cm(std::initializer_list<Ty> lst) noexcept { m_begin_ptr = lst.begin(); }
		inline cm(const Ty* ptr) noexcept { m_begin_ptr = ptr; }
		cm(const cool::cm<Ty>&) noexcept = default;
		cool::cm<Ty>& operator=(const cool::cm<Ty>&) noexcept = default;
		~cm() = default;

		inline const Ty* begin() const noexcept { return m_begin_ptr; }

	private:

		const Ty* m_begin_ptr;
	};

	template <class Ty> class rm
	{

	public:

		inline rm() noexcept { m_begin_ptr = nullptr; }
		inline rm(std::initializer_list<Ty> lst) noexcept { m_begin_ptr = lst.begin(); }
		inline rm(const Ty* ptr) noexcept { m_begin_ptr = ptr; }
		rm(const cool::rm<Ty>&) noexcept = default;
		cool::rm<Ty>& operator=(const cool::rm<Ty>&) noexcept = default;
		~rm() = default;

		inline const Ty* begin() const noexcept { return m_begin_ptr; }

	private:

		const Ty* m_begin_ptr;
	};


	// _matrix_ptr

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class _matrix_ptr
	{

	public:

		_matrix_ptr() = delete;
		inline _matrix_ptr(Ty* ptr) noexcept;
		inline _matrix_ptr(const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept = default;
		inline ~_matrix_ptr() = default;

		inline cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept;

		template <std::size_t _rhs_rows, std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_matrix_ptr<Ty, _rhs_rows, _rhs_cols, _rhs_rows_padded, _rhs_align>& rhs) noexcept;

		inline Ty* data() noexcept;
		inline const Ty* data() const noexcept;

	private:

		Ty* m_data_ptr;
	};

	// _const_matrix_ptr

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class _const_matrix_ptr
	{

	public:

		_const_matrix_ptr() = delete;
		inline _const_matrix_ptr(const Ty* ptr) noexcept;
		inline _const_matrix_ptr(const cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept = default;
		inline _const_matrix_ptr(const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept;
		inline ~_const_matrix_ptr() = default;

		inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) = delete;

		template <std::size_t _rhs_rows, std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_const_matrix_ptr<Ty, _rhs_rows, _rhs_cols, _rhs_rows_padded, _rhs_align>& rhs) = delete;

		inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) = delete;

		template <std::size_t _rhs_rows, std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& operator=(
			const cool::_matrix_ptr<Ty, _rhs_rows, _rhs_cols, _rhs_rows_padded, _rhs_align>& rhs) = delete;

		inline const Ty* data() const noexcept;

	private:

		const Ty* m_data_ptr;
	};

	// _matrix_array

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align> class alignas(_align) _matrix_array
	{

	public:

		inline _matrix_array() = default;
		inline _matrix_array(cool::no_init_t) {}

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline _matrix_array(const cool::_matrix_array<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs);

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline _matrix_array(const cool::_const_matrix_ptr<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs);

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
		inline _matrix_array(const cool::_matrix_ptr<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs);

		inline _matrix_array(Ty rhs);

		constexpr inline _matrix_array(std::initializer_list<Ty> rhs);
		inline _matrix_array(cool::cm<Ty> rhs);
		inline _matrix_array(cool::rm<Ty> rhs);

		inline ~_matrix_array() = default;

		inline Ty* data() noexcept;
		inline const Ty* data() const noexcept;

	private:

		template <class Ty2, std::size_t _rows2, std::size_t _cols2, std::size_t _rows_padded2, std::size_t _align2>
		friend class cool::_matrix_array;

		Ty m_data[_rows_padded * _cols];
	};


	template <std::size_t _rows, std::size_t _cols> class _diag_length {
	public:
		static constexpr std::size_t value = _rows < _cols ? _rows : _cols;
	};

	// const_matrix_interface

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> class const_matrix_interface : public _matrix_data_Ty
	{

	public:

		static_assert(_rows != 0, "cool::matrix...<value_type, cols, rows, rows_padded, align, ...> requirement : rows != 0");
		static_assert(_cols != 0, "cool::matrix...<value_type, cols, rows, rows_padded, align, ...> requirement : cols != 0");
		static_assert(_rows <= _rows_padded, "cool::matrix...<value_type, cols, rows, rows_padded, align, ...> requirement : rows <= rows_padded");
		static_assert(_align% alignof(Ty) == 0, "cool::matrix...<value_type, cols, rows, rows_padded, align, ...> requirement : align mutiple of alignof(value_type)");


		using _matrix_data_Ty::_matrix_data_Ty;

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		const_matrix_interface() = default;

		constexpr inline std::size_t size() const noexcept;
		constexpr inline std::size_t size_padded() const noexcept;

		template <class uint_Ty1, class uint_Ty2> inline const Ty& operator()(uint_Ty1 i, uint_Ty2 j) const noexcept;
		template <class uint_Ty> inline const Ty& operator[](uint_Ty mem_offset) const noexcept;


		template <class uint_Ty> inline cool::const_matrix_span<Ty, _rows, 1, _rows, 0> col(uint_Ty j) const noexcept;

		template <class uint_Ty> inline cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0> row(uint_Ty i) const noexcept;

		inline cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> diag() const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1, class uint_Ty2>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i, uint_Ty2 j) const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i) const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk() const noexcept;

		template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0>
		cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator-() const noexcept;

		template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator+(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator-(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0>
		cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator+(Ty rhs) const noexcept;

		template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0>
		cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator-(Ty rhs) const noexcept;

		template <std::size_t _res_rows_padded = _rows, std::size_t _res_align = cool::matrix_align_spec<Ty, _rows, _cols, _res_rows_padded>::value,
			std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix<Ty, _rows, _rhs_cols, _res_rows_padded, _res_align> operator*(const cool::const_matrix_interface<Ty, _cols, _rhs_cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;


		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator==(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator!=(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator<=(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator>=(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator<(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline bool operator>(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept;

		inline bool operator==(Ty rhs) const noexcept;

		inline bool operator!=(Ty rhs) const noexcept;

		inline bool operator<=(Ty rhs) const noexcept;

		inline bool operator>=(Ty rhs) const noexcept;

		inline bool operator<(Ty rhs) const noexcept;

		inline bool operator>(Ty rhs) const noexcept;

		// iterator

		class cm_const_iterator_proxy;
		class rm_const_iterator_proxy;

		class cm_const_iterator;
		class rm_const_iterator;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy cm() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy rm() const noexcept;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator begin() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator end() const noexcept;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cbegin() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cend() const noexcept;

		class cm_const_iterator_proxy
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			inline cm_const_iterator_proxy(const Ty* ptr) noexcept : m_data_ptr(ptr) {}

			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator begin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator end() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cbegin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cend() const noexcept;

		private:

			const Ty* m_data_ptr;
		};

		class rm_const_iterator_proxy
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			inline rm_const_iterator_proxy(const Ty* ptr) noexcept : m_data_ptr(ptr) {}

			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator begin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator end() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator cbegin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator cend() const noexcept;

		private:

			const Ty* m_data_ptr;
		};

		class cm_const_iterator
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			using iterator_category = std::bidirectional_iterator_tag;

			inline cm_const_iterator(const Ty* ptr, std::size_t j) noexcept;

			inline const Ty& operator*() const noexcept;
			inline const Ty& operator->() const noexcept;

			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& operator--() noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& operator++() noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator operator--(int) noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator operator++(int) noexcept;

			inline bool operator==(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;
			inline bool operator!=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;
			inline bool operator<=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;
			inline bool operator>=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;
			inline bool operator<(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;
			inline bool operator>(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept;

		private:

			const Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
		};

		class rm_const_iterator
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			using iterator_category = std::bidirectional_iterator_tag;

			inline rm_const_iterator(const Ty* ptr, std::size_t i) noexcept : m_data_ptr(ptr), m_i(i), m_j(0) {}

			inline const Ty& operator*() const noexcept;
			inline const Ty& operator->() const noexcept;

			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& operator--() noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& operator++() noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator operator--(int) noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator operator++(int) noexcept;

			inline bool operator==(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;
			inline bool operator!=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;
			inline bool operator<=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;
			inline bool operator>=(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;
			inline bool operator<(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;
			inline bool operator>(const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept;

		private:

			const Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
		};
	};


	// matrix_interface

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> class matrix_interface
		: public cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>
	{

	public:

		using cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::const_matrix_interface;

		matrix_interface() = default;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline matrix_interface(const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs);

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator=(
			const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs);

		inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator=(Ty rhs);

		inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator=(std::initializer_list<Ty> rhs);
		inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator=(cool::cm<Ty> rhs);
		inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator=(cool::rm<Ty> rhs);

		template <class uint_Ty1, class uint_Ty2> inline Ty& operator()(uint_Ty1 i, uint_Ty2 j) noexcept;
		template <class uint_Ty1, class uint_Ty2> inline const Ty& operator()(uint_Ty1 i, uint_Ty2 j) const noexcept;

		template <class uint_Ty> inline Ty& operator[](uint_Ty mem_offset) noexcept;
		template <class uint_Ty> inline const Ty& operator[](uint_Ty mem_offset) const noexcept;

		template <class uint_Ty>
		inline cool::matrix_span<Ty, _rows, 1, _rows, 0> col(uint_Ty j) noexcept;

		template <class uint_Ty>
		inline cool::const_matrix_span<Ty, _rows, 1, _rows, 0> col(uint_Ty j) const noexcept;

		template <class uint_Ty>
		inline cool::matrix_span<Ty, 1, _cols, _rows_padded, 0> row(uint_Ty i) noexcept;

		template <class uint_Ty>
		inline cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0> row(uint_Ty i) const noexcept;

		inline cool::matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> diag() noexcept;
		inline cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> diag() const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1, class uint_Ty2>
		inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i, uint_Ty2 j) noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1, class uint_Ty2>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i, uint_Ty2 j) const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1>
		inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i) noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk(uint_Ty1 i) const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1>
		inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk() noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1>
		inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> blk() const noexcept;


		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator+=(
			const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator-=(
			const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator*=(
			const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator+=(Ty rhs) noexcept;

		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator-=(Ty rhs) noexcept;

		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& operator*=(Ty rhs) noexcept;

		template <int _sign = 1, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& fmadd(Ty s,
			const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <int _sign = 1, std::size_t _lhs_cols, std::size_t _rhs_rows_padded, std::size_t _lhs_rows_padded, std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& fmadd(
			const cool::const_matrix_interface<Ty, _rows, _lhs_cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT lhs,
			const cool::const_matrix_interface<Ty, _lhs_cols, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <int _sign = 1, std::size_t _lhs_cols, std::size_t _rhs_rows_padded, std::size_t _lhs_rows_padded, std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
		cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& fmadd(Ty s,
			const cool::const_matrix_interface<Ty, _rows, _lhs_cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT lhs,
			const cool::const_matrix_interface<Ty, _lhs_cols, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept;

		// iterators

		class cm_iterator_proxy;
		class rm_iterator_proxy;

		class cm_iterator;
		class rm_iterator;

		inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy cm() noexcept;
		inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy rm() noexcept;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy cm() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy rm() const noexcept;

		inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator begin() noexcept;
		inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator end() noexcept;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator begin() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator end() const noexcept;

		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cbegin() const noexcept;
		inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cend() const noexcept;

		class cm_iterator_proxy
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			inline cm_iterator_proxy(Ty* ptr) noexcept : m_data_ptr(ptr) {}

			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator begin() const noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator end() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cbegin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator cend() const noexcept;

		private:

			Ty* m_data_ptr;
		};

		class rm_iterator_proxy
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			inline rm_iterator_proxy(Ty* ptr) noexcept : m_data_ptr(ptr) {}

			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator begin() const noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator end() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator cbegin() const noexcept;
			inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator cend() const noexcept;

		private:

			Ty* m_data_ptr;
		};

		class cm_iterator
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			using iterator_category = std::bidirectional_iterator_tag;

			inline cm_iterator(Ty* ptr, std::size_t j) noexcept;

			inline Ty& operator*() const noexcept;
			inline Ty& operator->() const noexcept;

			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& operator--() noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& operator++() noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator operator--(int) noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator operator++(int) noexcept;

			inline bool operator==(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;
			inline bool operator!=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;
			inline bool operator<=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;
			inline bool operator>=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;
			inline bool operator<(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;
			inline bool operator>(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept;

		private:

			Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
		};

		class rm_iterator
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;

			using iterator_category = std::bidirectional_iterator_tag;

			inline rm_iterator(Ty* ptr, std::size_t i) noexcept : m_data_ptr(ptr), m_i(i), m_j(0) {}

			inline Ty& operator*() const noexcept;
			inline Ty& operator->() const noexcept;

			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& operator--() noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& operator++() noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator operator--(int) noexcept;
			inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator operator++(int) noexcept;

			inline bool operator==(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;
			inline bool operator!=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;
			inline bool operator<=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;
			inline bool operator>=(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;
			inline bool operator<(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;
			inline bool operator>(const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept;

		private:

			Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
		};
	};

	// free operator

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator-(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator+(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> operator*(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator==(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator!=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator<=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator>=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator<(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline bool operator>(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept;

	// free functions

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded,
		std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
	inline Ty dot(const cool::const_matrix_interface<Ty, _rows, _cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& lhs,
		const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty trace(const cool::const_matrix_interface<Ty, _rows, _rows, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows, std::size_t _cols,
		std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty
	>
	inline cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> transp(
		const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows, std::size_t _cols,
		std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty
	>
	inline cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> adj(
		const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
		std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> vxv(
		const cool::const_matrix_interface<Ty, 3, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
		const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
		std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> vxv(
		const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
		const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
		std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> qxq(
		const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
		const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _x_rows_padded, std::size_t _x_align, class _x_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> qconj(
		const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _x_rows_padded, std::size_t _x_align, class _x_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> qinv(
		const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x) noexcept;

	template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty det(const cool::const_matrix_interface<Ty, _dim, _dim, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty det(const cool::const_matrix_interface<Ty, 1, 1, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty det(const cool::const_matrix_interface<Ty, 2, 2, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty det(const cool::const_matrix_interface<Ty, 3, 3, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline Ty det(const cool::const_matrix_interface<Ty, 4, 4, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty
	>
	inline Ty det2v(
		const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty
	>
	inline Ty det3v(
		const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty
	>
	inline Ty det4v(
		const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
		const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, _dim, _dim, _opt_res_rows_padded, _opt_res_align> inv(
		const cool::const_matrix_interface<Ty, _dim, _dim, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align> inv(
		const cool::const_matrix_interface<Ty, 1, 1, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> inv(
		const cool::const_matrix_interface<Ty, 2, 2, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> inv(
		const cool::const_matrix_interface<Ty, 3, 3, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> inv(
		const cool::const_matrix_interface<Ty, 4, 4, _rows_padded, _align, _matrix_data_Ty>& A) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> inv2v(
		const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> inv3v(
		const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> inv4v(
		const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
		const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3) noexcept;

	template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _dim, std::size_t _Y_cols,
		std::size_t _A_rows_padded, std::size_t _Y_rows_padded, std::size_t _A_align, std::size_t _Y_align, class _A_matrix_data_Ty, class _Y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, _dim, _Y_cols, _opt_res_rows_padded, _opt_res_align> solve(
		const cool::const_matrix_interface<Ty, _dim, _dim, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
		const cool::const_matrix_interface<Ty, _dim, _Y_cols, _Y_rows_padded, _Y_align, _Y_matrix_data_Ty>& Y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _A_rows_padded,
		std::size_t _y_rows_padded, std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align> solve(
		const cool::const_matrix_interface<Ty, 1, 1, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
		const cool::const_matrix_interface<Ty, 1, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _A_rows_padded,
		std::size_t _y_rows_padded, std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> solve(
		const cool::const_matrix_interface<Ty, 2, 2, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
		const cool::const_matrix_interface<Ty, 2, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
		std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> solve(
		const cool::const_matrix_interface<Ty, 3, 3, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
		const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
		std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> solve(
		const cool::const_matrix_interface<Ty, 4, 4, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
		const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _y_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _y_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> solve2v(
		const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 2, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _y_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _y_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> solve3v(
		const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
		const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty,
		std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded, std::size_t _y_rows_padded,
		std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align, std::size_t _y_align,
		class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> solve4v(
		const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
		const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
		const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
		const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3,
		const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept;

	template <class params_Ty, class Ty, std::size_t _dim,
		std::size_t _fx_rows_padded, std::size_t _J_rows_padded, std::size_t _xref_rows_padded, std::size_t _xarg_rows_padded, std::size_t _y_rows_padded,
		std::size_t _fx_align, std::size_t _J_align, std::size_t _xref_align, std::size_t _xarg_align, std::size_t _y_align,
		class _x_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline int nonlinear_solve(
		cool::matrix<Ty, _dim, 1, _fx_rows_padded, _fx_align>(*fn)(const cool::matrix<Ty, _dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
		cool::matrix<Ty, _dim, _dim, _J_rows_padded, _J_align>(*Jfn)(const cool::matrix<Ty, _dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
		cool::matrix_interface<Ty, _dim, 1, _xref_rows_padded, _xref_align, _x_matrix_data_Ty>& xref,
		const cool::const_matrix_interface<Ty, _dim, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y,
		Ty tol, int iter, params_Ty params) noexcept;

	template <class params_Ty, class Ty, std::size_t dim,
		std::size_t _fx_rows_padded, std::size_t _Jinv_rows_padded, std::size_t _xref_rows_padded, std::size_t _xarg_rows_padded, std::size_t _y_rows_padded,
		std::size_t _fx_align, std::size_t _Jinv_align, std::size_t _xref_align, std::size_t _xarg_align, std::size_t _y_align,
		class _Jinv_matrix_data_Ty, class _x_matrix_data_Ty, class _y_matrix_data_Ty
	>
	inline int nonlinear_qn_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded, _fx_align>(*fn)(const cool::matrix<Ty, dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
		cool::matrix_interface<Ty, dim, dim, _Jinv_rows_padded, _Jinv_align, _Jinv_matrix_data_Ty>& Jinvref,
		cool::matrix_interface<Ty, dim, 1, _xref_rows_padded, _xref_align, _x_matrix_data_Ty>& xref,
		const cool::const_matrix_interface<Ty, dim, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y,
		Ty tol, int iter, params_Ty params) noexcept;

	// lu_matrix

	template <class Ty, std::size_t _dim, std::size_t _rows_padded = _dim, std::size_t _align = cool::matrix_align_spec<Ty, _dim, _dim, _rows_padded>::value> class lu_matrix : public cool::matrix<Ty, _dim, _dim, _rows_padded, _align>
	{

	public:

		lu_matrix() noexcept = default;
		lu_matrix(const cool::lu_matrix<Ty, _dim, _rows_padded, _align>& rhs) noexcept = default;
		cool::lu_matrix<Ty, _dim, _rows_padded, _align>& operator=(const cool::lu_matrix<Ty, _dim, _rows_padded, _align>& rhs) noexcept = default;
		lu_matrix(cool::lu_matrix<Ty, _dim, _rows_padded, _align>&& rhs) noexcept = default;
		cool::lu_matrix<Ty, _dim, _rows_padded, _align>& operator=(cool::lu_matrix<Ty, _dim, _rows_padded, _align>&& rhs) noexcept = default;
		~lu_matrix() = default;

		template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline lu_matrix(const cool::const_matrix_interface<Ty, _dim, _dim, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) noexcept : cool::matrix<Ty, _dim, _dim, _rows_padded, _align>(rhs) {}

		inline Ty det() const noexcept;

		inline Ty diag_ratio() const noexcept;

		template <std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
		inline cool::matrix<Ty, _dim, _rhs_cols, _rows_padded, _align> solve(const cool::const_matrix_interface<Ty, _dim, _rhs_cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& Y) const noexcept;

		inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> inv() const noexcept;

		inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> lower() const noexcept;
		inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> upper() const noexcept;

		std::size_t perm[_dim];
		bool flipped;
	};

	template <std::size_t _opt_res_rows_padded = 0, std::size_t _opt_res_align = 0, class Ty, std::size_t _dim, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _matrix_data_Ty>
	inline cool::lu_matrix<Ty, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value, cool::_opt_dim<_opt_res_align, cool::matrix_align_spec<Ty, _dim, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value>::value>::value> lu(
		const cool::const_matrix_interface<Ty, _dim, _dim, _rhs_rows_padded, _rhs_align, _matrix_data_Ty>& rhs) noexcept;
}


// detail

// matrix_multiply_kernel_spec

template <class Ty, std::size_t _lhs_rows, std::size_t _rhs_cols, std::size_t _lhs_cols>
constexpr std::size_t cool::matrix_multiply_kernel_spec<Ty, _lhs_rows, _rhs_cols, _lhs_cols>::compute_packed_param() noexcept
{
	constexpr std::size_t packed_adjusted = (cool::matrix_type_multiply_kernel_spec<Ty>::packed == 0) ?
		_default_kernel_ps[0] : cool::matrix_type_multiply_kernel_spec<Ty>::packed;

	constexpr std::size_t broadcasted_adjusted = (cool::matrix_type_multiply_kernel_spec<Ty>::broadcasted == 0) ?
		_default_kernel_ps[1] : cool::matrix_type_multiply_kernel_spec<Ty>::broadcasted;
	constexpr std::size_t mv_packed_factor = (broadcasted_adjusted / 2 != 0) ? (broadcasted_adjusted / 2) : 1;

	if (cool::matrix_type_multiply_kernel_spec<Ty>::packed == 0)
	{
		constexpr std::size_t packed_ps = (_rhs_cols != 1) ? packed_adjusted
			: packed_adjusted * mv_packed_factor;
		constexpr std::size_t packed_unchecked = (packed_ps * sizeof(float)) / sizeof(Ty);
		return (packed_unchecked != 0) ? packed_unchecked : 1;
	}
	else
	{
		return (_rhs_cols != 1) ? packed_adjusted : packed_adjusted * mv_packed_factor;
	}
}

template <class Ty, std::size_t _lhs_rows, std::size_t _rhs_cols, std::size_t _lhs_cols>
constexpr std::size_t cool::matrix_multiply_kernel_spec<Ty, _lhs_rows, _rhs_cols, _lhs_cols>::compute_broadcasted_param() noexcept
{
	return (cool::matrix_type_multiply_kernel_spec<Ty>::broadcasted == 0) ?
		_default_kernel_ps[1] : cool::matrix_type_multiply_kernel_spec<Ty>::broadcasted;
}

template <class Ty, std::size_t _lhs_rows, std::size_t _rhs_cols, std::size_t _lhs_cols>
constexpr std::size_t cool::matrix_multiply_kernel_spec<Ty, _lhs_rows, _rhs_cols, _lhs_cols>::compute_repeat_threshold_param() noexcept
{
	return (cool::matrix_type_multiply_kernel_spec<Ty>::repeat_threshold == 0) ?
		_default_kernel_ps[2] : cool::matrix_type_multiply_kernel_spec<Ty>::repeat_threshold;
}

// matrix_align_spec

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline constexpr std::size_t cool::matrix_align_spec<Ty, _rows, _cols, _rows_padded>::compute_value() noexcept
{
	constexpr std::size_t _multiple_count = sizeof(_default_multiples) / sizeof(std::size_t);

	for (std::size_t n = _multiple_count; n > 0; )
	{
		n--;
		if (_default_multiples[n] != 0)
		{
			if (((_rows_padded * sizeof(Ty)) % _default_multiples[n] == 0)
				&& (_default_multiples[n] % alignof(Ty) == 0))
			{
				return _default_multiples[n];
			}
		}
	}

	return 0;
}

// matrix_scalar_subroutine

template <class Ty> inline Ty cool::matrix_scalar_subroutine::conj(const Ty& val) noexcept { return val; }

template <class Ty> inline std::complex<Ty> cool::matrix_scalar_subroutine::conj(const std::complex<Ty>& val) noexcept { return std::conj(val); }

template <class Ty> inline Ty cool::matrix_scalar_subroutine::abs_sq(const Ty& val) noexcept { return val * cool::matrix_scalar_subroutine::conj(val); }

template <class Ty> inline Ty cool::matrix_scalar_subroutine::abs_sq(const std::complex<Ty>& val) noexcept
{
	Ty re = val.real();
	Ty im = val.imag();

	return re * re + im * im;
}

// _matrix_ptr

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::_matrix_ptr(Ty* ptr) noexcept : m_data_ptr(ptr) {}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::operator=(
	const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = m_data_ptr;
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = *(rhs_ptr + i + _rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
template <std::size_t _rhs_rows, std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align>
inline cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::operator=(
	const cool::_matrix_ptr<Ty, _rhs_rows, _rhs_cols, _rhs_rows_padded, _rhs_align>& rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	Ty* res_ptr = m_data_ptr;
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = *(rhs_ptr + i + _rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline Ty* cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::data() noexcept
{
	return m_data_ptr;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline const Ty* cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::data() const noexcept
{
	return m_data_ptr;
}

// _const_matrix_ptr

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::_const_matrix_ptr(const Ty* ptr) noexcept : m_data_ptr(ptr) {}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::_const_matrix_ptr(const cool::_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>& rhs) noexcept
	: m_data_ptr(rhs.data()) {}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline const Ty* cool::_const_matrix_ptr<Ty, _rows, _cols, _rows_padded, _align>::data() const noexcept
{
	return m_data_ptr;
}

// _matrix_array

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(const cool::_matrix_array<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs)
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(const cool::_const_matrix_ptr<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs)
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(const cool::_matrix_ptr<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align>& rhs)
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(Ty rhs)
{
	constexpr std::size_t _size_padded = _rows_padded * _cols;

	Ty* res_ptr = this->data();

	for (std::size_t n = 0; n < _size_padded; n++)
	{
		*(res_ptr + n) = rhs;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
constexpr inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(std::initializer_list<Ty> rhs) : m_data{}
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* rhs_ptr = rhs.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *(rhs_ptr + i + _rows * j);
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(cool::cm<Ty> rhs)
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* rhs_ptr = rhs.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *(rhs_ptr + i + _rows * j);
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::_matrix_array(cool::rm<Ty> rhs)
{
	const Ty* rhs_ptr = rhs.begin();

	for (std::size_t i = 0; i < _rows; i++)
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			m_data[i + _rows_padded * j] = *rhs_ptr++;
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline Ty* cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::data() noexcept
{
	return static_cast<Ty*>(m_data);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align>
inline const Ty* cool::_matrix_array<Ty, _rows, _cols, _rows_padded, _align>::data() const noexcept
{
	return static_cast<const Ty*>(m_data);
}

// const_matrix_interface

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
constexpr inline std::size_t cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::size() const noexcept
{
	return _rows * _cols;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
constexpr inline std::size_t cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::size_padded() const noexcept
{
	return _rows_padded * _cols;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty1, class uint_Ty2>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator()(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return *(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator[](uint_Ty mem_offset) const noexcept
{
	return *(this->data() + static_cast<std::size_t>(mem_offset));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty> inline cool::const_matrix_span<Ty, _rows, 1, _rows, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::col(uint_Ty j) const noexcept
{
	return cool::const_matrix_span<Ty, _rows, 1, _rows, 0>(this->data() + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty> inline cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::row(uint_Ty i) const noexcept
{
	return cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::diag() const noexcept
{
	return cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1, class uint_Ty2>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i) const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk() const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align>
cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator-() const noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = -*(ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = -*(ptr + i + _rows_padded * j);
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator+(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(lhs_ptr + n) + *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = *(lhs_ptr + i + _rows_padded * j) + *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator-(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(lhs_ptr + n) - *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = *(lhs_ptr + i + _rows_padded * j) - *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align>
cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator+(Ty rhs) const noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(lhs_ptr + n) + rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = *(lhs_ptr + i + _rows_padded * j) + rhs;
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align>
cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator-(Ty rhs) const noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(lhs_ptr + n) - rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = *(lhs_ptr + i + _rows_padded * j) - rhs;
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _res_rows_padded, std::size_t _res_align, std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix<Ty, _rows, _rhs_cols, _res_rows_padded, _res_align> cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator*(
	const cool::const_matrix_interface<Ty, _cols, _rhs_cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::broadcasted;
	constexpr bool _lhs_cols_small = _cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::repeat_threshold;

	constexpr std::size_t _lhs_rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _rhs_cols_mult_j_size = _rhs_cols - _rhs_cols % j_size;
	constexpr std::size_t j_remainder = _rhs_cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	cool::matrix<Ty, _rows, _rhs_cols, _res_rows_padded, _res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_lhs_cols_small)
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty* ptr = res_ptr + i + _res_rows_padded * (j + m);
					for (std::size_t n = 0; n < i_size; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty* ptr = res_ptr + i + _res_rows_padded * (_rhs_cols_mult_j_size + m);
					for (std::size_t n = 0; n < i_size; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _res_rows_padded * (j + m);
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _res_rows_padded * (_rhs_cols_mult_j_size + m);
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size] = { {} };

#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty* ptr = res_ptr + i + _res_rows_padded * (j + m);
					for (std::size_t n = 0; n < i_size; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size] = { {} };

#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty* ptr = res_ptr + i + _res_rows_padded * (_rhs_cols_mult_j_size + m);
					for (std::size_t n = 0; n < i_size; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _res_rows_padded * (j + m);
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _res_rows_padded * (_rhs_cols_mult_j_size + m);
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						*(ptr + n) = acc[m][n];
					}
				}
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator==(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) == *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) == *(rhs_ptr + size_mult8 + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) == *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) == *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) == *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) == *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator!=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (*(lhs_ptr + i + k) == *(rhs_ptr + i + k));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (*(lhs_ptr + size_mult8 + k) == *(rhs_ptr + size_mult8 + k));
		}
		return !not_ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (*(lhs_ptr + _rows_padded * (i + k)) == *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) == *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return !not_ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool not_ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					not_ret &= (*(lhs_ptr + i + k + _rows_padded * j) == *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!not_ret)
				{
					return true;
				}
			}

			bool not_ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				not_ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) == *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		return false;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator<=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) <= *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) <= *(rhs_ptr + size_mult8 + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) <= *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) <= *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) <= *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) <= *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator>=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) >= *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) >= *(rhs_ptr + size_mult8 + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) >= *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) >= *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) >= *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) >= *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator<(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) < *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) < *(rhs_ptr + size_mult8 + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) < *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) < *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) < *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) < *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator>(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) > *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) > *(rhs_ptr + size_mult8 + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) > *(rhs_ptr + _rhs_rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) > *(rhs_ptr + _rhs_rows_padded * (size_mult8 + k)));
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) > *(rhs_ptr + i + k + _rhs_rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) > *(rhs_ptr + size_mult8 + k + _rhs_rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator==(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) == rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) == rhs);
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) == rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator!=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (*(lhs_ptr + i + k) == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (*(lhs_ptr + size_mult8 + k) == rhs);
		}
		return !not_ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (*(lhs_ptr + _rows_padded * (i + k)) == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) == rhs);
		}
		return !not_ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool not_ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					not_ret &= (*(lhs_ptr + i + k + _rows_padded * j) == rhs);
				}
				if (!not_ret)
				{
					return true;
				}
			}

			bool not_ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				not_ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		return false;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator<=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) <= rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) <= rhs);
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) <= rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator>=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) >= rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) >= rhs);
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) >= rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator<(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) < rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) < rhs);
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) < rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator>(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	const Ty* lhs_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + i + k) > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + size_mult8 + k) > rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		for (std::size_t i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (*(lhs_ptr + _rows_padded * (i + k)) > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (*(lhs_ptr + _rows_padded * (size_mult8 + k)) > rhs);
		}
		return ret;
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			constexpr std::size_t _size = _rows;
			constexpr std::size_t size_mult8 = _size - _size % 8;
			constexpr std::size_t size_mod8 = _size % 8;

			for (std::size_t i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (*(lhs_ptr + i + k + _rows_padded * j) > rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (*(lhs_ptr + size_mult8 + k + _rows_padded * j) > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::begin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::end() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), _cols);
}

// const_matrix_interface<...>::cm_const_iterator_proxy

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy::begin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy::end() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, _cols);
}

// const_matrix_interface<...>::rm_const_iterator_proxy

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy::begin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy::end() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, _rows);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, _rows);
}

// const_matrix_interface<...>::cm_const_iterator

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::cm_const_iterator(const Ty* ptr, std::size_t j) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	m_data_ptr = ptr;

	if (_contiguous)
	{
		m_i = _rows * j;
		m_j = 0;
	}
	else
	{
		m_i = 0;
		m_j = j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::cm_const_iterator::operator*() const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return *(m_data_ptr + m_i);
	}
	else
	{
		return *(m_data_ptr + m_i + _rows_padded * m_j);
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator->() const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_data_ptr + m_i;
	}
	else
	{
		return m_data_ptr + m_i + _rows_padded * m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator&
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator--() noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	if (_not_contiguous)
	{
		if (m_i == 0)
		{
			m_i = _rows;
			m_j--;
		}
	}
	m_i--;
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator&
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator++() noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	m_i++;
	if (_not_contiguous)
	{
		if (m_i == _rows)
		{
			m_i = 0;
			m_j++;
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator--(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator temp = *this;
	if (_not_contiguous)
	{
		if (m_i == 0)
		{
			m_i = _rows;
			m_j--;
		}
	}
	m_i--;
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator++(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator temp = *this;
	m_i++;
	if (_not_contiguous)
	{
		if (m_i == _rows)
		{
			m_i = 0;
			m_j++;
		}
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator==(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i == rhs.m_i;
	}
	else
	{
		return m_i == rhs.m_i && m_j == rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator!=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i != rhs.m_i;
	}
	else
	{
		return !(m_i == rhs.m_i && m_j == rhs.m_j);
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator<=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i <= rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j <= rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator>=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i >= rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j >= rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator<(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i < rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j < rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator::operator>(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i > rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j > rhs.m_i + _rows * rhs.m_j;
	}
}

// const_matrix_interface<...>::rm_const_iterator

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator*() const noexcept
{
	return *(m_data_ptr + m_i + _rows_padded * m_j);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline const Ty& cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator->() const noexcept
{
	return m_data_ptr + m_i + _rows_padded * m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator&
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator--() noexcept
{
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator&
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator++() noexcept
{
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator--(int) noexcept
{
	typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator temp = *this;
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator++(int) noexcept
{
	typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator temp = *this;
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator==(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return m_i == rhs.m_i && m_j == rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator!=(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return !(m_i == rhs.m_i && m_j == rhs.m_j);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator<=(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j <= _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator>=(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j >= _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator<(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j < _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator::operator>(
	const typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j > _cols * rhs.m_i + rhs.m_j;
}

// matrix_interface

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::matrix_interface(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs)
	: cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>(rhs) {}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs)
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator=(Ty rhs)
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = rhs;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator=(std::initializer_list<Ty> rhs)
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *rhs_ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = *rhs_ptr++;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator=(cool::cm<Ty> rhs)
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = *rhs_ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) = *rhs_ptr++;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator=(cool::rm<Ty> rhs)
{
	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.begin();

	for (std::size_t i = 0; i < _rows; i++)
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			*(res_ptr + i + _rows_padded * j) = *rhs_ptr++;
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty1, class uint_Ty2> inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator()(uint_Ty1 i, uint_Ty2 j) noexcept
{
	return *(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty1, class uint_Ty2> inline const Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator()(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return *(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty> inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator[](uint_Ty mem_offset) noexcept
{
	return *(this->data() + static_cast<std::size_t>(mem_offset));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <class uint_Ty> inline const Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator[](uint_Ty mem_offset) const noexcept
{
	return *(this->data() + static_cast<std::size_t>(mem_offset));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty>
inline cool::matrix_span<Ty, _rows, 1, _rows, 0>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::col(uint_Ty j) noexcept
{
	return cool::matrix_span<Ty, _rows, 1, _rows, 0>(this->data() + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty>
inline cool::const_matrix_span<Ty, _rows, 1, _rows, 0>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::col(uint_Ty j) const noexcept
{
	return cool::const_matrix_span<Ty, _rows, 1, _rows, 0>(this->data() + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty>
inline cool::matrix_span<Ty, 1, _cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::row(uint_Ty i) noexcept
{
	return cool::matrix_span<Ty, 1, _cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty> template <class uint_Ty>
inline cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::row(uint_Ty i) const noexcept
{
	return cool::const_matrix_span<Ty, 1, _cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::diag() noexcept
{
	return cool::matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::diag() const noexcept
{
	return cool::const_matrix_span<Ty, 1, cool::_diag_length<_rows, _cols>::value, _rows_padded + 1, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1, class uint_Ty2>
inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i, uint_Ty2 j) noexcept
{
	return cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1, class uint_Ty2>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1>
inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i) noexcept
{
	return cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk(uint_Ty1 i) const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data() + static_cast<std::size_t>(i));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols>
inline cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk() noexcept
{
	return cool::matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _blk_rows, std::size_t _blk_cols>
inline cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0> cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::blk() const noexcept
{
	return cool::const_matrix_span<Ty, _blk_rows, _blk_cols, _rows_padded, 0>(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator+=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) += *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) += *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator-=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) -= *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) -= *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator*=(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) *= *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) *= *(rhs_ptr + i + _rhs_rows_padded * j);
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator+=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) += rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) += rhs;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator-=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) -= rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) -= rhs;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::operator*=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	Ty* res_ptr = this->data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) *= rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _rows_padded * j) *= rhs;
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <int _sign, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::fmadd(Ty s,
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1, "sign template argument requirement : value must be 1 or -1");

	Ty* res_ptr = this->data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		if (_positive)
		{
			for (std::size_t n = 0; n < _size; n++)
			{
				*(res_ptr + n) += s * *(rhs_ptr + n);
			}
		}
		else
		{
			for (std::size_t n = 0; n < _size; n++)
			{
				*(res_ptr + n) -= s * *(rhs_ptr + n);
			}
		}
	}
	else
	{
		if (_positive)
		{
			for (std::size_t j = 0; j < _cols; j++)
			{
				for (std::size_t i = 0; i < _rows; i++)
				{
					*(res_ptr + i + _rows_padded * j) += s * *(rhs_ptr + i + _rhs_rows_padded * j);
				}
			}
		}
		else
		{
			for (std::size_t j = 0; j < _cols; j++)
			{
				for (std::size_t i = 0; i < _rows; i++)
				{
					*(res_ptr + i + _rows_padded * j) -= s * *(rhs_ptr + i + _rhs_rows_padded * j);
				}
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <int _sign, std::size_t _lhs_cols, std::size_t _rhs_rows_padded, std::size_t _lhs_rows_padded, std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::fmadd(
	const cool::const_matrix_interface<Ty, _rows, _lhs_cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT lhs,
	const cool::const_matrix_interface<Ty, _lhs_cols, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::broadcasted;
	constexpr bool _lhs_cols_small = _lhs_cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::repeat_threshold;

	constexpr std::size_t _lhs_rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _rhs_cols_mult_j_size = _cols - _cols % j_size;
	constexpr std::size_t j_remainder = _cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1, "sign template argument requirement : value must be 1 or -1");

	const Ty* lhs_ptr = lhs.data();
	const Ty* rhs_ptr = rhs.data();
	Ty* res_ptr = this->data();

	if (_lhs_cols_small)
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size] = { {} };
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size] = { {} };
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= acc[m][n];
						}
					}
				}
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
template <int _sign, std::size_t _lhs_cols, std::size_t _rhs_rows_padded, std::size_t _lhs_rows_padded, std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::fmadd(Ty s,
	const cool::const_matrix_interface<Ty, _rows, _lhs_cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT lhs,
	const cool::const_matrix_interface<Ty, _lhs_cols, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::broadcasted;
	constexpr bool _lhs_cols_small = _lhs_cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::repeat_threshold;

	constexpr std::size_t _lhs_rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _rhs_cols_mult_j_size = _cols - _cols % j_size;
	constexpr std::size_t j_remainder = _cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1, "sign template argument requirement : value must be 1 or -1");

	const Ty* lhs_ptr = lhs.data();
	const Ty* rhs_ptr = rhs.data();
	Ty* res_ptr = this->data();

	if (_lhs_cols_small)
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + i;
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (j + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = *(rhs_ptr + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
					const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size;
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = *(ptr + n) * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _lhs_rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size] = { {} };
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size] = { {} };
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + i + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + i + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_size; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (j + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (j + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = *(rhs_ptr + k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m));
						const Ty* ptr = lhs_ptr + _lhs_rows_mult_i_size + _lhs_rows_padded * k;
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += *(ptr + n) * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty* ptr = res_ptr + _lhs_rows_mult_i_size + _rows_padded * (_rhs_cols_mult_j_size + m);
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							*(ptr + n) -= s * acc[m][n];
						}
					}
				}
			}
		}
	}

	return *this;
}

// matrix_interface iterator_proxy/begin/end

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm() noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm() noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator_proxy(this->data());
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::begin() noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator(this->data(), 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::end() noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator(this->data(), _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::begin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::end() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(this->data(), _cols);
}

// matrix_interface<...>::cm_iterator_proxy

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy::begin() const noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy::end() const noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator(m_data_ptr, _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator_proxy::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_const_iterator(m_data_ptr, _cols);
}

// matrix_interface<...>::rm_iterator_proxy

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy::begin() const noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy::end() const noexcept
{
	return typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator(m_data_ptr, _rows);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy::cbegin() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, 0);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator_proxy::cend() const noexcept
{
	return typename cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_const_iterator(m_data_ptr, _rows);
}

// matrix_interface<...>::cm_iterator

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::cm_iterator(Ty* ptr, std::size_t j) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	m_data_ptr = ptr;

	if (_contiguous)
	{
		m_i = _rows * j;
		m_j = 0;
	}
	else
	{
		m_i = 0;
		m_j = j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::cm_iterator::operator*() const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return *(m_data_ptr + m_i);
	}
	else
	{
		return *(m_data_ptr + m_i + _rows_padded * m_j);
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator->() const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_data_ptr + m_i;
	}
	else
	{
		return m_data_ptr + m_i + _rows_padded * m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator--() noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	if (_not_contiguous)
	{
		if (m_i == 0)
		{
			m_i = _rows;
			m_j--;
		}
	}
	m_i--;
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator++() noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	m_i++;
	if (_not_contiguous)
	{
		if (m_i == _rows)
		{
			m_i = 0;
			m_j++;
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator--(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator temp = *this;
	if (_not_contiguous)
	{
		if (m_i == 0)
		{
			m_i = _rows;
			m_j--;
		}
	}
	m_i--;
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator++(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator temp = *this;
	m_i++;
	if (_not_contiguous)
	{
		if (m_i == _rows)
		{
			m_i = 0;
			m_j++;
		}
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator==(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i == rhs.m_i;
	}
	else
	{
		return m_i == rhs.m_i && m_j == rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator!=(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i != rhs.m_i;
	}
	else
	{
		return !(m_i == rhs.m_i && m_j == rhs.m_j);
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator<=(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i <= rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j <= rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator>=(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i >= rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j >= rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator<(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i < rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j < rhs.m_i + _rows * rhs.m_j;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator::operator>(
	const cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::cm_iterator& rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		return m_i > rhs.m_i;
	}
	else
	{
		return m_i + _rows * m_j > rhs.m_i + _rows * rhs.m_j;
	}
}

// matrix_interface<...>::rm_iterator

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator*() const noexcept
{
	return *(m_data_ptr + m_i + _rows_padded * m_j);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty& cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator->() const noexcept
{
	return m_data_ptr + m_i + _rows_padded * m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator--() noexcept
{
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator&
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator++() noexcept
{
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator--(int) noexcept
{
	typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator temp = *this;
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator
cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator++(int) noexcept
{
	typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator temp = *this;
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator==(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return m_i == rhs.m_i && m_j == rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator!=(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return !(m_i == rhs.m_i && m_j == rhs.m_j);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator<=(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j <= _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator>=(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j >= _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator<(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j < _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator::operator>(
	const typename cool::matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j > _cols * rhs.m_i + rhs.m_j;
}

// free operators

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::operator-(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = lhs - *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = lhs - *(rhs_ptr + i + _rows_padded * j);
			}
		}
	}

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::operator+(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = lhs + *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = lhs + *(rhs_ptr + i + _rows_padded * j);
			}
		}
	}

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> cool::operator*(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _rows>::value;
	constexpr bool _contiguous = ((_rows == _res_rows_padded) && (_rows == _rows_padded)) || (_cols == 1);

	cool::matrix_result<Ty, _rows, _cols, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* rhs_ptr = rhs.data();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			*(res_ptr + n) = lhs * *(rhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*(res_ptr + i + _res_rows_padded * j) = lhs * *(rhs_ptr + i + _rows_padded * j);
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator==(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs == lhs;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator!=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs != lhs;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator<=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs >= lhs;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator>=(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs <= lhs;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator<(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs > lhs;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline bool cool::operator>(Ty lhs, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
{
	return rhs < lhs;
}

// free functions

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded,
	std::size_t _lhs_align, std::size_t _rhs_align, class _lhs_matrix_data_Ty, class _rhs_matrix_data_Ty>
inline Ty cool::dot(const cool::const_matrix_interface<Ty, _rows, _cols, _lhs_rows_padded, _lhs_align, _lhs_matrix_data_Ty>& lhs,
	const cool::const_matrix_interface<Ty, _rows, _cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& rhs) noexcept
{
	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;

	if (no_conj_modif)
	{
		constexpr bool _contiguous = ((_rows == _lhs_rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

		if (_contiguous)
		{
			constexpr std::size_t _size = _rows * _cols;
			constexpr bool _size_lt_2 = _size < 2;
			constexpr bool _size_lt_8 = _size < 8;

			if (_size_lt_2)
			{
				return lhs[0] * rhs[0];
			}
			else if (_size_lt_8)
			{
				constexpr std::size_t _size_mult2 = _size - _size % 2;
				constexpr bool _size_odd = _size % 2 != 0;

				const Ty* lhs_ptr = lhs.data();
				const Ty* rhs_ptr = rhs.data();

				Ty acc0 = *lhs_ptr * *rhs_ptr;
				Ty acc1 = *(lhs_ptr + 1) * *(rhs_ptr + 1);

				for (std::size_t n = 2; n < _size_mult2; n += 2)
				{
					acc0 += *(lhs_ptr + n) * *(rhs_ptr + n);
					acc1 += *(lhs_ptr + n + 1) * *(rhs_ptr + n + 1);
				}
				if (_size_odd)
				{
					acc0 += *(lhs_ptr + _size_mult2) * *(rhs_ptr + _size_mult2);
				}

				return acc0 + acc1;
			}
			else
			{
				constexpr std::size_t _size_mult4 = _size - _size % 4;
				constexpr std::size_t remainder = _size % 4;

				const Ty* lhs_ptr = lhs.data();
				const Ty* rhs_ptr = rhs.data();

				Ty acc[4] = { *lhs_ptr * *rhs_ptr,
					*(lhs_ptr + 1) * *(rhs_ptr + 1),
					*(lhs_ptr + 2) * *(rhs_ptr + 2),
					*(lhs_ptr + 3) * *(rhs_ptr + 3)
				};

				for (std::size_t n = 4; n < _size_mult4; n += 4)
				{
					for (std::size_t m = 0; m < 4; m++)
					{
						acc[m] += *(lhs_ptr + n + m) * *(rhs_ptr + n + m);
					}
				}
				for (std::size_t m = 0; m < remainder; m++)
				{
					acc[m] += *(lhs_ptr + _size_mult4 + m) * *(rhs_ptr + _size_mult4 + m);
				}

				return (acc[0] + acc[1]) + (acc[2] + acc[3]);
			}
		}
		else
		{
			Ty ret = static_cast<Ty>(0);
			const Ty* lhs_ptr = lhs.data();
			const Ty* rhs_ptr = rhs.data();
			for (std::size_t j = 0; j < _cols; j++)
			{
				for (std::size_t i = 0; i < _rows; i++)
				{
					ret += *(lhs_ptr + i + _lhs_rows_padded * j) * *(rhs_ptr + i + _rhs_rows_padded * j);
				}
			}

			return ret;
		}
	}
	else
	{
		constexpr bool _contiguous = ((_rows == _lhs_rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

		if (_contiguous)
		{
			constexpr std::size_t _size = _rows * _cols;
			constexpr bool _size_lt_2 = _size < 2;
			constexpr bool _size_lt_8 = _size < 8;

			if (_size_lt_2)
			{
				return lhs[0] * cool::matrix_scalar_subroutine::conj(rhs[0]);
			}
			else if (_size_lt_8)
			{
				constexpr std::size_t _size_mult2 = _size - _size % 2;
				constexpr bool _size_odd = _size % 2 != 0;

				const Ty* lhs_ptr = lhs.data();
				const Ty* rhs_ptr = rhs.data();

				Ty acc0 = *lhs_ptr * cool::matrix_scalar_subroutine::conj(*rhs_ptr);
				Ty acc1 = *(lhs_ptr + 1) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + 1));

				for (std::size_t n = 2; n < _size_mult2; n += 2)
				{
					acc0 += *(lhs_ptr + n) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + n));
					acc1 += *(lhs_ptr + n + 1) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + n + 1));
				}
				if (_size_odd)
				{
					acc0 += *(lhs_ptr + _size_mult2) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + _size_mult2));
				}

				return acc0 + acc1;
			}
			else
			{
				constexpr std::size_t _size_mult4 = _size - _size % 4;
				constexpr std::size_t remainder = _size % 4;

				const Ty* lhs_ptr = lhs.data();
				const Ty* rhs_ptr = rhs.data();

				Ty acc[4] = { *lhs_ptr * cool::matrix_scalar_subroutine::conj(*rhs_ptr),
					*(lhs_ptr + 1) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + 1)),
					*(lhs_ptr + 2) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + 2)),
					*(lhs_ptr + 3) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + 3))
				};

				for (std::size_t n = 4; n < _size_mult4; n += 4)
				{
					for (std::size_t m = 0; m < 4; m++)
					{
						acc[m] += *(lhs_ptr + n + m) * cool::matrix_scalar_subroutine::conj(*(rhs_ptr + n + m));
					}
				}
				for (std::size_t m = 0; m < remainder; m++)
				{
					acc[m] += *(lhs_ptr + _size_mult4 + m)
						* cool::matrix_scalar_subroutine::conj(*(rhs_ptr + _size_mult4 + m));
				}

				return (acc[0] + acc[1]) + (acc[2] + acc[3]);
			}
		}
		else
		{
			Ty ret = static_cast<Ty>(0);
			const Ty* lhs_ptr = lhs.data();
			const Ty* rhs_ptr = rhs.data();
			for (std::size_t j = 0; j < _cols; j++)
			{
				for (std::size_t i = 0; i < _rows; i++)
				{
					ret += *(lhs_ptr + i + _lhs_rows_padded * j)
						* cool::matrix_scalar_subroutine::conj(*(rhs_ptr + i + _rhs_rows_padded * j));
				}
			}

			return ret;
		}
	}
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows, std::size_t _cols,
	std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty
>
inline cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> cool::transp(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t cols_mult4 = _cols - _cols % 4;
	constexpr std::size_t cols_mod4 = _cols % 4;

	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _cols>::value;

	constexpr bool _line_matrix = ((_rows == 1) && (_rows_padded == 1)) || ((_cols == 1) && (_res_rows_padded == 1));

	cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> ret;

	if (_line_matrix)
	{
		Ty* res_ptr = ret.data();
		const Ty* ptrA = A.data();

		for (std::size_t n = 0; n < _cols; n++)
		{
			*(res_ptr + n) = *(ptrA + n);
		}
	}
	else
	{
		for (std::size_t j = 0; j < cols_mult4; j += 4)
		{
			const Ty* ptrA0 = A.data() + _rows_padded * j;
			const Ty* ptrA1 = ptrA0 + _rows_padded;
			const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
			const Ty* ptrA3 = ptrA0 + _rows_padded * 3;
			Ty* res_ptr = ret.data() + j;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
			for (std::size_t i = 0; i < _rows; i++)
			{
				Ty temp0 = *ptrA0++;
				Ty temp1 = *ptrA1++;
				Ty temp2 = *ptrA2++;
				Ty temp3 = *ptrA3++;
				*res_ptr = temp0;
				*(res_ptr + 1) = temp1;
				*(res_ptr + 2) = temp2;
				*(res_ptr + 3) = temp3;
				res_ptr += _res_rows_padded;
			}
		}

		switch (cols_mod4)
		{

		case 1:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			Ty* res_ptr = ret.data() + cols_mult4;
			for (std::size_t i = 0; i < _rows; i++)
			{
				*res_ptr = *ptrA0++;
				res_ptr += _res_rows_padded;
			}
			break;
		}

		case 2:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			const Ty* ptrA1 = ptrA0 + _rows_padded;
			Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
			for (std::size_t i = 0; i < _rows; i++)
			{
				Ty temp0 = *ptrA0++;
				Ty temp1 = *ptrA1++;
				*res_ptr = temp0;
				*(res_ptr + 1) = temp1;
				res_ptr += _res_rows_padded;
			}
			break;
		}

		case 3:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			const Ty* ptrA1 = ptrA0 + _rows_padded;
			const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
			Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
			for (std::size_t i = 0; i < _rows; i++)
			{
				Ty temp0 = *ptrA0++;
				Ty temp1 = *ptrA1++;
				Ty temp2 = *ptrA2++;
				*res_ptr = temp0;
				*(res_ptr + 1) = temp1;
				*(res_ptr + 2) = temp2;
				res_ptr += _res_rows_padded;
			}
			break;
		}

		default:
			break;
		}
	}

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows, std::size_t _cols,
	std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty
>
inline cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> cool::adj(
	const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t cols_mult4 = _cols - _cols % 4;
	constexpr std::size_t cols_mod4 = _cols % 4;

	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, _cols>::value;

	constexpr bool _line_matrix = ((_rows == 1) && (_rows_padded == 1)) || ((_cols == 1) && (_res_rows_padded == 1));

	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;

	cool::matrix_result<Ty, _cols, _rows, _opt_res_rows_padded, _opt_res_align> ret;

	if (no_conj_modif)
	{
		if (_line_matrix)
		{
			Ty* res_ptr = ret.data();
			const Ty* ptrA = A.data();

			for (std::size_t n = 0; n < _cols; n++)
			{
				*(res_ptr + n) = *(ptrA + n);
			}
		}
		else
		{
			for (std::size_t j = 0; j < cols_mult4; j += 4)
			{
				const Ty* ptrA0 = A.data() + _rows_padded * j;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				const Ty* ptrA3 = ptrA0 + _rows_padded * 3;
				Ty* res_ptr = ret.data() + j;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = *ptrA0++;
					Ty temp1 = *ptrA1++;
					Ty temp2 = *ptrA2++;
					Ty temp3 = *ptrA3++;
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					*(res_ptr + 2) = temp2;
					*(res_ptr + 3) = temp3;
					res_ptr += _res_rows_padded;
				}
			}

			switch (cols_mod4)
			{

			case 1:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				Ty* res_ptr = ret.data() + cols_mult4;
				for (std::size_t i = 0; i < _rows; i++)
				{
					*res_ptr = *ptrA0++;
					res_ptr += _res_rows_padded;
				}
				break;
			}

			case 2:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = *ptrA0++;
					Ty temp1 = *ptrA1++;
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					res_ptr += _res_rows_padded;
				}
				break;
			}

			case 3:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = *ptrA0++;
					Ty temp1 = *ptrA1++;
					Ty temp2 = *ptrA2++;
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					*(res_ptr + 2) = temp2;
					res_ptr += _res_rows_padded;
				}
				break;
			}

			default:
				break;
			}
		}
	}
	else
	{
		if (_line_matrix)
		{
			Ty* res_ptr = ret.data();
			const Ty* ptrA = A.data();

			for (std::size_t n = 0; n < _cols; n++)
			{
				*(res_ptr + n) = cool::matrix_scalar_subroutine::conj(*(ptrA + n));
			}
		}
		else
		{
			for (std::size_t j = 0; j < cols_mult4; j += 4)
			{
				const Ty* ptrA0 = A.data() + _rows_padded * j;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				const Ty* ptrA3 = ptrA0 + _rows_padded * 3;
				Ty* res_ptr = ret.data() + j;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = cool::matrix_scalar_subroutine::conj(*ptrA0++);
					Ty temp1 = cool::matrix_scalar_subroutine::conj(*ptrA1++);
					Ty temp2 = cool::matrix_scalar_subroutine::conj(*ptrA2++);
					Ty temp3 = cool::matrix_scalar_subroutine::conj(*ptrA3++);
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					*(res_ptr + 2) = temp2;
					*(res_ptr + 3) = temp3;
					res_ptr += _res_rows_padded;
				}
			}

			switch (cols_mod4)
			{

			case 1:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				Ty* res_ptr = ret.data() + cols_mult4;
				for (std::size_t i = 0; i < _rows; i++)
				{
					*res_ptr = cool::matrix_scalar_subroutine::conj(*ptrA0++);
					res_ptr += _res_rows_padded;
				}
				break;
			}

			case 2:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = cool::matrix_scalar_subroutine::conj(*ptrA0++);
					Ty temp1 = cool::matrix_scalar_subroutine::conj(*ptrA1++);
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					res_ptr += _res_rows_padded;
				}
				break;
			}

			case 3:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				Ty* res_ptr = ret.data() + cols_mult4;
#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
				for (std::size_t i = 0; i < _rows; i++)
				{
					Ty temp0 = cool::matrix_scalar_subroutine::conj(*ptrA0++);
					Ty temp1 = cool::matrix_scalar_subroutine::conj(*ptrA1++);
					Ty temp2 = cool::matrix_scalar_subroutine::conj(*ptrA2++);
					*res_ptr = temp0;
					*(res_ptr + 1) = temp1;
					*(res_ptr + 2) = temp2;
					res_ptr += _res_rows_padded;
				}
				break;
			}

			default:
				break;
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::trace(const cool::const_matrix_interface<Ty, _rows, _rows, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr bool _size_lt_2 = _rows < 2;
	constexpr bool _size_lt_8 = _rows < 8;

	constexpr std::size_t _rows_padded_p1 = _rows_padded + 1;

	if (_size_lt_2)
	{
		return A[0];
	}
	else if (_size_lt_8)
	{
		constexpr std::size_t _rows_mult2 = _rows - _rows % 2;
		constexpr bool _rows_odd = _rows % 2 != 0;

		const Ty* ptr = A.data();

		Ty acc0 = *ptr;
		Ty acc1 = *(ptr + _rows_padded_p1);

		for (std::size_t k = 2; k < _rows_mult2; k += 2)
		{
			acc0 += *(ptr + _rows_padded_p1 * k);
			acc1 += *(ptr + _rows_padded_p1 * (k + 1));
		}
		if (_rows_odd)
		{
			acc0 += *(ptr + _rows_padded_p1 * _rows_mult2);
		}

		return acc0 + acc1;
	}
	else
	{
		constexpr std::size_t _rows_mult4 = _rows - _rows % 4;
		constexpr std::size_t remainder = _rows % 4;

		const Ty* ptr = A.data();

		Ty acc[4] = { *ptr,
			*(ptr + _rows_padded_p1),
			*(ptr + _rows_padded_p1 * 2),
			*(ptr + _rows_padded_p1 * 3)
		};

		for (std::size_t n = 4; n < _rows_mult4; n += 4)
		{
			for (std::size_t m = 0; m < 4; m++)
			{
				acc[m] += *(ptr + _rows_padded_p1 * (n + m));
			}
		}
		for (std::size_t m = 0; m < remainder; m++)
		{
			acc[m] += *(ptr + _rows_padded_p1 * (_rows_mult4 + m));
		}

		return (acc[0] + acc[1]) + (acc[2] + acc[3]);
	}
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
	std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> cool::vxv(
	const cool::const_matrix_interface<Ty, 3, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
	const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();

	*(res_ptr + 0) = *(xptr + 1) * *(yptr + 2) - *(xptr + 2) * *(yptr + 1);
	*(res_ptr + 1) = *(xptr + 2) * *(yptr + 0) - *(xptr + 0) * *(yptr + 2);
	*(res_ptr + 2) = *(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
	std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::vxv(
	const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
	const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();

	*(res_ptr + 0) = *(xptr + 1) * *(yptr + 2) - *(xptr + 2) * *(yptr + 1);
	*(res_ptr + 1) = *(xptr + 2) * *(yptr + 0) - *(xptr + 0) * *(yptr + 2);
	*(res_ptr + 2) = *(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0);
	*(res_ptr + 3) = static_cast<Ty>(1);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded,
	std::size_t _x_align, std::size_t _y_align, class _x_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::qxq(
	const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x,
	const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();

	Ty x0 = *(xptr + 0);
	Ty x1 = *(xptr + 1);
	Ty x2 = *(xptr + 2);
	Ty x3 = *(xptr + 3);

	*(res_ptr + 0) = x0 * *(yptr + 0) - x1 * *(yptr + 1) - x2 * *(yptr + 2) - x3 * *(yptr + 3);
	*(res_ptr + 1) = x0 * *(yptr + 1) + x1 * *(yptr + 0) - x2 * *(yptr + 3) + x3 * *(yptr + 2);
	*(res_ptr + 2) = x0 * *(yptr + 2) + x1 * *(yptr + 3) + x2 * *(yptr + 0) - x3 * *(yptr + 1);
	*(res_ptr + 3) = x0 * *(yptr + 3) - x1 * *(yptr + 2) + x2 * *(yptr + 1) + x3 * *(yptr + 0);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _x_rows_padded, std::size_t _x_align, class _x_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::qconj(
	const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x) noexcept
{
	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* xptr = x.data();

	*(res_ptr + 0) = *(xptr + 0);
	*(res_ptr + 1) = -*(xptr + 1);
	*(res_ptr + 2) = -*(xptr + 2);
	*(res_ptr + 3) = -*(xptr + 3);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _x_rows_padded, std::size_t _x_align, class _x_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::qinv(
	const cool::const_matrix_interface<Ty, 4, 1, _x_rows_padded, _x_align, _x_matrix_data_Ty>& x) noexcept
{
	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* xptr = x.data();

	Ty x0 = *(xptr + 0);
	Ty x1 = *(xptr + 1);
	Ty x2 = *(xptr + 2);
	Ty x3 = *(xptr + 3);

	Ty d = static_cast<Ty>(1) / ((x0 * x0 + x2 * x2) + (x1 * x1 + x3 * x3));

	*(res_ptr + 0) = d * x0;
	*(res_ptr + 1) = -d * x1;
	*(res_ptr + 2) = -d * x2;
	*(res_ptr + 3) = -d * x3;

	return ret;
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::det(const cool::const_matrix_interface<Ty, _dim, _dim, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	return cool::lu(A).det();
}

template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::det(const cool::const_matrix_interface<Ty, 1, 1, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	return A[0];
}

template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::det(const cool::const_matrix_interface<Ty, 2, 2, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	const Ty* ptr = A.data();

	return *(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01);
}

template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::det(const cool::const_matrix_interface<Ty, 3, 3, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i20 = 2 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	constexpr std::size_t i21 = 2 + _rows_padded * 1;
	constexpr std::size_t i02 = 0 + _rows_padded * 2;
	constexpr std::size_t i12 = 1 + _rows_padded * 2;
	constexpr std::size_t i22 = 2 + _rows_padded * 2;
	const Ty* ptr = A.data();

	return *(ptr + i00) * (*(ptr + i11) * *(ptr + i22) - *(ptr + i21) * *(ptr + i12))
		+ *(ptr + i10) * (*(ptr + i21) * *(ptr + i02) - *(ptr + i01) * *(ptr + i22))
		+ *(ptr + i20) * (*(ptr + i01) * *(ptr + i12) - *(ptr + i11) * *(ptr + i02));
}

template <class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline Ty cool::det(const cool::const_matrix_interface<Ty, 4, 4, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i20 = 2 + _rows_padded * 0;
	constexpr std::size_t i30 = 3 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	constexpr std::size_t i21 = 2 + _rows_padded * 1;
	constexpr std::size_t i31 = 3 + _rows_padded * 1;
	constexpr std::size_t i02 = 0 + _rows_padded * 2;
	constexpr std::size_t i12 = 1 + _rows_padded * 2;
	constexpr std::size_t i22 = 2 + _rows_padded * 2;
	constexpr std::size_t i32 = 3 + _rows_padded * 2;
	constexpr std::size_t i03 = 0 + _rows_padded * 3;
	constexpr std::size_t i13 = 1 + _rows_padded * 3;
	constexpr std::size_t i23 = 2 + _rows_padded * 3;
	constexpr std::size_t i33 = 3 + _rows_padded * 3;
	const Ty* ptr = A.data();

	return (*(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01)) * (*(ptr + i22) * *(ptr + i33) - *(ptr + i32) * *(ptr + i23))
		- (*(ptr + i00) * *(ptr + i21) - *(ptr + i20) * *(ptr + i01)) * (*(ptr + i12) * *(ptr + i33) - *(ptr + i32) * *(ptr + i13))
		+ (*(ptr + i00) * *(ptr + i31) - *(ptr + i30) * *(ptr + i01)) * (*(ptr + i12) * *(ptr + i23) - *(ptr + i22) * *(ptr + i13))
		+ (*(ptr + i10) * *(ptr + i21) - *(ptr + i20) * *(ptr + i11)) * (*(ptr + i02) * *(ptr + i33) - *(ptr + i32) * *(ptr + i03))
		- (*(ptr + i10) * *(ptr + i31) - *(ptr + i30) * *(ptr + i11)) * (*(ptr + i02) * *(ptr + i23) - *(ptr + i22) * *(ptr + i03))
		+ (*(ptr + i20) * *(ptr + i31) - *(ptr + i30) * *(ptr + i21)) * (*(ptr + i02) * *(ptr + i13) - *(ptr + i12) * *(ptr + i03));
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty
>
inline Ty cool::det2v(
	const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1) noexcept
{
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();

	return *(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0);
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty
>
inline Ty cool::det3v(
	const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2) noexcept
{
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();

	return *(a0ptr + 0) * (*(a1ptr + 1) * *(a2ptr + 2) - *(a1ptr + 2) * *(a2ptr + 1))
		+ *(a0ptr + 1) * (*(a1ptr + 2) * *(a2ptr + 0) - *(a1ptr + 0) * *(a2ptr + 2))
		+ *(a0ptr + 2) * (*(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0));
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty
>
inline Ty cool::det4v(
	const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
	const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3) noexcept
{
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* a3ptr = a3.data();

	return (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0)) * (*(a2ptr + 2) * *(a3ptr + 3) - *(a2ptr + 3) * *(a3ptr + 2))
		- (*(a0ptr + 0) * *(a1ptr + 2) - *(a0ptr + 2) * *(a1ptr + 0)) * (*(a2ptr + 1) * *(a3ptr + 3) - *(a2ptr + 3) * *(a3ptr + 1))
		+ (*(a0ptr + 0) * *(a1ptr + 3) - *(a0ptr + 3) * *(a1ptr + 0)) * (*(a2ptr + 1) * *(a3ptr + 2) - *(a2ptr + 2) * *(a3ptr + 1))
		+ (*(a0ptr + 1) * *(a1ptr + 2) - *(a0ptr + 2) * *(a1ptr + 1)) * (*(a2ptr + 0) * *(a3ptr + 3) - *(a2ptr + 3) * *(a3ptr + 0))
		- (*(a0ptr + 1) * *(a1ptr + 3) - *(a0ptr + 3) * *(a1ptr + 1)) * (*(a2ptr + 0) * *(a3ptr + 2) - *(a2ptr + 2) * *(a3ptr + 0))
		+ (*(a0ptr + 2) * *(a1ptr + 3) - *(a0ptr + 3) * *(a1ptr + 2)) * (*(a2ptr + 0) * *(a3ptr + 1) - *(a2ptr + 1) * *(a3ptr + 0));
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, _dim, _dim, _opt_res_rows_padded, _opt_res_align> cool::inv(const cool::const_matrix_interface<Ty, _dim, _dim, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	return cool::lu<_opt_res_rows_padded, _opt_res_align>(A).inv();
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align> cool::inv(const cool::const_matrix_interface<Ty, 1, 1, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	return cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align>(static_cast<Ty>(1) / A[0]);
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> cool::inv(const cool::const_matrix_interface<Ty, 2, 2, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;

	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 2>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;

	cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* ptr = A.data();

	Ty d = static_cast<Ty>(1) / (*(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01));

	*(res_ptr + j00) = d * *(ptr + i11);
	*(res_ptr + j10) = -d * *(ptr + i10);
	*(res_ptr + j01) = -d * *(ptr + i01);
	*(res_ptr + j11) = d * *(ptr + i00);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> cool::inv(const cool::const_matrix_interface<Ty, 3, 3, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i20 = 2 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	constexpr std::size_t i21 = 2 + _rows_padded * 1;
	constexpr std::size_t i02 = 0 + _rows_padded * 2;
	constexpr std::size_t i12 = 1 + _rows_padded * 2;
	constexpr std::size_t i22 = 2 + _rows_padded * 2;

	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 3>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j20 = 2 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;
	constexpr std::size_t j21 = 2 + _res_rows_padded * 1;
	constexpr std::size_t j02 = 0 + _res_rows_padded * 2;
	constexpr std::size_t j12 = 1 + _res_rows_padded * 2;
	constexpr std::size_t j22 = 2 + _res_rows_padded * 2;

	cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* ptr = A.data();

	Ty cof0 = *(ptr + i11) * *(ptr + i22) - *(ptr + i21) * *(ptr + i12);
	Ty cof1 = *(ptr + i21) * *(ptr + i02) - *(ptr + i01) * *(ptr + i22);
	Ty cof2 = *(ptr + i01) * *(ptr + i12) - *(ptr + i11) * *(ptr + i02);

	Ty d = static_cast<Ty>(1) / (*(ptr + i00) * cof0 + *(ptr + i10) * cof1 + *(ptr + i20) * cof2);

	*(res_ptr + j00) = d * cof0;
	*(res_ptr + j01) = d * cof1;
	*(res_ptr + j02) = d * cof2;

	*(res_ptr + j10) = d * (*(ptr + i20) * *(ptr + i12) - *(ptr + i10) * *(ptr + i22));
	*(res_ptr + j11) = d * (*(ptr + i00) * *(ptr + i22) - *(ptr + i20) * *(ptr + i02));
	*(res_ptr + j12) = d * (*(ptr + i10) * *(ptr + i02) - *(ptr + i00) * *(ptr + i12));

	*(res_ptr + j20) = d * (*(ptr + i10) * *(ptr + i21) - *(ptr + i20) * *(ptr + i11));
	*(res_ptr + j21) = d * (*(ptr + i20) * *(ptr + i01) - *(ptr + i00) * *(ptr + i21));
	*(res_ptr + j22) = d * (*(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01));

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
inline cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> cool::inv(const cool::const_matrix_interface<Ty, 4, 4, _rows_padded, _align, _matrix_data_Ty>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i20 = 2 + _rows_padded * 0;
	constexpr std::size_t i30 = 3 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	constexpr std::size_t i21 = 2 + _rows_padded * 1;
	constexpr std::size_t i31 = 3 + _rows_padded * 1;
	constexpr std::size_t i02 = 0 + _rows_padded * 2;
	constexpr std::size_t i12 = 1 + _rows_padded * 2;
	constexpr std::size_t i22 = 2 + _rows_padded * 2;
	constexpr std::size_t i32 = 3 + _rows_padded * 2;
	constexpr std::size_t i03 = 0 + _rows_padded * 3;
	constexpr std::size_t i13 = 1 + _rows_padded * 3;
	constexpr std::size_t i23 = 2 + _rows_padded * 3;
	constexpr std::size_t i33 = 3 + _rows_padded * 3;

	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 4>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j20 = 2 + _res_rows_padded * 0;
	constexpr std::size_t j30 = 3 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;
	constexpr std::size_t j21 = 2 + _res_rows_padded * 1;
	constexpr std::size_t j31 = 3 + _res_rows_padded * 1;
	constexpr std::size_t j02 = 0 + _res_rows_padded * 2;
	constexpr std::size_t j12 = 1 + _res_rows_padded * 2;
	constexpr std::size_t j22 = 2 + _res_rows_padded * 2;
	constexpr std::size_t j32 = 3 + _res_rows_padded * 2;
	constexpr std::size_t j03 = 0 + _res_rows_padded * 3;
	constexpr std::size_t j13 = 1 + _res_rows_padded * 3;
	constexpr std::size_t j23 = 2 + _res_rows_padded * 3;
	constexpr std::size_t j33 = 3 + _res_rows_padded * 3;

	cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* ptr = A.data();

	Ty temp_01_01 = *(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01);
	Ty temp_01_02 = *(ptr + i00) * *(ptr + i12) - *(ptr + i10) * *(ptr + i02);
	Ty temp_01_03 = *(ptr + i00) * *(ptr + i13) - *(ptr + i10) * *(ptr + i03);

	Ty temp_01_12 = *(ptr + i01) * *(ptr + i12) - *(ptr + i11) * *(ptr + i02);
	Ty temp_01_13 = *(ptr + i01) * *(ptr + i13) - *(ptr + i11) * *(ptr + i03);
	Ty temp_01_23 = *(ptr + i02) * *(ptr + i13) - *(ptr + i12) * *(ptr + i03);

	Ty temp_23_01 = *(ptr + i20) * *(ptr + i31) - *(ptr + i30) * *(ptr + i21);
	Ty temp_23_02 = *(ptr + i20) * *(ptr + i32) - *(ptr + i30) * *(ptr + i22);
	Ty temp_23_03 = *(ptr + i20) * *(ptr + i33) - *(ptr + i30) * *(ptr + i23);

	Ty temp_23_12 = *(ptr + i21) * *(ptr + i32) - *(ptr + i31) * *(ptr + i22);
	Ty temp_23_13 = *(ptr + i21) * *(ptr + i33) - *(ptr + i31) * *(ptr + i23);
	Ty temp_23_23 = *(ptr + i22) * *(ptr + i33) - *(ptr + i32) * *(ptr + i23);

	Ty d = static_cast<Ty>(1) / (temp_01_01 * temp_23_23 - temp_01_02 * temp_23_13
		+ temp_01_03 * temp_23_12 + temp_01_12 * temp_23_03
		- temp_01_13 * temp_23_02 + temp_01_23 * temp_23_01);

	*(res_ptr + j00) = d * (*(ptr + i11) * temp_23_23 - *(ptr + i12) * temp_23_13 + *(ptr + i13) * temp_23_12);
	*(res_ptr + j01) = -d * (*(ptr + i01) * temp_23_23 - *(ptr + i02) * temp_23_13 + *(ptr + i03) * temp_23_12);
	*(res_ptr + j02) = d * (*(ptr + i31) * temp_01_23 - *(ptr + i32) * temp_01_13 + *(ptr + i33) * temp_01_12);
	*(res_ptr + j03) = -d * (*(ptr + i21) * temp_01_23 - *(ptr + i22) * temp_01_13 + *(ptr + i23) * temp_01_12);

	*(res_ptr + j10) = -d * (*(ptr + i10) * temp_23_23 - *(ptr + i12) * temp_23_03 + *(ptr + i13) * temp_23_02);
	*(res_ptr + j11) = d * (*(ptr + i00) * temp_23_23 - *(ptr + i02) * temp_23_03 + *(ptr + i03) * temp_23_02);
	*(res_ptr + j12) = -d * (*(ptr + i30) * temp_01_23 - *(ptr + i32) * temp_01_03 + *(ptr + i33) * temp_01_02);
	*(res_ptr + j13) = d * (*(ptr + i20) * temp_01_23 - *(ptr + i22) * temp_01_03 + *(ptr + i23) * temp_01_02);

	*(res_ptr + j20) = d * (*(ptr + i10) * temp_23_13 - *(ptr + i11) * temp_23_03 + *(ptr + i13) * temp_23_01);
	*(res_ptr + j21) = -d * (*(ptr + i00) * temp_23_13 - *(ptr + i01) * temp_23_03 + *(ptr + i03) * temp_23_01);
	*(res_ptr + j22) = d * (*(ptr + i30) * temp_01_13 - *(ptr + i31) * temp_01_03 + *(ptr + i33) * temp_01_01);
	*(res_ptr + j23) = -d * (*(ptr + i20) * temp_01_13 - *(ptr + i21) * temp_01_03 + *(ptr + i23) * temp_01_01);

	*(res_ptr + j30) = -d * (*(ptr + i10) * temp_23_12 - *(ptr + i11) * temp_23_02 + *(ptr + i12) * temp_23_01);
	*(res_ptr + j31) = d * (*(ptr + i00) * temp_23_12 - *(ptr + i01) * temp_23_02 + *(ptr + i02) * temp_23_01);
	*(res_ptr + j32) = -d * (*(ptr + i30) * temp_01_12 - *(ptr + i31) * temp_01_02 + *(ptr + i32) * temp_01_01);
	*(res_ptr + j33) = d * (*(ptr + i20) * temp_01_12 - *(ptr + i21) * temp_01_02 + *(ptr + i22) * temp_01_01);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty
>
inline cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> cool::inv2v(
	const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 2>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;

	cool::matrix_result<Ty, 2, 2, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	*(res_ptr + j00) = d * *(a1ptr + 1);
	*(res_ptr + j10) = -d * *(a0ptr + 1);
	*(res_ptr + j01) = -d * *(a1ptr + 0);
	*(res_ptr + j11) = d * *(a0ptr + 0);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> cool::inv3v(
	const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 3>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j20 = 2 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;
	constexpr std::size_t j21 = 2 + _res_rows_padded * 1;
	constexpr std::size_t j02 = 0 + _res_rows_padded * 2;
	constexpr std::size_t j12 = 1 + _res_rows_padded * 2;
	constexpr std::size_t j22 = 2 + _res_rows_padded * 2;

	cool::matrix_result<Ty, 3, 3, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();

	Ty cof0 = *(a1ptr + 1) * *(a2ptr + 2) - *(a1ptr + 2) * *(a2ptr + 1);
	Ty cof1 = *(a1ptr + 2) * *(a2ptr + 0) - *(a1ptr + 0) * *(a2ptr + 2);
	Ty cof2 = *(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0);

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * cof0 + *(a0ptr + 1) * cof1 + *(a0ptr + 2) * cof2);

	*(res_ptr + j00) = d * cof0;
	*(res_ptr + j01) = d * cof1;
	*(res_ptr + j02) = d * cof2;

	*(res_ptr + j10) = d * (*(a0ptr + 2) * *(a2ptr + 1) - *(a0ptr + 1) * *(a2ptr + 2));
	*(res_ptr + j11) = d * (*(a0ptr + 0) * *(a2ptr + 2) - *(a0ptr + 2) * *(a2ptr + 0));
	*(res_ptr + j12) = d * (*(a0ptr + 1) * *(a2ptr + 0) - *(a0ptr + 0) * *(a2ptr + 1));

	*(res_ptr + j20) = d * (*(a0ptr + 1) * *(a1ptr + 2) - *(a0ptr + 2) * *(a1ptr + 1));
	*(res_ptr + j21) = d * (*(a0ptr + 2) * *(a1ptr + 0) - *(a0ptr + 0) * *(a1ptr + 2));
	*(res_ptr + j22) = d * (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> cool::inv4v(
	const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
	const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3) noexcept
{
	constexpr std::size_t _res_rows_padded = cool::_opt_dim<_opt_res_rows_padded, 4>::value;

	constexpr std::size_t j00 = 0 + _res_rows_padded * 0;
	constexpr std::size_t j10 = 1 + _res_rows_padded * 0;
	constexpr std::size_t j20 = 2 + _res_rows_padded * 0;
	constexpr std::size_t j30 = 3 + _res_rows_padded * 0;
	constexpr std::size_t j01 = 0 + _res_rows_padded * 1;
	constexpr std::size_t j11 = 1 + _res_rows_padded * 1;
	constexpr std::size_t j21 = 2 + _res_rows_padded * 1;
	constexpr std::size_t j31 = 3 + _res_rows_padded * 1;
	constexpr std::size_t j02 = 0 + _res_rows_padded * 2;
	constexpr std::size_t j12 = 1 + _res_rows_padded * 2;
	constexpr std::size_t j22 = 2 + _res_rows_padded * 2;
	constexpr std::size_t j32 = 3 + _res_rows_padded * 2;
	constexpr std::size_t j03 = 0 + _res_rows_padded * 3;
	constexpr std::size_t j13 = 1 + _res_rows_padded * 3;
	constexpr std::size_t j23 = 2 + _res_rows_padded * 3;
	constexpr std::size_t j33 = 3 + _res_rows_padded * 3;

	cool::matrix_result<Ty, 4, 4, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* a3ptr = a3.data();

	Ty temp_01_01 = *(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0);
	Ty temp_01_02 = *(a0ptr + 0) * *(a2ptr + 1) - *(a0ptr + 1) * *(a2ptr + 0);
	Ty temp_01_03 = *(a0ptr + 0) * *(a3ptr + 1) - *(a0ptr + 1) * *(a3ptr + 0);

	Ty temp_01_12 = *(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0);
	Ty temp_01_13 = *(a1ptr + 0) * *(a3ptr + 1) - *(a1ptr + 1) * *(a3ptr + 0);
	Ty temp_01_23 = *(a2ptr + 0) * *(a3ptr + 1) - *(a2ptr + 1) * *(a3ptr + 0);

	Ty temp_23_01 = *(a0ptr + 2) * *(a1ptr + 3) - *(a0ptr + 3) * *(a1ptr + 2);
	Ty temp_23_02 = *(a0ptr + 2) * *(a2ptr + 3) - *(a0ptr + 3) * *(a2ptr + 2);
	Ty temp_23_03 = *(a0ptr + 2) * *(a3ptr + 3) - *(a0ptr + 3) * *(a3ptr + 2);

	Ty temp_23_12 = *(a1ptr + 2) * *(a2ptr + 3) - *(a1ptr + 3) * *(a2ptr + 2);
	Ty temp_23_13 = *(a1ptr + 2) * *(a3ptr + 3) - *(a1ptr + 3) * *(a3ptr + 2);
	Ty temp_23_23 = *(a2ptr + 2) * *(a3ptr + 3) - *(a2ptr + 3) * *(a3ptr + 2);

	Ty d = static_cast<Ty>(1) / (temp_01_01 * temp_23_23 - temp_01_02 * temp_23_13
		+ temp_01_03 * temp_23_12 + temp_01_12 * temp_23_03
		- temp_01_13 * temp_23_02 + temp_01_23 * temp_23_01);

	*(res_ptr + j00) = d * (*(a1ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_13 + *(a3ptr + 1) * temp_23_12);
	*(res_ptr + j01) = -d * (*(a1ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_13 + *(a3ptr + 0) * temp_23_12);
	*(res_ptr + j02) = d * (*(a1ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_13 + *(a3ptr + 3) * temp_01_12);
	*(res_ptr + j03) = -d * (*(a1ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_13 + *(a3ptr + 2) * temp_01_12);

	*(res_ptr + j10) = -d * (*(a0ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_02);
	*(res_ptr + j11) = d * (*(a0ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_02);
	*(res_ptr + j12) = -d * (*(a0ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_02);
	*(res_ptr + j13) = d * (*(a0ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_02);

	*(res_ptr + j20) = d * (*(a0ptr + 1) * temp_23_13 - *(a1ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_01);
	*(res_ptr + j21) = -d * (*(a0ptr + 0) * temp_23_13 - *(a1ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_01);
	*(res_ptr + j22) = d * (*(a0ptr + 3) * temp_01_13 - *(a1ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_01);
	*(res_ptr + j23) = -d * (*(a0ptr + 2) * temp_01_13 - *(a1ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_01);

	*(res_ptr + j30) = -d * (*(a0ptr + 1) * temp_23_12 - *(a1ptr + 1) * temp_23_02 + *(a2ptr + 1) * temp_23_01);
	*(res_ptr + j31) = d * (*(a0ptr + 0) * temp_23_12 - *(a1ptr + 0) * temp_23_02 + *(a2ptr + 0) * temp_23_01);
	*(res_ptr + j32) = -d * (*(a0ptr + 3) * temp_01_12 - *(a1ptr + 3) * temp_01_02 + *(a2ptr + 3) * temp_01_01);
	*(res_ptr + j33) = d * (*(a0ptr + 2) * temp_01_12 - *(a1ptr + 2) * temp_01_02 + *(a2ptr + 2) * temp_01_01);

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _dim, std::size_t _Y_cols, std::size_t _A_rows_padded, std::size_t _Y_rows_padded,
	std::size_t _A_align, std::size_t _Y_align, class _A_matrix_data_Ty, class _Y_matrix_data_Ty
>
inline cool::matrix_result<Ty, _dim, _Y_cols, _opt_res_rows_padded, _opt_res_align> cool::solve(
	const cool::const_matrix_interface<Ty, _dim, _dim, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
	const cool::const_matrix_interface<Ty, _dim, _Y_cols, _Y_rows_padded, _Y_align, _Y_matrix_data_Ty>& Y) noexcept
{
	return cool::lu<_opt_res_rows_padded, _opt_res_align>(A).solve(Y);
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
	std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align> cool::solve(
	const cool::const_matrix_interface<Ty, 1, 1, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
	const cool::const_matrix_interface<Ty, 1, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	return cool::matrix_result<Ty, 1, 1, _opt_res_rows_padded, _opt_res_align>(y[0] / A[0]);
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
	std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> cool::solve(
	const cool::const_matrix_interface<Ty, 2, 2, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
	const cool::const_matrix_interface<Ty, 2, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	constexpr std::size_t i00 = 0 + _A_rows_padded * 0;
	constexpr std::size_t i10 = 1 + _A_rows_padded * 0;
	constexpr std::size_t i01 = 0 + _A_rows_padded * 1;
	constexpr std::size_t i11 = 1 + _A_rows_padded * 1;

	cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01));

	*(res_ptr + 0) = d * (*(Aptr + i11) * *(yptr + 0) - *(Aptr + i01) * *(yptr + 1));
	*(res_ptr + 1) = d * (*(Aptr + i00) * *(yptr + 1) - *(Aptr + i10) * *(yptr + 0));

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
	std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> cool::solve(
	const cool::const_matrix_interface<Ty, 3, 3, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
	const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	constexpr std::size_t i00 = 0 + _A_rows_padded * 0;
	constexpr std::size_t i10 = 1 + _A_rows_padded * 0;
	constexpr std::size_t i20 = 2 + _A_rows_padded * 0;
	constexpr std::size_t i01 = 0 + _A_rows_padded * 1;
	constexpr std::size_t i11 = 1 + _A_rows_padded * 1;
	constexpr std::size_t i21 = 2 + _A_rows_padded * 1;
	constexpr std::size_t i02 = 0 + _A_rows_padded * 2;
	constexpr std::size_t i12 = 1 + _A_rows_padded * 2;
	constexpr std::size_t i22 = 2 + _A_rows_padded * 2;

	cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();

	Ty cof0 = *(Aptr + i11) * *(Aptr + i22) - *(Aptr + i21) * *(Aptr + i12);
	Ty cof1 = *(Aptr + i21) * *(Aptr + i02) - *(Aptr + i01) * *(Aptr + i22);
	Ty cof2 = *(Aptr + i01) * *(Aptr + i12) - *(Aptr + i11) * *(Aptr + i02);

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * cof0 + *(Aptr + i10) * cof1 + *(Aptr + i20) * cof2);

	*(res_ptr + 0) = d * (cof0 * *(yptr + 0) + cof1 * *(yptr + 1) + cof2 * *(yptr + 2));

	*(res_ptr + 1) = d * ((*(Aptr + i20) * *(Aptr + i12) - *(Aptr + i10) * *(Aptr + i22)) * *(yptr + 0)
		+ (*(Aptr + i00) * *(Aptr + i22) - *(Aptr + i20) * *(Aptr + i02)) * *(yptr + 1)
		+ (*(Aptr + i10) * *(Aptr + i02) - *(Aptr + i00) * *(Aptr + i12)) * *(yptr + 2));

	*(res_ptr + 2) = d * ((*(Aptr + i10) * *(Aptr + i21) - *(Aptr + i20) * *(Aptr + i11)) * *(yptr + 0)
		+ (*(Aptr + i20) * *(Aptr + i01) - *(Aptr + i00) * *(Aptr + i21)) * *(yptr + 1)
		+ (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01)) * *(yptr + 2));

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded,
	std::size_t _A_align, std::size_t _y_align, class _A_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::solve(
	const cool::const_matrix_interface<Ty, 4, 4, _A_rows_padded, _A_align, _A_matrix_data_Ty>& A,
	const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	constexpr std::size_t i00 = 0 + _A_rows_padded * 0;
	constexpr std::size_t i10 = 1 + _A_rows_padded * 0;
	constexpr std::size_t i20 = 2 + _A_rows_padded * 0;
	constexpr std::size_t i30 = 3 + _A_rows_padded * 0;
	constexpr std::size_t i01 = 0 + _A_rows_padded * 1;
	constexpr std::size_t i11 = 1 + _A_rows_padded * 1;
	constexpr std::size_t i21 = 2 + _A_rows_padded * 1;
	constexpr std::size_t i31 = 3 + _A_rows_padded * 1;
	constexpr std::size_t i02 = 0 + _A_rows_padded * 2;
	constexpr std::size_t i12 = 1 + _A_rows_padded * 2;
	constexpr std::size_t i22 = 2 + _A_rows_padded * 2;
	constexpr std::size_t i32 = 3 + _A_rows_padded * 2;
	constexpr std::size_t i03 = 0 + _A_rows_padded * 3;
	constexpr std::size_t i13 = 1 + _A_rows_padded * 3;
	constexpr std::size_t i23 = 2 + _A_rows_padded * 3;
	constexpr std::size_t i33 = 3 + _A_rows_padded * 3;

	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> ret;

	Ty* res_ptr = ret.data();
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();

	Ty temp_01_01 = *(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01);
	Ty temp_01_02 = *(Aptr + i00) * *(Aptr + i12) - *(Aptr + i10) * *(Aptr + i02);
	Ty temp_01_03 = *(Aptr + i00) * *(Aptr + i13) - *(Aptr + i10) * *(Aptr + i03);

	Ty temp_01_12 = *(Aptr + i01) * *(Aptr + i12) - *(Aptr + i11) * *(Aptr + i02);
	Ty temp_01_13 = *(Aptr + i01) * *(Aptr + i13) - *(Aptr + i11) * *(Aptr + i03);
	Ty temp_01_23 = *(Aptr + i02) * *(Aptr + i13) - *(Aptr + i12) * *(Aptr + i03);

	Ty temp_23_01 = *(Aptr + i20) * *(Aptr + i31) - *(Aptr + i30) * *(Aptr + i21);
	Ty temp_23_02 = *(Aptr + i20) * *(Aptr + i32) - *(Aptr + i30) * *(Aptr + i22);
	Ty temp_23_03 = *(Aptr + i20) * *(Aptr + i33) - *(Aptr + i30) * *(Aptr + i23);

	Ty temp_23_12 = *(Aptr + i21) * *(Aptr + i32) - *(Aptr + i31) * *(Aptr + i22);
	Ty temp_23_13 = *(Aptr + i21) * *(Aptr + i33) - *(Aptr + i31) * *(Aptr + i23);
	Ty temp_23_23 = *(Aptr + i22) * *(Aptr + i33) - *(Aptr + i32) * *(Aptr + i23);

	Ty d = static_cast<Ty>(1) / (temp_01_01 * temp_23_23 - temp_01_02 * temp_23_13
		+ temp_01_03 * temp_23_12 + temp_01_12 * temp_23_03
		- temp_01_13 * temp_23_02 + temp_01_23 * temp_23_01);

	*(res_ptr + 0) = d * ((*(Aptr + i11) * temp_23_23 - *(Aptr + i12) * temp_23_13 + *(Aptr + i13) * temp_23_12) * *(yptr + 0)
		- (*(Aptr + i01) * temp_23_23 - *(Aptr + i02) * temp_23_13 + *(Aptr + i03) * temp_23_12) * *(yptr + 1)
		+ (*(Aptr + i31) * temp_01_23 - *(Aptr + i32) * temp_01_13 + *(Aptr + i33) * temp_01_12) * *(yptr + 2)
		- (*(Aptr + i21) * temp_01_23 - *(Aptr + i22) * temp_01_13 + *(Aptr + i23) * temp_01_12) * *(yptr + 3));

	*(res_ptr + 1) = d * (-(*(Aptr + i10) * temp_23_23 - *(Aptr + i12) * temp_23_03 + *(Aptr + i13) * temp_23_02) * *(yptr + 0)
		+ (*(Aptr + i00) * temp_23_23 - *(Aptr + i02) * temp_23_03 + *(Aptr + i03) * temp_23_02) * *(yptr + 1)
		- (*(Aptr + i30) * temp_01_23 - *(Aptr + i32) * temp_01_03 + *(Aptr + i33) * temp_01_02) * *(yptr + 2)
		+ (*(Aptr + i20) * temp_01_23 - *(Aptr + i22) * temp_01_03 + *(Aptr + i23) * temp_01_02) * *(yptr + 3));

	*(res_ptr + 2) = d * ((*(Aptr + i10) * temp_23_13 - *(Aptr + i11) * temp_23_03 + *(Aptr + i13) * temp_23_01) * *(yptr + 0)
		- (*(Aptr + i00) * temp_23_13 - *(Aptr + i01) * temp_23_03 + *(Aptr + i03) * temp_23_01) * *(yptr + 1)
		+ (*(Aptr + i30) * temp_01_13 - *(Aptr + i31) * temp_01_03 + *(Aptr + i33) * temp_01_01) * *(yptr + 2)
		- (*(Aptr + i20) * temp_01_13 - *(Aptr + i21) * temp_01_03 + *(Aptr + i23) * temp_01_01) * *(yptr + 3));

	*(res_ptr + 3) = d * (-(*(Aptr + i10) * temp_23_12 - *(Aptr + i11) * temp_23_02 + *(Aptr + i12) * temp_23_01) * *(yptr + 0)
		+ (*(Aptr + i00) * temp_23_12 - *(Aptr + i01) * temp_23_02 + *(Aptr + i02) * temp_23_01) * *(yptr + 1)
		- (*(Aptr + i30) * temp_01_12 - *(Aptr + i31) * temp_01_02 + *(Aptr + i32) * temp_01_01) * *(yptr + 2)
		+ (*(Aptr + i20) * temp_01_12 - *(Aptr + i21) * temp_01_02 + *(Aptr + i22) * temp_01_01) * *(yptr + 3));

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _y_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _y_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> cool::solve2v(
	const cool::const_matrix_interface<Ty, 2, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 2, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 2, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 2, 1, _opt_res_rows_padded, _opt_res_align> x;

	Ty* xptr = x.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* yptr = y.data();

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	*(xptr + 0) = d * (*(a1ptr + 1) * *(yptr + 0) - *(a1ptr + 0) * *(yptr + 1));
	*(xptr + 1) = d * (*(a0ptr + 0) * *(yptr + 1) - *(a0ptr + 1) * *(yptr + 0));

	return x;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _y_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _y_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> cool::solve3v(
	const cool::const_matrix_interface<Ty, 3, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 3, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 3, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
	const cool::const_matrix_interface<Ty, 3, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 3, 1, _opt_res_rows_padded, _opt_res_align> x;

	Ty* xptr = x.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* yptr = y.data();

	Ty cof0 = *(a1ptr + 1) * *(a2ptr + 2) - *(a1ptr + 2) * *(a2ptr + 1);
	Ty cof1 = *(a1ptr + 2) * *(a2ptr + 0) - *(a1ptr + 0) * *(a2ptr + 2);
	Ty cof2 = *(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0);

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * cof0 + *(a0ptr + 1) * cof1 + *(a0ptr + 2) * cof2);

	*(xptr + 0) = d * (cof0 * *(yptr + 0) + cof1 * *(yptr + 1) + cof2 * *(yptr + 2));

	*(xptr + 1) = d * ((*(a0ptr + 2) * *(a2ptr + 1) - *(a0ptr + 1) * *(a2ptr + 2)) * *(yptr + 0)
		+ (*(a0ptr + 0) * *(a2ptr + 2) - *(a0ptr + 2) * *(a2ptr + 0)) * *(yptr + 1)
		+ (*(a0ptr + 1) * *(a2ptr + 0) - *(a0ptr + 0) * *(a2ptr + 1)) * *(yptr + 2));

	*(xptr + 2) = d * ((*(a0ptr + 1) * *(a1ptr + 2) - *(a0ptr + 2) * *(a1ptr + 1)) * *(yptr + 0)
		+ (*(a0ptr + 2) * *(a1ptr + 0) - *(a0ptr + 0) * *(a1ptr + 2)) * *(yptr + 1)
		+ (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0)) * *(yptr + 2));

	return x;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty,
	std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded, std::size_t _y_rows_padded,
	std::size_t _a0_align, std::size_t _a1_align, std::size_t _a2_align, std::size_t _a3_align, std::size_t _y_align,
	class _a0_matrix_data_Ty, class _a1_matrix_data_Ty, class _a2_matrix_data_Ty, class _a3_matrix_data_Ty, class _y_matrix_data_Ty
>
inline cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> cool::solve4v(
	const cool::const_matrix_interface<Ty, 4, 1, _a0_rows_padded, _a0_align, _a0_matrix_data_Ty>& a0,
	const cool::const_matrix_interface<Ty, 4, 1, _a1_rows_padded, _a1_align, _a1_matrix_data_Ty>& a1,
	const cool::const_matrix_interface<Ty, 4, 1, _a2_rows_padded, _a2_align, _a2_matrix_data_Ty>& a2,
	const cool::const_matrix_interface<Ty, 4, 1, _a3_rows_padded, _a3_align, _a3_matrix_data_Ty>& a3,
	const cool::const_matrix_interface<Ty, 4, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y) noexcept
{
	cool::matrix_result<Ty, 4, 1, _opt_res_rows_padded, _opt_res_align> x;

	Ty* xptr = x.data();
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* a3ptr = a3.data();
	const Ty* yptr = y.data();

	Ty temp_01_01 = *(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0);
	Ty temp_01_02 = *(a0ptr + 0) * *(a2ptr + 1) - *(a0ptr + 1) * *(a2ptr + 0);
	Ty temp_01_03 = *(a0ptr + 0) * *(a3ptr + 1) - *(a0ptr + 1) * *(a3ptr + 0);

	Ty temp_01_12 = *(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0);
	Ty temp_01_13 = *(a1ptr + 0) * *(a3ptr + 1) - *(a1ptr + 1) * *(a3ptr + 0);
	Ty temp_01_23 = *(a2ptr + 0) * *(a3ptr + 1) - *(a2ptr + 1) * *(a3ptr + 0);

	Ty temp_23_01 = *(a0ptr + 2) * *(a1ptr + 3) - *(a0ptr + 3) * *(a1ptr + 2);
	Ty temp_23_02 = *(a0ptr + 2) * *(a2ptr + 3) - *(a0ptr + 3) * *(a2ptr + 2);
	Ty temp_23_03 = *(a0ptr + 2) * *(a3ptr + 3) - *(a0ptr + 3) * *(a3ptr + 2);

	Ty temp_23_12 = *(a1ptr + 2) * *(a2ptr + 3) - *(a1ptr + 3) * *(a2ptr + 2);
	Ty temp_23_13 = *(a1ptr + 2) * *(a3ptr + 3) - *(a1ptr + 3) * *(a3ptr + 2);
	Ty temp_23_23 = *(a2ptr + 2) * *(a3ptr + 3) - *(a2ptr + 3) * *(a3ptr + 2);

	Ty d = static_cast<Ty>(1) / (temp_01_01 * temp_23_23 - temp_01_02 * temp_23_13
		+ temp_01_03 * temp_23_12 + temp_01_12 * temp_23_03
		- temp_01_13 * temp_23_02 + temp_01_23 * temp_23_01);

	*(xptr + 0) = d * ((*(a1ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_13 + *(a3ptr + 1) * temp_23_12) * *(yptr + 0)
		- (*(a1ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_13 + *(a3ptr + 0) * temp_23_12) * *(yptr + 1)
		+ (*(a1ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_13 + *(a3ptr + 3) * temp_01_12) * *(yptr + 2)
		- (*(a1ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_13 + *(a3ptr + 2) * temp_01_12) * *(yptr + 3));

	*(xptr + 1) = d * (-(*(a0ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_02) * *(yptr + 0)
		+ (*(a0ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_02) * *(yptr + 1)
		- (*(a0ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_02) * *(yptr + 2)
		+ (*(a0ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_02) * *(yptr + 3));

	*(xptr + 2) = d * ((*(a0ptr + 1) * temp_23_13 - *(a1ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_01) * *(yptr + 0)
		- (*(a0ptr + 0) * temp_23_13 - *(a1ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_01) * *(yptr + 1)
		+ (*(a0ptr + 3) * temp_01_13 - *(a1ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_01) * *(yptr + 2)
		- (*(a0ptr + 2) * temp_01_13 - *(a1ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_01) * *(yptr + 3));

	*(xptr + 3) = d * (-(*(a0ptr + 1) * temp_23_12 - *(a1ptr + 1) * temp_23_02 + *(a2ptr + 1) * temp_23_01) * *(yptr + 0)
		+ (*(a0ptr + 0) * temp_23_12 - *(a1ptr + 0) * temp_23_02 + *(a2ptr + 0) * temp_23_01) * *(yptr + 1)
		- (*(a0ptr + 3) * temp_01_12 - *(a1ptr + 3) * temp_01_02 + *(a2ptr + 3) * temp_01_01) * *(yptr + 2)
		+ (*(a0ptr + 2) * temp_01_12 - *(a1ptr + 2) * temp_01_02 + *(a2ptr + 2) * temp_01_01) * *(yptr + 3));

	return x;
}

template <class params_Ty, class Ty, std::size_t _dim,
	std::size_t _fx_rows_padded, std::size_t _J_rows_padded, std::size_t _xref_rows_padded, std::size_t _xarg_rows_padded, std::size_t _y_rows_padded,
	std::size_t _fx_align, std::size_t _J_align, std::size_t _xref_align, std::size_t _xarg_align, std::size_t _y_align,
	class _x_matrix_data_Ty, class _y_matrix_data_Ty
>
inline int cool::nonlinear_solve(
	cool::matrix<Ty, _dim, 1, _fx_rows_padded, _fx_align>(*fn)(const cool::matrix<Ty, _dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
	cool::matrix<Ty, _dim, _dim, _J_rows_padded, _J_align>(*Jfn)(const cool::matrix<Ty, _dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
	cool::matrix_interface<Ty, _dim, 1, _xref_rows_padded, _xref_align, _x_matrix_data_Ty>& xref,
	const cool::const_matrix_interface<Ty, _dim, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y,
	Ty tol, int iter, params_Ty params) noexcept
{
	cool::matrix<Ty, _dim, 1, _xarg_rows_padded, _xarg_align> x = xref;
	cool::matrix<Ty, _dim, 1, _fx_rows_padded, _fx_align> fx = fn(x, params);
	fx -= y;
	tol = tol * tol;

	while (cool::dot(fx, fx) >= tol)
	{
		if (iter < 0)
		{
			break;
		}
		else
		{
			iter--;

			x -= cool::solve<_xarg_rows_padded, _xarg_align>(Jfn(x, params), fx);
			fx = fn(x, params);
			fx -= y;
		}
	}

	xref = x;

	return iter;
}

template <class params_Ty, class Ty, std::size_t dim,
	std::size_t _fx_rows_padded, std::size_t _Jinv_rows_padded, std::size_t _xref_rows_padded, std::size_t _xarg_rows_padded, std::size_t _y_rows_padded,
	std::size_t _fx_align, std::size_t _Jinv_align, std::size_t _xref_align, std::size_t _xarg_align, std::size_t _y_align,
	class _Jinv_matrix_data_Ty, class _x_matrix_data_Ty, class _y_matrix_data_Ty
>
inline int cool::nonlinear_qn_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded, _fx_align>(*fn)(const cool::matrix<Ty, dim, 1, _xarg_rows_padded, _xarg_align>&, params_Ty),
	cool::matrix_interface<Ty, dim, dim, _Jinv_rows_padded, _Jinv_align, _Jinv_matrix_data_Ty>& Jinvref,
	cool::matrix_interface<Ty, dim, 1, _xref_rows_padded, _xref_align, _x_matrix_data_Ty>& xref,
	const cool::const_matrix_interface<Ty, dim, 1, _y_rows_padded, _y_align, _y_matrix_data_Ty>& y,
	Ty tol, int iter, params_Ty params) noexcept
{
	cool::matrix<Ty, dim, dim, dim> Jinv = Jinvref;
	cool::matrix<Ty, dim, 1, _xarg_rows_padded, _xarg_align> x = xref;
	cool::matrix<Ty, dim, 1, _fx_rows_padded, _fx_align> fx = fn(x, params);
	tol = tol * tol;

	while (cool::dot(fx, fx) >= tol)
	{
		if (iter < 0)
		{
			break;
		}
		else
		{
			iter--;

			cool::matrix<Ty, dim, 1, dim> s = Jinv * fx;
			x -= s;
			fx -= fn(x, params);
			fx += y;
			cool::matrix<Ty, dim, 1, dim> Jinvfx = Jinv * fx;
			cool::matrix<Ty, dim, 1, dim> stemp = (static_cast<Ty>(1) / cool::dot(Jinvfx, s)) * (s - Jinvfx);
			cool::const_matrix_span<Ty, 1, dim, 1> st(s.data());
			Jinv.fmadd(stemp, st * Jinv);
			fx = fn(x, params);
			fx -= y;
		}
	}

	xref = x;
	Jinvref = Jinv;

	return iter;
}

// lu_matrix

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
inline Ty cool::lu_matrix<Ty, _dim, _rows_padded, _align>::det() const noexcept
{
	constexpr bool _dim_lt_2 = _dim < 2;
	constexpr bool _dim_lt_8 = _dim < 8;

	if (_dim_lt_2)
	{
		Ty ret = *(this->data());
		return flipped ? -ret : ret;
	}
	else if (_dim_lt_8)
	{
		constexpr std::size_t _dim_mult2 = _dim - _dim % 2;
		constexpr bool _dim_odd = _dim % 2 != 0;

		const Ty* ptr = this->data();

		Ty acc0 = *ptr;
		Ty acc1 = *(ptr + (_rows_padded + 1));

		for (std::size_t k = 2; k < _dim_mult2; k += 2)
		{
			acc0 *= *(ptr + (_rows_padded + 1) * k);
			acc1 *= *(ptr + (_rows_padded + 1) * (k + 1));
		}
		if (_dim_odd)
		{
			acc0 *= *(ptr + (_rows_padded + 1) * _dim_mult2);
		}

		Ty ret = acc0 * acc1;
		return flipped ? -ret : ret;
	}
	else
	{
		constexpr std::size_t _dim_mult4 = _dim - _dim % 4;
		constexpr std::size_t remainder = _dim % 4;

		const Ty* ptr = this->data();

		Ty acc[4] = { *ptr,
			*(ptr + (_rows_padded + 1)),
			*(ptr + (_rows_padded + 1) * 2),
			*(ptr + (_rows_padded + 1) * 3)
		};

		for (std::size_t n = 4; n < _dim_mult4; n += 4)
		{
			for (std::size_t m = 0; m < 4; m++)
			{
				acc[m] *= *(ptr + (_rows_padded + 1) * (n + m));
			}
		}
		for (std::size_t m = 0; m < remainder; m++)
		{
			acc[m] *= *(ptr + (_rows_padded + 1) * (_dim_mult4 + m));
		}

		Ty ret = (acc[0] * acc[1]) * (acc[2] * acc[3]);
		return flipped ? -ret : ret;
	}
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
inline Ty cool::lu_matrix<Ty, _dim, _rows_padded, _align>::diag_ratio() const noexcept
{
	constexpr bool _dim_lt_2 = _dim < 2;

	if (_dim_lt_2)
	{
		return (*(this->data()) != static_cast<Ty>(0)) ? static_cast<Ty>(1) : static_cast<Ty>(0);
	}
	else
	{
		std::size_t _dim_m1 = _dim - 1;

		const Ty* ptr = this->data();

		Ty min_value = *ptr;
		auto min_value_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(min_value);

		Ty max_value = min_value;
		auto max_value_sq = min_value_sq;

#ifdef __clang__
#pragma unroll 4
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 4
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t k = 1; k < _dim_m1; k++)
		{
			Ty temp = *(ptr + (_rows_padded + 1) * k);
			auto temp_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(temp);

			bool new_min = temp_sq < min_value_sq;
			min_value = new_min ? temp : min_value;
			min_value_sq = new_min ? temp_sq : min_value_sq;

			bool new_max = max_value_sq < temp_sq;
			max_value = new_max ? temp : max_value;
			max_value_sq = new_max ? temp_sq : max_value_sq;
		}

		Ty temp = *(ptr + (_rows_padded + 1) * _dim_m1);
		auto temp_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(temp);

		min_value = (temp_sq < min_value_sq) ? temp : min_value;
		max_value = (max_value_sq < temp_sq) ? temp : max_value;

		return (max_value != static_cast<Ty>(0)) ? (min_value / max_value) : static_cast<Ty>(0);
	}
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
template <std::size_t _rhs_cols, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _rhs_matrix_data_Ty>
inline cool::matrix<Ty, _dim, _rhs_cols, _rows_padded, _align> cool::lu_matrix<Ty, _dim, _rows_padded, _align>::solve(
	const cool::const_matrix_interface<Ty, _dim, _rhs_cols, _rhs_rows_padded, _rhs_align, _rhs_matrix_data_Ty>& Y) const noexcept
{
	cool::matrix<Ty, _dim, _rhs_cols, _rows_padded, _align> X;

	for (std::size_t k = 0; k < _rhs_cols; k++)
	{
		Ty* ptrX = X.data() + k * _rows_padded;
		const Ty* ptrY = Y.data() + k * _rhs_rows_padded;
		for (std::size_t j = 0; j < _dim; j++)
		{
			*ptrX++ = *(ptrY + perm[j]);
		}
	}

	constexpr std::size_t _rhs_cols_mmod4 = _rhs_cols - _rhs_cols % 4;
	constexpr std::size_t remainder = _rhs_cols % 4;

	for (std::size_t k = 0; k < _rhs_cols_mmod4; k += 4)
	{
		for (std::size_t j = 0; j < _dim; j++)
		{
			Ty* ptrX = X.data() + j + _rows_padded * k;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + _rows_padded);
			Ty temp2 = *(ptrX + _rows_padded * 2);
			Ty temp3 = *(ptrX + _rows_padded * 3);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + _rows_padded;
			Ty* ptrX2 = ptrX + 1 + _rows_padded * 2;
			Ty* ptrX3 = ptrX + 1 + _rows_padded * 3;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + _rows_padded * j;
			for (; i < _dim; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
				*ptrX3++ -= temp3 * temp;
			}
		}

		for (std::size_t j = _dim; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + _rows_padded * k;
			Ty* ptrX1 = ptrX0 + _rows_padded;
			Ty* ptrX2 = ptrX0 + _rows_padded * 2;
			Ty* ptrX3 = ptrX0 + _rows_padded * 3;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (_rows_padded + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + _rows_padded) *= temp_inv);
			Ty temp2 = (*(ptrX + _rows_padded * 2) *= temp_inv);
			Ty temp3 = (*(ptrX + _rows_padded * 3) *= temp_inv);
			const Ty* ptrLU = this->data() + _rows_padded * j;
			for (std::size_t i = 0; i < j; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
				*ptrX3++ -= temp3 * temp;
			}
		}
	}

	switch (remainder)
	{

	case 1:
	{
		for (std::size_t j = 0; j < _dim; j++)
		{
			Ty* ptrX0 = X.data() + j + _rows_padded * _rhs_cols_mmod4;
			Ty temp0 = *ptrX0++;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + _rows_padded * j;
			for (; i < _dim; i++)
			{
				*ptrX0++ -= temp0 * *ptrLU++;
			}
		}

		for (std::size_t j = _dim; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + _rows_padded * _rhs_cols_mmod4;
			Ty temp0 = (*(ptrX0 + j) /= *(this->data() + (_rows_padded + 1) * j));
			const Ty* ptrLU = this->data() + _rows_padded * j;
			for (std::size_t i = 0; i < j; i++)
			{
				*ptrX0++ -= temp0 * *ptrLU++;
			}
		}
		break;
	}

	case 2:
	{
		for (std::size_t j = 0; j < _dim; j++)
		{
			Ty* ptrX = X.data() + j + _rows_padded * _rhs_cols_mmod4;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + _rows_padded);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + _rows_padded;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + _rows_padded * j;
			for (; i < _dim; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
			}
		}

		for (std::size_t j = _dim; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + _rows_padded * _rhs_cols_mmod4;
			Ty* ptrX1 = ptrX0 + _rows_padded;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (_rows_padded + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + _rows_padded) *= temp_inv);
			const Ty* ptrLU = this->data() + _rows_padded * j;
			for (std::size_t i = 0; i < j; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
			}
		}
		break;
	}

	case 3:
	{
		for (std::size_t j = 0; j < _dim; j++)
		{

			Ty* ptrX = X.data() + j + _rows_padded * _rhs_cols_mmod4;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + _rows_padded);
			Ty temp2 = *(ptrX + _rows_padded * 2);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + _rows_padded;
			Ty* ptrX2 = ptrX + 1 + _rows_padded * 2;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + _rows_padded * j;
			for (; i < _dim; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
			}
		}

		for (std::size_t j = _dim; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + _rows_padded * _rhs_cols_mmod4;
			Ty* ptrX1 = ptrX0 + _rows_padded;
			Ty* ptrX2 = ptrX0 + _rows_padded * 2;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (_rows_padded + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + _rows_padded) *= temp_inv);
			Ty temp2 = (*(ptrX + _rows_padded * 2) *= temp_inv);
			const Ty* ptrLU = this->data() + _rows_padded * j;
			for (std::size_t i = 0; i < j; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
			}
		}
		break;
	}

	default:
		break;
	}

	return X;
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> cool::lu_matrix<Ty, _dim, _rows_padded, _align>::inv() const noexcept
{
	cool::matrix<Ty, _dim, _dim, _rows_padded, _align> Id = static_cast<Ty>(0);

	Ty* ptrId = Id.data();
	for (std::size_t k = 0; k < _dim; k++)
	{
		*(ptrId + (_rows_padded + 1) * k) = static_cast<Ty>(1);
	}

	return this->solve(Id);
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> cool::lu_matrix<Ty, _dim, _rows_padded, _align>::lower() const noexcept
{
	cool::matrix<Ty, _dim, _dim, _rows_padded, _align> ret = static_cast<Ty>(0);

	for (std::size_t j = 0; j < _dim; j++)
	{
		Ty* res_ptr = ret.data() + (_rows_padded + 1) * j;
		const Ty* ptrL = this->data() + 1 + (_rows_padded + 1) * j;
		*res_ptr++ = static_cast<Ty>(1);
		for (std::size_t i = j + 1; i < _dim; i++)
		{
			*res_ptr++ = *ptrL++;
		}
	}

	return ret;
}

template <class Ty, std::size_t _dim, std::size_t _rows_padded, std::size_t _align>
inline cool::matrix<Ty, _dim, _dim, _rows_padded, _align> cool::lu_matrix<Ty, _dim, _rows_padded, _align>::upper() const noexcept
{
	cool::matrix<Ty, _dim, _dim, _rows_padded, _align> ret = static_cast<Ty>(0);

	for (std::size_t j = 0; j < _dim; j++)
	{
		Ty* res_ptr = ret.data() + _rows_padded * j;
		const Ty* ptrL = this->data() + _rows_padded * j;
		for (std::size_t i = 0; i <= j; i++)
		{
			*res_ptr++ = *ptrL++;
		}
	}

	return ret;
}

template <std::size_t _opt_res_rows_padded, std::size_t _opt_res_align, class Ty, std::size_t _dim, std::size_t _rhs_rows_padded, std::size_t _rhs_align, class _matrix_data_Ty>
inline cool::lu_matrix<Ty, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value, cool::_opt_dim<_opt_res_align, cool::matrix_align_spec<Ty, _dim, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value>::value>::value> cool::lu(
	const cool::const_matrix_interface<Ty, _dim, _dim, _rhs_rows_padded, _rhs_align, _matrix_data_Ty>& rhs) noexcept
{
	constexpr std::size_t _dim_m1 = (_dim == 0) ? 0 : _dim - 1;
	constexpr std::size_t _rows_padded = cool::_opt_dim<_opt_res_rows_padded, _dim>::value;
	constexpr std::size_t _align = cool::_opt_dim<_opt_res_align, cool::matrix_align_spec<Ty, _dim, _dim, _rows_padded>::value>::value;

	cool::lu_matrix<Ty, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value, cool::_opt_dim<_opt_res_align, cool::matrix_align_spec<Ty, _dim, _dim, cool::_opt_dim<_opt_res_rows_padded, _dim>::value>::value>::value> LU = rhs;

	for (std::size_t n = 0; n < _dim; n++)
	{
		LU.perm[n] = n;
	}

	LU.flipped = false;

	for (std::size_t k = 0; k < _dim_m1; k++)
	{
		{
			Ty* ptrLU = LU.data() + k + _rows_padded * k;
			auto piv_abs_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(*ptrLU++);
			std::size_t grt = k;
			bool permute = false;

			for (std::size_t j = k + 1; j < _dim; j++)
			{
				auto piv_abs_sq_temp = cool::matrix_scalar_subroutine::abs_sq<Ty>(*ptrLU++);
				if (piv_abs_sq < piv_abs_sq_temp)
				{
					piv_abs_sq = piv_abs_sq_temp;
					grt = j;
					permute = true;
				}
			}

			if (permute)
			{
				Ty* ptrLU1 = LU.data() + k;
				Ty* ptrLU2 = LU.data() + grt;
				for (std::size_t j = 0; j < _dim; j++)
				{
					Ty temp = *ptrLU1;
					*ptrLU1 = *ptrLU2;
					*ptrLU2 = temp;
					ptrLU1 += _rows_padded;
					ptrLU2 += _rows_padded;
				}

				{
					std::size_t temp = LU.perm[grt];
					LU.perm[grt] = LU.perm[k];
					LU.perm[k] = temp;
				}

				LU.flipped ^= true;
			}
		}

		Ty* ptrLU = LU.data() + k + _rows_padded * k;

		if (*ptrLU != static_cast<Ty>(0))
		{
			{
				Ty temp = static_cast<Ty>(1) / *ptrLU++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < _dim; i++)
				{
					*ptrL++ *= temp;
				}
			}

			std::size_t j = k + 1;
			for (; j + 3 < _dim; j += 4)
			{
				Ty* ptr0 = LU.data() + k + _rows_padded * j;
				Ty* ptr1 = ptr0 + _rows_padded;
				Ty* ptr2 = ptr0 + _rows_padded * 2;
				Ty* ptr3 = ptr0 + _rows_padded * 3;
				Ty temp0 = *ptr0++;
				Ty temp1 = *ptr1++;
				Ty temp2 = *ptr2++;
				Ty temp3 = *ptr3++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < _dim; i++)
				{
					Ty temp = *ptrL++;
					*ptr0++ -= temp * temp0;
					*ptr1++ -= temp * temp1;
					*ptr2++ -= temp * temp2;
					*ptr3++ -= temp * temp3;
				}
			}
			for (; j < _dim; j++)
			{
				Ty* ptr0 = LU.data() + k + _rows_padded * j;
				Ty temp0 = *ptr0++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < _dim; i++)
				{
					*ptr0++ -= *ptrL++ * temp0;
				}
			}
		}
	}

	return LU;
}

#endif // _COOL_MATRIX_HPP


#if defined(_COOL_MATRIX_HPP) && (defined(_LIBCPP_IOSTREAM) || defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
#ifndef _COOL_MATRIX_HPP_IOSTREAM
#define _COOL_MATRIX_HPP_IOSTREAM
namespace cool
{
	template <class Ty> inline void print_matrix(const Ty* const ptr, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded,
		std::size_t _rows_blk, std::size_t _cols_blk, std::streamsize _cell_width) noexcept
	{
		std::cout << ">>>         [ size : " << _rows << " x " << _cols << " ]\n";

		if (_rows_blk == 0) { _rows_blk = _rows; }

		if (_cols_blk == 0) { _cols_blk = _cols; }

		for (std::size_t iOut = 0; iOut < _rows; iOut += _rows_blk)
		{
			std::size_t imax = (iOut + _rows_blk < _rows) ? iOut + _rows_blk : _rows;
			for (std::size_t jOut = 0; jOut < _cols; jOut += _cols_blk)
			{
				std::size_t jmax = (jOut + _cols_blk < _cols) ? jOut + _cols_blk : _cols;

				std::cout << "            ";

				for (std::size_t j = jOut; j < jmax; j++)
				{
					std::cout << "col ";
					std::cout.fill(' '); std::cout.width(_cell_width);
					std::cout << std::left << j;
				}

				for (std::size_t i = iOut; i < imax; i++)
				{
					for (std::size_t j = jOut; j < jmax; j++)
					{
						if (j == jOut)
						{
							std::cout << "\nrow ";
							std::cout.fill(' '); std::cout.width(8);
							std::cout << std::left << i;
						}
						std::cout << "> ";
						std::cout.fill(' '); std::cout.width(_cell_width + 2);
						std::cout << std::left << *(ptr + i + _rows_padded * j);
					}
				}

				std::cout << '\n' << std::endl;
			}
		}
	}
}

namespace cool
{
	template <class Ty> class _print_matrix_cols_blk {
	public:
		static constexpr std::streamsize value = 8;
	};

	template <class Ty> class _print_matrix_cols_blk<std::complex<Ty>> {
	public:
		static constexpr std::streamsize value = 4;
	};

	template <class Ty> class _print_matrix_cell_width {
	public:
		static constexpr std::streamsize value = 12;
	};

	template <class Ty> class _print_matrix_cell_width<std::complex<Ty>> {
	public:
		static constexpr std::streamsize value = 24;
	};

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded, std::size_t _align, class _matrix_data_Ty>
	inline std::ostream& operator<<(std::ostream& out, const cool::const_matrix_interface<Ty, _rows, _cols, _rows_padded, _align, _matrix_data_Ty>& rhs) noexcept
	{
		cool::print_matrix(rhs.data(), _rows, _cols, _rows_padded, 16,
			cool::_print_matrix_cols_blk<Ty>::value,
			cool::_print_matrix_cell_width<Ty>::value
		);
		return out;
	}
}
#endif // _COOL_MATRIX_HPP_IOSTREAM
#endif // defined(_COOL_MATRIX_HPP) && (defined(_LIBCPP_IOSTREAM) || defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))


// cool_matrix.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.