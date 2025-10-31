// cool_queues.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_QUEUES_HPP
#define xCOOL_QUEUES_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <new>
#include <cassert>

// to allow the use of atomic to enable cool::queue_spsc/cool::queue_mpmc : #define COOL_QUEUES_ATOMIC

#ifdef COOL_QUEUES_ATOMIC
#include <atomic>
#include <limits>
#include <cstring>

#endif // COOL_QUEUES_ATOMIC

// to allow the use of this_thread/mutex/condition_variable to enable cool::queue_wlock : #define COOL_QUEUES_THREAD

#ifdef COOL_QUEUES_THREAD
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef COOL_QUEUES_ATOMIC
#include <atomic>

#endif // COOL_QUEUES_ATOMIC
#endif // COOL_QUEUES_THREAD


// custom wait class prototype

//class wait_example
//{
//public:
//
//	using value_type = TYPE;
//
//	static inline void push_wait(value_type&) noexcept;
//	static inline void pop_wait(value_type&) noexcept;
//};

// TYPE must allow initialization with empty curly braces


namespace cool
{
	template <class Ty> class queue_nosync;

	class wait_noop;
#ifdef COOL_QUEUES_ATOMIC
	template <class Ty, std::size_t _cache_line_size, class _wait_Ty = wait_noop> class queue_spsc;
	template <class Ty, std::size_t _cache_line_size, class _wait_Ty = wait_noop, class _uintX_t = std::uint32_t> class queue_mpmc;
#endif // COOL_QUEUES_ATOMIC

#ifdef COOL_QUEUES_THREAD
	class wait_yield;
	template <class Ty, std::size_t _cache_line_size, class _wait_Ty = wait_yield> class queue_wlock;
#endif // COOL_QUEUES_THREAD

	class item_buffer_size;
	class queue_init_result;


	// init type

	class item_buffer_size {
	public:
		item_buffer_size() = delete;
		explicit inline constexpr item_buffer_size(std::size_t new_item_buffer_size) noexcept;
		inline constexpr std::size_t value() const noexcept;
	private:
		std::size_t m_value;
	};

	// queue_init_result

	class queue_init_result
	{

	public:

		static constexpr int success = 0;
		static constexpr int bad_align = 1;
		static constexpr int bad_parameters = 2;
		static constexpr int bad_alloc = 3;
		static constexpr int not_lockfree = 4;

		static constexpr int undefined = -1;

		queue_init_result() = default;

		inline operator bool() const noexcept; // returns true if queue init is successful
		inline bool good() const noexcept; // returns true if queue init is successful
		inline int value() const noexcept;
		inline const char* message() const noexcept;

	private:

		inline queue_init_result(int result) noexcept;

		int m_result = cool::queue_init_result::undefined;

		template <class Ty> friend class cool::queue_nosync;

#ifdef COOL_QUEUES_ATOMIC
		template <class Ty, std::size_t _cache_line_size, class _wait_Ty> friend class cool::queue_spsc;
		template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX> friend class cool::queue_mpmc;
#endif // COOL_QUEUES_ATOMIC

#ifdef COOL_QUEUES_THREAD
		template <class Ty, std::size_t _cache_line_size, class _wait_Ty> friend class cool::queue_wlock;
#endif // COOL_QUEUES_THREAD
	};


	// queue_nosync

	template <class Ty> class queue_nosync
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		queue_nosync() = default;
		queue_nosync(const cool::queue_nosync<Ty>&) = delete;
		cool::queue_nosync<Ty>& operator=(const cool::queue_nosync<Ty>&) = delete;
		queue_nosync(cool::queue_nosync<Ty>&&) = delete;
		cool::queue_nosync<Ty>& operator=(cool::queue_nosync<Ty>&&) = delete;
		~queue_nosync();

		// WARNING : array at data_ptr must have space for new_item_buffer_size.value() + 1 elements
		inline cool::queue_init_result init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size);
		inline cool::queue_init_result init_new_buffer(cool::item_buffer_size new_item_buffer_size);
		inline bool good() const noexcept;
		inline bool owns_buffer() const noexcept;
		inline std::size_t size() const noexcept;
		inline void delete_buffer() noexcept;

		template <class ... arg_Ty> inline bool try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value);
		inline bool try_pop(Ty& target) noexcept;

	private:

		Ty* m_item_buffer_data_ptr = nullptr;
		Ty* m_item_buffer_end_ptr = nullptr;
		Ty* m_last_item_ptr = nullptr;
		Ty* m_next_item_ptr = nullptr;

		bool m_good = false;
		bool m_owns_buffer = false;
	};


	// wait_noop

	class wait_noop
	{
	public:
		using value_type = unsigned int;
		static inline void push_wait(value_type&) noexcept;
		static inline void pop_wait(value_type&) noexcept;
	};

#ifdef COOL_QUEUES_ATOMIC

	// queue_spsc

	template <class Ty, std::size_t _cache_line_size, class _wait_Ty> class alignas(_cache_line_size) queue_spsc
	{

	public:

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::queue_spsc<value_type, cache_line_size, wait_type> requirement : cache_line_size must be a power of 2");

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t cache_line_size = alignof(cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>);

		using wait_type = _wait_Ty;

		queue_spsc() = default;
		queue_spsc(const cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>&) = delete;
		cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>& operator=(const cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>&) = delete;
		queue_spsc(cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>&&) = delete;
		cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>& operator=(cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>&&) = delete;
		~queue_spsc();

		// WARNING : array at data_ptr must have space for new_item_buffer_size.value() + 1 elements
		inline cool::queue_init_result init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size);
		inline cool::queue_init_result init_new_buffer(cool::item_buffer_size new_item_buffer_size);
		inline bool good() const noexcept;
		inline std::size_t size() const noexcept;
		inline bool owns_buffer() const noexcept;
		inline void delete_buffer() noexcept;

		template <class ... arg_Ty> inline bool try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value);
		inline bool try_pop(Ty& target) noexcept;
		template <class ... arg_Ty> inline void push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value);
		inline void pop(Ty& target) noexcept;

	private:

		Ty* m_item_buffer_data_ptr = nullptr;
		Ty* m_item_buffer_end_ptr = nullptr;
		std::atomic<bool> m_good{ false };
		char* m_item_buffer_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<Ty*> m_last_item_aptr{ nullptr };
		const Ty* m_next_item_cached_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<Ty*> m_next_item_aptr{ nullptr };
		const Ty* m_last_item_cached_ptr = nullptr;
	};

	// queue_mpmc

	template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t> class alignas(_cache_line_size) queue_mpmc
	{

	private:

#if defined(UINT8_MAX) && defined(UINT16_MAX)
		static inline constexpr std::uint16_t _upcast(std::uint8_t) noexcept { return 0; }
#endif // defined(UINT8_MAX) && defined(UINT16_MAX)

#if defined(UINT16_MAX) && defined(UINT32_MAX)
		static inline constexpr std::uint32_t _upcast(std::uint16_t) noexcept { return 0; }
#endif // defined(UINT16_MAX) && defined(UINT32_MAX)

#if defined(UINT32_MAX) && defined(UINT64_MAX)
		static inline constexpr std::uint64_t _upcast(std::uint32_t) noexcept { return 0; }
#endif // defined(UINT32_MAX) && defined(UINT64_MAX)

	public:

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::queue_mpmc<value_type, cache_line_size, wait_type, uintX_type> requirement : cache_line_size must be a power of 2");

		static_assert(std::is_integral<_uintX_t>::value && std::is_unsigned<_uintX_t>::value, "cool::queue_mpmc<...> : uintX_type must be unsigned integral type");
#ifdef UINT64_MAX
		static_assert(!std::is_same<_uintX_t, std::uint64_t>::value, "cool::queue_mpmc<...> : uintX_type must be strictly smaller type than std::uint64_t");
#endif // UINT64_MAX

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t cache_line_size = alignof(cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>);

		using wait_type = _wait_Ty;

		using uintX_type = _uintX_t;
		using uint2X_type = decltype(_upcast(static_cast<_uintX_t>(0)));

		class item_type;

		queue_mpmc() = default;
		queue_mpmc(const cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>&) = delete;
		cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>& operator=(const cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>&) = delete;
		queue_mpmc(cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>&&) = delete;
		cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>& operator=(cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>&&) = delete;
		inline ~queue_mpmc();

		// WARNING : array at data_ptr must have space for new_item_buffer_size.value() + 1 elements
		inline cool::queue_init_result init_new_buffer(item_type* data_ptr, cool::item_buffer_size new_item_buffer_size);
		inline cool::queue_init_result init_new_buffer(cool::item_buffer_size new_item_buffer_size);
		inline bool good() const noexcept;
		inline std::size_t size() const noexcept;
		inline bool owns_buffer() const noexcept;
		inline void delete_buffer() noexcept;

		template <class ... arg_Ty> inline bool try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value);
		inline bool try_pop(Ty& target) noexcept;
		template <class ... arg_Ty> inline void push(arg_Ty&& ... args)  noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value);
		inline void pop(Ty& target) noexcept;

		class item_type {
		public:
			Ty value;
			std::atomic<uintX_type> round_number{ 0 };
		};

	private:

		class alignas(uint2X_type) item_info_type
		{

		public:

			item_info_type() = delete;
			inline item_info_type(uintX_type _item_number, uintX_type _round_number) noexcept;
			item_info_type(const typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type&) noexcept = default;
			typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type& operator=(const typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type&) noexcept = default;
			item_info_type(typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type&&) noexcept = default;
			typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type& operator=(typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type&&) noexcept = default;
			~item_info_type() = default;

			uintX_type item_number;
			uintX_type round_number;

			inline bool is_equal_to(const item_info_type& rhs) const noexcept;
		};

		inline item_info_type update_info(const item_info_type& info) const noexcept;

		item_type* m_item_buffer_data_ptr = nullptr;
		std::size_t m_item_buffer_size = 0;
		std::atomic<bool> m_good{ false };
		char* m_item_buffer_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<item_info_type> m_last_item_info{ item_info_type(0, 0) };

		alignas(_cache_line_size) std::atomic<item_info_type> m_next_item_info{ item_info_type(0, 1) };
	};
#endif // COOL_QUEUES_ATOMIC

#ifdef COOL_QUEUES_THREAD

	// wait_yield

	class wait_yield
	{
	public:
		using value_type = unsigned int;
		static inline void push_wait(value_type&) noexcept;
		static inline void pop_wait(value_type&) noexcept;
	};

	// queue_wlock

	template <class Ty, std::size_t _cache_line_size, class _wait_Ty> class alignas(_cache_line_size) queue_wlock
	{

	public:

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::queue_wlock<value_type, cache_line_size, wait_type> requirement : cache_line_size must be a power of 2");

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		static constexpr std::size_t cache_line_size = alignof(cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>);

		using wait_type = _wait_Ty;

		queue_wlock() = default;
		queue_wlock(const cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>&) = delete;
		cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>& operator=(const cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>&) = delete;
		queue_wlock(cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>&&) = delete;
		cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>& operator=(cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>&&) = delete;
		~queue_wlock();

		// WARNING : array at data_ptr must have space for new_item_buffer_size.value() + 1 elements
		inline cool::queue_init_result init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size);
		inline cool::queue_init_result init_new_buffer(cool::item_buffer_size new_item_buffer_size);
		inline bool good() const noexcept;
		inline bool owns_buffer() const noexcept;
		inline std::size_t size() const noexcept;
		inline void delete_buffer() noexcept;

		template <class ... arg_Ty> inline bool try_push(arg_Ty&& ... args);
		inline bool try_pop(Ty& target);
		template <class ... arg_Ty> inline void push(arg_Ty&& ... args);
		inline bool pop(Ty& target);

	private:

		Ty* m_item_buffer_data_ptr = nullptr;
		Ty* m_item_buffer_end_ptr = nullptr;
		Ty* m_last_item_ptr = nullptr;
		Ty* m_next_item_ptr = nullptr;

		bool m_stop_queue = true;
		std::atomic<bool> m_good{ false };
		char* m_item_buffer_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::condition_variable m_condition_var;
		std::mutex m_mutex;
	};
#endif // COOL_QUEUES_THREAD
}


// detail

inline constexpr cool::item_buffer_size::item_buffer_size(std::size_t new_item_buffer_size) noexcept : m_value(new_item_buffer_size) {}

inline constexpr std::size_t cool::item_buffer_size::value() const noexcept
{
	return m_value;
}

inline cool::queue_init_result::operator bool() const noexcept
{
	return m_result == queue_init_result::success;
}

inline bool cool::queue_init_result::good() const noexcept
{
	return m_result == queue_init_result::success;
}

inline int cool::queue_init_result::value() const noexcept
{
	return m_result;
}

inline const char* cool::queue_init_result::message() const noexcept
{
	switch (m_result)
	{
	case queue_init_result::success: return "cool queue init success"; break;
	case queue_init_result::bad_align: return "cool queue init failed : bad alignment of queue_spsc/queue_mpmc/queue_wlock object"; break;
	case queue_init_result::bad_parameters: return "cool queue init failed : bad parameters"; break;
	case queue_init_result::bad_alloc: return "cool queue init failed : bad allocation"; break;
	case queue_init_result::not_lockfree: return "cool queue init failed : atomic pointers/counters are not lockfree"; break;
	default: return "cool queue init result undefined"; break;
	}
}

inline cool::queue_init_result::queue_init_result(int result) noexcept : m_result(result) {}

template <class Ty>
cool::queue_nosync<Ty>::queue_nosync::~queue_nosync()
{
	delete_buffer();
}

template <class Ty>
inline cool::queue_init_result cool::queue_nosync<Ty>::init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size)
{
	assert(data_ptr != nullptr);

	delete_buffer();

	if ((data_ptr == nullptr) || (new_item_buffer_size.value() == 0))
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	m_item_buffer_data_ptr = data_ptr;
	m_item_buffer_end_ptr = data_ptr + new_item_buffer_size.value() + 1;
	m_last_item_ptr = m_item_buffer_data_ptr;
	m_next_item_ptr = m_item_buffer_data_ptr;

	m_good = true;
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty>
inline cool::queue_init_result cool::queue_nosync<Ty>::init_new_buffer(cool::item_buffer_size new_item_buffer_size)
{
	delete_buffer();

	if (new_item_buffer_size.value() == 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	std::size_t new_item_buffer_size_p1 = new_item_buffer_size.value() + 1;
	m_item_buffer_data_ptr = static_cast<Ty*>(::operator new(new_item_buffer_size_p1 * sizeof(Ty), std::nothrow));

	if (m_item_buffer_data_ptr == nullptr)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_alloc);
	}

	m_item_buffer_end_ptr = m_item_buffer_data_ptr + new_item_buffer_size_p1;
	m_last_item_ptr = m_item_buffer_data_ptr;
	m_next_item_ptr = m_item_buffer_data_ptr;

	for (std::size_t k = 0; k < new_item_buffer_size_p1; k++)
	{
		new (m_item_buffer_data_ptr + k) Ty();
	}

	m_good = true;
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty>
inline bool cool::queue_nosync<Ty>::good() const noexcept
{
	return m_good;
}

template <class Ty>
inline std::size_t cool::queue_nosync<Ty>::size() const noexcept
{
	if (m_item_buffer_data_ptr != nullptr)
	{
		return static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr) - 1;
	}
	else
	{
		return 0;
	}
}

template <class Ty>
inline bool cool::queue_nosync<Ty>::owns_buffer() const noexcept
{
	return m_owns_buffer;
}

template <class Ty>
inline void cool::queue_nosync<Ty>::delete_buffer() noexcept
{
	m_good = false;

	if (m_owns_buffer)
	{
		std::size_t item_buffer_size_p1 = static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr);
		for (std::size_t k = 0; k < item_buffer_size_p1; k++)
		{
			(m_item_buffer_data_ptr + k)->~Ty();
		}

		::operator delete(m_item_buffer_data_ptr);
	}

	m_item_buffer_data_ptr = nullptr;
	m_item_buffer_end_ptr = nullptr;
	m_last_item_ptr = nullptr;
	m_next_item_ptr = nullptr;

	m_owns_buffer = false;
}

template <class Ty> template <class ... arg_Ty>
inline bool cool::queue_nosync<Ty>::try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value)
{
	Ty* last_item_ptr_p1 = (m_last_item_ptr + 1 != m_item_buffer_end_ptr) ? m_last_item_ptr + 1 : m_item_buffer_data_ptr;

	if (last_item_ptr_p1 != m_next_item_ptr)
	{
		m_last_item_ptr->~Ty();
		new (m_last_item_ptr) Ty(std::forward<arg_Ty>(args)...);
		m_last_item_ptr = last_item_ptr_p1;

		return true;
	}
	else
	{
		return false;
	}
}

template <class Ty>
inline bool cool::queue_nosync<Ty>::try_pop(Ty& target) noexcept
{
	if (m_next_item_ptr != m_last_item_ptr)
	{
		target = std::move(*m_next_item_ptr);
		m_next_item_ptr = (m_next_item_ptr + 1 != m_item_buffer_end_ptr) ? m_next_item_ptr + 1 : m_item_buffer_data_ptr;

		return true;
	}
	else
	{
		return false;
	}
}

inline void cool::wait_noop::push_wait(value_type&) noexcept {}
inline void cool::wait_noop::pop_wait(value_type&) noexcept {}

#ifdef COOL_QUEUES_ATOMIC
template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::queue_spsc::~queue_spsc()
{
	delete_buffer();
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline cool::queue_init_result cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_spsc<...> : object location must be aligned in memory");
	assert(data_ptr != nullptr);

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
	}

	if (!(m_last_item_aptr.is_lock_free() && m_next_item_aptr.is_lock_free()))
	{
		return cool::queue_init_result(cool::queue_init_result::not_lockfree);
	}

	if ((data_ptr == nullptr) || (new_item_buffer_size.value() == 0))
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	m_item_buffer_data_ptr = data_ptr;
	m_item_buffer_end_ptr = data_ptr + new_item_buffer_size.value() + 1;
	m_item_buffer_unaligned_data_ptr = nullptr;

	m_last_item_aptr.store(m_item_buffer_data_ptr, std::memory_order_relaxed);
	m_last_item_cached_ptr = m_item_buffer_data_ptr;
	m_next_item_aptr.store(m_item_buffer_data_ptr, std::memory_order_relaxed);
	m_next_item_cached_ptr = m_item_buffer_data_ptr;

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline cool::queue_init_result cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::init_new_buffer(cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_spsc<...> : object location must be aligned in memory");

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
	}

	if (!(m_last_item_aptr.is_lock_free() && m_next_item_aptr.is_lock_free()))
	{
		return cool::queue_init_result(cool::queue_init_result::not_lockfree);
	}

	if (new_item_buffer_size.value() == 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	constexpr std::size_t item_buffer_padding = (cache_line_size > alignof(Ty)) ? cache_line_size : alignof(Ty);
	std::size_t new_item_buffer_size_p1 = new_item_buffer_size.value() + 1;
	m_item_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(new_item_buffer_size_p1 * sizeof(Ty) + item_buffer_padding + cache_line_size, std::nothrow));

	if (m_item_buffer_unaligned_data_ptr == nullptr)
	{
		std::atomic_signal_fence(std::memory_order_release);
		return cool::queue_init_result(cool::queue_init_result::bad_alloc);
	}
	else
	{
		std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_item_buffer_unaligned_data_ptr) % static_cast<std::uintptr_t>(item_buffer_padding);

		m_item_buffer_data_ptr = reinterpret_cast<Ty*>(m_item_buffer_unaligned_data_ptr
			+ static_cast<std::size_t>(ptr_remainder != 0) * (item_buffer_padding - static_cast<std::size_t>(ptr_remainder)));
	}

	m_item_buffer_end_ptr = m_item_buffer_data_ptr + new_item_buffer_size_p1;
	m_last_item_aptr.store(m_item_buffer_data_ptr, std::memory_order_relaxed);
	m_last_item_cached_ptr = m_item_buffer_data_ptr;
	m_next_item_aptr.store(m_item_buffer_data_ptr, std::memory_order_relaxed);
	m_next_item_cached_ptr = m_item_buffer_data_ptr;

	for (std::size_t k = 0; k < new_item_buffer_size_p1; k++)
	{
		new (m_item_buffer_data_ptr + k) Ty();
	}

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::good() const noexcept
{
	return m_good.load(std::memory_order_seq_cst);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline std::size_t cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::size() const noexcept
{
	if (m_item_buffer_data_ptr != nullptr)
	{
		return static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr) - 1;
	}
	else
	{
		return 0;
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::owns_buffer() const noexcept
{
	return m_item_buffer_unaligned_data_ptr != nullptr;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline void cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::delete_buffer() noexcept
{
	m_good.store(false, std::memory_order_seq_cst);

	if (m_item_buffer_unaligned_data_ptr != nullptr)
	{
		std::size_t item_buffer_size_p1 = static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr);
		for (std::size_t k = 0; k < item_buffer_size_p1; k++)
		{
			(m_item_buffer_data_ptr + k)->~Ty();
		}

		::operator delete(m_item_buffer_unaligned_data_ptr);
	}

	m_item_buffer_data_ptr = nullptr;
	m_item_buffer_end_ptr = nullptr;
	m_item_buffer_unaligned_data_ptr = nullptr;

	m_last_item_aptr.store(nullptr, std::memory_order_relaxed);
	m_last_item_cached_ptr = nullptr;

	m_next_item_aptr.store(nullptr, std::memory_order_relaxed);
	m_next_item_cached_ptr = nullptr;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty> template <class ... arg_Ty>
inline bool cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value)
{
	Ty* last_item_ptr = m_last_item_aptr.load(std::memory_order_relaxed);
	Ty* last_item_ptr_p1 = (last_item_ptr + 1 != m_item_buffer_end_ptr) ? last_item_ptr + 1 : m_item_buffer_data_ptr;

	if (last_item_ptr_p1 == m_next_item_cached_ptr)
	{
		m_next_item_cached_ptr = m_next_item_aptr.load(std::memory_order_acquire);
		if (last_item_ptr_p1 == m_next_item_cached_ptr)
		{
			return false;
		}
	}

	last_item_ptr->~Ty();
	new (last_item_ptr) Ty(std::forward<arg_Ty>(args)...);
	m_last_item_aptr.store(last_item_ptr_p1, std::memory_order_release);
	return true;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::try_pop(Ty& target) noexcept
{
	Ty* next_item_ptr = m_next_item_aptr.load(std::memory_order_relaxed);

	if (next_item_ptr == m_last_item_cached_ptr)
	{
		m_last_item_cached_ptr = m_last_item_aptr.load(std::memory_order_acquire);
		if (next_item_ptr == m_last_item_cached_ptr)
		{
			return false;
		}
	}

	target = std::move(*next_item_ptr);
	Ty* next_item_ptr_p1 = (next_item_ptr + 1 != m_item_buffer_end_ptr) ? next_item_ptr + 1 : m_item_buffer_data_ptr;
	m_next_item_aptr.store(next_item_ptr_p1, std::memory_order_release);
	return true;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty> template <class ... arg_Ty>
inline void cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value)
{
	Ty* last_item_ptr = m_last_item_aptr.load(std::memory_order_relaxed);
	Ty* last_item_ptr_p1 = (last_item_ptr + 1 != m_item_buffer_end_ptr) ? last_item_ptr + 1 : m_item_buffer_data_ptr;

	typename _wait_Ty::value_type try_count{};
	while (last_item_ptr_p1 == m_next_item_cached_ptr)
	{
		m_next_item_cached_ptr = m_next_item_aptr.load(std::memory_order_acquire);
		_wait_Ty::push_wait(try_count);
	}

	last_item_ptr->~Ty();
	new (last_item_ptr) Ty(std::forward<arg_Ty>(args)...);
	m_last_item_aptr.store(last_item_ptr_p1, std::memory_order_release);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline void cool::queue_spsc<Ty, _cache_line_size, _wait_Ty>::pop(Ty& target) noexcept
{
	Ty* next_item_ptr = m_next_item_aptr.load(std::memory_order_relaxed);

	typename _wait_Ty::value_type try_count{};
	while (next_item_ptr == m_last_item_cached_ptr)
	{
		m_last_item_cached_ptr = m_last_item_aptr.load(std::memory_order_acquire);
		_wait_Ty::pop_wait(try_count);
	}

	target = std::move(*next_item_ptr);
	Ty* next_item_ptr_p1 = (next_item_ptr + 1 != m_item_buffer_end_ptr) ? next_item_ptr + 1 : m_item_buffer_data_ptr;
	m_next_item_aptr.store(next_item_ptr_p1, std::memory_order_release);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::queue_mpmc::~queue_mpmc()
{
	delete_buffer();
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline cool::queue_init_result cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::init_new_buffer(item_type* data_ptr, cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_mpmc<...> : object location must be aligned in memory");
	assert(data_ptr != nullptr);

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
	}

	if (!(m_last_item_info.is_lock_free() && m_next_item_info.is_lock_free()))
	{
		return cool::queue_init_result(cool::queue_init_result::not_lockfree);
	}

	if ((data_ptr == nullptr) || (new_item_buffer_size.value() == 0)
		|| (static_cast<std::uintmax_t>(new_item_buffer_size.value()) > static_cast<std::uintmax_t>(std::numeric_limits<_uintX_t>::max() - 1)))
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	m_item_buffer_data_ptr = data_ptr;
	m_item_buffer_size = new_item_buffer_size.value() + 1;
	m_item_buffer_unaligned_data_ptr = nullptr;

	m_last_item_info.store(item_info_type(0, 0), std::memory_order_relaxed);
	m_next_item_info.store(item_info_type(0, 1), std::memory_order_relaxed);

	for (std::size_t k = 0; k < m_item_buffer_size; k++)
	{
		(m_item_buffer_data_ptr + k)->value = Ty();
		(m_item_buffer_data_ptr + k)->round_number.store(0, std::memory_order_relaxed);
	}

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline cool::queue_init_result cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::init_new_buffer(cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_mpmc<...> : object location must be aligned in memory");

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
	}

	if (!(m_last_item_info.is_lock_free() && m_next_item_info.is_lock_free()))
	{
		return cool::queue_init_result(cool::queue_init_result::not_lockfree);
	}

	if ((new_item_buffer_size.value() == 0)
		|| (static_cast<std::uintmax_t>(new_item_buffer_size.value()) > static_cast<std::uintmax_t>(std::numeric_limits<_uintX_t>::max() - 1)))
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	constexpr std::size_t item_buffer_padding = (cache_line_size > alignof(item_type)) ? cache_line_size : alignof(item_type);
	m_item_buffer_size = new_item_buffer_size.value() + 1;
	m_item_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(m_item_buffer_size * sizeof(item_type) + item_buffer_padding + cache_line_size, std::nothrow));

	if (m_item_buffer_unaligned_data_ptr == nullptr)
	{
		std::atomic_signal_fence(std::memory_order_release);
		return cool::queue_init_result(cool::queue_init_result::bad_alloc);
	}
	else
	{
		std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_item_buffer_unaligned_data_ptr) % static_cast<std::uintptr_t>(item_buffer_padding);

		m_item_buffer_data_ptr = reinterpret_cast<item_type*>(m_item_buffer_unaligned_data_ptr
			+ static_cast<std::size_t>(ptr_remainder != 0) * (item_buffer_padding - static_cast<std::size_t>(ptr_remainder)));
	}

	m_item_buffer_size = new_item_buffer_size.value() + 1;

	m_last_item_info.store(item_info_type(0, 0), std::memory_order_relaxed);
	m_next_item_info.store(item_info_type(0, 1), std::memory_order_relaxed);

	for (std::size_t k = 0; k < m_item_buffer_size; k++)
	{
		new (m_item_buffer_data_ptr + k) item_type();
	}

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline bool cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::good() const noexcept { return m_good.load(std::memory_order_seq_cst); }

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline std::size_t cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::size() const noexcept
{
	if (m_item_buffer_data_ptr != nullptr)
	{
		return m_item_buffer_size - 1;
	}
	else
	{
		return 0;
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline bool cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::owns_buffer() const noexcept { return m_item_buffer_unaligned_data_ptr != nullptr; }

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline void cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::delete_buffer() noexcept
{
	m_good.store(false, std::memory_order_seq_cst);

	if (m_item_buffer_unaligned_data_ptr != nullptr)
	{
		for (std::size_t k = 0; k < m_item_buffer_size; k++)
		{
			(m_item_buffer_data_ptr + k)->~item_type();
		}

		::operator delete(m_item_buffer_unaligned_data_ptr);
	}

	m_item_buffer_data_ptr = nullptr;
	m_item_buffer_size = 0;
	m_item_buffer_unaligned_data_ptr = nullptr;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t> template <class ... arg_Ty>
inline bool cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::try_push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value)
{
	item_info_type last_item_info = m_last_item_info.load(std::memory_order_acquire);

	while (true)
	{
		item_type& item_ref = *(m_item_buffer_data_ptr + static_cast<std::size_t>(last_item_info.item_number));

		if (last_item_info.round_number == item_ref.round_number.load(std::memory_order_acquire))
		{
			if (m_last_item_info.compare_exchange_strong(last_item_info, update_info(last_item_info)))
			{
				item_ref.value.~Ty();
				new (&item_ref.value) Ty(std::forward<arg_Ty>(args)...);
				item_ref.round_number.store(last_item_info.round_number + 1, std::memory_order_release);

				return true;
			}
		}
		else
		{
			item_info_type previous_last_item_info = last_item_info;
			last_item_info = m_last_item_info.load(std::memory_order_acquire);

			if (last_item_info.is_equal_to(previous_last_item_info))
			{
				return false;
			}
		}
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline bool cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::try_pop(Ty& target) noexcept
{
	item_info_type next_item_info = m_next_item_info.load(std::memory_order_acquire);

	while (true)
	{
		item_type& item_ref = *(m_item_buffer_data_ptr + static_cast<std::size_t>(next_item_info.item_number));

		if (next_item_info.round_number == item_ref.round_number.load(std::memory_order_acquire))
		{
			if (m_next_item_info.compare_exchange_strong(next_item_info, update_info(next_item_info)))
			{
				target = std::move(item_ref.value);
				item_ref.round_number.store(next_item_info.round_number + 1, std::memory_order_release);

				return true;
			}
		}
		else
		{
			item_info_type previous_next_item_info = next_item_info;
			next_item_info = m_next_item_info.load(std::memory_order_acquire);

			if (next_item_info.is_equal_to(previous_next_item_info))
			{
				return false;
			}
		}
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t> template <class ... arg_Ty>
inline void cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::push(arg_Ty&& ... args) noexcept(std::is_nothrow_constructible<Ty, arg_Ty ...>::value)
{
	item_info_type last_item_info = m_last_item_info.load(std::memory_order_acquire);
	while (!m_last_item_info.compare_exchange_weak(last_item_info, update_info(last_item_info))) {}

	item_type& item_ref = *(m_item_buffer_data_ptr + static_cast<std::size_t>(last_item_info.item_number));

	typename _wait_Ty::value_type try_count{};
	while (last_item_info.round_number != item_ref.round_number.load(std::memory_order_acquire))
	{
		_wait_Ty::push_wait(try_count);
	}

	item_ref.value.~Ty();
	new (&item_ref.value) Ty(std::forward<arg_Ty>(args)...);
	item_ref.round_number.store(last_item_info.round_number + 1, std::memory_order_release);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline void cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::pop(Ty& target) noexcept
{
	item_info_type next_item_info = m_next_item_info.load(std::memory_order_acquire);
	while (!m_next_item_info.compare_exchange_weak(next_item_info, update_info(next_item_info))) {}

	item_type& item_ref = *(m_item_buffer_data_ptr + static_cast<std::size_t>(next_item_info.item_number));

	typename _wait_Ty::value_type try_count{};
	while (next_item_info.round_number != item_ref.round_number.load(std::memory_order_acquire))
	{
		_wait_Ty::pop_wait(try_count);
	}

	target = std::move(item_ref.value);
	item_ref.round_number.store(next_item_info.round_number + 1, std::memory_order_release);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type::item_info_type(uintX_type _item_number, uintX_type _round_number) noexcept
	: item_number(_item_number), round_number(_round_number) {}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline bool cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type::is_equal_to(const item_info_type& rhs) const noexcept
{
	return std::memcmp(this, &rhs, sizeof(item_info_type)) == 0;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty, class _uintX_t>
inline typename cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::item_info_type cool::queue_mpmc<Ty, _cache_line_size, _wait_Ty, _uintX_t>::update_info(const item_info_type& info) const noexcept
{
	if (info.item_number + 1 != m_item_buffer_size)
	{
		return item_info_type(info.item_number + 1, info.round_number);
	}
	else
	{
		return item_info_type(0, info.round_number + 2);
	}
}
#endif // COOL_QUEUES_ATOMIC

#ifdef COOL_QUEUES_THREAD
inline void cool::wait_yield::push_wait(value_type&) noexcept
{
	std::this_thread::yield();
}

inline void cool::wait_yield::pop_wait(value_type&) noexcept
{
	std::this_thread::yield();
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::queue_wlock::~queue_wlock()
{
	delete_buffer();
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline cool::queue_init_result cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::init_new_buffer(Ty* data_ptr, cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_wlock<...> : object location must be aligned in memory");
	assert(data_ptr != nullptr);

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		std::atomic_signal_fence(std::memory_order_release);
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
	}

	if ((data_ptr == nullptr) || (new_item_buffer_size.value() == 0))
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	m_item_buffer_data_ptr = data_ptr;
	m_item_buffer_end_ptr = data_ptr + new_item_buffer_size.value() + 1;
	m_last_item_ptr = m_item_buffer_data_ptr;
	m_next_item_ptr = m_item_buffer_data_ptr;

	m_item_buffer_unaligned_data_ptr = nullptr;

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline cool::queue_init_result cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::init_new_buffer(cool::item_buffer_size new_item_buffer_size)
{
	assert((reinterpret_cast<std::uintptr_t>(this) % cache_line_size == 0) && "cool::queue_wlock<...> : object location must be aligned in memory");

	if (reinterpret_cast<std::uintptr_t>(this) % cache_line_size != 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_align);
	}
	else
	{
		delete_buffer();
		m_stop_queue = false;
	}

	if (new_item_buffer_size.value() == 0)
	{
		return cool::queue_init_result(cool::queue_init_result::bad_parameters);
	}

	constexpr std::size_t item_buffer_padding = (cache_line_size > alignof(Ty)) ? cache_line_size : alignof(Ty);
	std::size_t new_item_buffer_size_p1 = new_item_buffer_size.value() + 1;
	m_item_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(new_item_buffer_size_p1 * sizeof(Ty) + item_buffer_padding + cache_line_size, std::nothrow));

	if (m_item_buffer_unaligned_data_ptr == nullptr)
	{
		std::atomic_signal_fence(std::memory_order_release);
		return cool::queue_init_result(cool::queue_init_result::bad_alloc);
	}
	else
	{
		std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_item_buffer_unaligned_data_ptr) % static_cast<std::uintptr_t>(item_buffer_padding);

		m_item_buffer_data_ptr = reinterpret_cast<Ty*>(m_item_buffer_unaligned_data_ptr
			+ static_cast<std::size_t>(ptr_remainder != 0) * (item_buffer_padding - static_cast<std::size_t>(ptr_remainder)));
	}

	m_item_buffer_end_ptr = m_item_buffer_data_ptr + new_item_buffer_size_p1;
	m_last_item_ptr = m_item_buffer_data_ptr;
	m_next_item_ptr = m_item_buffer_data_ptr;

	for (std::size_t k = 0; k < new_item_buffer_size_p1; k++)
	{
		new (m_item_buffer_data_ptr + k) Ty();
	}

	m_good.store(true, std::memory_order_seq_cst);
	return cool::queue_init_result(cool::queue_init_result::success);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::good() const noexcept
{
	return m_good.load(std::memory_order_seq_cst);
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline std::size_t cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::size() const noexcept
{
	if (m_item_buffer_data_ptr != nullptr)
	{
		return static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr) - 1;
	}
	else
	{
		return 0;
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::owns_buffer() const noexcept
{
	return m_item_buffer_unaligned_data_ptr != nullptr;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline void cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::delete_buffer() noexcept
{
	m_good.store(false, std::memory_order_seq_cst);

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_last_item_ptr = nullptr;
		m_next_item_ptr = nullptr;
		m_stop_queue = true;
	}

	m_condition_var.notify_all();

	if (m_item_buffer_unaligned_data_ptr != nullptr)
	{
		std::size_t item_buffer_size_p1 = static_cast<std::size_t>(m_item_buffer_end_ptr - m_item_buffer_data_ptr);
		for (std::size_t k = 0; k < item_buffer_size_p1; k++)
		{
			(m_item_buffer_data_ptr + k)->~Ty();
		}

		::operator delete(m_item_buffer_unaligned_data_ptr);
	}

	m_item_buffer_data_ptr = nullptr;
	m_item_buffer_end_ptr = nullptr;

	m_item_buffer_unaligned_data_ptr = nullptr;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty> template <class ... arg_Ty>
inline bool cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::try_push(arg_Ty&& ... args)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		Ty* last_item_ptr_p1 = (m_last_item_ptr + 1 != m_item_buffer_end_ptr) ? m_last_item_ptr + 1 : m_item_buffer_data_ptr;

		if (last_item_ptr_p1 != m_next_item_ptr)
		{
			m_last_item_ptr->~Ty();
			new (m_last_item_ptr) Ty(std::forward<arg_Ty>(args)...);
			m_last_item_ptr = last_item_ptr_p1;
		}
		else
		{
			return false;
		}
	}

	m_condition_var.notify_one();

	return true;
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::try_pop(Ty& target)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_next_item_ptr != m_last_item_ptr)
	{
		target = std::move(*m_next_item_ptr);
		m_next_item_ptr = (m_next_item_ptr + 1 != m_item_buffer_end_ptr) ? m_next_item_ptr + 1 : m_item_buffer_data_ptr;

		return true;
	}
	else
	{
		return false;
	}
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty> template <class ... arg_Ty>
inline void cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::push(arg_Ty&& ... args)
{
	typename _wait_Ty::value_type try_count{};

	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			Ty* last_item_ptr_p1 = (m_last_item_ptr + 1 != m_item_buffer_end_ptr) ? m_last_item_ptr + 1 : m_item_buffer_data_ptr;

			if (last_item_ptr_p1 != m_next_item_ptr)
			{
				m_last_item_ptr->~Ty();
				new (m_last_item_ptr) Ty(std::forward<arg_Ty>(args)...);
				m_last_item_ptr = last_item_ptr_p1;

				break;
			}
		}

		_wait_Ty::push_wait(try_count);
	}

	m_condition_var.notify_one();
}

template <class Ty, std::size_t _cache_line_size, class _wait_Ty>
inline bool cool::queue_wlock<Ty, _cache_line_size, _wait_Ty>::pop(Ty& target)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	m_condition_var.wait(lock, [this]() -> bool { return (m_last_item_ptr != m_next_item_ptr) || m_stop_queue; });

	if (m_last_item_ptr != m_next_item_ptr)
	{
		target = std::move(*m_next_item_ptr);
		m_next_item_ptr = (m_next_item_ptr + 1 != m_item_buffer_end_ptr) ? m_next_item_ptr + 1 : m_item_buffer_data_ptr;

		return true;
	}
	else
	{
		return false;
	}
}
#endif // COOL_QUEUES_THREAD

#endif // xCOOL_QUEUES_HPP


// cool_queues.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.
