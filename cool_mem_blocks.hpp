// cool_mem_blocks.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_MEM_BLOCKS_HPP
#define xCOOL_MEM_BLOCKS_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <initializer_list>
#include <new>


namespace cool
{
	template <std::uintptr_t bad_alloc_address = 0> class mem_blocks;
	template <std::size_t _pool_count, std::uintptr_t bad_alloc_address = 0> class mem_pools;

	template <std::uintptr_t bad_alloc_address> class mem_blocks
	{

	public:

		inline mem_blocks() noexcept;
		mem_blocks(const cool::mem_blocks<bad_alloc_address>& rhs) = delete;
		cool::mem_blocks<bad_alloc_address>& operator=(const cool::mem_blocks<bad_alloc_address>& rhs) = delete;
		inline mem_blocks(cool::mem_blocks<bad_alloc_address>&& rhs) noexcept;
		inline cool::mem_blocks<bad_alloc_address>& operator=(cool::mem_blocks<bad_alloc_address>&& rhs) noexcept;
		~mem_blocks() = default;

		explicit inline mem_blocks(
			void* data_ptr,
			std::size_t _block_size,
			std::size_t _block_count,
			std::size_t _block_alignment = alignof(std::max_align_t)) noexcept;

		static inline constexpr std::size_t eval_data_max_size(
			std::size_t _block_size,
			std::size_t _block_count,
			std::size_t _block_alignment = alignof(std::max_align_t)) noexcept;

		static inline const void* eval_data_end(
			const void* data_ptr,
			std::size_t _block_size,
			std::size_t _block_count,
			std::size_t _block_alignment = alignof(std::max_align_t)) noexcept;

		static inline constexpr std::uintptr_t eval_data_address_end(
			std::uintptr_t data_ptr_address,
			std::size_t _block_size,
			std::size_t _block_count,
			std::size_t _block_alignment = alignof(std::max_align_t)) noexcept;

		inline void* init_set_data(
			void* data_ptr,
			std::size_t _block_size,
			std::size_t _block_count,
			std::size_t _block_alignment = alignof(std::max_align_t)) noexcept;


		inline void* allocate() noexcept;
		inline bool deallocate(void* ptr) noexcept;

		inline std::size_t block_size() const noexcept;
		inline std::size_t block_count() const noexcept;
		inline std::size_t blocks_remaining() const noexcept;

		static inline constexpr void* bad_alloc_ptr() noexcept;

		inline void* data_begin() noexcept;
		inline const void* data_begin() const noexcept;

		inline void* data_end() noexcept;
		inline const void* data_end() const noexcept;

		inline cool::mem_blocks<bad_alloc_address>& clear() noexcept;

		// use only if this mem_blocks instance is known to have at least one block remaining
		inline void* allocate_unchecked() noexcept;
		// use only if this mem_blocks instance is known to own ptr 
		inline void deallocate_unchecked(void* ptr) noexcept;

	private:

		template <std::size_t _pool_count, std::uintptr_t bad_alloc_address2> friend class cool::mem_pools;

		static inline std::size_t _next_power_of_two(std::size_t n) noexcept;

		std::size_t m_block_size;
		void** m_next_block_ptr;
		std::size_t m_blocks_remaining;
		void** m_first_block_ptr;
		void** m_last_block_ptr;
	};

	template <std::size_t _pool_count, std::uintptr_t bad_alloc_address> class mem_pools
	{

	public:

		mem_pools() noexcept = default;
		mem_pools(const cool::mem_pools<_pool_count, bad_alloc_address>& rhs) = delete;
		cool::mem_pools<_pool_count, bad_alloc_address>& operator=(const cool::mem_pools<_pool_count, bad_alloc_address>& rhs) = delete;
		mem_pools(cool::mem_pools<_pool_count, bad_alloc_address>&& rhs) noexcept = default;
		cool::mem_pools<_pool_count, bad_alloc_address>& operator=(cool::mem_pools<_pool_count, bad_alloc_address>&& rhs) noexcept = default;
		~mem_pools() = default;

		explicit inline mem_pools(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment = alignof(std::max_align_t)) noexcept;
		explicit inline mem_pools(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		template <class uint_Ty> inline cool::mem_blocks<bad_alloc_address>& operator[](uint_Ty pool_number) noexcept;
		template <class uint_Ty> inline const cool::mem_blocks<bad_alloc_address>& operator[](uint_Ty pool_number) const noexcept;

		static inline constexpr std::size_t eval_data_max_size(
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment = alignof(std::max_align_t)) noexcept;
		static inline constexpr std::size_t eval_data_max_size(
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		static inline void* eval_data_end(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment = alignof(std::max_align_t)) noexcept;
		static inline void* eval_data_end(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		static inline constexpr std::uintptr_t eval_data_address_end(
			std::uintptr_t data_ptr_address,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment = alignof(std::max_align_t)) noexcept;
		static inline constexpr std::uintptr_t eval_data_address_end(
			std::uintptr_t data_ptr_address,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		inline void* init_set_data(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment = alignof(std::max_align_t)) noexcept;
		inline void* init_set_data(
			void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;


		inline void* allocate(std::size_t block_size) noexcept;
		inline bool deallocate(void* ptr) noexcept;

		static inline constexpr void* bad_alloc_ptr() noexcept;

		inline cool::mem_pools<_pool_count, bad_alloc_address>& clear() noexcept;

	private:

		cool::mem_blocks<bad_alloc_address> m_pools[_pool_count];
	};
}


// detail

// mem_blocks

template <std::uintptr_t bad_alloc_address>
inline cool::mem_blocks<bad_alloc_address>::mem_blocks() noexcept
{
	m_block_size = 0;
	m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
	m_blocks_remaining = 0;
	m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
	m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());
}

template <std::uintptr_t bad_alloc_address>
inline cool::mem_blocks<bad_alloc_address>::mem_blocks(cool::mem_blocks<bad_alloc_address>&& rhs) noexcept
{
	m_block_size = rhs.m_block_size;
	m_next_block_ptr = rhs.m_next_block_ptr;
	m_blocks_remaining = rhs.m_blocks_remaining;
	m_first_block_ptr = rhs.m_first_block_ptr;
	m_last_block_ptr = rhs.m_last_block_ptr;

	rhs.m_block_size = 0;
	rhs.m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
	rhs.m_blocks_remaining = 0;
	rhs.m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
	rhs.m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());
}

template <std::uintptr_t bad_alloc_address>
inline cool::mem_blocks<bad_alloc_address>& cool::mem_blocks<bad_alloc_address>::operator=(cool::mem_blocks<bad_alloc_address>&& rhs) noexcept
{
	m_block_size = rhs.m_block_size;
	m_next_block_ptr = rhs.m_next_block_ptr;
	m_blocks_remaining = rhs.m_blocks_remaining;
	m_first_block_ptr = rhs.m_first_block_ptr;
	m_last_block_ptr = rhs.m_last_block_ptr;

	rhs.m_block_size = 0;
	rhs.m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
	rhs.m_blocks_remaining = 0;
	rhs.m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
	rhs.m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());

	return *this;
}

template <std::uintptr_t bad_alloc_address>
inline cool::mem_blocks<bad_alloc_address>::mem_blocks(
	void* data_ptr,
	std::size_t _block_size,
	std::size_t _block_count,
	std::size_t _block_alignment) noexcept
{
	if ((_block_size != 0) && (_block_count != 0))
	{
		_block_alignment = _next_power_of_two(_block_alignment);

		{
			std::size_t block_size_remainder = _block_size % _block_alignment;
			if (block_size_remainder != 0)
			{
				_block_size += (_block_alignment - block_size_remainder);
			}
		}

		std::size_t block_offset = _block_size / sizeof(void*);

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr) % _block_alignment;
			if (ptr_remainder != 0)
			{
				data_ptr = static_cast<void*>(static_cast<char*>(data_ptr) + (_block_alignment - ptr_remainder));
			}
		}

		m_block_size = _block_size;
		m_next_block_ptr = static_cast<void**>(data_ptr);
		m_blocks_remaining = _block_count;
		m_first_block_ptr = static_cast<void**>(data_ptr);
		m_last_block_ptr = static_cast<void**>(data_ptr) + _block_count * block_offset;

		void** _current_ptr = static_cast<void**>(data_ptr);
		void** _next_ptr = static_cast<void**>(data_ptr) + block_offset;
		std::size_t block_count_m1 = _block_count - 1;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count_m1; n++)
		{
			new (_current_ptr + n * block_offset) void* (static_cast<void*>(_next_ptr + n * block_offset));
		}
		new (_current_ptr + _block_count * block_offset) void* (static_cast<void*>(bad_alloc_ptr()));
	}
	else
	{
		m_block_size = 0;
		m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
		m_blocks_remaining = 0;
		m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
		m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());
	}
}

template <std::uintptr_t bad_alloc_address>
inline constexpr std::size_t cool::mem_blocks<bad_alloc_address>::eval_data_max_size(
	std::size_t _block_size,
	std::size_t _block_count,
	std::size_t _block_alignment) noexcept
{
	if ((_block_size != 0) && (_block_count != 0))
	{
		_block_alignment = _next_power_of_two(_block_alignment);

		{
			std::size_t block_size_remainder = _block_size % _block_alignment;
			if (block_size_remainder != 0)
			{
				_block_size += (_block_alignment - block_size_remainder);
			}
		}

		return _block_size * _block_count + (_block_alignment - 1);
	}
	else
	{
		return 0;
	}
}

template <std::uintptr_t bad_alloc_address>
inline const void* cool::mem_blocks<bad_alloc_address>::eval_data_end(
	const void* data_ptr,
	std::size_t _block_size,
	std::size_t _block_count,
	std::size_t _block_alignment) noexcept
{
	if ((_block_size != 0) && (_block_count != 0))
	{
		_block_alignment = _next_power_of_two(_block_alignment);

		{
			std::size_t block_size_remainder = _block_size % _block_alignment;
			if (block_size_remainder != 0)
			{
				_block_size += (_block_alignment - block_size_remainder);
			}
		}

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr) % _block_alignment;
			if (ptr_remainder != 0)
			{
				data_ptr = static_cast<const void*>(static_cast<const char*>(data_ptr) + (_block_alignment - ptr_remainder));
			}
		}

		return static_cast<const void*>(static_cast<const char*>(data_ptr) + _block_count * _block_size);
	}
	else
	{
		return data_ptr;
	}
}

template <std::uintptr_t bad_alloc_address>
inline constexpr std::uintptr_t cool::mem_blocks<bad_alloc_address>::eval_data_address_end(
	std::uintptr_t data_ptr_address,
	std::size_t _block_size,
	std::size_t _block_count,
	std::size_t _block_alignment) noexcept
{
	if ((_block_size != 0) && (_block_count != 0))
	{
		_block_alignment = _next_power_of_two(_block_alignment);

		{
			std::size_t block_size_remainder = _block_size % _block_alignment;
			if (block_size_remainder != 0)
			{
				_block_size += (_block_alignment - block_size_remainder);
			}
		}

		{
			std::uintptr_t ptr_address_remainder = data_ptr_address % _block_alignment;
			if (ptr_address_remainder != 0)
			{
				return data_ptr_address += (_block_alignment - ptr_address_remainder);
			}
		}

		return data_ptr_address + _block_count * _block_size;
	}
	else
	{
		return data_ptr_address;
	}
}

template <std::uintptr_t bad_alloc_address>
inline void* cool::mem_blocks<bad_alloc_address>::init_set_data(
	void* data_ptr,
	std::size_t _block_size,
	std::size_t _block_count,
	std::size_t _block_alignment) noexcept
{
	if ((_block_size != 0) && (_block_count != 0))
	{
		_block_alignment = _next_power_of_two(_block_alignment);

		{
			std::size_t block_size_remainder = _block_size % _block_alignment;
			if (block_size_remainder != 0)
			{
				_block_size += (_block_alignment - block_size_remainder);
			}
		}

		std::size_t block_offset = _block_size / sizeof(void*);

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr) % _block_alignment;
			if (ptr_remainder != 0)
			{
				data_ptr = static_cast<void*>(static_cast<char*>(data_ptr) + (_block_alignment - ptr_remainder));
			}
		}

		m_block_size = _block_size;
		m_next_block_ptr = static_cast<void**>(data_ptr);
		m_blocks_remaining = _block_count;
		m_first_block_ptr = static_cast<void**>(data_ptr);
		m_last_block_ptr = static_cast<void**>(data_ptr) + _block_count * block_offset;

		void** _current_ptr = static_cast<void**>(data_ptr);
		void** _next_ptr = static_cast<void**>(data_ptr) + block_offset;
		std::size_t block_count_m1 = _block_count - 1;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count_m1; n++)
		{
			new (_current_ptr + n * block_offset) void* (static_cast<void*>(_next_ptr + n * block_offset));
		}
		new (_current_ptr + _block_count * block_offset) void* (static_cast<void*>(bad_alloc_ptr()));
	}
	else
	{
		m_block_size = 0;
		m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
		m_blocks_remaining = 0;
		m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
		m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());
	}

	return m_last_block_ptr;
}

template <std::uintptr_t bad_alloc_address>
inline void* cool::mem_blocks<bad_alloc_address>::allocate() noexcept
{
	if (m_next_block_ptr != static_cast<void**>(bad_alloc_ptr()))
	{
		m_blocks_remaining--;
		void* ret = static_cast<void*>(m_next_block_ptr);
		m_next_block_ptr = static_cast<void**>(*m_next_block_ptr);
		return ret;
	}
	else
	{
		return bad_alloc_ptr();
	}
}

template <std::uintptr_t bad_alloc_address>
inline bool cool::mem_blocks<bad_alloc_address>::deallocate(void* ptr) noexcept
{
	if ((reinterpret_cast<std::uintptr_t>(m_first_block_ptr) <= reinterpret_cast<std::uintptr_t>(ptr))
		&& (reinterpret_cast<std::uintptr_t>(ptr) < reinterpret_cast<std::uintptr_t>(m_last_block_ptr)))
	{
		m_blocks_remaining++;
		void* temp = static_cast<void*>(m_next_block_ptr);
		m_next_block_ptr = static_cast<void**>(ptr);
		new (static_cast<void**>(m_next_block_ptr)) void* (temp);
		return true;
	}
	else
	{
		return false;
	}
}

template <std::uintptr_t bad_alloc_address>
inline std::size_t cool::mem_blocks<bad_alloc_address>::block_size() const noexcept
{
	return m_block_size;
}

template <std::uintptr_t bad_alloc_address>
inline std::size_t cool::mem_blocks<bad_alloc_address>::block_count() const noexcept
{
	if (m_block_size != 0)
	{
		return (sizeof(void*) * static_cast<std::size_t>(m_last_block_ptr - m_first_block_ptr)) / m_block_size;
	}
	else
	{
		return 0;
	}
}

template <std::uintptr_t bad_alloc_address>
inline std::size_t cool::mem_blocks<bad_alloc_address>::blocks_remaining() const noexcept
{
	return m_blocks_remaining;
}

template <std::uintptr_t bad_alloc_address>
inline constexpr void* cool::mem_blocks<bad_alloc_address>::bad_alloc_ptr() noexcept
{
	return reinterpret_cast<void*>(bad_alloc_address);
}

template <std::uintptr_t bad_alloc_address>
inline void* cool::mem_blocks<bad_alloc_address>::data_begin() noexcept
{
	return static_cast<void*>(m_first_block_ptr);
}

template <std::uintptr_t bad_alloc_address>
inline const void* cool::mem_blocks<bad_alloc_address>::data_begin() const noexcept
{
	return static_cast<const void*>(m_first_block_ptr);
}

template <std::uintptr_t bad_alloc_address>
inline void* cool::mem_blocks<bad_alloc_address>::data_end() noexcept
{
	return static_cast<void*>(m_last_block_ptr);
}

template <std::uintptr_t bad_alloc_address>
inline const void* cool::mem_blocks<bad_alloc_address>::data_end() const noexcept
{
	return static_cast<const void*>(m_last_block_ptr);
}

template <std::uintptr_t bad_alloc_address>
inline cool::mem_blocks<bad_alloc_address>& cool::mem_blocks<bad_alloc_address>::clear() noexcept
{
	m_block_size = 0;
	m_next_block_ptr = static_cast<void**>(bad_alloc_ptr());
	m_blocks_remaining = 0;
	m_first_block_ptr = static_cast<void**>(bad_alloc_ptr());
	m_last_block_ptr = static_cast<void**>(bad_alloc_ptr());

	return *this;
}

template <std::uintptr_t bad_alloc_address>
inline void* cool::mem_blocks<bad_alloc_address>::allocate_unchecked() noexcept
{
	m_blocks_remaining--;
	void* ret = static_cast<void*>(m_next_block_ptr);
	m_next_block_ptr = static_cast<void**>(*m_next_block_ptr);
	return ret;
}

template <std::uintptr_t bad_alloc_address>
inline void cool::mem_blocks<bad_alloc_address>::deallocate_unchecked(void* ptr) noexcept
{
	m_blocks_remaining++;
	void* temp = static_cast<void*>(m_next_block_ptr);
	m_next_block_ptr = static_cast<void**>(ptr);
	new (static_cast<void**>(m_next_block_ptr)) void* (temp);
}

template <std::uintptr_t bad_alloc_address>
inline std::size_t cool::mem_blocks<bad_alloc_address>::_next_power_of_two(std::size_t n) noexcept
{
	if (n <= alignof(void*))
	{
		return alignof(void*);
	}
	else
	{
		n--;
		for (std::size_t m = 1; m < sizeof(std::size_t) * CHAR_BIT; m *= 2)
		{
			n |= (n >> m);
		}
		return n + 1;
	}
}


// mem_pools

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline cool::mem_pools<_pool_count, bad_alloc_address>::mem_pools(
	void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline cool::mem_pools<_pool_count, bad_alloc_address>::mem_pools(
	void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();
	m = (m < block_alignments.size()) ? m : block_alignments.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	const std::size_t* block_alignments_ptr = block_alignments.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address> template <class uint_Ty>
inline cool::mem_blocks<bad_alloc_address>& cool::mem_pools<_pool_count, bad_alloc_address>::operator[](uint_Ty pool_number) noexcept
{
	return m_pools[static_cast<std::size_t>(pool_number)];
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address> template <class uint_Ty>
inline const cool::mem_blocks<bad_alloc_address>& cool::mem_pools<_pool_count, bad_alloc_address>::operator[](uint_Ty pool_number) const noexcept
{
	return m_pools[static_cast<std::size_t>(pool_number)];
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline constexpr std::size_t cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_max_size(
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	std::size_t data_size = 0;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		data_size += cool::mem_blocks<bad_alloc_address>::eval_data_max_size(
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return data_size;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline constexpr std::size_t cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_max_size(
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();
	m = (m < block_alignments.size()) ? m : block_alignments.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	const std::size_t* block_alignments_ptr = block_alignments.begin();
	std::size_t data_size = 0;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		data_size += cool::mem_blocks<bad_alloc_address>::eval_data_max_size(
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return data_size;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline void* cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_end(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = cool::mem_blocks<bad_alloc_address>::eval_data_end(ptr,
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return ptr;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline void* cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_end(
	void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();
	m = (m < block_alignments.size()) ? m : block_alignments.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	const std::size_t* block_alignments_ptr = block_alignments.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = cool::mem_blocks<bad_alloc_address>::eval_data_end(ptr,
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return ptr;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline constexpr std::uintptr_t cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_address_end(
	std::uintptr_t data_ptr_address,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	std::uintptr_t ptr_address = data_ptr_address;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr_address = cool::mem_blocks<bad_alloc_address>::eval_data_address_end(ptr_address,
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return ptr_address;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline constexpr std::uintptr_t cool::mem_pools<_pool_count, bad_alloc_address>::eval_data_address_end(
	std::uintptr_t data_ptr_address,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();
	m = (m < block_alignments.size()) ? m : block_alignments.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	const std::size_t* block_alignments_ptr = block_alignments.begin();
	std::uintptr_t ptr_address = data_ptr_address;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr_address = cool::mem_blocks<bad_alloc_address>::eval_data_address_end(ptr_address,
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return ptr_address;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline void* cool::mem_pools<_pool_count, bad_alloc_address>::init_set_data(
	void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	clear();

	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return ptr;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline void* cool::mem_pools<_pool_count, bad_alloc_address>::init_set_data(
	void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	clear();

	std::size_t m = _pool_count;
	m = (m < block_sizes.size()) ? m : block_sizes.size();
	m = (m < block_counts.size()) ? m : block_counts.size();
	m = (m < block_alignments.size()) ? m : block_alignments.size();

	const std::size_t* block_sizes_ptr = block_sizes.begin();
	const std::size_t* block_counts_ptr = block_counts.begin();
	const std::size_t* block_alignments_ptr = block_alignments.begin();
	void* ptr = data_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
	for (std::size_t n = 0; n < m; n++)
	{
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return ptr;
}


template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline void* cool::mem_pools<_pool_count, bad_alloc_address>::allocate(std::size_t block_size) noexcept
{
	for (std::size_t n = 0; n < _pool_count; n++)
	{
		if ((block_size <= m_pools[n].block_size())
			&& (m_pools[n].m_next_block_ptr != static_cast<void**>(bad_alloc_ptr())))
		{
			m_pools[n].m_blocks_remaining--;
			void* ret = static_cast<void*>(m_pools[n].m_next_block_ptr);
			m_pools[n].m_next_block_ptr = static_cast<void**>(*m_pools[n].m_next_block_ptr);
			return ret;
		}
	}

	return bad_alloc_ptr();
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline bool cool::mem_pools<_pool_count, bad_alloc_address>::deallocate(void* ptr) noexcept
{
	for (std::size_t n = 0; n < _pool_count; n++)
	{
		if ((reinterpret_cast<std::uintptr_t>(m_pools[n].m_first_block_ptr) <= reinterpret_cast<std::uintptr_t>(ptr))
			&& (reinterpret_cast<std::uintptr_t>(ptr) < reinterpret_cast<std::uintptr_t>(m_pools[n].m_last_block_ptr)))
		{
			m_pools[n].m_blocks_remaining++;
			void* temp = static_cast<void*>(m_pools[n].m_next_block_ptr);
			m_pools[n].m_next_block_ptr = static_cast<void**>(ptr);
			new (static_cast<void**>(m_pools[n].m_next_block_ptr)) void* (temp);
			return true;
		}
	}

	return false;
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline constexpr void* cool::mem_pools<_pool_count, bad_alloc_address>::bad_alloc_ptr() noexcept
{
	return reinterpret_cast<void*>(bad_alloc_address);
}

template <std::size_t _pool_count, std::uintptr_t bad_alloc_address>
inline cool::mem_pools<_pool_count, bad_alloc_address>& cool::mem_pools<_pool_count, bad_alloc_address>::clear() noexcept
{
	for (std::size_t n = 0; n < _pool_count; n++)
	{
		m_pools[n].clear();
	}

	return *this;
}

#endif // xCOOL_MEM_BLOCKS_HPP


// cool_mem_blocks.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.