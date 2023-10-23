// cool_mem_blocks.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef _COOL_MEM_BLOCKS_HPP
#define _COOL_MEM_BLOCKS_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>


namespace cool
{
	template <std::uintptr_t default_mem_address = 0> class mem_blocks;
	template <std::size_t pool_count, std::uintptr_t default_mem_address = 0> class mem_pools;

	template <std::uintptr_t default_mem_address> class alignas(4 * sizeof(std::size_t)) mem_blocks
	{

	public:

		inline mem_blocks() noexcept;
		mem_blocks(const cool::mem_blocks<default_mem_address>& rhs) = delete;
		cool::mem_blocks<default_mem_address>& operator=(const cool::mem_blocks<default_mem_address>& rhs) = delete;
		inline mem_blocks(cool::mem_blocks<default_mem_address>&& rhs) noexcept;
		inline cool::mem_blocks<default_mem_address>& operator=(cool::mem_blocks<default_mem_address>&& rhs) noexcept;
		~mem_blocks() = default;

		explicit inline mem_blocks(void* data_ptr,
			std::size_t block_size,
			std::size_t block_count) noexcept;
		explicit inline mem_blocks(void* data_ptr,
			std::size_t block_size,
			std::size_t block_count,
			std::size_t block_alignment) noexcept;

		static constexpr inline std::size_t eval_data_max_size(
			std::size_t block_size,
			std::size_t block_count) noexcept;
		static constexpr inline std::size_t eval_data_max_size(
			std::size_t block_size,
			std::size_t block_count,
			std::size_t block_alignment) noexcept;

		static inline const void* eval_data_end(const void* data_ptr,
			std::size_t block_size,
			std::size_t block_count) noexcept;
		static inline const void* eval_data_end(const void* data_ptr,
			std::size_t block_size,
			std::size_t block_count,
			std::size_t block_alignment) noexcept;

		static constexpr inline std::uintptr_t eval_data_end(std::uintptr_t data_ptr_address,
			std::size_t block_size,
			std::size_t block_count) noexcept;
		static constexpr inline std::uintptr_t eval_data_end(std::uintptr_t data_ptr_address,
			std::size_t block_size,
			std::size_t block_count,
			std::size_t block_alignment) noexcept;

		inline void* init_set_data(void* data_ptr,
			std::size_t block_size,
			std::size_t block_count) noexcept;
		inline void* init_set_data(void* data_ptr,
			std::size_t block_size,
			std::size_t block_count,
			std::size_t block_alignment) noexcept;

		inline void* allocate() noexcept;

		inline bool deallocate(void* ptr) noexcept;

		static inline constexpr void* bad_alloc_ptr() noexcept;

		inline void* allocate_unchecked() noexcept;

		inline void deallocate_unchecked(void* ptr) noexcept;

		inline std::size_t block_size() const noexcept;

		inline std::size_t blocks_remaining() const noexcept;

		inline void* data() noexcept;
		inline const void* data() const noexcept;

		inline cool::mem_blocks<default_mem_address>& clear() noexcept;

	private:

		template <std::size_t pool_count, std::uintptr_t default_mem_address2> friend class cool::mem_pools;

		void** m_current_address_ptr = nullptr;
		void** m_first_address_ptr = nullptr;
		void* m_first_block_ptr = nullptr;
		std::size_t m_block_size = 0;
	};

	template <std::size_t pool_count, std::uintptr_t default_mem_address> class mem_pools
	{

	public:

		mem_pools() noexcept = default;
		mem_pools(const cool::mem_pools<pool_count, default_mem_address>& rhs) = delete;
		cool::mem_pools<pool_count, default_mem_address>& operator=(const cool::mem_pools<pool_count, default_mem_address>& rhs) = delete;
		mem_pools(cool::mem_pools<pool_count, default_mem_address>&& rhs) noexcept = default;
		cool::mem_pools<pool_count, default_mem_address>& operator=(cool::mem_pools<pool_count, default_mem_address>&& rhs) noexcept = default;
		~mem_pools() = default;

		explicit inline mem_pools(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts) noexcept;
		explicit inline mem_pools(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		template <class uint_Ty> inline cool::mem_blocks<default_mem_address>& operator[](uint_Ty pool_number) noexcept;
		template <class uint_Ty> inline const cool::mem_blocks<default_mem_address>& operator[](uint_Ty pool_number) const noexcept;

		static constexpr inline std::size_t eval_data_max_size(
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts) noexcept;
		static constexpr inline std::size_t eval_data_max_size(
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment) noexcept;
		static constexpr inline std::size_t eval_data_max_size(
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		static inline void* eval_data_end(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts) noexcept;
		static inline void* eval_data_end(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment) noexcept;
		static inline void* eval_data_end(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		static constexpr inline std::uintptr_t eval_data_end(std::uintptr_t data_ptr_address,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts) noexcept;
		static constexpr inline std::uintptr_t eval_data_end(std::uintptr_t data_ptr_address,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment) noexcept;
		static constexpr inline std::uintptr_t eval_data_end(std::uintptr_t data_ptr_address,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		inline void* init_set_data(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts) noexcept;
		inline void* init_set_data(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::size_t block_alignment) noexcept;
		inline void* init_set_data(void* data_ptr,
			std::initializer_list<std::size_t> block_sizes,
			std::initializer_list<std::size_t> block_counts,
			std::initializer_list<std::size_t> block_alignments) noexcept;

		inline void* allocate(std::size_t block_size) noexcept;

		inline bool deallocate(void* ptr) noexcept;

		static inline constexpr void* bad_alloc_ptr() noexcept;

		inline cool::mem_pools<pool_count, default_mem_address>& clear() noexcept;

	private:

		cool::mem_blocks<default_mem_address> m_pools[pool_count];
	};
}


// detail

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>::mem_blocks() noexcept
{
	m_current_address_ptr = nullptr;
	m_first_address_ptr = nullptr;
	m_first_block_ptr = nullptr;
	m_block_size = 0;
}

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>::mem_blocks(cool::mem_blocks<default_mem_address>&& rhs) noexcept
{
	m_current_address_ptr = rhs.m_current_address_ptr;
	m_first_address_ptr = rhs.m_first_address_ptr;
	m_first_block_ptr = rhs.m_first_block_ptr;
	m_block_size = rhs.m_block_size;

	rhs.m_current_address_ptr = nullptr;
	rhs.m_first_address_ptr = nullptr;
	rhs.m_first_block_ptr = nullptr;
	rhs.m_block_size = 0;
}

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>& cool::mem_blocks<default_mem_address>::operator=(cool::mem_blocks<default_mem_address>&& rhs) noexcept
{
	m_current_address_ptr = rhs.m_current_address_ptr;
	m_first_address_ptr = rhs.m_first_address_ptr;
	m_first_block_ptr = rhs.m_first_block_ptr;
	m_block_size = rhs.m_block_size;

	rhs.m_current_address_ptr = nullptr;
	rhs.m_first_address_ptr = nullptr;
	rhs.m_first_block_ptr = nullptr;
	rhs.m_block_size = 0;

	return *this;
}

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>::mem_blocks(void* data_ptr,
	std::size_t block_size,
	std::size_t block_count) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		m_block_size = block_size;
		m_first_block_ptr = data_ptr;

		char* _first_block_ptr = static_cast<char*>(data_ptr);
		char* _first_address_ptr = _first_block_ptr + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(void*) - static_cast<std::size_t>(ptr_remainder));
		}

		m_first_address_ptr = static_cast<void**>(static_cast<void*>(_first_address_ptr));

		void** _current_ptr = m_first_address_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count; n++)
		{
			*_current_ptr++ = static_cast<void*>(_first_block_ptr + block_size * n);
		}

		m_current_address_ptr = _current_ptr;
	}
	else
	{
		m_current_address_ptr = nullptr;
		m_first_address_ptr = nullptr;
		m_first_block_ptr = nullptr;
		m_block_size = 0;
	}
}

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>::mem_blocks(void* data_ptr,
	std::size_t block_size,
	std::size_t block_count,
	std::size_t block_alignment) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		if (block_alignment > 1)
		{
			std::size_t size_remainder = block_size % block_alignment;
			block_size += static_cast<std::size_t>(size_remainder != 0) * (block_alignment - size_remainder);

			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr)
				% static_cast<std::uintptr_t>(block_alignment);

			data_ptr = static_cast<void*>(static_cast<char*>(data_ptr)
				+ static_cast<std::size_t>(ptr_remainder != 0)
				* (block_alignment - static_cast<std::size_t>(ptr_remainder)));
		}

		m_block_size = block_size;
		m_first_block_ptr = data_ptr;

		char* _first_block_ptr = static_cast<char*>(data_ptr);
		char* _first_address_ptr = _first_block_ptr + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(void*) - static_cast<std::size_t>(ptr_remainder));
		}

		m_first_address_ptr = static_cast<void**>(static_cast<void*>(_first_address_ptr));

		void** _current_ptr = m_first_address_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count; n++)
		{
			*_current_ptr++ = static_cast<void*>(_first_block_ptr + block_size * n);
		}

		m_current_address_ptr = _current_ptr;
	}
	else
	{
		m_current_address_ptr = nullptr;
		m_first_address_ptr = nullptr;
		m_first_block_ptr = nullptr;
		m_block_size = 0;
	}
}

template <std::uintptr_t default_mem_address>
inline constexpr std::size_t cool::mem_blocks<default_mem_address>::eval_data_max_size(
	std::size_t block_size,
	std::size_t block_count) noexcept
{
	return block_size * block_count + (sizeof(void*) - 1)
		+ block_count * sizeof(void*);
}

template <std::uintptr_t default_mem_address>
inline constexpr std::size_t cool::mem_blocks<default_mem_address>::eval_data_max_size(
	std::size_t block_size,
	std::size_t block_count,
	std::size_t block_alignment) noexcept
{
	if (block_alignment > 1)
	{
		std::size_t size_remainder = block_size % block_alignment;
		block_size += static_cast<std::size_t>(size_remainder != 0) * (block_alignment - size_remainder);

		return (block_alignment - 1) + block_size * block_count
			+ (sizeof(void*) - 1) + block_count * sizeof(void*);
	}
	else
	{
		return block_size * block_count + (sizeof(void*) - 1)
			+ block_count * sizeof(void*);
	}
}

template <std::uintptr_t default_mem_address>
inline const void* cool::mem_blocks<default_mem_address>::eval_data_end(const void* data_ptr,
	std::size_t block_size,
	std::size_t block_count) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		const char* _first_address_ptr = static_cast<const char*>(data_ptr) + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(const void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(const void*) - static_cast<std::size_t>(ptr_remainder));
		}

		return static_cast<const void*>(_first_address_ptr + block_count * sizeof(const void*));
	}
	else
	{
		return data_ptr;
	}
}

template <std::uintptr_t default_mem_address>
inline const void* cool::mem_blocks<default_mem_address>::eval_data_end(const void* data_ptr,
	std::size_t block_size,
	std::size_t block_count,
	std::size_t block_alignment) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		if (block_alignment > 1)
		{
			std::size_t size_remainder = block_size % block_alignment;
			block_size += static_cast<std::size_t>(size_remainder != 0) * (block_alignment - size_remainder);

			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr)
				% static_cast<std::uintptr_t>(block_alignment);

			data_ptr = static_cast<const void*>(static_cast<const char*>(data_ptr)
				+ static_cast<std::size_t>(ptr_remainder != 0)
				* (block_alignment - static_cast<std::size_t>(ptr_remainder)));
		}

		const char* _first_address_ptr = static_cast<const char*>(data_ptr) + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(const void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(const void*) - static_cast<std::size_t>(ptr_remainder));
		}

		return static_cast<const void*>(_first_address_ptr + block_count * sizeof(const void*));
	}
	else
	{
		return data_ptr;
	}
}

template <std::uintptr_t default_mem_address>
inline constexpr std::uintptr_t cool::mem_blocks<default_mem_address>::eval_data_end(std::uintptr_t data_ptr_address,
	std::size_t block_size,
	std::size_t block_count) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		std::uintptr_t _first_address_ptr = data_ptr_address + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = _first_address_ptr
				% static_cast<std::uintptr_t>(sizeof(const void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(const void*) - static_cast<std::size_t>(ptr_remainder));
		}

		return _first_address_ptr + block_count * sizeof(const void*);
	}
	else
	{
		return data_ptr_address;
	}
}

template <std::uintptr_t default_mem_address>
inline constexpr std::uintptr_t cool::mem_blocks<default_mem_address>::eval_data_end(std::uintptr_t data_ptr_address,
	std::size_t block_size,
	std::size_t block_count,
	std::size_t block_alignment) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		if (block_alignment > 1)
		{
			std::size_t size_remainder = block_size % block_alignment;
			block_size += static_cast<std::size_t>(size_remainder != 0) * (block_alignment - size_remainder);

			std::uintptr_t ptr_remainder = data_ptr_address
				% static_cast<std::uintptr_t>(block_alignment);

			data_ptr_address += static_cast<std::size_t>(ptr_remainder != 0)
				* (block_alignment - static_cast<std::size_t>(ptr_remainder));
		}

		std::uintptr_t _first_address_ptr = data_ptr_address + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = _first_address_ptr
				% static_cast<std::uintptr_t>(sizeof(const void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(const void*) - static_cast<std::size_t>(ptr_remainder));
		}

		return _first_address_ptr + block_count * sizeof(const void*);
	}
	else
	{
		return data_ptr_address;
	}
}

template <std::uintptr_t default_mem_address>
inline void* cool::mem_blocks<default_mem_address>::init_set_data(void* data_ptr,
	std::size_t block_size,
	std::size_t block_count) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		m_block_size = block_size;
		m_first_block_ptr = data_ptr;

		char* _first_block_ptr = static_cast<char*>(data_ptr);
		char* _first_address_ptr = _first_block_ptr + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(void*) - static_cast<std::size_t>(ptr_remainder));
		}

		m_first_address_ptr = static_cast<void**>(static_cast<void*>(_first_address_ptr));

		void** _current_ptr = m_first_address_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count; n++)
		{
			new (_current_ptr) void*(static_cast<void*>(_first_block_ptr + block_size * n));
			_current_ptr++;
		}

		m_current_address_ptr = _current_ptr;

		return static_cast<void*>(_current_ptr);
	}
	else
	{
		m_current_address_ptr = nullptr;
		m_first_address_ptr = nullptr;
		m_first_block_ptr = nullptr;
		m_block_size = 0;

		return data_ptr;
	}
}

template <std::uintptr_t default_mem_address>
inline void* cool::mem_blocks<default_mem_address>::init_set_data(void* data_ptr,
	std::size_t block_size,
	std::size_t block_count,
	std::size_t block_alignment) noexcept
{
	if ((block_size != 0) && (block_count != 0))
	{
		if (block_alignment > 1)
		{
			std::size_t size_remainder = block_size % block_alignment;
			block_size += static_cast<std::size_t>(size_remainder != 0) * (block_alignment - size_remainder);

			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(data_ptr)
				% static_cast<std::uintptr_t>(block_alignment);

			data_ptr = static_cast<void*>(static_cast<char*>(data_ptr)
				+ static_cast<std::size_t>(ptr_remainder != 0)
				* (block_alignment - static_cast<std::size_t>(ptr_remainder)));
		}

		m_block_size = block_size;
		m_first_block_ptr = data_ptr;

		char* _first_block_ptr = static_cast<char*>(data_ptr);
		char* _first_address_ptr = _first_block_ptr + block_size * block_count;

		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(_first_address_ptr)
				% static_cast<std::uintptr_t>(sizeof(void*));

			_first_address_ptr += static_cast<std::size_t>(ptr_remainder != 0)
				* (sizeof(void*) - static_cast<std::size_t>(ptr_remainder));
		}

		m_first_address_ptr = static_cast<void**>(static_cast<void*>(_first_address_ptr));

		void** _current_ptr = m_first_address_ptr;
#ifdef __clang__
#pragma unroll 1
#endif // __clang__
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC unroll 1
#endif // defined(__GNUC__) && !defined(__clang__)
		for (std::size_t n = 0; n < block_count; n++)
		{
			new (_current_ptr) void*(static_cast<void*>(_first_block_ptr + block_size * n));
			_current_ptr++;
		}

		m_current_address_ptr = _current_ptr;

		return static_cast<void*>(_current_ptr);
	}
	else
	{
		m_current_address_ptr = nullptr;
		m_first_address_ptr = nullptr;
		m_first_block_ptr = nullptr;
		m_block_size = 0;

		return data_ptr;
	}
}

template <std::uintptr_t default_mem_address>
inline void* cool::mem_blocks<default_mem_address>::allocate() noexcept
{
	if (m_current_address_ptr > m_first_address_ptr)
	{
		m_current_address_ptr--;
		return *m_current_address_ptr;
	}
	else
	{
		return reinterpret_cast<void*>(default_mem_address);
	}
}

template <std::uintptr_t default_mem_address>
inline bool cool::mem_blocks<default_mem_address>::deallocate(void* ptr) noexcept
{
	if ((m_first_block_ptr <= ptr) && (ptr < static_cast<void*>(m_first_address_ptr)))
	{
		*m_current_address_ptr++ = ptr;
		return true;
	}
	else
	{
		return false;
	}
}

template <std::uintptr_t default_mem_address>
inline constexpr void* cool::mem_blocks<default_mem_address>::bad_alloc_ptr() noexcept
{
	return reinterpret_cast<void*>(default_mem_address);
}

template <std::uintptr_t default_mem_address>
inline void* cool::mem_blocks<default_mem_address>::allocate_unchecked() noexcept
{
	m_current_address_ptr--;
	return *m_current_address_ptr;
}

template <std::uintptr_t default_mem_address>
inline void cool::mem_blocks<default_mem_address>::deallocate_unchecked(void* ptr) noexcept
{
	*m_current_address_ptr++ = ptr;
}

template <std::uintptr_t default_mem_address>
inline std::size_t cool::mem_blocks<default_mem_address>::block_size() const noexcept
{
	return m_block_size;
}

template <std::uintptr_t default_mem_address>
inline std::size_t cool::mem_blocks<default_mem_address>::blocks_remaining() const noexcept
{
	return static_cast<std::size_t>(m_current_address_ptr - m_first_address_ptr);
}

template <std::uintptr_t default_mem_address>
inline void* cool::mem_blocks<default_mem_address>::data() noexcept
{
	return m_first_block_ptr;
}

template <std::uintptr_t default_mem_address>
inline const void* cool::mem_blocks<default_mem_address>::data() const noexcept
{
	return m_first_block_ptr;
}

template <std::uintptr_t default_mem_address>
inline cool::mem_blocks<default_mem_address>& cool::mem_blocks<default_mem_address>::clear() noexcept
{
	m_current_address_ptr = nullptr;
	m_first_address_ptr = nullptr;
	m_first_block_ptr = nullptr;
	m_block_size = 0;

	return *this;
}


template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline cool::mem_pools<pool_count, default_mem_address>::mem_pools(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts) noexcept
{
	std::size_t m = pool_count;
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
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++);
	}
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline cool::mem_pools<pool_count, default_mem_address>::mem_pools(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = pool_count;
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

template <std::size_t pool_count, std::uintptr_t default_mem_address> template <class uint_Ty>
inline cool::mem_blocks<default_mem_address>& cool::mem_pools<pool_count, default_mem_address>::operator[](uint_Ty pool_number) noexcept
{
	return m_pools[static_cast<std::size_t>(pool_number)];
}

template <std::size_t pool_count, std::uintptr_t default_mem_address> template <class uint_Ty>
inline const cool::mem_blocks<default_mem_address>& cool::mem_pools<pool_count, default_mem_address>::operator[](uint_Ty pool_number) const noexcept
{
	return m_pools[static_cast<std::size_t>(pool_number)];
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline constexpr std::size_t cool::mem_pools<pool_count, default_mem_address>::eval_data_max_size(
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts) noexcept
{
	std::size_t m = pool_count;
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
		data_size += cool::mem_blocks<default_mem_address>::eval_data_max_size(
			*block_sizes_ptr++, *block_counts_ptr++);
	}

	return data_size;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline constexpr std::size_t cool::mem_pools<pool_count, default_mem_address>::eval_data_max_size(
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = pool_count;
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
		data_size += cool::mem_blocks<default_mem_address>::eval_data_max_size(
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return data_size;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline constexpr std::size_t cool::mem_pools<pool_count, default_mem_address>::eval_data_max_size(
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = pool_count;
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
		data_size += cool::mem_blocks<default_mem_address>::eval_data_max_size(
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return data_size;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::eval_data_end(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts) noexcept
{
	std::size_t m = pool_count;
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
		ptr = cool::mem_blocks<default_mem_address>::eval_data_end(ptr,
			*block_sizes_ptr++, *block_counts_ptr++);
	}

	return ptr;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::eval_data_end(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = pool_count;
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
		ptr = cool::mem_blocks<default_mem_address>::eval_data_end(ptr,
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return ptr;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::eval_data_end(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = pool_count;
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
		ptr = cool::mem_blocks<default_mem_address>::eval_data_end(ptr,
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return ptr;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
constexpr inline std::uintptr_t cool::mem_pools<pool_count, default_mem_address>::eval_data_end(std::uintptr_t data_ptr_address,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts) noexcept
{
	std::size_t m = pool_count;
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
		ptr_address = cool::mem_blocks<default_mem_address>::eval_data_end(ptr_address,
			*block_sizes_ptr++, *block_counts_ptr++);
	}

	return ptr_address;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
constexpr inline std::uintptr_t cool::mem_pools<pool_count, default_mem_address>::eval_data_end(std::uintptr_t data_ptr_address,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	std::size_t m = pool_count;
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
		ptr_address = cool::mem_blocks<default_mem_address>::eval_data_end(ptr_address,
			*block_sizes_ptr++, *block_counts_ptr++, block_alignment);
	}

	return ptr_address;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
constexpr inline std::uintptr_t cool::mem_pools<pool_count, default_mem_address>::eval_data_end(std::uintptr_t data_ptr_address,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	std::size_t m = pool_count;
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
		ptr_address = cool::mem_blocks<default_mem_address>::eval_data_end(ptr_address,
			*block_sizes_ptr++, *block_counts_ptr++, *block_alignments_ptr++);
	}

	return ptr_address;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::init_set_data(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts) noexcept
{
	clear();

	std::size_t m = pool_count;
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
		ptr = m_pools[n].init_set_data(ptr, *block_sizes_ptr++, *block_counts_ptr++);
	}

	return ptr;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::init_set_data(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::size_t block_alignment) noexcept
{
	clear();

	std::size_t m = pool_count;
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

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::init_set_data(void* data_ptr,
	std::initializer_list<std::size_t> block_sizes,
	std::initializer_list<std::size_t> block_counts,
	std::initializer_list<std::size_t> block_alignments) noexcept
{
	clear();

	std::size_t m = pool_count;
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

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline void* cool::mem_pools<pool_count, default_mem_address>::allocate(std::size_t block_size) noexcept
{
	for (std::size_t n = 0; n < pool_count; n++)
	{
		if ((block_size <= m_pools[n].block_size())
			&& (m_pools[n].m_current_address_ptr > m_pools[n].m_first_address_ptr))
		{
			m_pools[n].m_current_address_ptr--;
			return *(m_pools[n].m_current_address_ptr);
		}
	}

	return reinterpret_cast<void*>(default_mem_address);
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline bool cool::mem_pools<pool_count, default_mem_address>::deallocate(void* ptr) noexcept
{
	for (std::size_t n = 0; n < pool_count; n++)
	{
		if (m_pools[n].deallocate(ptr))
		{
			return true;
		}
	}

	return false;
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline constexpr void* cool::mem_pools<pool_count, default_mem_address>::bad_alloc_ptr() noexcept
{
	return reinterpret_cast<void*>(default_mem_address);
}

template <std::size_t pool_count, std::uintptr_t default_mem_address>
inline cool::mem_pools<pool_count, default_mem_address>& cool::mem_pools<pool_count, default_mem_address>::clear() noexcept
{
	for (std::size_t n = 0; n < pool_count; n++)
	{
		m_pools[n].clear();
	}

	return *this;
}

#endif // _COOL_MEM_BLOCKS_HPP


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
