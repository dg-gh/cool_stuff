// cool_threads.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_THREADS_HPP
#define xCOOL_THREADS_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <new>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>


namespace cool
{
	// 64 (bytes) is the most usual value for _cache_line_size
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class threads_sq;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class threads_mq;

#ifndef xCOOL_NOT_TARGET_ENUM
#define xCOOL_NOT_TARGET_ENUM
	enum no_target_t { no_target };
#endif // xCOOL_NOT_TARGET_ENUM

#ifndef xCOOL_DYNAMIC_SIZE_CONSTANT
#define xCOOL_DYNAMIC_SIZE_CONSTANT
	static constexpr std::size_t dynamic_size = 0;
#endif // xCOOL_DYNAMIC_SIZE_CONSTANT

	class async_task_end;
	// _size == 0 or cool::dynamic_size means dynamically resizable, otherwise fixed size
	template <class Ty, std::size_t _size = cool::dynamic_size, class _allocator = std::allocator<Ty>> class async_task_result;

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_sq_data;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_mq_data;
	class _async_task_end_incr_proxy;
	template <class Ty, std::size_t _size, class _allocator> class _async_task_result_proxy;
	template <class Ty, std::size_t _size, class _allocator> class _async_task_result_incr_proxy;

	// threads_sq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = 0>
	class threads_sq : private cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = alignof(typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task);

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");

		threads_sq() = default;
		threads_sq(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		threads_sq(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		inline ~threads_sq() { delete_threads(); }

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must not throw exceptions
		// 'args_Ty ... args' must be movable or copyable without throwing exceptions

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		inline bool init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size);
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline void delete_threads() noexcept;
	};

	// threads_mq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = 0>
	class threads_mq : private cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

#ifdef UINT64_MAX
		using countdown_type = std::uint64_t;
#else // UINT64_MAX
		using countdown_type = std::uint32_t;
#endif // UINT64_MAX

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = alignof(typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task);

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");

		threads_mq() = default;
		threads_mq(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		threads_mq(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		inline ~threads_mq() { delete_threads(); }

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must not throw exceptions
		// 'args_Ty ... args' must be movable or copyable without throwing exceptions

		// WARNING: queuing tasks with 'try_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept;

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept;

		inline bool init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size,
			countdown_type push_rounds = static_cast<countdown_type>(-1), countdown_type pop_rounds = 1,
			std::uint16_t dispatch_interval = 1);
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline countdown_type push_rounds() const noexcept;
		inline countdown_type pop_rounds() const noexcept;
		inline std::uint16_t dispatch_interval() const noexcept;
		inline void delete_threads() noexcept;
	};

	// async_task_end

	class async_task_end
	{

	public:

		async_task_end() = default;
		async_task_end(const cool::async_task_end& rhs) = delete;
		cool::async_task_end& operator=(const cool::async_task_end& rhs) = delete;
		async_task_end(cool::async_task_end&& rhs) = delete;
		cool::async_task_end& operator=(cool::async_task_end&& rhs) = delete;
		inline ~async_task_end();

		inline cool::async_task_end& add_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_end& sub_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_end& set_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::async_task_end& finish() noexcept;
		inline bool finished() const noexcept;

		inline cool::_async_task_end_incr_proxy try_incr_awaited() noexcept;

	private:

		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
	};

	// async_task_result

	template <class return_Ty, std::size_t _size, class _allocator> class async_task_result
	{

	public:

		using value_type = return_Ty;
		using pointer = return_Ty*;
		using const_pointer = const return_Ty*;
		using reference = return_Ty&;
		using const_reference = const return_Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using allocator_type = _allocator;

		async_task_result() = default;
		async_task_result(const cool::async_task_result<return_Ty, _size, _allocator>& rhs) = delete;
		cool::async_task_result<return_Ty, _size, _allocator>& operator=(const cool::async_task_result<return_Ty, _size, _allocator>& rhs) = delete;
		async_task_result(cool::async_task_result<return_Ty, _size, _allocator>&& rhs) = delete;
		cool::async_task_result<return_Ty, _size, _allocator>& operator=(cool::async_task_result<return_Ty, _size, _allocator>&& rhs) = delete;
		inline ~async_task_result();

		explicit async_task_result(const return_Ty& rhs);

		inline cool::async_task_result<return_Ty, _size, _allocator>& add_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, _size, _allocator>& sub_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, _size, _allocator>& set_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::_async_task_result_proxy<return_Ty, _size, _allocator> at(std::size_t offset) noexcept;
		inline cool::async_task_result<return_Ty, _size, _allocator>& finish() noexcept;
		inline bool finished() const noexcept;
		inline return_Ty& get(std::size_t offset) noexcept;
		inline return_Ty& get_unchecked(std::size_t offset) noexcept;
		inline const return_Ty& get_unchecked(std::size_t offset) const noexcept;
		inline cool::async_task_result<return_Ty, _size, _allocator>& reset();
		inline cool::async_task_result<return_Ty, _size, _allocator>& reset(const return_Ty& rhs);
		inline cool::async_task_result<return_Ty, _size, _allocator>& reset_unchecked();
		inline cool::async_task_result<return_Ty, _size, _allocator>& reset_unchecked(const return_Ty& rhs);

		static constexpr inline std::size_t size() noexcept;

	private:

		return_Ty m_stored_values[_size] = {};
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
	};

	template <class return_Ty, class _allocator> class async_task_result<return_Ty, cool::dynamic_size, _allocator> : private _allocator
	{

	public:

		using value_type = return_Ty;
		using pointer = return_Ty*;
		using const_pointer = const return_Ty*;
		using reference = return_Ty&;
		using const_reference = const return_Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using allocator_type = _allocator;

		async_task_result() = default;
		async_task_result(const cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& rhs) = delete;
		cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& operator=(const cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& rhs) = delete;
		async_task_result(cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>&& rhs) = delete;
		cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& operator=(cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>&& rhs) = delete;
		inline ~async_task_result();

		explicit async_task_result(std::size_t new_size);
		explicit async_task_result(std::size_t new_size, const return_Ty& rhs);

		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& add_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& sub_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& set_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::_async_task_result_proxy<return_Ty, cool::dynamic_size, _allocator> at(std::size_t offset) noexcept;
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& finish() noexcept;
		inline bool finished() const noexcept;
		inline return_Ty& get(std::size_t offset) noexcept;
		inline return_Ty& get_unchecked(std::size_t offset) noexcept;
		inline const return_Ty& get_unchecked(std::size_t offset) const noexcept;
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& reset();
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& reset(const return_Ty& rhs);
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& reset_unchecked();
		inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& reset_unchecked(const return_Ty& rhs);

		inline void resize(std::size_t new_size);
		inline std::size_t size() const noexcept;
		inline void clear() noexcept;

	private:

		return_Ty* m_stored_values = nullptr;
		std::size_t m_size = 0;
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
	};

	// _async_task_result_proxy

	template <class return_Ty, std::size_t _size, class _allocator> class _async_task_result_proxy
	{

	public:

		_async_task_result_proxy() = delete;
		_async_task_result_proxy(const cool::_async_task_result_proxy<return_Ty, _size, _allocator>&) = delete;
		cool::_async_task_result_proxy<return_Ty, _size, _allocator>& operator=(const cool::_async_task_result_proxy<return_Ty, _size, _allocator>&) = delete;
		_async_task_result_proxy(cool::_async_task_result_proxy<return_Ty, _size, _allocator>&&) noexcept = default;
		cool::_async_task_result_proxy<return_Ty, _size, _allocator>& operator=(cool::_async_task_result_proxy<return_Ty, _size, _allocator>&&) noexcept = default;
		~_async_task_result_proxy() {}

		inline cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> try_incr_awaited() noexcept;

	private:

		_async_task_result_proxy(cool::async_task_result<return_Ty, _size, _allocator>* target_ptr, std::size_t offset) noexcept
			: m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty, _size, _allocator>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		template <class return_Ty2, std::size_t _size2, class _allocator2> friend class cool::async_task_result;
	};
}


namespace cool
{
	// _threads_sq_data detail

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class alignas(_cache_line_size) _threads_sq_data
	{

	private:

		friend class cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;

		_threads_sq_data() = default;
		_threads_sq_data(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_sq_data(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		virtual ~_threads_sq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed) noexcept;

		class _task;
		using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

		_cool_thsq_task* m_task_buffer_data_ptr = nullptr;
		_cool_thsq_task* m_task_buffer_end_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;

		_cool_thsq_task* m_last_task_ptr = nullptr;
		_cool_thsq_task* m_next_task_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;

		std::thread* m_threads_data_ptr = nullptr;
		std::size_t m_thread_count = 0;

		std::condition_variable m_condition_var;
		std::mutex m_mutex;

		bool m_stop_threads = true;

		char* m_task_buffer_unaligned_data_ptr = nullptr;

		class _task
		{

		public:

			static constexpr std::size_t _arg_buffer_size_padded = (_arg_buffer_size != 0) ? _arg_buffer_size : 1;

			alignas(_arg_buffer_align) unsigned char m_arg_buffer[_arg_buffer_size_padded];
			void(*m_callable)(_cool_thsq_task*, _cool_thsq_task*);
			void(*m_function_ptr)(void);
			void* m_target_ptr;
			std::size_t m_offset;
		};

		template <std::size_t ... _indices> struct indices {};
		template <std::size_t n, std::size_t ... _indices> struct build_indices : build_indices<n - 1, n - 1, _indices...> {};
		template <std::size_t ... _indices> struct build_indices<0, _indices...> : indices<_indices...> {};

		template <class function_Ty, class tuple_Ty, std::size_t ... _indices>
		static inline auto call(function_Ty task, tuple_Ty&& args, indices<_indices...>) -> decltype(task(std::get<_indices>(std::forward<tuple_Ty>(args))...)) {
			return std::move(task(std::get<_indices>(std::forward<tuple_Ty>(args))...));
		}

		template <class function_Ty, class tuple_Ty>
		static inline auto call(function_Ty task, tuple_Ty&& args) -> decltype(call(task, args, build_indices<std::tuple_size<tuple_Ty>::value>())) {
			return std::move(call(task, std::move(args), build_indices<std::tuple_size<tuple_Ty>::value>()));
		}

		template <class function_Ty, class tuple_Ty, std::size_t ... _indices>
		static inline void call_no_return(function_Ty task, tuple_Ty&& args, indices<_indices...>) {
			task(std::get<_indices>(std::forward<tuple_Ty>(args))...);
		}

		template <class function_Ty, class tuple_Ty>
		static inline void call_no_return(function_Ty task, tuple_Ty&& args) {
			call_no_return(task, std::move(args), build_indices<std::tuple_size<tuple_Ty>::value>());
		}
	};

	// _threads_mq_data detail

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class alignas(_cache_line_size) _threads_mq_data
	{

	private:

#ifdef UINT64_MAX
		using countdown_type = std::uint64_t;
		using _uintX = std::uint32_t;
		using _uint2X = std::uint64_t;
#else // UINT64_MAX
		using countdown_type = std::uint32_t;
		using _uintX = std::uint16_t;
		using _uint2X = std::uint32_t;
#endif // UINT64_MAX

		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;

		_threads_mq_data() = default;
		_threads_mq_data(const cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_mq_data(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		virtual ~_threads_mq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept;

		class _task;
		using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

		class _thread_block;
		using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

		_cool_thmq_tblk* m_thread_blocks_data_ptr = nullptr;
		std::size_t m_thread_count = 0;
		countdown_type m_push_rounds = 1;
		countdown_type m_pop_rounds = 1;
		
		_uint2X m_mod_D = 1;
		_uint2X m_mod_a = static_cast<_uint2X>(1) << (sizeof(_uintX) * CHAR_BIT);
		_uintX m_mod_k = 0;
		_uintX m_dispatch_interval = 1;
		char* m_thread_blocks_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<_uintX> m_thread_dispatch{ 0 };

		class _task
		{

		public:

			static constexpr std::size_t _arg_buffer_size_padded = (_arg_buffer_size != 0) ? _arg_buffer_size : 1;

			alignas(_arg_buffer_align) unsigned char m_arg_buffer[_arg_buffer_size_padded];
			void(*m_callable)(_cool_thmq_task*, _cool_thmq_task*);
			void(*m_function_ptr)(void);
			void* m_target_ptr;
			std::size_t m_offset;
		};

		class alignas(_cache_line_size) _thread_block
		{

		public:

			inline bool new_task_buffer(std::size_t new_task_buffer_size);
			inline void delete_task_buffer() noexcept;

			_cool_thmq_task* m_task_buffer_data_ptr = nullptr;
			_cool_thmq_task* m_task_buffer_end_ptr = static_cast<_cool_thmq_task*>(nullptr) + 1;

			_cool_thmq_task* m_last_task_ptr = nullptr;
			_cool_thmq_task* m_next_task_ptr = static_cast<_cool_thmq_task*>(nullptr) + 1;

			std::condition_variable m_condition_var;
			std::mutex m_mutex;
			std::thread m_thread;

			bool m_stop_threads = true;

			char* m_task_buffer_unaligned_data_ptr = nullptr;
		};

		template <std::size_t ... _indices> struct indices {};
		template <std::size_t n, std::size_t ... _indices> struct build_indices : build_indices<n - 1, n - 1, _indices...> {};
		template <std::size_t ... _indices> struct build_indices<0, _indices...> : indices<_indices...> {};

		template <class function_Ty, class tuple_Ty, std::size_t ... _indices>
		static inline auto call(function_Ty task, tuple_Ty&& args, indices<_indices...>) -> decltype(task(std::get<_indices>(std::forward<tuple_Ty>(args))...)) {
			return std::move(task(std::get<_indices>(std::forward<tuple_Ty>(args))...));
		}

		template <class function_Ty, class tuple_Ty>
		static inline auto call(function_Ty task, tuple_Ty&& args) -> decltype(call(task, args, build_indices<std::tuple_size<tuple_Ty>::value>())) {
			return std::move(call(task, std::move(args), build_indices<std::tuple_size<tuple_Ty>::value>()));
		}

		template <class function_Ty, class tuple_Ty, std::size_t... _indices>
		static inline void call_no_return(function_Ty task, tuple_Ty&& args, indices<_indices...>) {
			task(std::get<_indices>(std::forward<tuple_Ty>(args))...);
		}

		template <class function_Ty, class tuple_Ty>
		static inline void call_no_return(function_Ty task, tuple_Ty&& args) {
			call_no_return(task, std::move(args), build_indices<std::tuple_size<tuple_Ty>::value>());
		}
	};

	class _async_task_end_incr_proxy
	{

	public:

		_async_task_end_incr_proxy() = delete;
		_async_task_end_incr_proxy(const cool::_async_task_end_incr_proxy&) = delete;
		cool::_async_task_end_incr_proxy& operator=(const cool::_async_task_end_incr_proxy&) = delete;
		_async_task_end_incr_proxy(cool::_async_task_end_incr_proxy&&) noexcept = default;
		cool::_async_task_end_incr_proxy& operator=(cool::_async_task_end_incr_proxy&&) noexcept = default;
		~_async_task_end_incr_proxy() {}

	private:

		_async_task_end_incr_proxy(cool::async_task_end* target_ptr) noexcept
			: m_parent_ptr(target_ptr) {}

		cool::async_task_end* m_parent_ptr;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		friend class cool::async_task_end;
	};

	template <class return_Ty, std::size_t _size, class _allocator> class _async_task_result_incr_proxy
	{

	public:

		_async_task_result_incr_proxy() = delete;
		_async_task_result_incr_proxy(const cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>&) = delete;
		cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>& operator=(const cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>&) = delete;
		_async_task_result_incr_proxy(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>&&) noexcept = default;
		cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>& operator=(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>&&) noexcept = default;
		~_async_task_result_incr_proxy() {}

	private:

		_async_task_result_incr_proxy(cool::async_task_result<return_Ty, _size, _allocator>* target_ptr, std::size_t offset) noexcept
			: m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty, _size, _allocator>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		template <class return_Ty2, std::size_t _size2, class _allocator2> friend class cool::_async_task_result_proxy;
	};
}


// threads_sq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);
					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
				}
			};

			this->m_last_task_ptr = (last_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
				last_task_ptr_p1 : this->m_task_buffer_data_ptr;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();
	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
				this->m_next_task_ptr : this->m_task_buffer_end_ptr;
			(this->m_next_task_ptr)--;

			new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_next_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);
					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
				}
			};
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(&target);

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;
						cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
				}
			};

			this->m_last_task_ptr = (last_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
				last_task_ptr_p1 : this->m_task_buffer_data_ptr;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
				this->m_next_task_ptr : this->m_task_buffer_end_ptr;
			(this->m_next_task_ptr)--;

			new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_next_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_next_task_ptr->m_target_ptr = reinterpret_cast<void*>(&target);

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;
						cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
				}
			};
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;
						cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
				}
			};

			this->m_last_task_ptr = (last_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
				last_task_ptr_p1 : this->m_task_buffer_data_ptr;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

			this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
				this->m_next_task_ptr : this->m_task_buffer_end_ptr;
			(this->m_next_task_ptr)--;

			new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_next_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_next_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;
						cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
				}
			};
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

					*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
					_task_ptr->m_offset = _fetch_task_ptr->m_offset;
				}
			};

			this->m_last_task_ptr = (last_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
				last_task_ptr_p1 : this->m_task_buffer_data_ptr;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
				this->m_next_task_ptr : this->m_task_buffer_end_ptr;
			(this->m_next_task_ptr)--;

			new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_next_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_next_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
			this->m_next_task_ptr->m_offset = target.m_offset;

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

					*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
					_task_ptr->m_offset = _fetch_task_ptr->m_offset;
				}
			};
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : arguments alignment too large");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

					*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
					_task_ptr->m_offset = _fetch_task_ptr->m_offset;
				}
			};

			this->m_last_task_ptr = (last_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
				last_task_ptr_p1 : this->m_task_buffer_data_ptr;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "");

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

			this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
				this->m_next_task_ptr : this->m_task_buffer_end_ptr;
			(this->m_next_task_ptr)--;

			new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			this->m_next_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
			this->m_next_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
			this->m_next_task_ptr->m_offset = target.m_offset;

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

					*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
						reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
						std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
					);

					{
						bool notify_target;

						{
							std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
							notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
						}

						if (notify_target)
						{
							target_ref.m_finish_condition_var.notify_one();
						}
					}

					reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
				}
				else
				{
					new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
					reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

					_task_ptr->m_callable = _fetch_task_ptr->m_callable;
					_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
					_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
					_task_ptr->m_offset = _fetch_task_ptr->m_offset;
				}
			};
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}


template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

	delete_threads();

	this->m_stop_threads = false;

	if ((new_thread_count == 0) || (new_task_buffer_size == 0))
	{
		return false;
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count);

	new_task_buffer_size = (_new_thread_count > new_task_buffer_size) ? _new_thread_count : new_task_buffer_size;

	std::size_t threads_constructed = 0;

	try
	{
		this->m_threads_data_ptr = static_cast<std::thread*>(::operator new(
			_new_thread_count * sizeof(std::thread) + 2 * cache_line_size, std::nothrow));

		if (this->m_threads_data_ptr == nullptr)
		{
			this->delete_threads_detail(threads_constructed);
			return false;
		}
		else
		{
			this->m_threads_data_ptr = reinterpret_cast<std::thread*>(
				reinterpret_cast<char*>(this->m_threads_data_ptr) + cache_line_size);
		}

		this->m_thread_count = _new_thread_count;

		constexpr std::size_t task_buffer_padding = (cache_line_size > arg_buffer_align) ? cache_line_size : arg_buffer_align;
		std::size_t new_task_buffer_size_p1 = new_task_buffer_size + 1;

		this->m_task_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(new_task_buffer_size_p1 * sizeof(_cool_thsq_task) + task_buffer_padding + cache_line_size, std::nothrow));

		if (this->m_task_buffer_unaligned_data_ptr == nullptr)
		{
			this->delete_threads_detail(threads_constructed);
			return false;
		}
		else
		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_task_buffer_unaligned_data_ptr) % static_cast<std::uintptr_t>(task_buffer_padding);

			this->m_task_buffer_data_ptr = reinterpret_cast<_cool_thsq_task*>(this->m_task_buffer_unaligned_data_ptr
				+ static_cast<std::size_t>(ptr_remainder != 0) * (task_buffer_padding - static_cast<std::size_t>(ptr_remainder)));
		}

		this->m_task_buffer_end_ptr = this->m_task_buffer_data_ptr + new_task_buffer_size_p1;
		this->m_next_task_ptr = this->m_task_buffer_data_ptr;
		this->m_last_task_ptr = this->m_task_buffer_data_ptr;

		for (std::size_t k = 0; k < new_task_buffer_size_p1; k++)
		{
			new (this->m_task_buffer_data_ptr + k) _cool_thsq_task();
		}

		for (std::size_t k = 0; k < _new_thread_count; k++)
		{
			new (this->m_threads_data_ptr + k) std::thread([this]()
			{
				_cool_thsq_task current_task;

				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(this->m_mutex);

						this->m_condition_var.wait(lock, [=]() -> bool { return (this->m_last_task_ptr != this->m_next_task_ptr) || this->m_stop_threads; });

						if (this->m_last_task_ptr != this->m_next_task_ptr)
						{
							this->m_next_task_ptr->m_callable(&current_task, this->m_next_task_ptr);

							_cool_thsq_task* next_task_ptr_p1 = this->m_next_task_ptr + 1;

							this->m_next_task_ptr = (next_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
								next_task_ptr_p1 : this->m_task_buffer_data_ptr;
						}
						else
						{
							return;
						}
					}

					current_task.m_callable(&current_task, nullptr);
				}
			});

			threads_constructed++;
		}
	}
	catch (...)
	{
		this->delete_threads_detail(threads_constructed);
		return false;
	}

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::uint16_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::thread_count() const noexcept
{
	return static_cast<std::uint16_t>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::size_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::task_buffer_size() const noexcept
{
	if (this->m_task_buffer_data_ptr != nullptr)
	{
		return static_cast<std::size_t>(this->m_task_buffer_end_ptr - this->m_task_buffer_data_ptr) - 1;
	}
	else
	{
		return 0;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads() noexcept
{
	this->delete_threads_detail(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

	constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);

	{
		std::unique_lock<std::mutex> lock(this->m_mutex);
		this->m_stop_threads = true;
	}

	this->m_condition_var.notify_all();

	if (this->m_threads_data_ptr != nullptr)
	{
		for (size_t k = 0; k < threads_constructed; k++)
		{
			if ((this->m_threads_data_ptr + k)->joinable())
			{
				(this->m_threads_data_ptr + k)->join();
			}
			(this->m_threads_data_ptr + k)->~thread();
		}

		::operator delete(reinterpret_cast<char*>(this->m_threads_data_ptr) - cache_line_size);
	}

	if (this->m_task_buffer_data_ptr != nullptr)
	{
		std::size_t task_buffer_size = static_cast<std::size_t>(this->m_task_buffer_end_ptr - this->m_task_buffer_data_ptr);
		for (size_t k = 0; k < task_buffer_size; k++)
		{
			(this->m_task_buffer_data_ptr + k)->~_cool_thsq_task();
		}

		::operator delete(this->m_task_buffer_unaligned_data_ptr);
	}

	this->m_threads_data_ptr = nullptr;
	this->m_thread_count = 0;

	this->m_task_buffer_data_ptr = nullptr;
	this->m_task_buffer_end_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;
	this->m_next_task_ptr = nullptr;
	this->m_last_task_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;

	this->m_task_buffer_unaligned_data_ptr = nullptr;
}


// threads_mq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::no_target_t, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : arguments alignment too large");

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	bool success = false;

	for (countdown_type n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : arguments alignment too large");

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	bool success = false;

	for (countdown_type n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;
									cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;
									cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : arguments alignment too large");

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	bool success = false;

	for (countdown_type n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;
									cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;
									cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : arguments alignment too large");

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	bool success = false;

	for (countdown_type n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

								*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
								_task_ptr->m_offset = _fetch_task_ptr->m_offset;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

								*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
								_task_ptr->m_offset = _fetch_task_ptr->m_offset;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, std::size_t _size, class _allocator, class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> target, function_Ty task, args_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : arguments alignment too large");

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	bool success = false;

	for (countdown_type n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

								*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
								_task_ptr->m_offset = _fetch_task_ptr->m_offset;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_acquire);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<void(*)(void)>(task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = reinterpret_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								cool::async_task_result<return_Ty, _size, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _size, _allocator>*>(_task_ptr->m_target_ptr);

								*(static_cast<return_Ty*>(target_ref.m_stored_values) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
									reinterpret_cast<function_Ty>(_task_ptr->m_function_ptr),
									std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);

								{
									bool notify_target;

									{
										std::lock_guard<std::mutex> target_lock(target_ref.m_finish_mutex);
										notify_target = (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) <= 1);
									}

									if (notify_target)
									{
										target_ref.m_finish_condition_var.notify_one();
									}
								}

								reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
							}
							else
							{
								new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
								reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

								_task_ptr->m_callable = _fetch_task_ptr->m_callable;
								_task_ptr->m_function_ptr = _fetch_task_ptr->m_function_ptr;
								_task_ptr->m_target_ptr = _fetch_task_ptr->m_target_ptr;
								_task_ptr->m_offset = _fetch_task_ptr->m_offset;
							}
						};

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::init_new_threads(
	std::uint16_t new_thread_count,
	std::size_t new_task_buffer_size,
	typename cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::countdown_type push_rounds,
	typename cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::countdown_type pop_rounds,
	std::uint16_t dispatch_interval)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	delete_threads();

	if ((new_thread_count == 0) || (new_task_buffer_size == 0)
		|| (push_rounds == 0) || (pop_rounds == 0))
	{
		return false;
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count);
	std::size_t task_buffer_size_per_thread = new_task_buffer_size / _new_thread_count;
	if (_new_thread_count * task_buffer_size_per_thread != new_task_buffer_size)
	{
		task_buffer_size_per_thread++;
	}

	this->m_push_rounds = push_rounds;
	this->m_pop_rounds = pop_rounds;

	std::size_t threads_constructed = 0;
	std::size_t threads_launched = 0;

	try
	{
		this->m_thread_blocks_unaligned_data_ptr = static_cast<char*>(::operator new(
			_new_thread_count * sizeof(_cool_thmq_tblk) + 2 * cache_line_size, std::nothrow));

		if (this->m_thread_blocks_unaligned_data_ptr == nullptr)
		{
			this->delete_threads_detail(threads_constructed, threads_launched);
			return false;
		}
		else
		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_thread_blocks_unaligned_data_ptr) % static_cast<std::uintptr_t>(cache_line_size);

			this->m_thread_blocks_data_ptr = reinterpret_cast<_cool_thmq_tblk*>(this->m_thread_blocks_unaligned_data_ptr
				+ static_cast<std::size_t>(ptr_remainder != 0) * (cache_line_size - static_cast<std::size_t>(ptr_remainder)));
		}

		this->m_thread_count = _new_thread_count;

		for (std::size_t thread_num = 0; thread_num < _new_thread_count; thread_num++)
		{
			new (this->m_thread_blocks_data_ptr + thread_num) _cool_thmq_tblk();
			if (!((this->m_thread_blocks_data_ptr + thread_num)->new_task_buffer(task_buffer_size_per_thread)))
			{
				this->delete_threads_detail(threads_constructed, threads_launched);
				return false;
			}
			threads_constructed++;
		}

		if (new_thread_count != 1)
		{
			constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

			this->m_mod_D = static_cast<_cool_thmq_uint2X>(new_thread_count);

			_cool_thmq_uintX i = 0;
			while ((static_cast<_cool_thmq_uint2X>(1) << i) < static_cast<_cool_thmq_uint2X>(new_thread_count))
			{
				i++;
			}
			this->m_mod_k = uintX_bitcount + i;

			_cool_thmq_uint2X temp = (static_cast<_cool_thmq_uint2X>(1) << this->m_mod_k) / this->m_mod_D;
			if ((temp * this->m_mod_D) != (static_cast<_cool_thmq_uint2X>(1) << this->m_mod_k))
			{
				temp++;
			}

			this->m_mod_a = temp - (static_cast<_cool_thmq_uint2X>(1) << uintX_bitcount);
			this->m_mod_k -= (uintX_bitcount + 1);
		}
		else
		{
			constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

			this->m_mod_D = 1;
			this->m_mod_a = static_cast<_cool_thmq_uint2X>(1) << uintX_bitcount;
			this->m_mod_k = 0;
		}

		this->m_dispatch_interval = static_cast<_cool_thmq_uintX>(dispatch_interval);

		for (std::size_t thread_num = 0; thread_num < _new_thread_count; thread_num++)
		{
			(this->m_thread_blocks_data_ptr + thread_num)->m_stop_threads = false;

			(this->m_thread_blocks_data_ptr + thread_num)->m_thread = std::thread([this, thread_num, _new_thread_count, pop_rounds]()
			{
				_cool_thmq_task current_task;
				_cool_thmq_tblk* ptr;

				while (true)
				{
					bool ongoing = [&]() -> bool
					{
						for (countdown_type n = pop_rounds; n > 0; n--)
						{
							for (std::size_t k = thread_num; k < _new_thread_count; k++)
							{
								ptr = this->m_thread_blocks_data_ptr + k;

								std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

								if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
								{
									ptr->m_next_task_ptr->m_callable(&current_task, ptr->m_next_task_ptr);
									current_task = std::move(*(ptr->m_next_task_ptr));

									_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

									ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
										next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

									return true; // return from lambda, sets variable 'ongoing' as true
								}
							}

							for (std::size_t k = 0; k < thread_num; k++)
							{
								ptr = this->m_thread_blocks_data_ptr + k;

								std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

								if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
								{
									ptr->m_next_task_ptr->m_callable(&current_task, ptr->m_next_task_ptr);

									_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

									ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
										next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

									return true; // return from lambda, sets variable 'ongoing' as true
								}
							}
						}

						{
							ptr = this->m_thread_blocks_data_ptr + thread_num;

							std::unique_lock<std::mutex> lock(ptr->m_mutex);

							ptr->m_condition_var.wait(lock, [=]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

							if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
							{
								ptr->m_next_task_ptr->m_callable(&current_task, ptr->m_next_task_ptr);

								_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

								ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
									next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

								return true; // return from lambda, sets variable 'ongoing' as true
							}
							else
							{
								return false; // return from lambda, sets variable 'ongoing' as false
							}
						}
					}();

					if (ongoing)
					{
						current_task.m_callable(&current_task, nullptr);
					}
					else
					{
						return;
					}
				}
			});

			threads_launched++;
		}
	}
	catch (...)
	{
		this->delete_threads_detail(threads_constructed, threads_launched);
		return false;
	}

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::thread_count() const noexcept
{
	return static_cast<std::uint16_t>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::size_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::task_buffer_size() const noexcept
{
	if (this->m_thread_blocks_data_ptr != nullptr)
	{
		if (this->m_thread_blocks_data_ptr->m_task_buffer_data_ptr != nullptr)
		{
			return static_cast<std::size_t>((this->m_thread_blocks_data_ptr->m_task_buffer_end_ptr
				- this->m_thread_blocks_data_ptr->m_task_buffer_data_ptr) - 1) * this->m_thread_count;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline typename cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::countdown_type cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::push_rounds() const noexcept
{
	return this->m_push_rounds;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline typename cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::countdown_type cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::pop_rounds() const noexcept
{
	return this->m_pop_rounds;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::dispatch_interval() const noexcept
{
	return static_cast<std::uint16_t>(this->m_dispatch_interval);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads() noexcept
{
	this->delete_threads_detail(this->m_thread_count, this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept
{
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	if (this->m_thread_blocks_data_ptr != nullptr)
	{
		for (size_t k = 0; k < this->m_thread_count; k++)
		{
			_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + k;

			if (k < threads_launched)
			{
				{
					std::lock_guard<std::mutex> lock(ptr->m_mutex);
					ptr->m_stop_threads = true;
				}

				ptr->m_condition_var.notify_one();

				if (ptr->m_thread.joinable())
				{
					ptr->m_thread.join();
				}
			}

			if (k < threads_constructed)
			{
				ptr->~_cool_thmq_tblk();
			}

			ptr->delete_task_buffer();
		}

		::operator delete(this->m_thread_blocks_unaligned_data_ptr);
	}

	this->m_thread_blocks_data_ptr = nullptr;
	this->m_thread_count = 0;
	this->m_push_rounds = 1;
	this->m_pop_rounds = 1;

	constexpr std::size_t uintX_bitcount = sizeof(_uintX) * CHAR_BIT;

	this->m_mod_D = 1;
	this->m_mod_a = static_cast<_uint2X>(1) << uintX_bitcount;
	this->m_mod_k = 0;
	this->m_dispatch_interval = 1;
	this->m_thread_blocks_unaligned_data_ptr = nullptr;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline bool cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block::new_task_buffer(std::size_t new_task_buffer_size)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

	constexpr std::size_t cache_line_size = alignof(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
	constexpr std::size_t arg_buffer_align = alignof(_cool_thmq_task);

	constexpr std::size_t task_buffer_padding = (cache_line_size > arg_buffer_align) ? cache_line_size : arg_buffer_align;
	std::size_t new_task_buffer_size_p1 = new_task_buffer_size + 1;

	m_task_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(new_task_buffer_size_p1 * sizeof(_cool_thmq_task) + task_buffer_padding + cache_line_size, std::nothrow));

	if (m_task_buffer_unaligned_data_ptr == nullptr)
	{
		return false;
	}
	else
	{
		std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_task_buffer_unaligned_data_ptr) % static_cast<std::uintptr_t>(task_buffer_padding);

		m_task_buffer_data_ptr = reinterpret_cast<_cool_thmq_task*>(m_task_buffer_unaligned_data_ptr
			+ static_cast<std::size_t>(ptr_remainder != 0) * (task_buffer_padding - static_cast<std::size_t>(ptr_remainder)));
	}

	m_task_buffer_end_ptr = m_task_buffer_data_ptr + new_task_buffer_size_p1;
	m_next_task_ptr = m_task_buffer_data_ptr;
	m_last_task_ptr = m_task_buffer_data_ptr;

	for (std::size_t k = 0; k < new_task_buffer_size_p1; k++)
	{
		new (m_task_buffer_data_ptr + k) _cool_thmq_task();
	}

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block::delete_task_buffer() noexcept
{
	if (m_task_buffer_data_ptr != nullptr)
	{
		std::size_t task_buffer_size_p1 = static_cast<std::size_t>(m_task_buffer_end_ptr - m_task_buffer_data_ptr);

		for (std::size_t k = 0; k < task_buffer_size_p1; k++)
		{
			(m_task_buffer_data_ptr + k)->~_cool_thmq_task();
		}

		::operator delete(m_task_buffer_unaligned_data_ptr);
	}
}


// async_task_end detail

inline cool::async_task_end::~async_task_end()
{
	finish();
}

inline cool::async_task_end& cool::async_task_end::add_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

inline cool::async_task_end& cool::async_task_end::sub_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_sub(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

inline cool::async_task_end& cool::async_task_end::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.store(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

inline std::ptrdiff_t cool::async_task_end::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

inline cool::async_task_end& cool::async_task_end::finish() noexcept
{
	std::unique_lock<std::mutex> lock(m_finish_mutex);
	m_finish_condition_var.wait(lock, [&]() -> bool { return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0; });
	return *this;
}

inline bool cool::async_task_end::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

inline cool::_async_task_end_incr_proxy cool::async_task_end::try_incr_awaited() noexcept
{
	return cool::_async_task_end_incr_proxy(this);
}


// async_task_result detail

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>::~async_task_result()
{
	finish();
}

template <class return_Ty, std::size_t _size, class _allocator>
cool::async_task_result<return_Ty, _size, _allocator>::async_task_result(const return_Ty& rhs)
{
	for (std::size_t k = 0; k < _size; k++)
	{
		m_stored_values[k] = rhs;
	}
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::add_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::sub_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_sub(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline std::ptrdiff_t cool::async_task_result<return_Ty, _size, _allocator>::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::_async_task_result_proxy<return_Ty, _size, _allocator> cool::async_task_result<return_Ty, _size, _allocator>::at(std::size_t offset) noexcept
{
	return cool::_async_task_result_proxy<return_Ty, _size, _allocator>(this, offset);
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::finish() noexcept
{
	std::unique_lock<std::mutex> lock(m_finish_mutex);
	m_finish_condition_var.wait(lock, [&]() -> bool { return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0; });
	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline bool cool::async_task_result<return_Ty, _size, _allocator>::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, _size, _allocator>::get(std::size_t offset) noexcept
{
	finish();
	return m_stored_values[offset];
}

template <class return_Ty, std::size_t _size, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, _size, _allocator>::get_unchecked(std::size_t offset) noexcept
{
	return m_stored_values[offset];
}

template <class return_Ty, std::size_t _size, class _allocator>
inline const return_Ty& cool::async_task_result<return_Ty, _size, _allocator>::get_unchecked(std::size_t offset) const noexcept
{
	return m_stored_values[offset];
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::reset()
{
	finish();

	for (std::size_t k = 0; k < _size; k++)
	{
		m_stored_values[k] = return_Ty();
	}

	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::reset(const return_Ty& rhs)
{
	finish();

	for (std::size_t k = 0; k < _size; k++)
	{
		m_stored_values[k] = rhs;
	}

	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::reset_unchecked()
{
	for (std::size_t k = 0; k < _size; k++)
	{
		m_stored_values[k] = return_Ty();
	}

	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::async_task_result<return_Ty, _size, _allocator>& cool::async_task_result<return_Ty, _size, _allocator>::reset_unchecked(const return_Ty& rhs)
{
	for (std::size_t k = 0; k < _size; k++)
	{
		m_stored_values[k] = rhs;
	}

	return *this;
}

template <class return_Ty, std::size_t _size, class _allocator>
constexpr inline std::size_t cool::async_task_result<return_Ty, _size, _allocator>::size() noexcept
{
	return _size;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::~async_task_result()
{
	finish();

	if (m_stored_values != nullptr)
	{
		for (std::size_t k = m_size; k > 0; )
		{
			k--;
			(m_stored_values + k)->~return_Ty();
		}

		this->deallocate(m_stored_values, m_size);
	}
}

template <class return_Ty, class _allocator>
cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::async_task_result(std::size_t new_size)
{
	m_stored_values = this->allocate(new_size);
	for (std::size_t k = 0; k < new_size; k++)
	{
		new (m_stored_values + k) return_Ty();
	}
	m_size = new_size;
}

template <class return_Ty, class _allocator>
cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::async_task_result(std::size_t new_size, const return_Ty& rhs)
{
	m_stored_values = this->allocate(new_size);
	for (std::size_t k = 0; k < new_size; k++)
	{
		new (m_stored_values + k) return_Ty(rhs);
	}
	m_size = new_size;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, 0, _allocator>::add_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, 0, _allocator>::sub_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_sub(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, 0, _allocator>::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline std::ptrdiff_t cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

template <class return_Ty, class _allocator>
inline cool::_async_task_result_proxy<return_Ty, cool::dynamic_size, _allocator> cool::async_task_result<return_Ty, 0, _allocator>::at(std::size_t offset) noexcept
{
	return cool::_async_task_result_proxy<return_Ty, cool::dynamic_size, _allocator>(this, offset);
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::finish() noexcept
{
	std::unique_lock<std::mutex> lock(m_finish_mutex);
	m_finish_condition_var.wait(lock, [&]() -> bool { return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0; });
	return *this;
}

template <class return_Ty, class _allocator>
inline bool cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

template <class return_Ty, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::get(std::size_t offset) noexcept
{
	finish();
	return *(m_stored_values + offset);
}

template <class return_Ty, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::get_unchecked(std::size_t offset) noexcept
{
	return *(m_stored_values + offset);
}

template <class return_Ty, class _allocator>
inline const return_Ty& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::get_unchecked(std::size_t offset) const noexcept
{
	return *(m_stored_values + offset);
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::reset()
{
	finish();

	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values + k) = return_Ty();
	}

	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::reset(const return_Ty& rhs)
{
	finish();

	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values + k) = rhs;
	}

	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::reset_unchecked()
{
	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values + k) = return_Ty();
	}

	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>& cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::reset_unchecked(const return_Ty& rhs)
{
	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values + k) = rhs;
	}

	return *this;
}

template <class return_Ty, class _allocator>
inline std::size_t cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::size() const noexcept
{
	return m_size;
}

template <class return_Ty, class _allocator>
inline void cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::resize(std::size_t new_size)
{
	clear();

	m_stored_values = this->allocate(new_size);
	if (m_stored_values != nullptr)
	{
		for (std::size_t k = 0; k < new_size; k++)
		{
			new (m_stored_values + k) return_Ty();
		}
		m_size = new_size;
	}
}

template <class return_Ty, class _allocator>
inline void cool::async_task_result<return_Ty, cool::dynamic_size, _allocator>::clear() noexcept
{
	finish();

	if (m_stored_values != nullptr)
	{
		for (std::size_t k = m_size; k > 0; )
		{
			k--;
			(m_stored_values + k)->~return_Ty();
		}

		this->deallocate(m_stored_values, m_size);
	}

	m_stored_values = nullptr;
	m_size = 0;
	m_tasks_awaited.store(0, std::memory_order_relaxed);
}

template <class return_Ty, std::size_t _size, class _allocator>
inline cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator> cool::_async_task_result_proxy<return_Ty, _size, _allocator>::try_incr_awaited() noexcept
{
	return cool::_async_task_result_incr_proxy<return_Ty, _size, _allocator>(m_parent_ptr, m_offset);
}

#endif // xCOOL_THREADS_HPP


// cool_threads.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.