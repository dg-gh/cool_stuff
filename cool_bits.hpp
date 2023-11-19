// cool_bits.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient


#ifdef COOL_BITS_DEFINE_LIST_MACROS
#if (!defined(ZEROx4) && !defined(ZEROx8) && !defined(ZEROx16)\
	&& !defined(ZEROx32) && !defined(ZEROx64)\
	&& !defined(ONEx4) && !defined(ONEx8) && !defined(ONEx16)\
	&& !defined(ONEx32) && !defined(ONEx64)\
)
#define ZEROx4 0,0,0,0
#define ZEROx8 0,0,0,0,0,0,0,0
#define ZEROx16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define ZEROx32 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
#define ZEROx64 ZEROx32,ZEROx32
#define ONEx4 1,1,1,1
#define ONEx8 1,1,1,1,1,1,1,1
#define ONEx16 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
#define ONEx32 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
#define ONEx64 ONEx32,ONEx32
#endif // (!defined(ZEROx4) && ... && !defined(ONEx4) && ... )
#endif // COOL_BITS_DEFINE_LIST_MACROS


#ifndef _COOL_BITS_HPP
#define _COOL_BITS_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <initializer_list>
#include <type_traits>
#include <memory>


namespace cool
{
	template <std::size_t bit_count, class word_Ty = unsigned char> class bits;
}

namespace cool
{
	// constexpr constructing functions

	template <std::size_t bit_count, class word_Ty = unsigned char, class ... args_uint_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> one_bits(args_uint_Ty&& ... bit_offsets) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class ... args_uint_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> zero_bits(args_uint_Ty&& ... bit_offsets) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uint_Ty1, class uint_Ty2>
	constexpr inline cool::bits<bit_count, word_Ty> one_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uint_Ty1, class uint_Ty2>
	constexpr inline cool::bits<bit_count, word_Ty> zero_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class rhs_word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> value_in_bits(rhs_word_Ty word_val) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uint_Ty, class rhs_word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> value_in_bits(rhs_word_Ty word_val, uint_Ty bit_offset) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uint_Ty1, class uint_Ty2, class rhs_word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> value_in_bits(rhs_word_Ty word_val, uint_Ty1 bit_offset, uint_Ty2 word_offset) noexcept;

	template <std::size_t bit_count, class ptr_Ty1, class ptr_Ty2> class _masked_proxy;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>
		masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept;
}

namespace cool
{
	// _vbits_proxy, _cvbits_proxy, masked

	template <std::size_t bit_count, class word_Ty> class _vbits_proxy;
	template <std::size_t bit_count, class word_Ty> class _cvbits_proxy;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>
		masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>
		masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>
		masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
		masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
		masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>
		masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
		masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char>
	inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
		masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept;
}

namespace cool
{
#ifndef _COOL_NO_INIT_ENUM
#define _COOL_NO_INIT_ENUM
	enum no_init_t { no_init };
#endif // _COOL_NO_INIT_ENUM


	// bits

	template <std::size_t bit_count, class word_Ty> class bits
	{

	public:

		using word_type = word_Ty;

		static_assert(std::is_integral<word_type>::value,
			"cool::bits<bit_count, word_type> requirement : word_type must be integral");

		static_assert(bit_count != 0,
			"cool::bits<bit_count, word_type> requirement : bit_count must be non-zero");

		static constexpr inline std::size_t size() noexcept;

		static constexpr std::size_t byte_size = CHAR_BIT;
		static constexpr std::size_t byte_capacity = bit_count / byte_size;
		static constexpr std::size_t byte_count = (bit_count % byte_size == 0) ? bit_count / byte_size : bit_count / byte_size + 1;

		static constexpr std::size_t word_size = CHAR_BIT * sizeof(word_Ty);
		static constexpr std::size_t word_capacity = bit_count / word_size;
		static constexpr std::size_t word_count = (bit_count % word_size == 0) ? bit_count / word_size : bit_count / word_size + 1;

		bits() noexcept = default;
		inline bits(cool::no_init_t) noexcept {};
		constexpr bits(const cool::bits<bit_count, word_Ty>&) noexcept = default;
		inline cool::bits<bit_count, word_Ty>& operator=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		bits(cool::bits<bit_count, word_Ty>&&) noexcept = default;
		inline cool::bits<bit_count, word_Ty>& operator=(cool::bits<bit_count, word_Ty>&& rhs) noexcept;
		~bits() = default;

		constexpr inline bits(bool val) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(bool val) noexcept;
		constexpr inline bits(std::initializer_list<bool> lst) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(std::initializer_list<bool> lst) noexcept;

		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>& rhs) noexcept;

		constexpr inline operator word_Ty() const noexcept;
		constexpr inline std::size_t count() const noexcept;

		// bit level accessors

		class bit_proxy;
		class vbit_proxy;

		template <class uint_Ty> constexpr inline bool operator[](uint_Ty bit_offset) const noexcept;
		template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::bit_proxy operator[](uint_Ty bit_offset) noexcept;

		template <class uint_Ty> constexpr inline bool bit(uint_Ty bit_offset) const noexcept;
		template <class uint_Ty1, class uint_Ty2> inline bool bit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) const noexcept;
		template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::bit_proxy bit(uint_Ty bit_offset) noexcept;
		template <class uint_Ty1, class uint_Ty2> inline typename cool::bits<bit_count, word_Ty>::bit_proxy bit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) noexcept;

		template <class uint_Ty> inline bool vbit(uint_Ty bit_offset) const volatile noexcept;
		template <class uint_Ty1, class uint_Ty2> inline bool vbit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) const volatile noexcept;
		template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::vbit_proxy vbit(uint_Ty bit_offset) volatile noexcept;
		template <class uint_Ty1, class uint_Ty2> inline typename cool::bits<bit_count, word_Ty>::vbit_proxy vbit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) volatile noexcept;

		// op assign

		inline cool::bits<bit_count, word_Ty>& operator&=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator|=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator^=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& flip() noexcept;

		inline cool::bits<bit_count, word_Ty>& operator&=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator|=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator^=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		inline cool::bits<bit_count, word_Ty>& operator&=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator|=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::bits<bit_count, word_Ty>& operator^=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		// op

		constexpr inline cool::bits<bit_count, word_Ty> operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline cool::bits<bit_count, word_Ty> operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline cool::bits<bit_count, word_Ty> operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline cool::bits<bit_count, word_Ty> operator~() const noexcept;

		constexpr inline cool::bits<bit_count, word_Ty> operator&(bool rhs) const noexcept;
		constexpr inline cool::bits<bit_count, word_Ty> operator|(bool rhs) const noexcept;
		constexpr inline cool::bits<bit_count, word_Ty> operator^(bool rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// cmp

		constexpr inline bool operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator==(bool rhs) const noexcept;
		constexpr inline bool operator!=(bool rhs) const noexcept;
		inline bool operator==(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator==(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		constexpr inline bool operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator<=(bool rhs) const noexcept;
		constexpr inline bool operator>=(bool rhs) const noexcept;
		inline bool operator<=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		constexpr inline bool operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		constexpr inline bool operator<(bool rhs) const noexcept;
		constexpr inline bool operator>(bool rhs) const noexcept;
		inline bool operator<(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// volatile access

		inline cool::_vbits_proxy<bit_count, word_Ty> v() volatile noexcept;
		inline cool::_cvbits_proxy<bit_count, word_Ty> v() const volatile noexcept;

		// proxy for bit level access

		class bit_proxy
		{

		public:

			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator&=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator|=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator^=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& flip() noexcept;
			inline operator bool() const noexcept;

			bit_proxy() = delete;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator=(const typename cool::bits<bit_count, word_Ty>::bit_proxy& rhs) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::bit_proxy& operator=(typename cool::bits<bit_count, word_Ty>::bit_proxy&& rhs) noexcept;
			~bit_proxy() = default;

		private:

			template <std::size_t bit_count2, class word_Ty2> friend class bits;
			friend class cool::bits<bit_count, word_Ty>;

			bit_proxy(const typename cool::bits<bit_count, word_Ty>::bit_proxy&) noexcept = default;
			bit_proxy(typename cool::bits<bit_count, word_Ty>::bit_proxy&&) noexcept = default;
			inline bit_proxy(word_Ty* data_ptr, word_Ty mask) noexcept;

			word_Ty* m_data_ptr;
			word_Ty m_mask;
		};

		class vbit_proxy
		{

		public:

			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator&=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator|=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator^=(bool val) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& flip() noexcept;
			inline operator bool() const noexcept;

			vbit_proxy() = delete;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator=(const typename cool::bits<bit_count, word_Ty>::vbit_proxy& rhs) noexcept;
			inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& operator=(typename cool::bits<bit_count, word_Ty>::vbit_proxy&& rhs) noexcept;
			~vbit_proxy() = default;

		private:

			template <std::size_t bit_count2, class word_Ty2> friend class bits;
			friend class cool::bits<bit_count, word_Ty>;

			vbit_proxy(const typename cool::bits<bit_count, word_Ty>::vbit_proxy&) noexcept = default;
			vbit_proxy(typename cool::bits<bit_count, word_Ty>::vbit_proxy&&) noexcept = default;
			inline vbit_proxy(volatile word_Ty* data_ptr, word_Ty mask) noexcept;

			volatile word_Ty* m_data_ptr;
			word_Ty m_mask;
		};

	private:

		template <std::size_t bit_count2, class word_Ty2, class ... args_uint_Ty>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::one_bits(args_uint_Ty&& ... bit_offsets) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class ... args_uint_Ty>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::zero_bits(args_uint_Ty&& ... bit_offsets) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class uint_Ty1, class uint_Ty2>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::one_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class uint_Ty1, class uint_Ty2>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::zero_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class rhs_word_Ty>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::value_in_bits(rhs_word_Ty word_val) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class uint_Ty, class rhs_word_Ty>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::value_in_bits(rhs_word_Ty word_val, uint_Ty bit_offset) noexcept;

		template <std::size_t bit_count2, class word_Ty2, class uint_Ty1, class uint_Ty2, class rhs_word_Ty>
		friend constexpr inline cool::bits<bit_count2, word_Ty2> cool::value_in_bits(rhs_word_Ty word_val, uint_Ty1 bit_offset, uint_Ty2 word_offset) noexcept;

		template <std::size_t bit_count2, class word_Ty2> friend class cool::_vbits_proxy;
		template <std::size_t bit_count2, class word_Ty2> friend class cool::_cvbits_proxy;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		word_Ty m_field[word_count];

		// subroutines

		template <class word_Ty2> static inline constexpr word_Ty2 _pcmask_op(word_Ty2 _mask8b) noexcept;
		template <class word_Ty2> static inline constexpr std::size_t _popcount_op(word_Ty2 val) noexcept;
		template <class ptr_Ty> static inline std::size_t _count_op(ptr_Ty data_ptr) noexcept;

		template <class ptr_Ty1, class ptr_Ty2> static inline void _assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty1, class ptr_Ty2, class ptr_Ty3> static inline void _assign_masked_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr, ptr_Ty3 mask_ptr) noexcept;
		template <class ptr_Ty> static inline void _assign_bool_op(ptr_Ty data_ptr, bool val) noexcept;
		template <class ptr_Ty> static inline void _assign_lst_op(ptr_Ty data_ptr, std::initializer_list<bool> lst) noexcept;

		template <class ptr_Ty1, class ptr_Ty2> static inline void _and_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline void _or_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline void _xor_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty> static inline void _not_assign_op(ptr_Ty data_ptr) noexcept;

		template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2> static inline void _and_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2> static inline void _or_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2> static inline void _xor_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty0, class ptr_Ty> static inline void _not_op(ptr_Ty0 ret_ptr, ptr_Ty ptr) noexcept;

		template <class ptr_Ty1, class ptr_Ty2> static inline void _and_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline void _or_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline void _xor_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept;

		template <class ptr_Ty1, class ptr_Ty2> static inline bool _equal_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline bool _not_equal_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty> static inline bool _equal_bool_cmp(ptr_Ty lhs_ptr, bool rhs) noexcept;
		template <class ptr_Ty> static inline bool _not_equal_bool_cmp(ptr_Ty lhs_ptr, bool rhs) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline bool _less_or_equal_cmp(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
		template <class ptr_Ty1, class ptr_Ty2> static inline bool _less_cmp(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept;
	};

	template <std::size_t bit_count, class word_Ty> class _vbits_proxy
	{

	public:

		// assign

		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(bool val) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(std::initializer_list<bool> lst) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>& rhs) noexcept;

		inline operator word_Ty() const noexcept;
		inline std::size_t count() const noexcept;

		// op assign

		inline cool::_vbits_proxy<bit_count, word_Ty>& operator&=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator|=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator^=(const cool::bits<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& flip() noexcept;

		inline cool::_vbits_proxy<bit_count, word_Ty>& operator&=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator|=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator^=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		inline cool::_vbits_proxy<bit_count, word_Ty>& operator&=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator|=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator^=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

		// op

		inline cool::bits<bit_count, word_Ty> operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator~() const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(bool rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(bool rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(bool rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// cmp

		inline bool operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator==(bool rhs) const noexcept;
		inline bool operator!=(bool rhs) const noexcept;
		inline bool operator==(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator==(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline bool operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<=(bool rhs) const noexcept;
		inline bool operator>=(bool rhs) const noexcept;
		inline bool operator<=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline bool operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<(bool rhs) const noexcept;
		inline bool operator>(bool rhs) const noexcept;
		inline bool operator<(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// spec

		_vbits_proxy() = delete;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
		inline cool::_vbits_proxy<bit_count, word_Ty>& operator=(cool::_vbits_proxy<bit_count, word_Ty>&& rhs) noexcept;
		~_vbits_proxy() = default;

	private:

		_vbits_proxy(const cool::_vbits_proxy<bit_count, word_Ty>&) noexcept = default;
		_vbits_proxy(cool::_vbits_proxy<bit_count, word_Ty>&&) noexcept = default;
		inline _vbits_proxy(volatile word_Ty* data_ptr) noexcept;

		friend class cool::bits<bit_count, word_Ty>;
		friend class cool::_cvbits_proxy<bit_count, word_Ty>;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		volatile word_Ty* m_data_ptr;
	};

	template <std::size_t bit_count, class word_Ty> class _cvbits_proxy
	{

	public:

		inline operator word_Ty() const noexcept;
		inline std::size_t count() const noexcept;

		// op

		inline cool::bits<bit_count, word_Ty> operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator~() const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(bool rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(bool rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(bool rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline cool::bits<bit_count, word_Ty> operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline cool::bits<bit_count, word_Ty> operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// cmp

		inline bool operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator==(bool rhs) const noexcept;
		inline bool operator!=(bool rhs) const noexcept;
		inline bool operator==(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator==(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator!=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline bool operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<=(bool rhs) const noexcept;
		inline bool operator>=(bool rhs) const noexcept;
		inline bool operator<=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		inline bool operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<(bool rhs) const noexcept;
		inline bool operator>(bool rhs) const noexcept;
		inline bool operator<(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator<(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;
		inline bool operator>(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept;

		// spec

		_cvbits_proxy() = delete;
		inline cool::_cvbits_proxy<bit_count, word_Ty>& operator=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) = delete;
		inline cool::_cvbits_proxy<bit_count, word_Ty>& operator=(cool::_cvbits_proxy<bit_count, word_Ty>&& rhs) = delete;
		~_cvbits_proxy() = default;

	private:

		_cvbits_proxy(const cool::_cvbits_proxy<bit_count, word_Ty>&) noexcept = default;
		_cvbits_proxy(cool::_cvbits_proxy<bit_count, word_Ty>&&) noexcept = default;
		inline _cvbits_proxy(const volatile word_Ty* data_ptr) noexcept;

		friend class cool::bits<bit_count, word_Ty>;
		friend class cool::_vbits_proxy<bit_count, word_Ty>;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		const volatile word_Ty* m_data_ptr;
	};

	template <std::size_t bit_count, class ptr_Ty1, class ptr_Ty2> class _masked_proxy
	{

	public:

		~_masked_proxy() = default;

	private:

		_masked_proxy() = delete;
		_masked_proxy(const cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>&) = delete;
		cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>& operator=(const cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>&) = delete;
		_masked_proxy(cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>&&) noexcept = default;
		cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>& operator=(cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>&&) = default;

		inline _masked_proxy(ptr_Ty1 rhs_ptr, ptr_Ty2 mask_ptr) noexcept;

		template <std::size_t bit_count2, class word_Ty2> friend class cool::bits;
		template <std::size_t bit_count2, class word_Ty2> friend class cool::_vbits_proxy;
		template <std::size_t bit_count2, class word_Ty2> friend class cool::_cvbits_proxy;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::bits<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_vbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::bits<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_vbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		template <std::size_t bit_count2, class word_Ty2>
		friend inline cool::_masked_proxy<bit_count2, const volatile word_Ty2*, const volatile word_Ty2*> cool::masked(
			const cool::_cvbits_proxy<bit_count2, word_Ty2>& rhs, const cool::_cvbits_proxy<bit_count2, word_Ty2>& mask) noexcept;

		ptr_Ty1 m_rhs_ptr;
		ptr_Ty2 m_mask_ptr;
	};
}

namespace cool
{
	// cmp

	template <std::size_t bit_count, class word_Ty>
	constexpr inline bool operator==(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	constexpr inline bool operator!=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator==(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator!=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator==(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator!=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

	template <std::size_t bit_count, class word_Ty>
	constexpr inline bool operator<=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	constexpr inline bool operator>=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator<=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator>=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator<=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline bool operator>=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

	// op

	template <std::size_t bit_count, class word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> operator&(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> operator|(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	constexpr inline cool::bits<bit_count, word_Ty> operator^(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator&(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator|(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator^(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator&(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator|(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;
	template <std::size_t bit_count, class word_Ty>
	inline cool::bits<bit_count, word_Ty> operator^(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept;

	// reinterpret address (should be called only at fixed addresses without other existing objects aliasing)
	// address must be a multiple of alignof(word_Ty)

	template <std::size_t bit_count, class word_Ty = unsigned char, class uintptr_Ty>
	inline cool::bits<bit_count, word_Ty>& bits_at(uintptr_Ty mem_address) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uintptr_Ty>
	inline const cool::bits<bit_count, word_Ty>& const_bits_at(uintptr_Ty mem_address) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uintptr_Ty>
	inline volatile cool::bits<bit_count, word_Ty>& volatile_bits_at(uintptr_Ty mem_address) noexcept;

	template <std::size_t bit_count, class word_Ty = unsigned char, class uintptr_Ty>
	inline const volatile cool::bits<bit_count, word_Ty>& const_volatile_bits_at(uintptr_Ty mem_address) noexcept;
}


// detail

// bits

template <std::size_t bit_count, class word_Ty>
constexpr inline std::size_t cool::bits<bit_count, word_Ty>::size() noexcept
{
	return bit_count;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	_assign_op(static_cast<word_Ty*>(m_field), static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(cool::bits<bit_count, word_Ty>&& rhs) noexcept {
	_assign_op(static_cast<word_Ty*>(m_field), static_cast<word_Ty*>(rhs.m_field)); return *this;
}

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty>::bits(bool val) noexcept : m_field{ static_cast<word_Ty>(0) }
{
	constexpr std::size_t word_count = (bit_count + ((word_size - (bit_count % word_size)) % word_size)) / word_size;

	word_Ty word_val = val ? static_cast<word_Ty>(-1) : 0;
	for (std::size_t n = 0; n < word_count; n++)
	{
		m_field[n] = word_val;
	}
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(bool val) noexcept
{
	_assign_bool_op(static_cast<word_Ty*>(m_field), val);

	return *this;
}

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty>::bits(std::initializer_list<bool> lst) noexcept : m_field{ static_cast<word_Ty>(0) }
{
	const bool* ptr = lst.begin();
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		word_Ty val = 0;
		for (std::size_t m = 0; m < word_size; m++)
		{
			val |= *ptr++ ? static_cast<word_Ty>(static_cast<word_Ty>(1) << m) : 0;
		}
		m_field[n] = val;
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr std::size_t remainder = bit_count % word_size;

		word_Ty val = 0;
		for (std::size_t m = 0; m < remainder; m++)
		{
			val |= *ptr++ ? static_cast<word_Ty>(static_cast<word_Ty>(1) << m) : 0;
		}
		*(static_cast<word_Ty*>(m_field) + word_capacity) = val;
	}
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(std::initializer_list<bool> lst) noexcept {
	_assign_lst_op(static_cast<word_Ty*>(m_field), lst); return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>& rhs) noexcept {
	_assign_masked_op(m_field, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>& rhs) noexcept {
	_assign_masked_op(m_field, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>& rhs) noexcept {
	_assign_masked_op(m_field, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>& rhs) noexcept {
	_assign_masked_op(m_field, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty>::operator word_Ty() const noexcept
{
	static_assert(bit_count <= word_size, "cool::bits<bit_count, word_type> operator word_type requirement : word_count must be one");
	constexpr bool no_mask_needed = bit_count == word_size;
	if (no_mask_needed)
	{
		return m_field[0];
	}
	else
	{
		constexpr word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
		return m_field[0] & mask;
	}
}

template <std::size_t bit_count, class word_Ty>
constexpr inline std::size_t cool::bits<bit_count, word_Ty>::count() const noexcept
{
	constexpr bool cast_upward = sizeof(word_Ty) < sizeof(unsigned int);

	if (cast_upward)
	{
		std::size_t ret = 0;

		for (std::size_t n = 0; n < word_capacity; n++)
		{
			ret += _popcount_op(static_cast<unsigned int>(m_field[n]));
		}

		constexpr bool end_mask_needed = bit_count % word_size != 0;
		if (end_mask_needed)
		{
			constexpr word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
			ret += _popcount_op(static_cast<unsigned int>(*(static_cast<const word_Ty*>(m_field) + word_capacity) & mask));
		}

		return ret;
	}
	else
	{
		std::size_t ret = 0;

		for (std::size_t n = 0; n < word_capacity; n++)
		{
			ret += _popcount_op(m_field[n]);
		}

		constexpr bool end_mask_needed = bit_count % word_size != 0;
		if (end_mask_needed)
		{
			constexpr word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
			ret += _popcount_op(*(static_cast<const word_Ty*>(m_field) + word_capacity) & mask);
		}

		return ret;
	}
}

// bit level accessors

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> constexpr inline bool cool::bits<bit_count, word_Ty>::operator[](uint_Ty bit_offset) const noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << static_cast<std::size_t>(bit_offset));
		return (m_field[0] & mask) != static_cast<word_Ty>(0);
	}
	else
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return (m_field[static_cast<std::size_t>(bit_offset) / word_size] & mask) != static_cast<word_Ty>(0);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::bit_proxy cool::bits<bit_count, word_Ty>::operator[](uint_Ty bit_offset) noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << static_cast<std::size_t>(bit_offset));
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
	else
	{
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field) + static_cast<std::size_t>(bit_offset) / word_size;
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> constexpr inline bool cool::bits<bit_count, word_Ty>::bit(uint_Ty bit_offset) const noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << static_cast<std::size_t>(bit_offset));
		return (m_field[0] & mask) != static_cast<word_Ty>(0);
	}
	else
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return (m_field[static_cast<std::size_t>(bit_offset) / word_size] & mask) != static_cast<word_Ty>(0);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty1, class uint_Ty2> inline bool cool::bits<bit_count, word_Ty>::bit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) const noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << new_bit_offset);
		return (m_field[0] & mask) != static_cast<word_Ty>(0);
	}
	else
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (new_bit_offset % word_size));
		return (m_field[new_bit_offset / word_size] & mask) != static_cast<word_Ty>(0);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::bit_proxy cool::bits<bit_count, word_Ty>::bit(uint_Ty bit_offset) noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << static_cast<std::size_t>(bit_offset));
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
	else
	{
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field) + static_cast<std::size_t>(bit_offset) / word_size;
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty1, class uint_Ty2> inline typename cool::bits<bit_count, word_Ty>::bit_proxy cool::bits<bit_count, word_Ty>::bit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << new_bit_offset);
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
	else
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty* data_ptr = static_cast<word_Ty*>(m_field) + new_bit_offset / word_size;
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (new_bit_offset % word_size));
		return typename cool::bits<bit_count, word_Ty>::bit_proxy(data_ptr, mask);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> inline bool cool::bits<bit_count, word_Ty>::vbit(uint_Ty bit_offset) const volatile noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << static_cast<std::size_t>(bit_offset));
		return (*static_cast<const volatile word_Ty*>(m_field) & mask) != static_cast<word_Ty>(0);
	}
	else
	{
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return (*(static_cast<const volatile word_Ty*>(m_field) + static_cast<std::size_t>(bit_offset) / word_size) & mask) != static_cast<word_Ty>(0);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty1, class uint_Ty2> inline bool cool::bits<bit_count, word_Ty>::vbit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) const volatile noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << new_bit_offset);
		return (*static_cast<const volatile word_Ty*>(m_field) & mask) != static_cast<word_Ty>(0);
	}
	else
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (new_bit_offset % word_size));
		return (*(static_cast<const volatile word_Ty*>(m_field) + new_bit_offset / word_size) & mask) != static_cast<word_Ty>(0);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty> inline typename cool::bits<bit_count, word_Ty>::vbit_proxy cool::bits<bit_count, word_Ty>::vbit(uint_Ty bit_offset) volatile noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		volatile word_Ty* data_ptr = static_cast<volatile word_Ty*>(m_field);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return typename cool::bits<bit_count, word_Ty>::vbit_proxy(data_ptr, mask);
	}
	else
	{
		volatile word_Ty* data_ptr = static_cast<volatile word_Ty*>(m_field) + static_cast<std::size_t>(bit_offset) / word_size;
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (static_cast<std::size_t>(bit_offset) % word_size));
		return typename cool::bits<bit_count, word_Ty>::vbit_proxy(data_ptr, mask);
	}
}

template <std::size_t bit_count, class word_Ty>
template <class uint_Ty1, class uint_Ty2> inline typename cool::bits<bit_count, word_Ty>::vbit_proxy cool::bits<bit_count, word_Ty>::vbit(uint_Ty1 bit_offset, uint_Ty2 additional_byte_offset) volatile noexcept
{
	constexpr bool no_word_offset = bit_count <= CHAR_BIT * sizeof(word_Ty);

	if (no_word_offset)
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		volatile word_Ty* data_ptr = static_cast<volatile word_Ty*>(m_field);
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (new_bit_offset % word_size));
		return typename cool::bits<bit_count, word_Ty>::vbit_proxy(data_ptr, mask);
	}
	else
	{
		std::size_t new_bit_offset = static_cast<std::size_t>(bit_offset) + CHAR_BIT * static_cast<std::size_t>(additional_byte_offset);
		volatile word_Ty* data_ptr = static_cast<volatile word_Ty*>(m_field) + new_bit_offset / word_size;
		word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (new_bit_offset % word_size));
		return typename cool::bits<bit_count, word_Ty>::vbit_proxy(data_ptr, mask);
	}
}

// bits op assign

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator&=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	_and_assign_op(static_cast<word_Ty*>(m_field), static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator|=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	_or_assign_op(static_cast<word_Ty*>(m_field), static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator^=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	_xor_assign_op(static_cast<word_Ty*>(m_field), static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::flip() noexcept {
	_not_assign_op(static_cast<word_Ty*>(m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator&=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_and_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator|=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_or_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator^=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_xor_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator&=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_and_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator|=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_or_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits<bit_count, word_Ty>::operator^=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	_xor_assign_op(static_cast<word_Ty*>(m_field), rhs.m_data_ptr); return *this;
}

// bits op

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;
	for (std::size_t n = 0; n < word_count; n++)
	{
		ret.m_field[n] = m_field[n] & rhs.m_field[n];
	}
	return ret;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;
	for (std::size_t n = 0; n < word_count; n++)
	{
		ret.m_field[n] = m_field[n] | rhs.m_field[n];
	}
	return ret;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;
	for (std::size_t n = 0; n < word_count; n++)
	{
		ret.m_field[n] = m_field[n] ^ rhs.m_field[n];
	}
	return ret;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator~() const noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;
	for (std::size_t n = 0; n < word_count; n++)
	{
		ret.m_field[n] = m_field[n] ^ static_cast<word_Ty>(-1);
	}
	return ret;
}

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator&(bool rhs) const noexcept {
	return rhs ? *this : cool::bits<bit_count, word_Ty>(false);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator|(bool rhs) const noexcept {
	return rhs ? cool::bits<bit_count, word_Ty>(true) : *this;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator^(bool rhs) const noexcept {
	return rhs ? ~(*this) : *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; _and_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; _or_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; xow_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; _and_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; _or_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; _xor_op(static_cast<word_Ty*>(ret.m_field), static_cast<const word_Ty*>(m_field), rhs.m_data_ptr); return ret;
}

// bits cmp

template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (m_field[n] ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(static_cast<const word_Ty*>(m_field) + word_capacity)
			^ *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity))
			& end_mask);
	}

	return test == static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (m_field[n] ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(static_cast<const word_Ty*>(m_field) + word_capacity)
			^ *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity))
			& end_mask);
	}

	return test != static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator==(bool rhs) const noexcept
{
	word_Ty test = static_cast<word_Ty>(0);
	word_Ty rhs_word = rhs ? static_cast<word_Ty>(-1) : 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (m_field[n] ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(static_cast<const word_Ty*>(m_field) + word_capacity) ^ rhs_word) & end_mask);
	}

	return test == static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator!=(bool rhs) const noexcept
{
	word_Ty test = static_cast<word_Ty>(0);
	word_Ty rhs_word = rhs ? static_cast<word_Ty>(-1) : 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (m_field[n] ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(static_cast<const word_Ty*>(m_field) + word_capacity) ^ rhs_word) & end_mask);
	}

	return test != static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator==(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator!=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _not_equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator==(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator!=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _not_equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= ((m_field[n] | rhs.m_field[n]) ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty word = *(static_cast<const word_Ty*>(m_field) + word_capacity);
		word_Ty rhs_word = *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity);
		test |= (((word | rhs_word) ^ rhs_word) & end_mask);
	}

	return test == static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= ((m_field[n] & rhs.m_field[n]) ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty word = *(static_cast<const word_Ty*>(m_field) + word_capacity);
		word_Ty rhs_word = *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity);
		test |= (((word & rhs_word) ^ rhs_word) & end_mask);
	}

	return test == static_cast<word_Ty>(0);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator<=(bool rhs) const noexcept
{
	return rhs || (*this == false);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator>=(bool rhs) const noexcept
{
	return !rhs || (*this == true);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator<=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_or_equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator>=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_or_equal_cmp(rhs.m_data_ptr, static_cast<const word_Ty*>(m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator<=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_or_equal_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator>=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_or_equal_cmp(rhs.m_data_ptr, static_cast<const word_Ty*>(m_field));
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test_less = 0;
	word_Ty test_neq = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test_less |= ((m_field[n] | rhs.m_field[n]) ^ rhs.m_field[n]);
		test_neq |= (m_field[n] ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty word = *(static_cast<const word_Ty*>(m_field) + word_capacity);
		word_Ty rhs_word = *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity);
		test_less |= (((word | rhs_word) ^ rhs_word) & end_mask);
		test_neq |= ((word ^ rhs_word) & end_mask);
	}

	return (test_less == static_cast<word_Ty>(0)) && (test_neq != static_cast<word_Ty>(0));
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept
{
	word_Ty test_greater = 0;
	word_Ty test_neq = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test_greater |= ((m_field[n] & rhs.m_field[n]) ^ rhs.m_field[n]);
		test_neq |= (m_field[n] ^ rhs.m_field[n]);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty word = *(static_cast<const word_Ty*>(m_field) + word_capacity);
		word_Ty rhs_word = *(static_cast<const word_Ty*>(rhs.m_field) + word_capacity);
		test_greater |= (((word & rhs_word) ^ rhs_word) & end_mask);
		test_neq |= ((word ^ rhs_word) & end_mask);
	}

	return (test_greater == static_cast<word_Ty>(0)) && (test_neq != static_cast<word_Ty>(0));
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator<(bool rhs) const noexcept
{
	return rhs && (*this != true);
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::bits<bit_count, word_Ty>::operator>(bool rhs) const noexcept
{
	return !rhs && (*this != false);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator<(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator>(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_cmp(rhs.m_data_ptr, static_cast<const word_Ty*>(m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator<(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_cmp(static_cast<const word_Ty*>(m_field), rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::bits<bit_count, word_Ty>::operator>(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return _less_cmp(rhs.m_data_ptr, static_cast<const word_Ty*>(m_field));
}

// bits volatile access

template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::v() volatile noexcept
{
	return cool::_vbits_proxy<bit_count, word_Ty>(static_cast<volatile word_Ty*>(m_field));
}

template <std::size_t bit_count, class word_Ty>
inline cool::_cvbits_proxy<bit_count, word_Ty> cool::bits<bit_count, word_Ty>::v() const volatile noexcept
{
	return cool::_cvbits_proxy<bit_count, word_Ty>(static_cast<const volatile word_Ty*>(m_field));
}

// bit_proxy

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator&=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator|=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val | m_mask : data_val;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator^=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val ^ m_mask : data_val;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::flip() noexcept
{
	*m_data_ptr ^= m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>::bit_proxy::operator bool() const noexcept
{
	return (*m_data_ptr & m_mask) != static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator=(const typename cool::bits<bit_count, word_Ty>::bit_proxy& rhs) noexcept
{
	bool rhs_val = *rhs.m_data_ptr & rhs.m_mask;
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = rhs_val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::bit_proxy& cool::bits<bit_count, word_Ty>::bit_proxy::operator=(typename cool::bits<bit_count, word_Ty>::bit_proxy&& rhs) noexcept
{
	bool rhs_val = *rhs.m_data_ptr & rhs.m_mask;
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = rhs_val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>::bit_proxy::bit_proxy(word_Ty* data_ptr, word_Ty mask) noexcept
	: m_data_ptr(data_ptr), m_mask(mask) {}

// vbit_proxy

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator&=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator|=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val | m_mask : data_val;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator^=(bool val) noexcept
{
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = val ? data_val ^ m_mask : data_val;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::flip() noexcept
{
	*m_data_ptr ^= m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>::vbit_proxy::operator bool() const noexcept
{
	return (*m_data_ptr & m_mask) != static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator=(const typename cool::bits<bit_count, word_Ty>::vbit_proxy& rhs) noexcept
{
	bool rhs_val = *rhs.m_data_ptr & rhs.m_mask;
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = rhs_val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline typename cool::bits<bit_count, word_Ty>::vbit_proxy& cool::bits<bit_count, word_Ty>::vbit_proxy::operator=(typename cool::bits<bit_count, word_Ty>::vbit_proxy&& rhs) noexcept
{
	bool rhs_val = *rhs.m_data_ptr & rhs.m_mask;
	word_Ty data_val = *m_data_ptr;
	*m_data_ptr = rhs_val ? data_val | m_mask : data_val & ~m_mask;
	return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>::vbit_proxy::vbit_proxy(volatile word_Ty* data_ptr, word_Ty mask) noexcept
	: m_data_ptr(data_ptr), m_mask(mask) {}

// _vbits_proxy

// _vbits_proxy assign

template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_op(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(bool val) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_bool_op(m_data_ptr, val); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(std::initializer_list<bool> lst) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_lst_op(m_data_ptr, lst); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}

template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_masked_op(m_data_ptr, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_masked_op(m_data_ptr, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_masked_op(m_data_ptr, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_masked_op(m_data_ptr, rhs.m_rhs_ptr, rhs.m_mask_ptr); return *this;
}

// _vbits_proxy op assign

template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>::operator word_Ty() const noexcept {
	return *m_data_ptr;
}
template <std::size_t bit_count, class word_Ty>
inline std::size_t cool::_vbits_proxy<bit_count, word_Ty>::count() const noexcept {
	return cool::bits<bit_count, word_Ty>::_count_op(m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator&=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_and_assign_op(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator|=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_or_assign_op(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator^=(const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_xor_assign_op(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::flip() noexcept {
	cool::bits<bit_count, word_Ty>::_not_assign_op(m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator&=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_and_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator|=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_or_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator^=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_xor_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator&=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_and_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator|=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_or_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline typename cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator^=(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_xor_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}

// _vbits_proxy op

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>cool::_vbits_proxy<bit_count, word_Ty>:: operator~() const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_not_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator&(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator|(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator^(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_vbits_proxy<bit_count, word_Ty>::operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}

// _vbits_proxy cmp

template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator==(bool rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, rhs);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator!=(bool rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, rhs);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator==(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator!=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator==(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator!=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(static_cast<const word_Ty*>(rhs.m_field), m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<=(bool rhs) const noexcept {
	return rhs || cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, false);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>=(bool rhs) const noexcept {
	return !rhs || cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, true);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(static_cast<const word_Ty*>(rhs.m_field), m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<(bool rhs) const noexcept {
	return rhs && cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, true);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>(bool rhs) const noexcept {
	return !rhs && cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, false);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_lesscmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator<(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_vbits_proxy<bit_count, word_Ty>::operator>(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(rhs.m_data_ptr, m_data_ptr);
}

// _vbits_proxy spec

template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>& cool::_vbits_proxy<bit_count, word_Ty>::operator=(cool::_vbits_proxy<bit_count, word_Ty>&& rhs) noexcept {
	cool::bits<bit_count, word_Ty>::_assign_op(m_data_ptr, rhs.m_data_ptr); return *this;
}
template <std::size_t bit_count, class word_Ty>
inline cool::_vbits_proxy<bit_count, word_Ty>::_vbits_proxy(volatile word_Ty* data_ptr) noexcept
	: m_data_ptr(data_ptr) {}

// _cvbits_proxy

// _cvbits_proxy op

template <std::size_t bit_count, class word_Ty>
inline cool::_cvbits_proxy<bit_count, word_Ty>::operator word_Ty() const noexcept {
	return *m_data_ptr;
}
template <std::size_t bit_count, class word_Ty>
inline std::size_t cool::_cvbits_proxy<bit_count, word_Ty>::count() const noexcept {
	return cool::bits<bit_count, word_Ty>::_count_op(m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator&(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator|(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator^(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, static_cast<const word_Ty*>(rhs.m_field)); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty>cool::_cvbits_proxy<bit_count, word_Ty>:: operator~() const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_not_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator&(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator|(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator^(bool rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_bool_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator&(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator|(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator^(const cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}

template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator&(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_and_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator|(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_or_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::_cvbits_proxy<bit_count, word_Ty>::operator^(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	cool::bits<bit_count, word_Ty> ret; cool::bits<bit_count, word_Ty>::_xor_op(static_cast<word_Ty*>(ret.m_field), m_data_ptr, rhs.m_data_ptr); return ret;
}

// _cvbits_proxy cmp

template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator==(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator!=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator==(bool rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, rhs);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator!=(bool rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, rhs);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator==(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator!=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator==(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator!=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_not_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>=(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(static_cast<const word_Ty*>(rhs.m_field), m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<=(bool rhs) const noexcept {
	return rhs || cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, false);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>=(bool rhs) const noexcept {
	return !rhs || cool::bits<bit_count, word_Ty>::_equal_bool_cmp(m_data_ptr, true);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>=(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>=(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(m_data_ptr, static_cast<const word_Ty*>(rhs.m_field));
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>(const cool::bits<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(static_cast<const word_Ty*>(rhs.m_field), m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<(bool rhs) const noexcept {
	return rhs && cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, true);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>(bool rhs) const noexcept {
	return !rhs && cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(m_data_ptr, false);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>(const typename cool::_vbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(rhs.m_data_ptr, m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator<(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(m_data_ptr, rhs.m_data_ptr);
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::_cvbits_proxy<bit_count, word_Ty>::operator>(const typename cool::_cvbits_proxy<bit_count, word_Ty>& rhs) const noexcept {
	return cool::bits<bit_count, word_Ty>::_less_cmp(rhs.m_data_ptr, m_data_ptr);
}

// _cvbits_proxy spec

template <std::size_t bit_count, class word_Ty>
inline cool::_cvbits_proxy<bit_count, word_Ty>::_cvbits_proxy(const volatile word_Ty* data_ptr) noexcept
	: m_data_ptr(data_ptr) {}

// bits subroutines

template <std::size_t bit_count, class word_Ty> template <class word_Ty2>
inline constexpr word_Ty2 cool::bits<bit_count, word_Ty>::_pcmask_op(word_Ty2 _mask8b) noexcept
{
	constexpr std::size_t n = (sizeof(word_Ty2) * CHAR_BIT) / 8;
	word_Ty2 ret = 0;

	for (std::size_t m = 0; m < n; m++)
	{
		ret |= static_cast<word_Ty2>(static_cast<word_Ty2>(_mask8b) << (8 * m));
	}

	return ret;
}

template <std::size_t bit_count, class word_Ty> template <class word_Ty2>
inline constexpr std::size_t cool::bits<bit_count, word_Ty>::_popcount_op(word_Ty2 val) noexcept
{
	constexpr word_Ty2 mask01010101 = _pcmask_op(static_cast<word_Ty2>(0b01010101));
	constexpr word_Ty2 mask00110011 = _pcmask_op(static_cast<word_Ty2>(0b00110011));
	constexpr word_Ty2 mask00001111 = _pcmask_op(static_cast<word_Ty2>(0b00001111));
	constexpr word_Ty2 mask00000001 = _pcmask_op(static_cast<word_Ty2>(0b00000001));
	constexpr std::size_t end_shift = sizeof(word_Ty2) * CHAR_BIT - 8;
	constexpr bool gt8b = sizeof(word_Ty2) * CHAR_BIT > 8;

	word_Ty2 val2 = val - ((val >> 1) & mask01010101);
	word_Ty2 val4 = (val2 & mask00110011) + ((val2 >> 2) & mask00110011);
	word_Ty2 val8 = (val4 + (val4 >> 4)) & mask00001111;

	if (gt8b)
	{
		return (val8 * mask00000001) >> end_shift;
	}
	else
	{
		return val8;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline std::size_t cool::bits<bit_count, word_Ty>::_count_op(ptr_Ty data_ptr) noexcept
{
	constexpr bool cast_upward = sizeof(word_Ty) < sizeof(unsigned int);

	if (cast_upward)
	{
		std::size_t ret = 0;

		for (std::size_t n = 0; n < word_capacity; n++)
		{
			ret += _popcount_op(static_cast<unsigned int>(*(data_ptr + n)));
		}

		constexpr bool end_mask_needed = bit_count % word_size != 0;
		if (end_mask_needed)
		{
			constexpr word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
			ret += _popcount_op(static_cast<unsigned int>(*(data_ptr + word_capacity) & mask));
		}

		return ret;
	}
	else
	{
		std::size_t ret = 0;

		for (std::size_t n = 0; n < word_capacity; n++)
		{
			ret += _popcount_op(*(data_ptr + n));
		}

		constexpr bool end_mask_needed = bit_count % word_size != 0;
		if (end_mask_needed)
		{
			constexpr word_Ty mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
			ret += _popcount_op(*(data_ptr + word_capacity) & mask);
		}

		return ret;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) = *(rhs_ptr + n);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
		constexpr word_Ty zero_end_mask = ~end_mask;

		word_Ty temp = *(data_ptr + word_capacity) & zero_end_mask;
		temp |= (*(rhs_ptr + word_capacity) & end_mask);
		*(data_ptr + word_capacity) = temp;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2, class ptr_Ty3>
inline void cool::bits<bit_count, word_Ty>::_assign_masked_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr, ptr_Ty3 mask_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		word_Ty mask = *(mask_ptr + n);
		word_Ty temp = *(data_ptr + n) & ~mask;
		temp |= *(rhs_ptr + n) & mask;
		*(data_ptr + n) = temp;
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
		constexpr word_Ty zero_end_mask = ~end_mask;

		word_Ty mask = *(mask_ptr + word_capacity);
		word_Ty temp = *(data_ptr + word_capacity) & (zero_end_mask | ~mask);
		temp |= (*(rhs_ptr + word_capacity) & mask & end_mask);
		*(data_ptr + word_capacity) = temp;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline void cool::bits<bit_count, word_Ty>::_assign_bool_op(ptr_Ty data_ptr, bool val) noexcept
{
	word_Ty word_val = val ? static_cast<word_Ty>(-1) : 0;
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) = word_val;
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;
		constexpr word_Ty zero_end_mask = ~end_mask;

		word_Ty temp = *(data_ptr + word_capacity);
		*(data_ptr + word_capacity) = val ? temp | end_mask : temp & ~zero_end_mask;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline void cool::bits<bit_count, word_Ty>::_assign_lst_op(ptr_Ty data_ptr, std::initializer_list<bool> lst) noexcept
{
	const bool* ptr = lst.begin();
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		word_Ty word_val = 0;
		for (std::size_t m = 0; m < word_size; m++)
		{
			word_val |= *ptr++ ? static_cast<word_Ty>(static_cast<word_Ty>(1) << m) : 0;
		}
		*(data_ptr + n) = word_val;
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = ~(static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1);
		constexpr std::size_t remainder = bit_count % word_size;

		word_Ty word_val = 0;
		for (std::size_t m = 0; m < remainder; m++)
		{
			word_val |= *ptr++ ? static_cast<word_Ty>(static_cast<word_Ty>(1) << m) : 0;
		}
		word_Ty temp = *(data_ptr + word_capacity) & end_mask;
		temp |= word_val;
		*(data_ptr + word_capacity) = temp;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_and_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) &= *(rhs_ptr + n);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = ~(static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1);

		*(data_ptr + word_capacity) &= (*(rhs_ptr + word_capacity) | end_mask);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_or_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) |= *(rhs_ptr + n);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		*(data_ptr + word_capacity) |= (*(rhs_ptr + word_capacity) & end_mask);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_xor_assign_op(ptr_Ty1 data_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) ^= *(rhs_ptr + n);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		*(data_ptr + word_capacity) ^= (*(rhs_ptr + word_capacity) & end_mask);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline void cool::bits<bit_count, word_Ty>::_not_assign_op(ptr_Ty data_ptr) noexcept
{
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		*(data_ptr + n) ^= static_cast<word_Ty>(-1);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		*(data_ptr + word_capacity) ^= end_mask;
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_and_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_count; n++)
	{
		*(ret_ptr + n) = *(lhs_ptr + n) & *(rhs_ptr + n);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_or_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_count; n++)
	{
		*(ret_ptr + n) = *(lhs_ptr + n) | *(rhs_ptr + n);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty0, class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_xor_op(ptr_Ty0 ret_ptr, ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	for (std::size_t n = 0; n < word_count; n++)
	{
		*(ret_ptr + n) = *(lhs_ptr + n) ^ *(rhs_ptr + n);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_and_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept
{
	if (rhs)
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = *(lhs_ptr + n);
		}
	}
	else
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = static_cast<word_Ty>(0);
		}
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_or_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept
{
	if (rhs)
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = static_cast<word_Ty>(-1);
		}
	}
	else
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = *(lhs_ptr + n);
		}
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline void cool::bits<bit_count, word_Ty>::_xor_bool_op(ptr_Ty1 ret_ptr, ptr_Ty2 lhs_ptr, bool rhs) noexcept
{
	if (rhs)
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = ~(*(lhs_ptr + n));
		}
	}
	else
	{
		for (std::size_t n = 0; n < word_count; n++)
		{
			*(ret_ptr + n) = *(lhs_ptr + n);
		}
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty0, class ptr_Ty>
inline void cool::bits<bit_count, word_Ty>::_not_op(ptr_Ty0 ret_ptr, ptr_Ty ptr) noexcept
{
	for (std::size_t n = 0; n < word_count; n++)
	{
		*(ret_ptr + n) = *(ptr + n) ^ static_cast<word_Ty>(-1);
	}
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline bool cool::bits<bit_count, word_Ty>::_equal_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (*(lhs_ptr + n) ^ *(rhs_ptr + n));
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(lhs_ptr + word_capacity) ^ *(rhs_ptr + word_capacity)) & end_mask);
	}

	return test == static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline bool cool::bits<bit_count, word_Ty>::_not_equal_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (*(lhs_ptr + n) ^ *(rhs_ptr + n));
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(lhs_ptr + word_capacity) ^ *(rhs_ptr + word_capacity)) & end_mask);
	}

	return test != static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline bool cool::bits<bit_count, word_Ty>::_equal_bool_cmp(ptr_Ty lhs_ptr, bool rhs) noexcept
{
	word_Ty test = 0;

	word_Ty rhs_word = rhs ? static_cast<word_Ty>(-1) : 0;
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (*(lhs_ptr + n) ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(lhs_ptr + word_capacity) ^ rhs_word) & end_mask);
	}

	return test == static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty>
inline bool cool::bits<bit_count, word_Ty>::_not_equal_bool_cmp(ptr_Ty lhs_ptr, bool rhs) noexcept
{
	word_Ty test = 0;

	word_Ty rhs_word = rhs ? static_cast<word_Ty>(-1) : 0;
	for (std::size_t n = 0; n < word_capacity; n++)
	{
		test |= (*(lhs_ptr + n) ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		test |= ((*(lhs_ptr + word_capacity) ^ rhs_word) & end_mask);
	}

	return test != static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline bool cool::bits<bit_count, word_Ty>::_less_or_equal_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	word_Ty test = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		word_Ty rhs_word = *(rhs_ptr + n);
		test |= ((*(lhs_ptr + n) | rhs_word) ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty rhs_word = *(rhs_ptr + word_capacity);
		test |= (((*(lhs_ptr + word_capacity) | rhs_word) ^ rhs_word) & end_mask);
	}

	return test != static_cast<word_Ty>(0);
}

template <std::size_t bit_count, class word_Ty> template <class ptr_Ty1, class ptr_Ty2>
inline bool cool::bits<bit_count, word_Ty>::_less_cmp(ptr_Ty1 lhs_ptr, ptr_Ty2 rhs_ptr) noexcept
{
	word_Ty test_less = 0;
	word_Ty test_neq = 0;

	for (std::size_t n = 0; n < word_capacity; n++)
	{
		word_Ty lhs_word = *(lhs_ptr + n);
		word_Ty rhs_word = *(rhs_ptr + n);
		test_less |= ((lhs_word | rhs_word) ^ rhs_word);
		test_neq |= (lhs_word ^ rhs_word);
	}

	constexpr bool end_mask_needed = bit_count % word_size != 0;
	if (end_mask_needed)
	{
		constexpr word_Ty end_mask = static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_count % word_size)) - 1;

		word_Ty lhs_word = *(lhs_ptr + word_capacity) & end_mask;
		word_Ty rhs_word = *(rhs_ptr + word_capacity) & end_mask;
		test_less |= ((lhs_word | rhs_word) ^ rhs_word);
		test_neq |= (lhs_word ^ rhs_word);
	}

	return (test_less == static_cast<word_Ty>(0)) && (test_less != static_cast<word_Ty>(0));
}

// cmp

template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::operator==(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs == lhs;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::operator!=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs != lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator==(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs == lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator!=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs != lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator==(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs == lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator!=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs != lhs;
}

template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::operator<=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs >= lhs;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline bool cool::operator>=(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs <= lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator<=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs >= lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator>=(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs <= lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator<=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs >= lhs;
}
template <std::size_t bit_count, class word_Ty>
inline bool cool::operator>=(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs <= lhs;
}

// op

template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::operator&(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs & lhs;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::operator|(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs | lhs;
}
template <std::size_t bit_count, class word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::operator^(bool lhs, const cool::bits<bit_count, word_Ty>& rhs) noexcept {
	return rhs ^ lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator&(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs & lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator|(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs | lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator^(bool lhs, const cool::_vbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs ^ lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator&(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs & lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator|(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs | lhs;
}
template <std::size_t bit_count, class word_Ty>
inline cool::bits<bit_count, word_Ty> cool::operator^(bool lhs, const cool::_cvbits_proxy<bit_count, word_Ty>& rhs) noexcept {
	return rhs ^ lhs;
}

// constexpr constructing functions

template <std::size_t bit_count, class word_Ty, class ... args_uint_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::one_bits(args_uint_Ty&& ... bit_offsets) noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;

	constexpr std::size_t arg_count = sizeof...(args_uint_Ty);
	std::size_t bit_offsets_converted[arg_count] = { static_cast<std::size_t>(bit_offsets)... };

	for (std::size_t n = 0; n < arg_count; n++)
	{
		ret.m_field[bit_offsets_converted[n] / cool::bits<bit_count, word_Ty>::word_size]
			|= static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_offsets_converted[n]
				% cool::bits<bit_count, word_Ty>::word_size));
	}

	return ret;
}

template <std::size_t bit_count, class word_Ty, class ... args_uint_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::zero_bits(args_uint_Ty&& ... bit_offsets) noexcept
{
	cool::bits<bit_count, word_Ty> ret = true;

	constexpr std::size_t arg_count = sizeof...(args_uint_Ty);
	std::size_t bit_offsets_converted[arg_count] = { static_cast<std::size_t>(bit_offsets)... };

	for (std::size_t n = 0; n < arg_count; n++)
	{
		ret.m_field[bit_offsets_converted[n] / cool::bits<bit_count, word_Ty>::word_size]
			&= ~static_cast<word_Ty>(static_cast<word_Ty>(1) << (bit_offsets_converted[n]
				% cool::bits<bit_count, word_Ty>::word_size));
	}

	return ret;
}

template <std::size_t bit_count, class word_Ty, class uint_Ty1, class uint_Ty2>
constexpr inline cool::bits<bit_count, word_Ty> cool::one_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;

	if (static_cast<std::size_t>(lower) < static_cast<std::size_t>(higher))
	{
		std::size_t lower_word = static_cast<std::size_t>(lower) / cool::bits<bit_count, word_Ty>::word_size;
		ret.m_field[lower_word] = ~((static_cast<word_Ty>(1) << (static_cast<std::size_t>(lower)
			% cool::bits<bit_count, word_Ty>::word_size)) - 1);

		std::size_t higher_word = (static_cast<std::size_t>(higher) - 1) / cool::bits<bit_count, word_Ty>::word_size;
		for (std::size_t n = lower_word + 1; n <= higher_word; n++)
		{
			ret.m_field[n] = static_cast<word_Ty>(-1);
		}

		std::size_t remainder = static_cast<std::size_t>(higher) % cool::bits<bit_count, word_Ty>::word_size;
		if (remainder != 0)
		{
			ret.m_field[higher_word] &= static_cast<word_Ty>((static_cast<word_Ty>(1) << remainder) - 1);
		}
	}

	return ret;
}

template <std::size_t bit_count, class word_Ty, class uint_Ty1, class uint_Ty2>
constexpr inline cool::bits<bit_count, word_Ty> cool::zero_bits_range(uint_Ty1 lower, uint_Ty2 higher) noexcept
{
	cool::bits<bit_count, word_Ty> ret = true;

	if (static_cast<std::size_t>(lower) < static_cast<std::size_t>(higher))
	{
		std::size_t lower_word = static_cast<std::size_t>(lower) / cool::bits<bit_count, word_Ty>::word_size;
		ret.m_field[lower_word] = ((static_cast<word_Ty>(1) << (static_cast<std::size_t>(lower)
			% cool::bits<bit_count, word_Ty>::word_size)) - 1);

		std::size_t higher_word = (static_cast<std::size_t>(higher) - 1) / cool::bits<bit_count, word_Ty>::word_size;
		for (std::size_t n = lower_word + 1; n <= higher_word; n++)
		{
			ret.m_field[n] = static_cast<word_Ty>(0);
		}

		std::size_t remainder = static_cast<std::size_t>(higher) % cool::bits<bit_count, word_Ty>::word_size;
		if (remainder != 0)
		{
			ret.m_field[higher_word] |= ~static_cast<word_Ty>((static_cast<word_Ty>(1) << remainder) - 1);
		}
	}

	return ret;
}

template <std::size_t bit_count, class word_Ty, class rhs_word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::value_in_bits(rhs_word_Ty word_val) noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;

	ret.m_field[0] = static_cast<word_Ty>(word_val);

	return ret;
}

template <std::size_t bit_count, class word_Ty, class uint_Ty, class rhs_word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::value_in_bits(rhs_word_Ty word_val, uint_Ty bit_offset) noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;

	ret.m_field[0] = static_cast<word_Ty>(static_cast<word_Ty>(word_val) << static_cast<std::size_t>(bit_offset));

	return ret;
}

template <std::size_t bit_count, class word_Ty, class uint_Ty1, class uint_Ty2, class rhs_word_Ty>
constexpr inline cool::bits<bit_count, word_Ty> cool::value_in_bits(rhs_word_Ty word_val, uint_Ty1 bit_offset, uint_Ty2 word_offset) noexcept
{
	cool::bits<bit_count, word_Ty> ret = false;

	ret.m_field[word_offset] = static_cast<word_Ty>(static_cast<word_Ty>(word_val) << static_cast<std::size_t>(bit_offset));

	return ret;
}

// reinterpret address

template <std::size_t bit_count, class word_Ty, class uintptr_Ty>
inline cool::bits<bit_count, word_Ty>& cool::bits_at(uintptr_Ty mem_address) noexcept
{
	cool::bits<bit_count, word_Ty>* ptr = reinterpret_cast<cool::bits<bit_count, word_Ty>*>(static_cast<std::uintptr_t>(mem_address));
	new (ptr) cool::bits<bit_count, word_Ty>(cool::no_init);
	return *ptr;
}

template <std::size_t bit_count, class word_Ty, class uintptr_Ty>
inline const cool::bits<bit_count, word_Ty>& cool::const_bits_at(uintptr_Ty mem_address) noexcept
{
	cool::bits<bit_count, word_Ty>* ptr = reinterpret_cast<cool::bits<bit_count, word_Ty>*>(static_cast<std::uintptr_t>(mem_address));
	new (ptr) cool::bits<bit_count, word_Ty>(cool::no_init);
	return *ptr;
}

template <std::size_t bit_count, class word_Ty, class uintptr_Ty>
inline volatile cool::bits<bit_count, word_Ty>& cool::volatile_bits_at(uintptr_Ty mem_address) noexcept
{
	cool::bits<bit_count, word_Ty>* ptr = reinterpret_cast<cool::bits<bit_count, word_Ty>*>(static_cast<std::uintptr_t>(mem_address));
	new (ptr) cool::bits<bit_count, word_Ty>(cool::no_init);
	return *ptr;
}

template <std::size_t bit_count, class word_Ty, class uintptr_Ty>
inline const volatile cool::bits<bit_count, word_Ty>& cool::const_volatile_bits_at(uintptr_Ty mem_address) noexcept
{
	cool::bits<bit_count, word_Ty>* ptr = reinterpret_cast<cool::bits<bit_count, word_Ty>*>(static_cast<std::uintptr_t>(mem_address));
	new (ptr) cool::bits<bit_count, word_Ty>(cool::no_init);
	return *ptr;
}

// masked

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>
cool::masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const word_Ty*, const word_Ty*>(
		static_cast<const word_Ty*>(rhs.m_field), static_cast<const word_Ty*>(mask.m_field));
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>
cool::masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const word_Ty*, volatile const word_Ty*>(
		static_cast<const word_Ty*>(rhs.m_field), mask.m_data_ptr);
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const word_Ty*, const volatile word_Ty*>
cool::masked(const cool::bits<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const word_Ty*, volatile const word_Ty*>(
		static_cast<const word_Ty*>(rhs.m_field), mask.m_data_ptr);
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>
cool::masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>(
		rhs.m_data_ptr, static_cast<const word_Ty*>(mask.m_field));
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
cool::masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>(rhs.m_data_ptr, mask.m_data_ptr);
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
cool::masked(const cool::_vbits_proxy<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>(rhs.m_data_ptr, mask.m_data_ptr);
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>
cool::masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::bits<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const word_Ty*>(
		rhs.m_data_ptr, static_cast<const word_Ty*>(mask.m_field));
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
cool::masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::_vbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>(rhs.m_data_ptr, mask.m_data_ptr);
}

template <std::size_t bit_count, class word_Ty>
inline cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>
cool::masked(const cool::_cvbits_proxy<bit_count, word_Ty>& rhs, const cool::_cvbits_proxy<bit_count, word_Ty>& mask) noexcept {
	return cool::_masked_proxy<bit_count, const volatile word_Ty*, const volatile word_Ty*>(rhs.m_data_ptr, mask.m_data_ptr);
}

template <std::size_t bit_count, class ptr_Ty1, class ptr_Ty2>
inline cool::_masked_proxy<bit_count, ptr_Ty1, ptr_Ty2>::_masked_proxy(ptr_Ty1 rhs_ptr, ptr_Ty2 mask_ptr) noexcept
	: m_rhs_ptr(rhs_ptr), m_mask_ptr(mask_ptr) {}

#endif // _COOL_BITS_HPP


// cool_bits.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.