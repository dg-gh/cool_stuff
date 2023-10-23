// cool_matrix.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef _COOL_MATRIX_HPP
#define _COOL_MATRIX_HPP

#include <cstddef>
#include <initializer_list>
#include <utility>
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
	template <class Ty, std::size_t _rows, std::size_t _cols = 1, std::size_t _rows_padded = _rows> class matrix;

	namespace matrix_scalar_subroutine
	{
		// template specializable functions
		template <class Ty> static inline Ty conj(const Ty& val) noexcept; // return type must be same type as val
		template <class Ty> static inline Ty real(const Ty& val) noexcept; // return type must be same type as val
		template <class Ty> static inline Ty imag(const Ty& val) noexcept; // return type must be same type as val
		template <class Ty> static inline Ty abs_sq(const Ty& val) noexcept; // return type must be comparable with operator<

		// template specializations
		template <class Ty> static inline std::complex<Ty> conj(const std::complex<Ty>& val) noexcept;
		template <class Ty> static inline std::complex<Ty> real(const std::complex<Ty>& val) noexcept;
		template <class Ty> static inline std::complex<Ty> imag(const std::complex<Ty>& val) noexcept;
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
		cm(cool::cm<Ty>&&) noexcept = default;
		cool::cm<Ty>& operator=(cool::cm<Ty>&&) noexcept = default;
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
		rm(cool::rm<Ty>&&) noexcept = default;
		cool::rm<Ty>& operator=(cool::rm<Ty>&&) noexcept = default;
		~rm() = default;

		inline const Ty* begin() const noexcept { return m_begin_ptr; }

	private:

		const Ty* m_begin_ptr;
	};

#ifndef _COOL_NO_INIT_ENUM
#define _COOL_NO_INIT_ENUM
	enum no_init_t { no_init };
#endif // _COOL_NO_INIT_ENUM

	template <std::size_t _rows, std::size_t _cols> class _diag;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> class matrix
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t rows = _rows;
		static constexpr std::size_t cols = _cols;
		static constexpr std::size_t rows_padded = _rows_padded;
		static constexpr std::size_t diag_length = _rows < _cols ? _rows : _cols;

		static_assert(_rows != 0, "cool::matrix<value_type, cols, rows, rows_padded> requirement : rows != 0");
		static_assert(_cols != 0, "cool::matrix<value_type, cols, rows, rows_padded> requirement : cols != 0");
		static_assert(_rows <= _rows_padded, "cool::matrix<value_type, cols, rows, rows_padded> requirement : rows <= rows_padded");


		matrix() = default;

		inline matrix(cool::no_init_t) noexcept {};

		template <std::size_t _rhs_rows_padded>
		inline matrix(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept;

		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept;

		template <std::size_t _rhs_rows_padded>
		inline matrix(cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>&& rhs) noexcept;

		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>&& rhs) noexcept;

		inline matrix(Ty rhs) noexcept;

		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(Ty rhs) noexcept;

		constexpr inline matrix(std::initializer_list<Ty> lst) noexcept;

		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(std::initializer_list<Ty> lst) noexcept;

		inline matrix(cool::rm<Ty> lst) noexcept;

		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(cool::rm<Ty> lst) noexcept;

		inline matrix(cool::cm<Ty> lst) noexcept;

		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator=(cool::cm<Ty> lst) noexcept;

		~matrix() = default;


		constexpr inline std::size_t size() const noexcept;
		constexpr inline std::size_t size_padded() const noexcept;

		inline Ty* data() noexcept;
		inline const Ty* data() const noexcept;

		template<class uint_Ty1, class uint_Ty2> inline Ty& operator()(uint_Ty1 i, uint_Ty2 j) noexcept;
		template<class uint_Ty1, class uint_Ty2> inline const Ty& operator()(uint_Ty1 i, uint_Ty2 j) const noexcept;

		template<class uint_Ty> inline Ty& operator[](uint_Ty mem_offset) noexcept;
		template<class uint_Ty> inline const Ty& operator[](uint_Ty mem_offset) const noexcept;

		template<class uint_Ty> inline cool::matrix<Ty, _rows, 1, _rows>& col(uint_Ty j) noexcept;
		template<class uint_Ty> inline const cool::matrix<Ty, _rows, 1, _rows>& col(uint_Ty j) const noexcept;

		template<class uint_Ty> inline cool::matrix<Ty, 1, _cols, _rows_padded>& row(uint_Ty i) noexcept;
		template<class uint_Ty> inline const cool::matrix<Ty, 1, _cols, _rows_padded>& row(uint_Ty i) const noexcept;

		inline cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>& diag() noexcept;
		inline const cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>& diag() const noexcept;

		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1, class uint_Ty2>
		inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk(uint_Ty1 i, uint_Ty2 j) noexcept;
		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty1, class uint_Ty2>
		inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk(uint_Ty1 i, uint_Ty2 j) const noexcept;
		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty>
		inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk(uint_Ty i) noexcept;
		template <std::size_t _blk_rows, std::size_t _blk_cols = 1, class uint_Ty>
		inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk(uint_Ty i) const noexcept;
		template <std::size_t _blk_rows, std::size_t _blk_cols = 1>
		inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk() noexcept;
		template <std::size_t _blk_rows, std::size_t _blk_cols = 1>
		inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& blk() const noexcept;


		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols> operator-(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols> operator+(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		inline cool::matrix<Ty, _rows, _cols> operator-(Ty rhs) const noexcept;
		inline cool::matrix<Ty, _rows, _cols> operator+(Ty rhs) const noexcept;

		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator-=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;
		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator+=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;
		template <std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator*=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;

		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator-=(Ty rhs) noexcept;
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator+=(Ty rhs) noexcept;
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& operator*=(Ty rhs) noexcept;

		template <int _sign = 1, std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& fmadd(Ty s, const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <std::size_t _rhs_cols, std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _rhs_cols> operator*(const cool::matrix<Ty, _cols, _rhs_cols, _rhs_rows_padded>& rhs) const noexcept;

		template <int _sign = 1, std::size_t _lhs_cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& fmadd(
			const cool::matrix<Ty, _rows, _lhs_cols, _lhs_rows_padded>& COOL_MATRIX_RESTRICT lhs,
			const cool::matrix<Ty, _lhs_cols, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;
		template <int _sign = 1, std::size_t _lhs_cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
		inline cool::matrix<Ty, _rows, _cols, _rows_padded>& fmadd(Ty s,
			const cool::matrix<Ty, _rows, _lhs_cols, _lhs_rows_padded>& COOL_MATRIX_RESTRICT lhs,
			const cool::matrix<Ty, _lhs_cols, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept;

		template <std::size_t _rhs_rows_padded>
		inline bool operator==(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline bool operator!=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline bool operator<=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline bool operator>=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline bool operator<(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;
		template <std::size_t _rhs_rows_padded>
		inline bool operator>(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept;

		inline bool operator==(Ty rhs) const noexcept;
		inline bool operator!=(Ty rhs) const noexcept;
		inline bool operator<=(Ty rhs) const noexcept;
		inline bool operator>=(Ty rhs) const noexcept;
		inline bool operator<(Ty rhs) const noexcept;
		inline bool operator>(Ty rhs) const noexcept;


		class cm_iterator;
		class cm_const_iterator;
		class rm_iterator;
		class rm_const_iterator;

		class cm_iterator_proxy;
		class cm_const_iterator_proxy;
		class rm_iterator_proxy;
		class rm_const_iterator_proxy;

		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy cm() noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy cm() const noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy rm() noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy rm() const noexcept;

		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator begin() noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator end() noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator begin() const noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator end() const noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cbegin() const noexcept;
		inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cend() const noexcept;

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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator begin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator end() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cbegin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cend() const noexcept;

		private:

			Ty* m_data_ptr;
		};

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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator begin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator end() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cbegin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cend() const noexcept;

		private:

			const Ty* m_data_ptr;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator begin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator end() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cbegin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cend() const noexcept;

		private:

			Ty* m_data_ptr;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator begin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator end() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cbegin() const noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cend() const noexcept;

		private:

			const Ty* m_data_ptr;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& operator--() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& operator++() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator operator--(int) noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator operator++(int) noexcept;

			inline bool operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;
			inline bool operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;
			inline bool operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;
			inline bool operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;
			inline bool operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;
			inline bool operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept;

		private:

			Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& operator--() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& operator++() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator operator--(int) noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator operator++(int) noexcept;

			inline bool operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;
			inline bool operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;
			inline bool operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;
			inline bool operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;
			inline bool operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;
			inline bool operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept;

		private:

			const Ty* m_data_ptr;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& operator--() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& operator++() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator operator--(int) noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator operator++(int) noexcept;

			inline bool operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;
			inline bool operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;
			inline bool operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;
			inline bool operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;
			inline bool operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;
			inline bool operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept;

		private:

			Ty* m_data_ptr;
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

			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& operator--() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& operator++() noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator operator--(int) noexcept;
			inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator operator++(int) noexcept;

			inline bool operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;
			inline bool operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;
			inline bool operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;
			inline bool operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;
			inline bool operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;
			inline bool operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept;

		private:

			const Ty* m_data_ptr;
			std::size_t m_i;
			std::size_t m_j;
		};

	private:

		Ty m_data[_rows_padded * _cols];

		template <class Ty2, std::size_t _rows2, std::size_t _cols2, std::size_t _rows_padded2> friend class matrix;
	};

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> operator-(const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> operator-(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> operator+(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> operator*(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator==(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator!=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator<=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator>=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator<(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;
	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline bool operator>(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
	inline Ty dot(const cool::matrix<Ty, _rows, _cols, _lhs_rows_padded>& lhs, const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _cols, _rows> transp(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _cols, _rows> adj(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> conj(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> real(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
	inline cool::matrix<Ty, _rows, _cols> imag(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 3, 1> vxv(const cool::matrix<Ty, 3, 1, _x_rows_padded>& x, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 4, 1> vxv(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 4, 1> qxq(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
	inline Ty det(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows_padded> inline Ty det(const cool::matrix<Ty, 1, 1, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows_padded> inline Ty det(const cool::matrix<Ty, 2, 2, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows_padded> inline Ty det(const cool::matrix<Ty, 3, 3, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _rows_padded> inline Ty det(const cool::matrix<Ty, 4, 4, _rows_padded>& A) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline Ty det2v(const cool::matrix<Ty, 2, 1, _x_rows_padded>& x, const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded, std::size_t _z_rows_padded>
	inline Ty det3v(const cool::matrix<Ty, 3, 1, _x_rows_padded>& x, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y,
		const cool::matrix<Ty, 3, 1, _z_rows_padded>& z) noexcept;

	template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded, std::size_t _z_rows_padded, std::size_t _w_rows_padded>
	inline Ty det4v(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y,
		const cool::matrix<Ty, 4, 1, _z_rows_padded>& z, const cool::matrix<Ty, 4, 1, _w_rows_padded>& w) noexcept;

	template <class Ty, std::size_t Ar, std::size_t Yc, std::size_t _A_rows_padded, std::size_t _Y_rows_padded>
	inline cool::matrix<Ty, Ar, Yc> solve(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A, const cool::matrix<Ty, Ar, Yc, _Y_rows_padded>& Y) noexcept;

	template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 1, 1> solve(const cool::matrix<Ty, 1, 1, _A_rows_padded>& A, const cool::matrix<Ty, 1, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 2, 1> solve(const cool::matrix<Ty, 2, 2, _A_rows_padded>& A, const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 3, 1> solve(const cool::matrix<Ty, 3, 3, _A_rows_padded>& A, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 4, 1> solve(const cool::matrix<Ty, 4, 4, _A_rows_padded>& A, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 2, 1> solve2v(const cool::matrix<Ty, 2, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 2, 1, _a1_rows_padded>& a1,
		const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 3, 1> solve3v(const cool::matrix<Ty, 3, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 3, 1, _a1_rows_padded>& a1,
		const cool::matrix<Ty, 3, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded, std::size_t _y_rows_padded>
	inline cool::matrix<Ty, 4, 1> solve4v(const cool::matrix<Ty, 4, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 4, 1, _a1_rows_padded>& a1,
		const cool::matrix<Ty, 4, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 4, 1, _a3_rows_padded>& a3, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept;

	template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
	inline cool::matrix<Ty, Ar, Ar> inv(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _A_rows_padded>
	inline cool::matrix<Ty, 1, 1> inv(const cool::matrix<Ty, 1, 1, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _A_rows_padded>
	inline cool::matrix<Ty, 2, 2> inv(const cool::matrix<Ty, 2, 2, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _A_rows_padded>
	inline cool::matrix<Ty, 3, 3> inv(const cool::matrix<Ty, 3, 3, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _A_rows_padded>
	inline cool::matrix<Ty, 4, 4> inv(const cool::matrix<Ty, 4, 4, _A_rows_padded>& A) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded>
	inline cool::matrix<Ty, 2, 2> inv2v(const cool::matrix<Ty, 2, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 2, 1, _a1_rows_padded>& a1) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded>
	inline cool::matrix<Ty, 3, 3> inv3v(const cool::matrix<Ty, 3, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 3, 1, _a1_rows_padded>& a1,
		const cool::matrix<Ty, 3, 1, _a2_rows_padded>& a2) noexcept;

	template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded>
	inline cool::matrix<Ty, 4, 4> inv4v(const cool::matrix<Ty, 4, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 4, 1, _a1_rows_padded>& a1,
		const cool::matrix<Ty, 4, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 4, 1, _a3_rows_padded>& a3) noexcept;


	template <class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _J_rows_padded, std::size_t _x_rows_padded>
	inline int nonlinear_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		Ty tol, int iter);

	template <class Ty, std::size_t dim, std::size_t _fx_rows_padded, std::size_t _J_rows_padded,
		std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline int nonlinear_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
		Ty tol, int iter);

	template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _J_rows_padded, std::size_t _x_rows_padded>
	inline int nonlinear_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		Ty tol, int iter, params_Ty params);

	template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _J_rows_padded, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline int nonlinear_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
		Ty tol, int iter, params_Ty params);

	template <class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded>
	inline int nonlinear_qn_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		Ty tol, int iter);

	template <class Ty, std::size_t dim, std::size_t _fx_rows_padded, std::size_t _Jinv_rows_padded,
		std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline int nonlinear_qn_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
		cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
		Ty tol, int iter);

	template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded>
	inline int nonlinear_qn_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		Ty tol, int iter, params_Ty params);

	template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
		std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
	inline int nonlinear_qn_solve(
		cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
		cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
		cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
		const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
		Ty tol, int iter, params_Ty params);


	template <class Ty, std::size_t Ar> class lu_matrix : public cool::matrix<Ty, Ar, Ar>
	{

	public:

		lu_matrix() noexcept = default;
		lu_matrix(const cool::lu_matrix<Ty, Ar>& rhs) noexcept = default;
		cool::lu_matrix<Ty, Ar>& operator=(const cool::lu_matrix<Ty, Ar>& rhs) noexcept = default;
		lu_matrix(cool::lu_matrix<Ty, Ar>&& rhs) noexcept = default;
		cool::lu_matrix<Ty, Ar>& operator=(cool::lu_matrix<Ty, Ar>&& rhs) noexcept = default;
		~lu_matrix() = default;

		template <std::size_t _A_rows_padded>
		inline lu_matrix(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& rhs) noexcept : cool::matrix<Ty, Ar, Ar>(rhs) {}
		template <std::size_t _A_rows_padded>
		inline lu_matrix(cool::matrix<Ty, Ar, Ar, _A_rows_padded>&& rhs) noexcept : cool::matrix<Ty, Ar, Ar>(std::move(rhs)) {}

		inline Ty det() const noexcept;
		inline Ty diag_ratio() const noexcept;
		template <std::size_t Yc, std::size_t _Y_rows_padded> inline cool::matrix<Ty, Ar, Yc> solve(const cool::matrix<Ty, Ar, Yc, _Y_rows_padded>& Y) const noexcept;
		inline cool::matrix<Ty, Ar, Ar> inv() const noexcept;
		inline cool::matrix<Ty, Ar, Ar> lower() const noexcept;
		inline cool::matrix<Ty, Ar, Ar> upper() const noexcept;

		std::size_t perm[Ar];
		bool flipped;
	};

	template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
	inline cool::lu_matrix<Ty, Ar> lu(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept;

	template <std::size_t _rows, std::size_t _cols> class _diag
	{

	public:

		static constexpr std::size_t length = _rows < _cols ? _rows : _cols;
	};

#if defined(_COOL_MATRIX_HPP) && (defined(_LIBCPP_IOSTREAM) || defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
	//	template <class Ty> inline void print_matrix(const Ty* const ptr, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded,
	//		std::size_t _rows_blk, std::size_t _cols_blk, std::streamsize _cell_width) noexcept;
#endif // defined(_COOL_MATRIX_HPP) && (defined(_LIBCPP_IOSTREAM) || defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_))
}


// detail

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

template <class Ty> inline Ty cool::matrix_scalar_subroutine::conj(const Ty& val) noexcept
{
	return val;
}

template <class Ty> inline std::complex<Ty> cool::matrix_scalar_subroutine::conj(const std::complex<Ty>& val) noexcept
{
	return std::conj(val);
}

template <class Ty> inline Ty cool::matrix_scalar_subroutine::real(const Ty& val) noexcept
{
	return val;
}

template <class Ty> inline std::complex<Ty> cool::matrix_scalar_subroutine::real(const std::complex<Ty>& val) noexcept
{
	return std::complex<Ty>(std::real(val));
}

template <class Ty> inline Ty cool::matrix_scalar_subroutine::imag(const Ty& val) noexcept
{
	return static_cast<Ty>(0);
}

template <class Ty> inline std::complex<Ty> cool::matrix_scalar_subroutine::imag(const std::complex<Ty>& val) noexcept
{
	return std::complex<Ty>(std::imag(val));
}

template <class Ty> inline Ty cool::matrix_scalar_subroutine::abs_sq(const Ty& val) noexcept
{
	return val * val;
}

template <class Ty> inline Ty cool::matrix_scalar_subroutine::abs_sq(const std::complex<Ty>& val) noexcept
{
	return std::real(val) * std::real(val) + std::imag(val) * std::imag(val);
}


template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept
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
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>&& rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = std::move(rhs.m_data[n]);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = std::move(rhs.m_data[i + _rhs_rows_padded * j]);
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>&& rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = std::move(rhs.m_data[n]);
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = std::move(rhs.m_data[i + _rhs_rows_padded * j]);
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = rhs;
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = rhs;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
constexpr inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(std::initializer_list<Ty> lst) noexcept : m_data{}
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* ptr = lst.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *ptr++;
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(std::initializer_list<Ty> lst) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* ptr = lst.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *ptr++;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(cool::cm<Ty> lst) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* ptr = lst.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *ptr++;
			}
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(cool::cm<Ty> lst) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	const Ty* ptr = lst.begin();

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] = *ptr++;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] = *ptr++;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::matrix(cool::rm<Ty> lst) noexcept
{
	const Ty* ptr = lst.begin();
	for (std::size_t i = 0; i < _rows; i++)
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			m_data[i + _rows_padded * j] = *ptr++;
		}
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator=(cool::rm<Ty> lst) noexcept
{
	const Ty* ptr = lst.begin();
	for (std::size_t i = 0; i < _rows; i++)
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			m_data[i + _rows_padded * j] = *ptr++;
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
constexpr inline std::size_t cool::matrix<Ty, _rows, _cols, _rows_padded>::size() const noexcept
{
	return _rows * _cols;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
constexpr inline std::size_t cool::matrix<Ty, _rows, _cols, _rows_padded>::size_padded() const noexcept
{
	return _rows_padded * _cols;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline Ty* cool::matrix<Ty, _rows, _cols, _rows_padded>::data() noexcept
{
	return static_cast<Ty*>(m_data);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const Ty* cool::matrix<Ty, _rows, _cols, _rows_padded>::data() const noexcept
{
	return static_cast<const Ty*>(m_data);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template<class uint_Ty1, class uint_Ty2>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator()(uint_Ty1 i, uint_Ty2 j) noexcept
{
	return m_data[static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j)];
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template<class uint_Ty1, class uint_Ty2>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator()(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return m_data[static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j)];
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template<class uint_Ty>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator[](uint_Ty mem_offset) noexcept
{
	return m_data[static_cast<std::size_t>(mem_offset)];
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template<class uint_Ty>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator[](uint_Ty mem_offset) const noexcept
{
	return m_data[static_cast<std::size_t>(mem_offset)];
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <class uint_Ty>
inline cool::matrix<Ty, _rows, 1, _rows>& cool::matrix<Ty, _rows, _cols, _rows_padded>::col(uint_Ty j) noexcept
{
	return reinterpret_cast<cool::matrix<Ty, _rows, 1, _rows>&>(m_data[_rows_padded * static_cast<std::size_t>(j)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <class uint_Ty>
inline const cool::matrix<Ty, _rows, 1, _rows>& cool::matrix<Ty, _rows, _cols, _rows_padded>::col(uint_Ty j) const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, _rows, 1, _rows>&>(m_data[_rows_padded * static_cast<std::size_t>(j)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <class uint_Ty>
inline cool::matrix<Ty, 1, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::row(uint_Ty i) noexcept
{
	return reinterpret_cast<cool::matrix<Ty, 1, _cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <class uint_Ty>
inline const cool::matrix<Ty, 1, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::row(uint_Ty i) const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, 1, _cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>& cool::matrix<Ty, _rows, _cols, _rows_padded>::diag() noexcept
{
	return reinterpret_cast<cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>&>(m_data[0]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>& cool::matrix<Ty, _rows, _cols, _rows_padded>::diag() const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, 1, cool::_diag<_rows, _cols>::length, _rows_padded + 1>&>(m_data[0]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1, class uint_Ty2>
inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk(uint_Ty1 i, uint_Ty2 j) noexcept
{
	return reinterpret_cast<cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty1, class uint_Ty2>
inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk(uint_Ty1 i, uint_Ty2 j) const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i) + _rows_padded * static_cast<std::size_t>(j)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty>
inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk(uint_Ty i) noexcept
{
	return reinterpret_cast<cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <std::size_t _blk_rows, std::size_t _blk_cols, class uint_Ty>
inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk(uint_Ty i) const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[static_cast<std::size_t>(i)]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _blk_rows, std::size_t _blk_cols>
inline cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk() noexcept
{
	return reinterpret_cast<cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[0]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _blk_rows, std::size_t _blk_cols>
inline const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::blk() const noexcept
{
	return reinterpret_cast<const cool::matrix<Ty, _blk_rows, _blk_cols, _rows_padded>&>(m_data[0]);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::matrix<Ty, _rows, _cols, _rows_padded>::operator-(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;

	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			ret.m_data[n] = m_data[n] - rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				ret.m_data[i + _rows * j] = m_data[i + _rows_padded * j] - rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::matrix<Ty, _rows, _cols, _rows_padded>::operator+(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;

	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			ret.m_data[n] = m_data[n] + rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				ret.m_data[i + _rows * j] = m_data[i + _rows_padded * j] + rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::matrix<Ty, _rows, _cols, _rows_padded>::operator-(Ty rhs) const noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;

	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			ret.m_data[n] = m_data[n] - rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				ret.m_data[i + _rows * j] = m_data[i + _rows_padded * j] - rhs;
			}
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::matrix<Ty, _rows, _cols, _rows_padded>::operator+(Ty rhs) const noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;

	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			ret.m_data[n] = m_data[n] + rhs;
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				ret.m_data[i + _rows * j] = m_data[i + _rows_padded * j] + rhs;
			}
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator-=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] -= rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] -= rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator+=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] += rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] += rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator*=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] *= rhs.m_data[n];
		}
	}
	else
	{
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] *= rhs.m_data[i + _rhs_rows_padded * j];
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>template <int _sign, std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::fmadd(Ty s, const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1,
		"first template argument of cool::matrix< ... >::fmadd requirement : value must be 1 or -1");

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		if (_positive)
		{
			for (std::size_t n = 0; n < _size; n++)
			{
				m_data[n] += s * rhs.m_data[n];
			}
		}
		else
		{
			for (std::size_t n = 0; n < _size; n++)
			{
				m_data[n] -= s * rhs.m_data[n];
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
					m_data[i + _rows_padded * j] += s * rhs.m_data[i + _rhs_rows_padded * j];
				}
			}
		}
		else
		{
			for (std::size_t j = 0; j < _cols; j++)
			{
				for (std::size_t i = 0; i < _rows; i++)
				{
					m_data[i + _rows_padded * j] -= s * rhs.m_data[i + _rhs_rows_padded * j];
				}
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator-=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] -= rhs;
		}
	}
	else
	{

		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] -= rhs;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator+=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] += rhs;
		}
	}
	else
	{

		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] += rhs;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::operator*=(Ty rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;

		for (std::size_t n = 0; n < _size; n++)
		{
			m_data[n] *= rhs;
		}
	}
	else
	{

		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				m_data[i + _rows_padded * j] *= rhs;
			}
		}
	}
	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_cols, std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _rhs_cols> cool::matrix<Ty, _rows, _cols, _rows_padded>::operator*(const cool::matrix<Ty, _cols, _rhs_cols, _rhs_rows_padded>& rhs) const noexcept
{
	cool::matrix<Ty, _rows, _rhs_cols> ret;

	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::broadcasted;
	constexpr bool _cols_small = _cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _rhs_cols, _cols>::repeat_threshold;

	constexpr std::size_t _rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _rhs_cols_mult_j_size = _rhs_cols - _rhs_cols % j_size;
	constexpr std::size_t j_remainder = _rhs_cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	if (_cols_small)
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _rhs_cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += m_data[i + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					for (std::size_t n = 0; n < i_size; n++)
					{
						ret.m_data[i + n + _rows * (j + m)] = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += m_data[i + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					for (std::size_t n = 0; n < i_size; n++)
					{
						ret.m_data[i + n + _rows * (_rhs_cols_mult_j_size + m)] = acc[m][n];
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
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += m_data[_rows_mult_i_size + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						ret.m_data[_rows_mult_i_size + n + _rows * (j + m)] = acc[m][n];
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += m_data[_rows_mult_i_size + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						ret.m_data[_rows_mult_i_size + n + _rows * (_rhs_cols_mult_j_size + m)] = acc[m][n];
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += m_data[i + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					for (std::size_t n = 0; n < i_size; n++)
					{
						ret.m_data[i + n + _rows * (j + m)] = acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += m_data[i + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					for (std::size_t n = 0; n < i_size; n++)
					{
						ret.m_data[i + n + _rows * (_rhs_cols_mult_j_size + m)] = acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += m_data[_rows_mult_i_size + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_size; m++)
				{
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						ret.m_data[_rows_mult_i_size + n + _rows * (j + m)] = acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_rhs_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += m_data[_rows_mult_i_size + n + _rows_padded * k] * rhs_temp;
						}
					}
				}

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						ret.m_data[_rows_mult_i_size + n + _rows * (_rhs_cols_mult_j_size + m)] = acc[m][n];
					}
				}
			}
		}
	}

	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <int _sign, std::size_t _lhs_cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::fmadd(
	const cool::matrix<Ty, _rows, _lhs_cols, _lhs_rows_padded>& COOL_MATRIX_RESTRICT lhs,
	const cool::matrix<Ty, _lhs_cols, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::broadcasted;
	constexpr bool _lhs_cols_small = _lhs_cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::repeat_threshold;

	constexpr std::size_t _rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _cols_mult_j_size = _cols - _cols % j_size;
	constexpr std::size_t j_remainder = _cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1,
		"first template argument of cool::matrix< ... >::fmadd requirement : value must be 1 or -1");

	if (_lhs_cols_small)
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = lhs.m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = lhs.m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] -= acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = lhs.m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] -= acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = lhs.m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] -= acc[m][n];
						}
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] -= acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] -= acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] -= acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] += acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] -= acc[m][n];
						}
					}
				}
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
template <int _sign, std::size_t _lhs_cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>& cool::matrix<Ty, _rows, _cols, _rows_padded>::fmadd(Ty s,
	const cool::matrix<Ty, _rows, _lhs_cols, _lhs_rows_padded>& COOL_MATRIX_RESTRICT lhs,
	const cool::matrix<Ty, _lhs_cols, _cols, _rhs_rows_padded>& COOL_MATRIX_RESTRICT rhs) noexcept
{
	constexpr std::size_t i_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::packed;
	constexpr std::size_t j_size = cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::broadcasted;
	constexpr bool _lhs_cols_small = _lhs_cols < cool::matrix_multiply_kernel_spec<Ty, _rows, _cols, _lhs_cols>::repeat_threshold;

	constexpr std::size_t _rows_mult_i_size = _rows - _rows % i_size;
	constexpr std::size_t i_remainder = _rows % i_size;
	constexpr std::size_t i_remainder_padded = (i_remainder != 0) ? i_remainder : 1;
	constexpr bool i_remainder_non_zero = i_remainder != 0;

	constexpr std::size_t _cols_mult_j_size = _cols - _cols % j_size;
	constexpr std::size_t j_remainder = _cols % j_size;
	constexpr std::size_t j_remainder_padded = (j_remainder != 0) ? j_remainder : 1;
	constexpr bool j_remainder_non_zero = j_remainder != 0;

	constexpr bool _positive = _sign >= 0;

	static_assert(_sign == 1 || _sign == -1,
		"first template argument of cool::matrix< ... >::fmadd requirement : value must be 1 or -1");

	if (_lhs_cols_small)
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_size];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = lhs.m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_size];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_size; n++)
					{
						acc[m][n] = lhs.m_data[i + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] -= s * acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded];

				for (std::size_t m = 0; m < j_size; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (j + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = lhs.m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] -= s * acc[m][n];
						}
					}
				}
			}

			if (j_remainder_non_zero)
			{
				Ty acc[j_remainder_padded][i_remainder_padded];

				for (std::size_t m = 0; m < j_remainder; m++)
				{
					Ty rhs_temp = rhs.m_data[_rhs_rows_padded * (_cols_mult_j_size + m)];
					for (std::size_t n = 0; n < i_remainder; n++)
					{
						acc[m][n] = lhs.m_data[_rows_mult_i_size + n] * rhs_temp;
					}
				}

				for (std::size_t k = 1; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] -= s * acc[m][n];
						}
					}
				}
			}
		}
	}
	else
	{
		for (std::size_t i = 0; i < _rows_mult_i_size; i += i_size)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (j + m)] -= s * acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_size; n++)
						{
							acc[m][n] += lhs.m_data[i + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_size; n++)
						{
							m_data[i + n + _rows_padded * (_cols_mult_j_size + m)] -= s * acc[m][n];
						}
					}
				}
			}
		}

		if (i_remainder_non_zero)
		{
			for (std::size_t j = 0; j < _cols_mult_j_size; j += j_size)
			{
				Ty acc[j_size][i_remainder_padded] = { {} };

				for (std::size_t k = 0; k < _lhs_cols; k++)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (j + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_size; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (j + m)] -= s * acc[m][n];
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
						Ty rhs_temp = rhs.m_data[k + _rhs_rows_padded * (_cols_mult_j_size + m)];
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							acc[m][n] += lhs.m_data[_rows_mult_i_size + n + _lhs_rows_padded * k] * rhs_temp;
						}
					}
				}

				if (_positive)
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] += s * acc[m][n];
						}
					}
				}
				else
				{
					for (std::size_t m = 0; m < j_remainder; m++)
					{
						for (std::size_t n = 0; n < i_remainder; n++)
						{
							m_data[_rows_mult_i_size + n + _rows_padded * (_cols_mult_j_size + m)] -= s * acc[m][n];
						}
					}
				}
			}
		}
	}

	return *this;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator==(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] == rhs.m_data[i + k]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] == rhs.m_data[i + k]);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] == rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] == rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] == rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] == rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator!=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (m_data[i + k] == rhs.m_data[i + k]);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (m_data[i + k] == rhs.m_data[i + k]);
		}
		return !not_ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (m_data[_rows_padded * (i + k)] == rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (m_data[_rows_padded * (i + k)] == rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool not_ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					not_ret &= (m_data[i + k + _rows_padded * j] == rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!not_ret)
				{
					return true;
				}
			}

			bool not_ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				not_ret &= (m_data[i + k + _rows_padded * j] == rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		return false;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator<=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] <= rhs.m_data[i + k]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] <= rhs.m_data[i + k]);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] <= rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] <= rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] <= rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] <= rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator>=(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] >= rhs.m_data[i + k]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] >= rhs.m_data[i + k]);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] >= rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] >= rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] >= rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] >= rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator<(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] < rhs.m_data[i + k]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] < rhs.m_data[i + k]);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] < rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] < rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] < rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] < rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded> template <std::size_t _rhs_rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator>(const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) const noexcept
{
	constexpr bool _contiguous = ((_rows == _rows_padded) && (_rows == _rhs_rows_padded)) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] > rhs.m_data[i + k]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] > rhs.m_data[i + k]);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] > rhs.m_data[_rhs_rows_padded * (i + k)]);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] > rhs.m_data[_rhs_rows_padded * (i + k)]);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] > rhs.m_data[i + k + _rhs_rows_padded * j]);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] > rhs.m_data[i + k + _rhs_rows_padded * j]);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator==(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] == rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] == rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] == rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] == rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator!=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (m_data[i + k] == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (m_data[i + k] == rhs);
		}
		return !not_ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (m_data[_rows_padded * (i + k)] == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (m_data[_rows_padded * (i + k)] == rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool not_ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					not_ret &= (m_data[i + k + _rows_padded * j] == rhs);
				}
				if (!not_ret)
				{
					return true;
				}
			}

			bool not_ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				not_ret &= (m_data[i + k + _rows_padded * j] == rhs);
			}
			if (!not_ret)
			{
				return true;
			}
		}

		return false;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator<=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] <= rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] <= rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] <= rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] <= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator>=(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] >= rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] >= rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] >= rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] >= rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator<(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] < rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] < rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] < rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] < rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::operator>(Ty rhs) const noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[i + k] > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[i + k] > rhs);
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (m_data[_rows_padded * (i + k)] > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (m_data[_rows_padded * (i + k)] > rhs);
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

			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (m_data[i + k + _rows_padded * j] > rhs);
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (m_data[i + k + _rows_padded * j] > rhs);
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::operator-(const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;
	const Ty* rhs_ptr = rhs.data();
	Ty* ret_ptr = ret.data();
	for (std::size_t j = 0; j < _cols; j++)
	{
		for (std::size_t i = 0; i < _rows; i++)
		{
			*ret_ptr++ = -(*(rhs_ptr + i + _rows_padded * j));
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::operator-(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;
	const Ty* rhs_ptr = rhs.data();
	Ty* ret_ptr = ret.data();
	for (std::size_t j = 0; j < _cols; j++)
	{
		for (std::size_t i = 0; i < _rows; i++)
		{
			*ret_ptr++ = lhs - (*(rhs_ptr + i + _rows_padded * j));
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::operator+(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;
	const Ty* rhs_ptr = rhs.data();
	Ty* ret_ptr = ret.data();
	for (std::size_t j = 0; j < _cols; j++)
	{
		for (std::size_t i = 0; i < _rows; i++)
		{
			*ret_ptr++ = lhs + (*(rhs_ptr + i + _rows_padded * j));
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::operator*(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	cool::matrix<Ty, _rows, _cols> ret;
	const Ty* rhs_ptr = rhs.data();
	Ty* ret_ptr = ret.data();
	for (std::size_t j = 0; j < _cols; j++)
	{
		for (std::size_t i = 0; i < _rows; i++)
		{
			*ret_ptr++ = lhs * *(rhs_ptr + i + _rows_padded * j);
		}
	}
	return ret;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator==(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs == *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs == *(rhs_ptr + i + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs == *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs == *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (lhs == *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (lhs == *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator!=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (lhs == *(rhs_ptr + i + k));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (lhs == *(rhs_ptr + i + k));
		}
		return !not_ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool not_ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				not_ret &= (lhs == *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		bool not_ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			not_ret &= (lhs == *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool not_ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					not_ret &= (lhs == *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!not_ret)
				{
					return true;
				}
			}

			bool not_ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				not_ret &= (lhs == *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!not_ret)
			{
				return true;
			}
		}

		return false;
	}
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator<=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs <= *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs <= *(rhs_ptr + i + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs <= *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs <= *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (lhs <= *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (lhs <= *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator>=(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs >= *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs >= *(rhs_ptr + i + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs >= *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs >= *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (lhs >= *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (lhs >= *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator<(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs < *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs < *(rhs_ptr + i + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs < *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs < *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (lhs < *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (lhs < *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::operator>(Ty lhs, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	constexpr bool _contiguous = (_rows == _rows_padded) || (_cols == 1);
	constexpr bool _line_matrix = _rows == 1;

	if (_contiguous)
	{
		constexpr std::size_t _size = _rows * _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs > *(rhs_ptr + i + k));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs > *(rhs_ptr + i + k));
		}
		return ret;
	}
	else if (_line_matrix)
	{
		constexpr std::size_t _size = _cols;
		constexpr std::size_t size_mult8 = _size - _size % 8;
		constexpr std::size_t size_mod8 = _size % 8;

		const Ty* rhs_ptr = rhs.data();
		std::size_t i;

		for (i = 0; i < size_mult8; i += 8)
		{
			bool ret = true;
			for (std::size_t k = 0; k < 8; k++)
			{
				ret &= (lhs > *(rhs_ptr + _rows_padded * (i + k)));
			}
			if (!ret)
			{
				return false;
			}
		}

		bool ret = true;
		for (std::size_t k = 0; k < size_mod8; k++)
		{
			ret &= (lhs > *(rhs_ptr + _rows_padded * (i + k)));
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

			const Ty* rhs_ptr = rhs.data();
			std::size_t i;

			for (i = 0; i < size_mult8; i += 8)
			{
				bool ret = true;
				for (std::size_t k = 0; k < 8; k++)
				{
					ret &= (lhs > *(rhs_ptr + i + k + _rows_padded * j));
				}
				if (!ret)
				{
					return false;
				}
			}

			bool ret = true;
			for (std::size_t k = 0; k < size_mod8; k++)
			{
				ret &= (lhs > *(rhs_ptr + i + k + _rows_padded * j));
			}
			if (!ret)
			{
				return false;
			}
		}

		return true;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _lhs_rows_padded, std::size_t _rhs_rows_padded>
inline Ty cool::dot(const cool::matrix<Ty, _rows, _cols, _lhs_rows_padded>& lhs, const cool::matrix<Ty, _rows, _cols, _rhs_rows_padded>& rhs) noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _cols, _rows> cool::transp(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept
{
	cool::matrix<Ty, _cols, _rows> C;
	constexpr bool _line_matrix = (_rows == 1) && (_rows_padded == 1);

	constexpr std::size_t cols_mult4 = _cols - _cols % 4;
	constexpr std::size_t cols_mod4 = _cols % 4;

	if (_line_matrix)
	{
		Ty* ptr = C.data();
		const Ty* ptrA = A.data();

		for (std::size_t j = 0; j < _cols; j++)
		{
			*ptr++ = *ptrA++;
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
			Ty* ptr = C.data() + j;
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
				*ptr = temp0;
				*(ptr + 1) = temp1;
				*(ptr + 2) = temp2;
				*(ptr + 3) = temp3;
				ptr += _cols;
			}
		}

		switch (cols_mod4)
		{

		case 1:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			Ty* ptr = C.data() + cols_mult4;
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr = *ptrA0++;
				ptr += _cols;
			}
			break;
		}

		case 2:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			const Ty* ptrA1 = ptrA0 + _rows_padded;
			Ty* ptr = C.data() + cols_mult4;
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
				*ptr = temp0;
				*(ptr + 1) = temp1;
				ptr += _cols;
			}
			break;
		}

		case 3:
		{
			const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
			const Ty* ptrA1 = ptrA0 + _rows_padded;
			const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
			Ty* ptr = C.data() + cols_mult4;
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
				*ptr = temp0;
				*(ptr + 1) = temp1;
				*(ptr + 2) = temp2;
				ptr += _cols;
			}
			break;
		}

		default:
			break;
		}
	}

	return C;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _cols, _rows> cool::adj(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept
{
	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;
	constexpr bool _line_matrix = (_rows == 1) && (_rows_padded == 1);

	constexpr std::size_t cols_mult4 = _cols - _cols % 4;
	constexpr std::size_t cols_mod4 = _cols % 4;

	if (no_conj_modif)
	{
		cool::matrix<Ty, _cols, _rows> C;

		if (_line_matrix)
		{
			Ty* ptr = C.data();
			const Ty* ptrA = A.data();

			for (std::size_t j = 0; j < _cols; j++)
			{
				*ptr++ = *ptrA++;
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
				Ty* ptr = C.data() + j;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					*(ptr + 2) = temp2;
					*(ptr + 3) = temp3;
					ptr += _cols;
				}
			}

			switch (cols_mod4)
			{

			case 1:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				Ty* ptr = C.data() + cols_mult4;
				for (std::size_t i = 0; i < _rows; i++)
				{
					*ptr = *ptrA0++;
					ptr += _cols;
				}
				break;
			}

			case 2:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				Ty* ptr = C.data() + cols_mult4;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					ptr += _cols;
				}
				break;
			}

			case 3:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				Ty* ptr = C.data() + cols_mult4;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					*(ptr + 2) = temp2;
					ptr += _cols;
				}
				break;
			}

			default:
				break;
			}
		}

		return C;
	}
	else
	{
		cool::matrix<Ty, _cols, _rows> C;

		if (_line_matrix)
		{
			Ty* ptr = C.data();
			const Ty* ptrA = A.data();

			for (std::size_t j = 0; j < _cols; j++)
			{
				*ptr++ = cool::matrix_scalar_subroutine::conj(*ptrA++);
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
				Ty* ptr = C.data() + j;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					*(ptr + 2) = temp2;
					*(ptr + 3) = temp3;
					ptr += _cols;
				}
			}

			switch (cols_mod4)
			{

			case 1:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				Ty* ptr = C.data() + cols_mult4;
				for (std::size_t i = 0; i < _rows; i++)
				{
					*ptr = cool::matrix_scalar_subroutine::conj(*ptrA0++);
					ptr += _cols;
				}
				break;
			}

			case 2:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				Ty* ptr = C.data() + cols_mult4;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					ptr += _cols;
				}
				break;
			}

			case 3:
			{
				const Ty* ptrA0 = A.data() + _rows_padded * cols_mult4;
				const Ty* ptrA1 = ptrA0 + _rows_padded;
				const Ty* ptrA2 = ptrA0 + _rows_padded * 2;
				Ty* ptr = C.data() + cols_mult4;
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
					*ptr = temp0;
					*(ptr + 1) = temp1;
					*(ptr + 2) = temp2;
					ptr += _cols;
				}
				break;
			}

			default:
				break;
			}
		}

		return C;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::conj(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept
{
	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;

	if (no_conj_modif)
	{
		cool::matrix<Ty, _cols, _rows> C;
		const Ty* Aptr = A.data();
		Ty* ptr = C.data();
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr++ = *(Aptr + i + _rows_padded * j);
			}
		}
		return C;
	}
	else
	{
		cool::matrix<Ty, _cols, _rows> C;
		const Ty* Aptr = A.data();
		Ty* ptr = C.data();
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr++ = cool::matrix_scalar_subroutine::conj(*(Aptr + i + _rows_padded * j));
			}
		}
		return C;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::real(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept
{
	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;

	if (no_conj_modif)
	{
		cool::matrix<Ty, _cols, _rows> C;
		const Ty* Aptr = A.data();
		Ty* ptr = C.data();
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr++ = *(Aptr + i + _rows_padded * j);
			}
		}
		return C;
	}
	else
	{
		cool::matrix<Ty, _cols, _rows> C;
		const Ty* Aptr = A.data();
		Ty* ptr = C.data();
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr++ = cool::matrix_scalar_subroutine::real(*(Aptr + i + _rows_padded * j));
			}
		}
		return C;
	}
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols> cool::imag(const cool::matrix<Ty, _rows, _cols, _rows_padded>& A) noexcept
{
	constexpr bool no_conj_modif = std::is_integral<Ty>::value || std::is_floating_point<Ty>::value;

	if (no_conj_modif)
	{
		return cool::matrix<Ty, _cols, _rows>(static_cast<Ty>(0));
	}
	else
	{
		cool::matrix<Ty, _cols, _rows> C;
		const Ty* Aptr = A.data();
		Ty* ptr = C.data();
		for (std::size_t j = 0; j < _cols; j++)
		{
			for (std::size_t i = 0; i < _rows; i++)
			{
				*ptr++ = cool::matrix_scalar_subroutine::imag(*(Aptr + i + _rows_padded * j));
			}
		}
		return C;
	}
}

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 3, 1> cool::vxv(const cool::matrix<Ty, 3, 1, _x_rows_padded>& x, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 3, 1> ret;
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();
	Ty* ptr = ret.data();

	*(ptr + 0) = *(xptr + 1) * *(yptr + 2) - *(xptr + 2) * *(yptr + 1);
	*(ptr + 1) = *(xptr + 2) * *(yptr + 0) - *(xptr + 0) * *(yptr + 2);
	*(ptr + 2) = *(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0);
	return ret;
}

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 4, 1> cool::vxv(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 4, 1> ret;
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();
	Ty* ptr = ret.data();

	*(ptr + 0) = *(xptr + 1) * *(yptr + 2) - *(xptr + 2) * *(yptr + 1);
	*(ptr + 1) = *(xptr + 2) * *(yptr + 0) - *(xptr + 0) * *(yptr + 2);
	*(ptr + 2) = *(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0);
	*(ptr + 3) = static_cast<Ty>(1);
	return ret;
}

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 4, 1> cool::qxq(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 4, 1> ret;
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();
	Ty* ptr = ret.data();

	const Ty x0 = *(xptr + 0);
	const Ty x1 = *(xptr + 1);
	const Ty x2 = *(xptr + 2);
	const Ty x3 = *(xptr + 3);

	*(ptr + 0) = x0 * *(yptr + 0) - x1 * *(yptr + 1) - x2 * *(yptr + 2) - x3 * *(yptr + 3);
	*(ptr + 1) = x0 * *(yptr + 1) + x1 * *(yptr + 0) - x2 * *(yptr + 3) + x3 * *(yptr + 2);
	*(ptr + 2) = x0 * *(yptr + 2) + x1 * *(yptr + 3) + x2 * *(yptr + 0) - x3 * *(yptr + 1);
	*(ptr + 3) = x0 * *(yptr + 3) - x1 * *(yptr + 2) + x2 * *(yptr + 1) + x3 * *(yptr + 0);
	return ret;
}

template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
inline Ty cool::det(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept
{
	return cool::lu(A).det();
}

template <class Ty, std::size_t _rows_padded> inline Ty cool::det(const cool::matrix<Ty, 1, 1, _rows_padded>& A) noexcept
{
	return A[0];
}

template <class Ty, std::size_t _rows_padded> inline Ty cool::det(const cool::matrix<Ty, 2, 2, _rows_padded>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _rows_padded * 0;
	constexpr std::size_t i10 = 1 + _rows_padded * 0;
	constexpr std::size_t i01 = 0 + _rows_padded * 1;
	constexpr std::size_t i11 = 1 + _rows_padded * 1;
	const Ty* ptr = A.data();

	return *(ptr + i00) * *(ptr + i11) - *(ptr + i10) * *(ptr + i01);
}

template <class Ty, std::size_t _rows_padded> inline Ty cool::det(const cool::matrix<Ty, 3, 3, _rows_padded>& A) noexcept
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

template <class Ty, std::size_t _rows_padded> inline Ty cool::det(const cool::matrix<Ty, 4, 4, _rows_padded>& A) noexcept
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

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline Ty cool::det2v(const cool::matrix<Ty, 2, 1, _x_rows_padded>& x, const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept
{
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();

	return *(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0);
}

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded, std::size_t _z_rows_padded>
inline Ty cool::det3v(const cool::matrix<Ty, 3, 1, _x_rows_padded>& x, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y,
	const cool::matrix<Ty, 3, 1, _z_rows_padded>& z) noexcept
{
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();
	const Ty* zptr = z.data();

	return *(xptr + 0) * (*(yptr + 1) * *(zptr + 2) - *(yptr + 2) * *(zptr + 1))
		+ *(xptr + 1) * (*(yptr + 2) * *(zptr + 0) - *(yptr + 0) * *(zptr + 2))
		+ *(xptr + 2) * (*(yptr + 0) * *(zptr + 1) - *(yptr + 1) * *(zptr + 0));
}

template <class Ty, std::size_t _x_rows_padded, std::size_t _y_rows_padded, std::size_t _z_rows_padded, std::size_t _w_rows_padded>
inline Ty cool::det4v(const cool::matrix<Ty, 4, 1, _x_rows_padded>& x, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y,
	const cool::matrix<Ty, 4, 1, _z_rows_padded>& z, const cool::matrix<Ty, 4, 1, _w_rows_padded>& w) noexcept
{
	const Ty* xptr = x.data();
	const Ty* yptr = y.data();
	const Ty* zptr = z.data();
	const Ty* wptr = w.data();

	return (*(xptr + 0) * *(yptr + 1) - *(xptr + 1) * *(yptr + 0)) * (*(zptr + 2) * *(wptr + 3) - *(zptr + 3) * *(wptr + 2))
		- (*(xptr + 0) * *(yptr + 2) - *(xptr + 2) * *(yptr + 0)) * (*(zptr + 1) * *(wptr + 3) - *(zptr + 3) * *(wptr + 1))
		+ (*(xptr + 0) * *(yptr + 3) - *(xptr + 3) * *(yptr + 0)) * (*(zptr + 1) * *(wptr + 2) - *(zptr + 2) * *(wptr + 1))
		+ (*(xptr + 1) * *(yptr + 2) - *(xptr + 2) * *(yptr + 1)) * (*(zptr + 0) * *(wptr + 3) - *(zptr + 3) * *(wptr + 0))
		- (*(xptr + 1) * *(yptr + 3) - *(xptr + 3) * *(yptr + 1)) * (*(zptr + 0) * *(wptr + 2) - *(zptr + 2) * *(wptr + 0))
		+ (*(xptr + 2) * *(yptr + 3) - *(xptr + 3) * *(yptr + 2)) * (*(zptr + 0) * *(wptr + 1) - *(zptr + 1) * *(wptr + 0));
}

template <class Ty, std::size_t Ar, std::size_t Yc, std::size_t _A_rows_padded, std::size_t _Y_rows_padded>
inline cool::matrix<Ty, Ar, Yc> cool::solve(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A, const cool::matrix<Ty, Ar, Yc, _Y_rows_padded>& Y) noexcept
{
	return cool::lu(A).solve(Y);
}

template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 1, 1> cool::solve(const cool::matrix<Ty, 1, 1, _A_rows_padded>& A, const cool::matrix<Ty, 1, 1, _y_rows_padded>& y) noexcept
{
	return cool::matrix<Ty, 1, 1>(y[0] / A[0]);
}

template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 2, 1> cool::solve(const cool::matrix<Ty, 2, 2, _A_rows_padded>& A, const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept
{
	constexpr std::size_t i00 = 0 + _A_rows_padded * 0;
	constexpr std::size_t i10 = 1 + _A_rows_padded * 0;
	constexpr std::size_t i01 = 0 + _A_rows_padded * 1;
	constexpr std::size_t i11 = 1 + _A_rows_padded * 1;

	cool::matrix<Ty, 2, 1> x;
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01));

	*(xptr + 0) = d * (*(Aptr + i11) * *(yptr + 0) - *(Aptr + i01) * *(yptr + 1));
	*(xptr + 1) = d * (*(Aptr + i00) * *(yptr + 1) - *(Aptr + i10) * *(yptr + 0));

	return x;
}

template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 3, 1> cool::solve(const cool::matrix<Ty, 3, 3, _A_rows_padded>& A, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept
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

	cool::matrix<Ty, 3, 1> x;
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

	Ty cof0 = *(Aptr + i11) * *(Aptr + i22) - *(Aptr + i21) * *(Aptr + i12);
	Ty cof1 = *(Aptr + i21) * *(Aptr + i02) - *(Aptr + i01) * *(Aptr + i22);
	Ty cof2 = *(Aptr + i01) * *(Aptr + i12) - *(Aptr + i11) * *(Aptr + i02);

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * cof0 + *(Aptr + i10) * cof1 + *(Aptr + i20) * cof2);

	*(xptr + 0) = d * (cof0 * *(yptr + 0) + cof1 * *(yptr + 1) + cof2 * *(yptr + 2));

	*(xptr + 1) = d * ((*(Aptr + i20) * *(Aptr + i12) - *(Aptr + i10) * *(Aptr + i22)) * *(yptr + 0)
		+ (*(Aptr + i00) * *(Aptr + i22) - *(Aptr + i20) * *(Aptr + i02)) * *(yptr + 1)
		+ (*(Aptr + i10) * *(Aptr + i02) - *(Aptr + i00) * *(Aptr + i12)) * *(yptr + 2));

	*(xptr + 2) = d * ((*(Aptr + i10) * *(Aptr + i21) - *(Aptr + i20) * *(Aptr + i11)) * *(yptr + 0)
		+ (*(Aptr + i20) * *(Aptr + i01) - *(Aptr + i00) * *(Aptr + i21)) * *(yptr + 1)
		+ (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01)) * *(yptr + 2));

	return x;
}

template <class Ty, std::size_t _A_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 4, 1> cool::solve(const cool::matrix<Ty, 4, 4, _A_rows_padded>& A, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept
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

	cool::matrix<Ty, 4, 1> x;
	const Ty* Aptr = A.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

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

	*(xptr + 0) = d * ((*(Aptr + i11) * temp_23_23 - *(Aptr + i12) * temp_23_13 + *(Aptr + i13) * temp_23_12) * *(yptr + 0)
		- (*(Aptr + i01) * temp_23_23 - *(Aptr + i02) * temp_23_13 + *(Aptr + i03) * temp_23_12) * *(yptr + 1)
		+ (*(Aptr + i31) * temp_01_23 - *(Aptr + i32) * temp_01_13 + *(Aptr + i33) * temp_01_12) * *(yptr + 2)
		- (*(Aptr + i21) * temp_01_23 - *(Aptr + i22) * temp_01_13 + *(Aptr + i23) * temp_01_12) * *(yptr + 3));

	*(xptr + 1) = d * (-(*(Aptr + i10) * temp_23_23 - *(Aptr + i12) * temp_23_03 + *(Aptr + i13) * temp_23_02) * *(yptr + 0)
		+ (*(Aptr + i00) * temp_23_23 - *(Aptr + i02) * temp_23_03 + *(Aptr + i03) * temp_23_02) * *(yptr + 1)
		- (*(Aptr + i30) * temp_01_23 - *(Aptr + i32) * temp_01_03 + *(Aptr + i33) * temp_01_02) * *(yptr + 2)
		+ (*(Aptr + i20) * temp_01_23 - *(Aptr + i22) * temp_01_03 + *(Aptr + i23) * temp_01_02) * *(yptr + 3));

	*(xptr + 2) = d * ((*(Aptr + i10) * temp_23_13 - *(Aptr + i11) * temp_23_03 + *(Aptr + i13) * temp_23_01) * *(yptr + 0)
		- (*(Aptr + i00) * temp_23_13 - *(Aptr + i01) * temp_23_03 + *(Aptr + i03) * temp_23_01) * *(yptr + 1)
		+ (*(Aptr + i30) * temp_01_13 - *(Aptr + i31) * temp_01_03 + *(Aptr + i33) * temp_01_01) * *(yptr + 2)
		- (*(Aptr + i20) * temp_01_13 - *(Aptr + i21) * temp_01_03 + *(Aptr + i23) * temp_01_01) * *(yptr + 3));

	*(xptr + 3) = d * (-(*(Aptr + i10) * temp_23_12 - *(Aptr + i11) * temp_23_02 + *(Aptr + i12) * temp_23_01) * *(yptr + 0)
		+ (*(Aptr + i00) * temp_23_12 - *(Aptr + i01) * temp_23_02 + *(Aptr + i02) * temp_23_01) * *(yptr + 1)
		- (*(Aptr + i30) * temp_01_12 - *(Aptr + i31) * temp_01_02 + *(Aptr + i32) * temp_01_01) * *(yptr + 2)
		+ (*(Aptr + i20) * temp_01_12 - *(Aptr + i21) * temp_01_02 + *(Aptr + i22) * temp_01_01) * *(yptr + 3));

	return x;
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 2, 1> cool::solve2v(const cool::matrix<Ty, 2, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 2, 1, _a1_rows_padded>& a1,
	const cool::matrix<Ty, 2, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 2, 1> x;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	*(xptr + 0) = d * (*(a1ptr + 1) * *(yptr + 0) - *(a1ptr + 0) * *(yptr + 1));
	*(xptr + 1) = d * (*(a0ptr + 0) * *(yptr + 1) - *(a0ptr + 1) * *(yptr + 0));

	return x;
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 3, 1> cool::solve3v(const cool::matrix<Ty, 3, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 3, 1, _a1_rows_padded>& a1,
	const cool::matrix<Ty, 3, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 3, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 3, 1> x;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

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

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded, std::size_t _y_rows_padded>
inline cool::matrix<Ty, 4, 1> cool::solve4v(const cool::matrix<Ty, 4, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 4, 1, _a1_rows_padded>& a1,
	const cool::matrix<Ty, 4, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 4, 1, _a3_rows_padded>& a3, const cool::matrix<Ty, 4, 1, _y_rows_padded>& y) noexcept
{
	cool::matrix<Ty, 4, 1> x;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* a3ptr = a3.data();
	const Ty* yptr = y.data();
	Ty* xptr = x.data();

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

template <class Ty, std::size_t _A_rows_padded>
inline cool::matrix<Ty, 1, 1> cool::inv(const cool::matrix<Ty, 1, 1, _A_rows_padded>& A) noexcept
{
	return cool::matrix<Ty, 1, 1>(static_cast<Ty>(1) / A[0]);
}

template <class Ty, std::size_t _A_rows_padded>
inline cool::matrix<Ty, 2, 2> cool::inv(const cool::matrix<Ty, 2, 2, _A_rows_padded>& A) noexcept
{
	constexpr std::size_t i00 = 0 + _A_rows_padded * 0;
	constexpr std::size_t i10 = 1 + _A_rows_padded * 0;
	constexpr std::size_t i01 = 0 + _A_rows_padded * 1;
	constexpr std::size_t i11 = 1 + _A_rows_padded * 1;

	constexpr std::size_t j00 = 0 + 2 * 0;
	constexpr std::size_t j10 = 1 + 2 * 0;
	constexpr std::size_t j01 = 0 + 2 * 1;
	constexpr std::size_t j11 = 1 + 2 * 1;

	cool::matrix<Ty, 2, 2> C;
	const Ty* Aptr = A.data();
	Ty* Cptr = C.data();

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01));

	*(Cptr + j00) = d * *(Aptr + i11);
	*(Cptr + j10) = -d * *(Aptr + i10);
	*(Cptr + j01) = -d * *(Aptr + i01);
	*(Cptr + j11) = d * *(Aptr + i00);

	return C;
}

template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
inline cool::matrix<Ty, Ar, Ar> cool::inv(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept
{
	return cool::lu(A).inv();
}

template <class Ty, std::size_t _A_rows_padded>
inline cool::matrix<Ty, 3, 3> cool::inv(const cool::matrix<Ty, 3, 3, _A_rows_padded>& A) noexcept
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

	constexpr std::size_t j00 = 0 + 3 * 0;
	constexpr std::size_t j10 = 1 + 3 * 0;
	constexpr std::size_t j20 = 2 + 3 * 0;
	constexpr std::size_t j01 = 0 + 3 * 1;
	constexpr std::size_t j11 = 1 + 3 * 1;
	constexpr std::size_t j21 = 2 + 3 * 1;
	constexpr std::size_t j02 = 0 + 3 * 2;
	constexpr std::size_t j12 = 1 + 3 * 2;
	constexpr std::size_t j22 = 2 + 3 * 2;

	cool::matrix<Ty, 3, 3> C;
	const Ty* Aptr = A.data();
	Ty* Cptr = C.data();

	Ty cof0 = *(Aptr + i11) * *(Aptr + i22) - *(Aptr + i21) * *(Aptr + i12);
	Ty cof1 = *(Aptr + i21) * *(Aptr + i02) - *(Aptr + i01) * *(Aptr + i22);
	Ty cof2 = *(Aptr + i01) * *(Aptr + i12) - *(Aptr + i11) * *(Aptr + i02);

	Ty d = static_cast<Ty>(1) / (*(Aptr + i00) * cof0 + *(Aptr + i10) * cof1 + *(Aptr + i20) * cof2);

	*(Cptr + j00) = d * cof0;
	*(Cptr + j01) = d * cof1;
	*(Cptr + j02) = d * cof2;

	*(Cptr + j10) = d * (*(Aptr + i20) * *(Aptr + i12) - *(Aptr + i10) * *(Aptr + i22));
	*(Cptr + j11) = d * (*(Aptr + i00) * *(Aptr + i22) - *(Aptr + i20) * *(Aptr + i02));
	*(Cptr + j12) = d * (*(Aptr + i10) * *(Aptr + i02) - *(Aptr + i00) * *(Aptr + i12));

	*(Cptr + j20) = d * (*(Aptr + i10) * *(Aptr + i21) - *(Aptr + i20) * *(Aptr + i11));
	*(Cptr + j21) = d * (*(Aptr + i20) * *(Aptr + i01) - *(Aptr + i00) * *(Aptr + i21));
	*(Cptr + j22) = d * (*(Aptr + i00) * *(Aptr + i11) - *(Aptr + i10) * *(Aptr + i01));

	return C;
}

template <class Ty, std::size_t _A_rows_padded>
inline cool::matrix<Ty, 4, 4> cool::inv(const cool::matrix<Ty, 4, 4, _A_rows_padded>& A) noexcept
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

	constexpr std::size_t j00 = 0 + 4 * 0;
	constexpr std::size_t j10 = 1 + 4 * 0;
	constexpr std::size_t j20 = 2 + 4 * 0;
	constexpr std::size_t j30 = 3 + 4 * 0;
	constexpr std::size_t j01 = 0 + 4 * 1;
	constexpr std::size_t j11 = 1 + 4 * 1;
	constexpr std::size_t j21 = 2 + 4 * 1;
	constexpr std::size_t j31 = 3 + 4 * 1;
	constexpr std::size_t j02 = 0 + 4 * 2;
	constexpr std::size_t j12 = 1 + 4 * 2;
	constexpr std::size_t j22 = 2 + 4 * 2;
	constexpr std::size_t j32 = 3 + 4 * 2;
	constexpr std::size_t j03 = 0 + 4 * 3;
	constexpr std::size_t j13 = 1 + 4 * 3;
	constexpr std::size_t j23 = 2 + 4 * 3;
	constexpr std::size_t j33 = 3 + 4 * 3;

	cool::matrix<Ty, 4, 4> C;
	const Ty* Aptr = A.data();
	Ty* Cptr = C.data();

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

	*(Cptr + j00) = d * (*(Aptr + i11) * temp_23_23 - *(Aptr + i12) * temp_23_13 + *(Aptr + i13) * temp_23_12);
	*(Cptr + j01) = -d * (*(Aptr + i01) * temp_23_23 - *(Aptr + i02) * temp_23_13 + *(Aptr + i03) * temp_23_12);
	*(Cptr + j02) = d * (*(Aptr + i31) * temp_01_23 - *(Aptr + i32) * temp_01_13 + *(Aptr + i33) * temp_01_12);
	*(Cptr + j03) = -d * (*(Aptr + i21) * temp_01_23 - *(Aptr + i22) * temp_01_13 + *(Aptr + i23) * temp_01_12);

	*(Cptr + j10) = -d * (*(Aptr + i10) * temp_23_23 - *(Aptr + i12) * temp_23_03 + *(Aptr + i13) * temp_23_02);
	*(Cptr + j11) = d * (*(Aptr + i00) * temp_23_23 - *(Aptr + i02) * temp_23_03 + *(Aptr + i03) * temp_23_02);
	*(Cptr + j12) = -d * (*(Aptr + i30) * temp_01_23 - *(Aptr + i32) * temp_01_03 + *(Aptr + i33) * temp_01_02);
	*(Cptr + j13) = d * (*(Aptr + i20) * temp_01_23 - *(Aptr + i22) * temp_01_03 + *(Aptr + i23) * temp_01_02);

	*(Cptr + j20) = d * (*(Aptr + i10) * temp_23_13 - *(Aptr + i11) * temp_23_03 + *(Aptr + i13) * temp_23_01);
	*(Cptr + j21) = -d * (*(Aptr + i00) * temp_23_13 - *(Aptr + i01) * temp_23_03 + *(Aptr + i03) * temp_23_01);
	*(Cptr + j22) = d * (*(Aptr + i30) * temp_01_13 - *(Aptr + i31) * temp_01_03 + *(Aptr + i33) * temp_01_01);
	*(Cptr + j23) = -d * (*(Aptr + i20) * temp_01_13 - *(Aptr + i21) * temp_01_03 + *(Aptr + i23) * temp_01_01);

	*(Cptr + j30) = -d * (*(Aptr + i10) * temp_23_12 - *(Aptr + i11) * temp_23_02 + *(Aptr + i12) * temp_23_01);
	*(Cptr + j31) = d * (*(Aptr + i00) * temp_23_12 - *(Aptr + i01) * temp_23_02 + *(Aptr + i02) * temp_23_01);
	*(Cptr + j32) = -d * (*(Aptr + i30) * temp_01_12 - *(Aptr + i31) * temp_01_02 + *(Aptr + i32) * temp_01_01);
	*(Cptr + j33) = d * (*(Aptr + i20) * temp_01_12 - *(Aptr + i21) * temp_01_02 + *(Aptr + i22) * temp_01_01);

	return C;
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded>
inline cool::matrix<Ty, 2, 2> cool::inv2v(const cool::matrix<Ty, 2, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 2, 1, _a1_rows_padded>& a1) noexcept
{
	constexpr std::size_t j00 = 0 + 2 * 0;
	constexpr std::size_t j10 = 1 + 2 * 0;
	constexpr std::size_t j01 = 0 + 2 * 1;
	constexpr std::size_t j11 = 1 + 2 * 1;

	cool::matrix<Ty, 2, 2> C;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	Ty* Cptr = C.data();

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	*(Cptr + j00) = d * *(a1ptr + 1);
	*(Cptr + j10) = -d * *(a0ptr + 1);
	*(Cptr + j01) = -d * *(a1ptr + 0);
	*(Cptr + j11) = d * *(a0ptr + 0);

	return C;
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded>
inline cool::matrix<Ty, 3, 3> cool::inv3v(const cool::matrix<Ty, 3, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 3, 1, _a1_rows_padded>& a1,
	const cool::matrix<Ty, 3, 1, _a2_rows_padded>& a2) noexcept
{
	constexpr std::size_t j00 = 0 + 3 * 0;
	constexpr std::size_t j10 = 1 + 3 * 0;
	constexpr std::size_t j20 = 2 + 3 * 0;
	constexpr std::size_t j01 = 0 + 3 * 1;
	constexpr std::size_t j11 = 1 + 3 * 1;
	constexpr std::size_t j21 = 2 + 3 * 1;
	constexpr std::size_t j02 = 0 + 3 * 2;
	constexpr std::size_t j12 = 1 + 3 * 2;
	constexpr std::size_t j22 = 2 + 3 * 2;

	cool::matrix<Ty, 3, 3> C;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	Ty* Cptr = C.data();

	Ty cof0 = *(a1ptr + 1) * *(a2ptr + 2) - *(a1ptr + 2) * *(a2ptr + 1);
	Ty cof1 = *(a1ptr + 2) * *(a2ptr + 0) - *(a1ptr + 0) * *(a2ptr + 2);
	Ty cof2 = *(a1ptr + 0) * *(a2ptr + 1) - *(a1ptr + 1) * *(a2ptr + 0);

	Ty d = static_cast<Ty>(1) / (*(a0ptr + 0) * cof0 + *(a0ptr + 1) * cof1 + *(a0ptr + 2) * cof2);

	*(Cptr + j00) = d * cof0;
	*(Cptr + j01) = d * cof1;
	*(Cptr + j02) = d * cof2;

	*(Cptr + j10) = d * (*(a0ptr + 2) * *(a2ptr + 1) - *(a0ptr + 1) * *(a2ptr + 2));
	*(Cptr + j11) = d * (*(a0ptr + 0) * *(a2ptr + 2) - *(a0ptr + 2) * *(a2ptr + 0));
	*(Cptr + j12) = d * (*(a0ptr + 1) * *(a2ptr + 0) - *(a0ptr + 0) * *(a2ptr + 1));

	*(Cptr + j20) = d * (*(a0ptr + 1) * *(a1ptr + 2) - *(a0ptr + 2) * *(a1ptr + 1));
	*(Cptr + j21) = d * (*(a0ptr + 2) * *(a1ptr + 0) - *(a0ptr + 0) * *(a1ptr + 2));
	*(Cptr + j22) = d * (*(a0ptr + 0) * *(a1ptr + 1) - *(a0ptr + 1) * *(a1ptr + 0));

	return C;
}

template <class Ty, std::size_t _a0_rows_padded, std::size_t _a1_rows_padded, std::size_t _a2_rows_padded, std::size_t _a3_rows_padded>
inline cool::matrix<Ty, 4, 4> cool::inv4v(const cool::matrix<Ty, 4, 1, _a0_rows_padded>& a0, const cool::matrix<Ty, 4, 1, _a1_rows_padded>& a1,
	const cool::matrix<Ty, 4, 1, _a2_rows_padded>& a2, const cool::matrix<Ty, 4, 1, _a3_rows_padded>& a3) noexcept
{
	constexpr std::size_t j00 = 0 + 4 * 0;
	constexpr std::size_t j10 = 1 + 4 * 0;
	constexpr std::size_t j20 = 2 + 4 * 0;
	constexpr std::size_t j30 = 3 + 4 * 0;
	constexpr std::size_t j01 = 0 + 4 * 1;
	constexpr std::size_t j11 = 1 + 4 * 1;
	constexpr std::size_t j21 = 2 + 4 * 1;
	constexpr std::size_t j31 = 3 + 4 * 1;
	constexpr std::size_t j02 = 0 + 4 * 2;
	constexpr std::size_t j12 = 1 + 4 * 2;
	constexpr std::size_t j22 = 2 + 4 * 2;
	constexpr std::size_t j32 = 3 + 4 * 2;
	constexpr std::size_t j03 = 0 + 4 * 3;
	constexpr std::size_t j13 = 1 + 4 * 3;
	constexpr std::size_t j23 = 2 + 4 * 3;
	constexpr std::size_t j33 = 3 + 4 * 3;

	cool::matrix<Ty, 4, 4> C;
	const Ty* a0ptr = a0.data();
	const Ty* a1ptr = a1.data();
	const Ty* a2ptr = a2.data();
	const Ty* a3ptr = a3.data();
	Ty* Cptr = C.data();

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

	*(Cptr + j00) = d * (*(a1ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_13 + *(a3ptr + 1) * temp_23_12);
	*(Cptr + j01) = -d * (*(a1ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_13 + *(a3ptr + 0) * temp_23_12);
	*(Cptr + j02) = d * (*(a1ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_13 + *(a3ptr + 3) * temp_01_12);
	*(Cptr + j03) = -d * (*(a1ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_13 + *(a3ptr + 2) * temp_01_12);

	*(Cptr + j10) = -d * (*(a0ptr + 1) * temp_23_23 - *(a2ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_02);
	*(Cptr + j11) = d * (*(a0ptr + 0) * temp_23_23 - *(a2ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_02);
	*(Cptr + j12) = -d * (*(a0ptr + 3) * temp_01_23 - *(a2ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_02);
	*(Cptr + j13) = d * (*(a0ptr + 2) * temp_01_23 - *(a2ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_02);

	*(Cptr + j20) = d * (*(a0ptr + 1) * temp_23_13 - *(a1ptr + 1) * temp_23_03 + *(a3ptr + 1) * temp_23_01);
	*(Cptr + j21) = -d * (*(a0ptr + 0) * temp_23_13 - *(a1ptr + 0) * temp_23_03 + *(a3ptr + 0) * temp_23_01);
	*(Cptr + j22) = d * (*(a0ptr + 3) * temp_01_13 - *(a1ptr + 3) * temp_01_03 + *(a3ptr + 3) * temp_01_01);
	*(Cptr + j23) = -d * (*(a0ptr + 2) * temp_01_13 - *(a1ptr + 2) * temp_01_03 + *(a3ptr + 2) * temp_01_01);

	*(Cptr + j30) = -d * (*(a0ptr + 1) * temp_23_12 - *(a1ptr + 1) * temp_23_02 + *(a2ptr + 1) * temp_23_01);
	*(Cptr + j31) = d * (*(a0ptr + 0) * temp_23_12 - *(a1ptr + 0) * temp_23_02 + *(a2ptr + 0) * temp_23_01);
	*(Cptr + j32) = -d * (*(a0ptr + 3) * temp_01_12 - *(a1ptr + 3) * temp_01_02 + *(a2ptr + 3) * temp_01_01);
	*(Cptr + j33) = d * (*(a0ptr + 2) * temp_01_12 - *(a1ptr + 2) * temp_01_02 + *(a2ptr + 2) * temp_01_01);

	return C;
}

template <class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _J_rows_padded, std::size_t _x_rows_padded>
inline int cool::nonlinear_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	Ty tol, int iter)
{
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x);
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
			x -= cool::solve(Jfn(x), fx);
			fx = fn(x);
		}
	}

	*xptr = x;

	return iter;
}

template <class Ty, std::size_t dim, std::size_t _fx_rows_padded, std::size_t _J_rows_padded,
	std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline int cool::nonlinear_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
	Ty tol, int iter)
{
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x) - y;
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
			x -= cool::solve(Jfn(x), fx);
			fx = fn(x) - y;
		}
	}

	*xptr = x;

	return iter;
}

template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _J_rows_padded, std::size_t _x_rows_padded>
inline int cool::nonlinear_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	Ty tol, int iter, params_Ty params)
{
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x, params);
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
			x -= cool::solve(Jfn(x, params), fx);
			fx = fn(x, params);
		}
	}

	*xptr = x;

	return iter;
}

template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _J_rows_padded, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline int cool::nonlinear_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, dim, _J_rows_padded>(*Jfn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
	Ty tol, int iter, params_Ty params)
{
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x, params) - y;
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
			x -= cool::solve(Jfn(x, params), fx);
			fx = fn(x, params) - y;
		}
	}

	*xptr = x;

	return iter;
}

template <class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded>
inline int cool::nonlinear_qn_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	Ty tol, int iter)
{
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded> Jinv = *Jinvptr;
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x);
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
			fx -= fn(x -= s);
			cool::matrix<Ty, dim, 1, dim> Jinvfx = Jinv * fx;
			cool::matrix<Ty, dim, 1, dim> stemp = (static_cast<Ty>(1)
				/ cool::dot(Jinvfx, s)) * (s - Jinvfx);
			Jinv.fmadd(stemp, (reinterpret_cast<cool::matrix<Ty, 1, dim, 1>&>(s) * Jinv));
			fx = fn(x);
		}
	}

	*xptr = x;
	*Jinvptr = Jinv;

	return iter;
}

template <class Ty, std::size_t dim, std::size_t _fx_rows_padded, std::size_t _Jinv_rows_padded,
	std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline int cool::nonlinear_qn_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&),
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
	Ty tol, int iter)
{
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded> Jinv = *Jinvptr;
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x);
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
			fx -= fn(x -= s) - y;
			cool::matrix<Ty, dim, 1, dim> Jinvfx = Jinv * fx;
			cool::matrix<Ty, dim, 1, dim> stemp = (static_cast<Ty>(1)
				/ cool::dot(Jinvfx, s)) * (s - Jinvfx);
			Jinv.fmadd(stemp, (reinterpret_cast<cool::matrix<Ty, 1, dim, 1>&>(s) * Jinv));
			fx = fn(x) - y;
		}
	}

	*xptr = x;
	*Jinvptr = Jinv;

	return iter;
}

template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded>
inline int cool::nonlinear_qn_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	Ty tol, int iter, params_Ty params)
{
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded> Jinv = *Jinvptr;
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x, params);
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
			fx -= fn(x -= s, params);
			cool::matrix<Ty, dim, 1, dim> Jinvfx = Jinv * fx;
			cool::matrix<Ty, dim, 1, dim> stemp = (static_cast<Ty>(1)
				/ cool::dot(Jinvfx, s)) * (s - Jinvfx);
			Jinv.fmadd(stemp, (reinterpret_cast<cool::matrix<Ty, 1, dim, 1>&>(s) * Jinv));
			fx = fn(x, params);
		}
	}

	*xptr = x;
	*Jinvptr = Jinv;

	return iter;
}

template <class params_Ty, class Ty, std::size_t dim, std::size_t _fx_rows_padded,
	std::size_t _Jinv_rows_padded, std::size_t _x_rows_padded, std::size_t _y_rows_padded>
inline int cool::nonlinear_qn_solve(
	cool::matrix<Ty, dim, 1, _fx_rows_padded>(*fn)(const cool::matrix<Ty, dim, 1, _x_rows_padded>&, params_Ty),
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded>* Jinvptr,
	cool::matrix<Ty, dim, 1, _x_rows_padded>* xptr,
	const cool::matrix<Ty, dim, 1, _y_rows_padded>& y,
	Ty tol, int iter, params_Ty params)
{
	cool::matrix<Ty, dim, dim, _Jinv_rows_padded> Jinv = *Jinvptr;
	cool::matrix<Ty, dim, 1, _x_rows_padded> x = *xptr;
	cool::matrix<Ty, dim, 1, _fx_rows_padded> fx = fn(x, params);
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
			fx -= fn(x -= s, params) - y;
			cool::matrix<Ty, dim, 1, dim> Jinvfx = Jinv * fx;
			cool::matrix<Ty, dim, 1, dim> stemp = (static_cast<Ty>(1)
				/ cool::dot(Jinvfx, s)) * (s - Jinvfx);
			Jinv.fmadd(stemp, (reinterpret_cast<cool::matrix<Ty, 1, dim, 1>&>(s) * Jinv));
			fx = fn(x, params) - y;
		}
	}

	*xptr = x;
	*Jinvptr = Jinv;

	return iter;
}

template <class Ty, std::size_t Ar>
inline Ty cool::lu_matrix<Ty, Ar>::det() const noexcept
{
	constexpr bool _size_lt_2 = Ar < 2;
	constexpr bool _size_lt_8 = Ar < 8;

	if (_size_lt_2)
	{
		Ty ret = *(this->data());
		return flipped ? -ret : ret;
	}
	else if (_size_lt_8)
	{
		constexpr std::size_t Ar_mult2 = Ar - Ar % 2;
		constexpr bool Ar_odd = Ar % 2 != 0;

		const Ty* ptr = this->data();

		Ty acc0 = *ptr;
		Ty acc1 = *(ptr + (Ar + 1));

		for (std::size_t k = 2; k < Ar_mult2; k += 2)
		{
			acc0 *= *(ptr + (Ar + 1) * k);
			acc1 *= *(ptr + (Ar + 1) * (k + 1));
		}
		if (Ar_odd)
		{
			acc0 *= *(ptr + (Ar + 1) * Ar_mult2);
		}

		Ty ret = acc0 * acc1;
		return flipped ? -ret : ret;
	}
	else
	{
		constexpr std::size_t Ar_mult4 = Ar - Ar % 4;
		constexpr std::size_t remainder = Ar % 4;

		const Ty* ptr = this->data();

		Ty acc[4] = { *ptr,
			*(ptr + (Ar + 1)),
			*(ptr + (Ar + 1) * 2),
			*(ptr + (Ar + 1) * 3)
		};

		for (std::size_t n = 4; n < Ar_mult4; n += 4)
		{
			for (std::size_t m = 0; m < 4; m++)
			{
				acc[m] *= *(ptr + (Ar + 1) * (n + m));
			}
		}
		for (std::size_t m = 0; m < remainder; m++)
		{
			acc[m] *= *(ptr + (Ar + 1) * (Ar_mult4 + m));
		}

		Ty ret = (acc[0] * acc[1]) * (acc[2] * acc[3]);
		return flipped ? -ret : ret;
	}
}

template <class Ty, std::size_t Ar>
inline Ty cool::lu_matrix<Ty, Ar>::diag_ratio() const noexcept
{
	constexpr bool _size_lt_2 = Ar < 2;

	if (_size_lt_2)
	{
		return (*(this->data()) != static_cast<Ty>(0)) ? static_cast<Ty>(1) : static_cast<Ty>(0);
	}
	else
	{
		std::size_t Ar_m1 = Ar - 1;

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
		for (std::size_t k = 1; k < Ar_m1; k++)
		{
			Ty temp = *(ptr + (Ar + 1) * k);
			auto temp_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(temp);

			bool new_min = temp_sq < min_value_sq;
			min_value = new_min ? temp : min_value;
			min_value_sq = new_min ? temp_sq : min_value_sq;

			bool new_max = max_value_sq < temp_sq;
			max_value = new_max ? temp : max_value;
			max_value_sq = new_max ? temp_sq : max_value_sq;
		}

		Ty temp = *(ptr + (Ar + 1) * Ar_m1);
		auto temp_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(temp);

		min_value = (temp_sq < min_value_sq) ? temp : min_value;
		max_value = (max_value_sq < temp_sq) ? temp : max_value;

		return (max_value != static_cast<Ty>(0)) ? (min_value / max_value) : static_cast<Ty>(0);
	}
}

template <class Ty, std::size_t Ar> template <std::size_t Yc, std::size_t _Y_rows_padded>
inline cool::matrix<Ty, Ar, Yc> cool::lu_matrix<Ty, Ar>::solve(const cool::matrix<Ty, Ar, Yc, _Y_rows_padded>& Y) const noexcept
{
	cool::matrix<Ty, Ar, Yc> X;

	for (std::size_t k = 0; k < Yc; k++)
	{
		Ty* ptrX = X.data() + k * Ar;
		const Ty* ptrY = Y.data() + k * Ar;
		for (std::size_t j = 0; j < Ar; j++)
		{
			*ptrX++ = *(ptrY + perm[j]);
		}
	}

	constexpr std::size_t Yc_mmod4 = Yc - Yc % 4;
	constexpr std::size_t remainder = Yc % 4;

	for (std::size_t k = 0; k < Yc_mmod4; k += 4)
	{
		for (std::size_t j = 0; j < Ar; j++)
		{
			Ty* ptrX = X.data() + j + Ar * k;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + Ar);
			Ty temp2 = *(ptrX + Ar * 2);
			Ty temp3 = *(ptrX + Ar * 3);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + Ar;
			Ty* ptrX2 = ptrX + 1 + Ar * 2;
			Ty* ptrX3 = ptrX + 1 + Ar * 3;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + Ar * j;
			for (; i < Ar; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
				*ptrX3++ -= temp3 * temp;
			}
		}

		for (std::size_t j = Ar; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + Ar * k;
			Ty* ptrX1 = ptrX0 + Ar;
			Ty* ptrX2 = ptrX0 + Ar * 2;
			Ty* ptrX3 = ptrX0 + Ar * 3;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (Ar + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + Ar) *= temp_inv);
			Ty temp2 = (*(ptrX + Ar * 2) *= temp_inv);
			Ty temp3 = (*(ptrX + Ar * 3) *= temp_inv);
			const Ty* ptrLU = this->data() + Ar * j;
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
		for (std::size_t j = 0; j < Ar; j++)
		{
			Ty* ptrX0 = X.data() + j + Ar * Yc_mmod4;
			Ty temp0 = *ptrX0++;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + Ar * j;
			for (; i < Ar; i++)
			{
				*ptrX0++ -= temp0 * *ptrLU++;
			}
		}

		for (std::size_t j = Ar; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + Ar * Yc_mmod4;
			Ty temp0 = (*(ptrX0 + j) /= *(this->data() + (Ar + 1) * j));
			const Ty* ptrLU = this->data() + Ar * j;
			for (std::size_t i = 0; i < j; i++)
			{
				*ptrX0++ -= temp0 * *ptrLU++;
			}
		}
		break;
	}

	case 2:
	{
		for (std::size_t j = 0; j < Ar; j++)
		{
			Ty* ptrX = X.data() + j + Ar * Yc_mmod4;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + Ar);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + Ar;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + Ar * j;
			for (; i < Ar; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
			}
		}

		for (std::size_t j = Ar; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + Ar * Yc_mmod4;
			Ty* ptrX1 = ptrX0 + Ar;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (Ar + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + Ar) *= temp_inv);
			const Ty* ptrLU = this->data() + Ar * j;
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
		for (std::size_t j = 0; j < Ar; j++)
		{

			Ty* ptrX = X.data() + j + Ar * Yc_mmod4;
			Ty temp0 = *ptrX;
			Ty temp1 = *(ptrX + Ar);
			Ty temp2 = *(ptrX + Ar * 2);
			Ty* ptrX0 = ptrX + 1;
			Ty* ptrX1 = ptrX + 1 + Ar;
			Ty* ptrX2 = ptrX + 1 + Ar * 2;
			std::size_t i = j + 1;
			const Ty* ptrLU = this->data() + i + Ar * j;
			for (; i < Ar; i++)
			{
				Ty temp = *ptrLU++;
				*ptrX0++ -= temp0 * temp;
				*ptrX1++ -= temp1 * temp;
				*ptrX2++ -= temp2 * temp;
			}
		}

		for (std::size_t j = Ar; j > 0; )
		{
			j--;
			Ty* ptrX0 = X.data() + Ar * Yc_mmod4;
			Ty* ptrX1 = ptrX0 + Ar;
			Ty* ptrX2 = ptrX0 + Ar * 2;
			Ty* ptrX = ptrX0 + j;
			Ty temp_inv = (static_cast<Ty>(1) / *(this->data() + (Ar + 1) * j));
			Ty temp0 = (*ptrX *= temp_inv);
			Ty temp1 = (*(ptrX + Ar) *= temp_inv);
			Ty temp2 = (*(ptrX + Ar * 2) *= temp_inv);
			const Ty* ptrLU = this->data() + Ar * j;
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

template <class Ty, std::size_t Ar>
inline cool::matrix<Ty, Ar, Ar> cool::lu_matrix<Ty, Ar>::inv() const noexcept
{
	cool::matrix<Ty, Ar, Ar> Id = static_cast<Ty>(0);

	Ty* ptrId = Id.data();
	for (std::size_t k = 0; k < Ar; k++)
	{
		*(ptrId + (Ar + 1) * k) = static_cast<Ty>(1);
	}

	return this->solve(Id);
}

template <class Ty, std::size_t Ar>
inline cool::matrix<Ty, Ar, Ar> cool::lu_matrix<Ty, Ar>::lower() const noexcept
{
	cool::matrix<Ty, Ar, Ar> X = static_cast<Ty>(0);

	for (std::size_t j = 0; j < Ar; j++)
	{
		Ty* ptrX = X.data() + (Ar + 1) * j;
		const Ty* ptrL = this->data() + 1 + (Ar + 1) * j;
		*ptrX++ = static_cast<Ty>(1);
		for (std::size_t i = j + 1; i < Ar; i++)
		{
			*ptrX++ = *ptrL++;
		}
	}

	return X;
}

template <class Ty, std::size_t Ar>
inline cool::matrix<Ty, Ar, Ar> cool::lu_matrix<Ty, Ar>::upper() const noexcept
{
	cool::matrix<Ty, Ar, Ar> X = static_cast<Ty>(0);

	for (std::size_t j = 0; j < Ar; j++)
	{
		Ty* ptrX = X.data() + Ar * j;
		const Ty* ptrL = this->data() + Ar * j;
		for (std::size_t i = 0; i <= j; i++)
		{
			*ptrX++ = *ptrL++;
		}
	}

	return X;
}

template <class Ty, std::size_t Ar, std::size_t _A_rows_padded>
inline cool::lu_matrix<Ty, Ar> cool::lu(const cool::matrix<Ty, Ar, Ar, _A_rows_padded>& A) noexcept
{
	constexpr std::size_t Ar_m1 = (Ar == 0) ? 0 : Ar - 1;

	cool::lu_matrix<Ty, Ar> LU = A;

	for (std::size_t n = 0; n < Ar; n++)
	{
		LU.perm[n] = n;
	}

	LU.flipped = false;

	for (std::size_t k = 0; k < Ar_m1; k++)
	{
		{
			Ty* ptrLU = LU.data() + k + Ar * k;
			auto piv_abs_sq = cool::matrix_scalar_subroutine::abs_sq<Ty>(*ptrLU++);
			std::size_t grt = k;
			bool permute = false;

			for (std::size_t j = k + 1; j < Ar; j++)
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
				for (std::size_t j = 0; j < Ar; j++)
				{
					Ty temp = *ptrLU1;
					*ptrLU1 = *ptrLU2;
					*ptrLU2 = temp;
					ptrLU1 += Ar;
					ptrLU2 += Ar;
				}

				{
					std::size_t temp = LU.perm[grt];
					LU.perm[grt] = LU.perm[k];
					LU.perm[k] = temp;
				}

				LU.flipped ^= true;
			}
		}

		Ty* ptrLU = LU.data() + k + Ar * k;

		if (*ptrLU != static_cast<Ty>(0))
		{
			{
				Ty temp = static_cast<Ty>(1) / *ptrLU++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < Ar; i++)
				{
					*ptrL++ *= temp;
				}
			}

			std::size_t j = k + 1;
			for (; j + 3 < Ar; j += 4)
			{
				Ty* ptr0 = LU.data() + k + Ar * j;
				Ty* ptr1 = ptr0 + Ar;
				Ty* ptr2 = ptr0 + Ar * 2;
				Ty* ptr3 = ptr0 + Ar * 3;
				Ty temp0 = *ptr0++;
				Ty temp1 = *ptr1++;
				Ty temp2 = *ptr2++;
				Ty temp3 = *ptr3++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < Ar; i++)
				{
					Ty temp = *ptrL++;
					*ptr0++ -= temp * temp0;
					*ptr1++ -= temp * temp1;
					*ptr2++ -= temp * temp2;
					*ptr3++ -= temp * temp3;
				}
			}
			for (; j < Ar; j++)
			{
				Ty* ptr0 = LU.data() + k + Ar * j;
				Ty temp0 = *ptr0++;
				Ty* ptrL = ptrLU;
				for (std::size_t i = k + 1; i < Ar; i++)
				{
					*ptr0++ -= *ptrL++ * temp0;
				}
			}
		}
	}

	return LU;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy cool::matrix<Ty, _rows, _cols, _rows_padded>::cm() noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy(static_cast<Ty*>(m_data));
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy cool::matrix<Ty, _rows, _cols, _rows_padded>::cm() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy(static_cast<const Ty*>(m_data));
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy cool::matrix<Ty, _rows, _cols, _rows_padded>::rm() noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy(static_cast<Ty*>(m_data));
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy cool::matrix<Ty, _rows, _cols, _rows_padded>::rm() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy(static_cast<const Ty*>(m_data));
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::begin() noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator(static_cast<Ty*>(m_data), 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::end() noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator(static_cast<Ty*>(m_data), _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::begin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(static_cast<const Ty*>(m_data), 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::end() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(static_cast<const Ty*>(m_data), _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cbegin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator(static_cast<const Ty*>(m_data), 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cend() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(static_cast<const Ty*>(m_data), _cols);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy::begin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy::end() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator(m_data_ptr, _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy::cbegin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator_proxy::cend() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy::begin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy::end() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy::cbegin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator_proxy::cend() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator(m_data_ptr, _cols);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy::begin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy::end() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator(m_data_ptr, _rows);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy::cbegin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator_proxy::cend() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, _rows);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy::begin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy::end() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, _rows);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy::cbegin() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, 0);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator_proxy::cend() const noexcept
{
	return cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator(m_data_ptr, _rows);
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::cm_iterator(Ty* ptr, std::size_t j) noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator*() const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator->() const noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator--() noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator++() noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator--(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator temp = *this;
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator++(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator temp = *this;
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator::operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_iterator& rhs) const noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::cm_const_iterator(const Ty* ptr, std::size_t j) noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator*() const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator->() const noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator--() noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator++() noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator--(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator temp = *this;
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator++(int) noexcept
{
	constexpr bool _not_contiguous = (_rows != _rows_padded) && (_cols != 1);

	cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator temp = *this;
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator::operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::cm_const_iterator& rhs) const noexcept
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

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator*() const noexcept
{
	return *(m_data_ptr + m_i + _rows_padded * m_j);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator->() const noexcept
{
	return m_data_ptr + m_i + _rows_padded * m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator--() noexcept
{
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return *this;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator++() noexcept
{
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return *this;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator--(int) noexcept
{
	cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator temp = *this;
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return temp;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator++(int) noexcept
{
	cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator temp = *this;
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return m_i == rhs.m_i && m_j == rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return !(m_i == rhs.m_i && m_j == rhs.m_j);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j <= _cols * rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j >= _cols * rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j < _cols* rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator::operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j > _cols * rhs.m_i + rhs.m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator*() const noexcept
{
	return *(m_data_ptr + m_i + _rows_padded * m_j);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline const Ty& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator->() const noexcept
{
	return m_data_ptr + m_i + _rows_padded * m_j;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator--() noexcept
{
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return *this;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator++() noexcept
{
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return *this;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator--(int) noexcept
{
	cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator temp = *this;
	if (m_j == 0)
	{
		m_j = _cols;
		m_i--;
	}
	m_j--;
	return temp;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline typename cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator++(int) noexcept
{
	cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator temp = *this;
	m_j++;
	if (m_j == _cols)
	{
		m_j = 0;
		m_i++;
	}
	return temp;
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator==(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return m_i == rhs.m_i && m_j == rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator!=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return !(m_i == rhs.m_i && m_j == rhs.m_j);
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator<=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j <= _cols * rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator>=(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j >= _cols * rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator<(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j < _cols* rhs.m_i + rhs.m_j;
}
template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline bool cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator::operator>(const cool::matrix<Ty, _rows, _cols, _rows_padded>::rm_const_iterator& rhs) const noexcept
{
	return _cols * m_i + m_j > _cols * rhs.m_i + rhs.m_j;
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
	public: static constexpr std::streamsize value = 8;
	};
	template <class Ty> class _print_matrix_cols_blk<std::complex<Ty>> {
	public: static constexpr std::streamsize value = 4;
	};

	template <class Ty> class _print_matrix_cell_width {
	public: static constexpr std::streamsize value = 12;
	};
	template <class Ty> class _print_matrix_cell_width<std::complex<Ty>> {
	public: static constexpr std::streamsize value = 24;
	};
}

template <class Ty, std::size_t _rows, std::size_t _cols, std::size_t _rows_padded>
inline std::ostream& operator<<(std::ostream& out, const cool::matrix<Ty, _rows, _cols, _rows_padded>& rhs) noexcept
{
	cool::print_matrix(rhs.data(), _rows, _cols, _rows_padded, 16,
		cool::_print_matrix_cols_blk<Ty>::value,
		cool::_print_matrix_cell_width<Ty>::value
	);
	return out;
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
