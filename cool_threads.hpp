// cool_threads.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_THREADS_HPP
#define xCOOL_THREADS_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <cstring>
#include <utility>
#include <new>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>
#include <exception>
#include <system_error>
#include <cassert>


// to allow the use of std::thread::native_handle : #define COOL_THREADS_NATIVE_HANDLE

#ifndef COOL_THREADS_NATIVE_HANDLE
#endif // COOL_THREADS_NATIVE_HANDLE

// to disable exceptions : #define COOL_THREADS_NO_EXCEPTIONS

#if !defined(xCOOL_THREADS_TRY) && !defined(xCOOL_THREADS_CATCH) && !defined(xCOOL_THREADS_EXCEPTION) && !defined(xCOOL_THREADS_SYSTEM_ERROR)
#ifndef COOL_THREADS_NO_EXCEPTIONS
#define xCOOL_THREADS_TRY try
#define xCOOL_THREADS_CATCH(expr) catch (expr)
#define xCOOL_THREADS_EXCEPTION excep
#define xCOOL_THREADS_SYSTEM_ERROR error
#else // COOL_THREADS_NO_EXCEPTIONS
#define xCOOL_THREADS_TRY
#define xCOOL_THREADS_CATCH(expr) if (false)
#define xCOOL_THREADS_EXCEPTION std::exception{}
#define xCOOL_THREADS_SYSTEM_ERROR std::system_error{ std::error_code{} }
#endif // COOL_THREADS_NO_EXCEPTIONS
#endif // !defined(xCOOL_THREADS_TRY) && !defined(xCOOL_THREADS_CATCH) && !defined(xCOOL_THREADS_EXCEPTION) && !defined(xCOOL_THREADS_SYSTEM_ERROR)


namespace cool
{
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> class threads_sq;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> class threads_mq;
	class threads_init_result;
	class threads_exception_handler;

#ifndef xCOOL_NOT_TARGET_ENUM
#define xCOOL_NOT_TARGET_ENUM
	enum no_target_t { no_target };
#endif // xCOOL_NOT_TARGET_ENUM

	class async_end;
	template <class return_Ty> class async_result;
	class safety_refresh_proxy;

	class _threads_base;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_sq_data;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_mq_data;
	class _async_end_incr_proxy;
	template <class Ty> class _async_result_to_proxy;
	template <class Ty> class _async_result_incr_proxy;

	template <class data_Ty> class _thread_iterator;
	template <class _thread_iterator_Ty> class _thread_iterator_proxy;
	template <class _thread_iterator_Ty> class _thread_const_iterator_proxy;
	class _thread_sq_id;
	class _thread_sq_native_handle;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _thread_mq_id;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _thread_mq_native_handle;


	// init types

	class thread_count {
	public:
		thread_count() = delete;
		explicit inline constexpr thread_count(std::uint16_t new_thread_count) noexcept;
		inline constexpr std::uint16_t value() const noexcept;
	private:
		std::uint16_t m_value;
	};

	class task_buffer_size {
	public:
		task_buffer_size() = delete;
		explicit inline constexpr task_buffer_size(std::size_t new_task_buffer_size) noexcept;
		inline constexpr std::size_t value() const noexcept;
	private:
		std::size_t m_value;
	};

	class try_pop_count {
	public:
		try_pop_count() = delete;
		explicit inline constexpr try_pop_count(unsigned int new_try_pop_count) noexcept;
		inline constexpr unsigned int value() const noexcept;
	private:
		unsigned int m_value;
	};

	class dispatch_interval {
	public:
		dispatch_interval() = delete;
		explicit inline constexpr dispatch_interval(std::uint16_t new_dispatch_interval) noexcept;
		inline constexpr std::uint16_t value() const noexcept;
	private:
		std::uint16_t m_value;
	};


	// threads_sq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t), bool _arg_type_static_check = true>
	class threads_sq : private cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;
		static constexpr bool arg_type_static_check = _arg_type_static_check;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_sq() = default;
		threads_sq(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& operator=(
			const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&) = delete;
		threads_sq(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& operator=(
			cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&&) = delete;
		inline ~threads_sq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'arg_Ty ... args' must be movable or copyable without throwing exceptions

		// sequentially consistent : task1 being submitted before task2 guarantees that task1 will start executing before task2

		// if 'function_Ty task' throws an std::exception, the thread will call the exception handler and jump to the next task
		// WARNING : other unrelated types of exceptions will not be caught

		// WARNING : queuing tasks with 'async' / 'priority_async' / 'try_async' / 'try_priority_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		// 'arg_type_is_valid<arg_Ty>' returns true if arg_Ty parameter pack type can be called as an argument for 'task' in 'async' / 'priority_async' / 'try_async' / 'try_priority_async'

		template <class ... arg_Ty>
		static inline constexpr bool arg_type_is_valid() noexcept;

		// WARNING : 'init_new_threads' and 'delete_threads' must not be called in concurrency with any other method
		// except 'safety_refresh' or the case of 'init_new_thread' with 'good'

		inline cool::threads_init_result init_new_threads(cool::thread_count new_thread_count, cool::task_buffer_size new_task_buffer_size) noexcept; // arguments must be > 0
		inline bool good() const noexcept; // true if 'init_new_threads' has finished successfully, must not be relied upon if a 'delete_threads' concurrent call is imminent
		inline std::size_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline void delete_threads() noexcept;

		// 'safety_refresh' produces a spurious wake up of all the potential waits on a condition and should only
		// be called for safety (possibly cyclically although not in a high frequency loop) if threads_sq could be suspected of deadlocks

		inline void safety_refresh() noexcept;

		// WARNING : 'thread_id' / 'thread_native_handle' does not check wether threads have been initialized beforehand

		inline std::thread::id thread_id(std::size_t thread_number) const noexcept;

		using thread_id_iterator = cool::_thread_iterator<cool::_thread_sq_id>;
		using thread_id_iterator_proxy = cool::_thread_const_iterator_proxy<thread_id_iterator>;

		inline thread_id_iterator_proxy thread_ids() const noexcept; // provides access to 'begin', 'end', 'cbegin', 'cend'

#ifdef COOL_THREADS_NATIVE_HANDLE
		inline std::thread::native_handle_type thread_native_handle(std::size_t thread_number);

		using thread_native_handle_iterator = cool::_thread_iterator<cool::_thread_sq_native_handle>;
		using thread_native_handle_iterator_proxy = cool::_thread_iterator_proxy<thread_native_handle_iterator>;

		inline thread_native_handle_iterator_proxy thread_native_handles() noexcept; // provides access to 'begin', 'end'
#endif // COOL_THREADS_NATIVE_HANDLE
	};


	// threads_mq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t), bool _arg_type_static_check = true>
	class threads_mq : private cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;
		static constexpr bool arg_type_static_check = _arg_type_static_check;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_mq() = default;
		threads_mq(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& operator=(
			const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&) = delete;
		threads_mq(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& operator=(
			cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>&&) = delete;
		inline ~threads_mq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'arg_Ty ... args' must be movable or copyable without throwing exceptions

		// not sequentially consistent : task1 being submitted before task2 does not guarantee that task1 will start executing before task2

		// if 'function_Ty task' throws an std::exception, the thread will call the exception handler and jump to the next task
		// WARNING : other unrelated types of exceptions will not be caught

		// WARNING : queuing tasks with 'async' / 'try_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept;

		// 'arg_type_is_valid<arg_Ty>' returns true if arg_Ty parameter pack type can be called as an argument for 'task' in 'async' / 'priority_async'

		template <class ... arg_Ty>
		static inline constexpr bool arg_type_is_valid() noexcept;

		// WARNING : 'init_new_threads' and 'delete_threads' must not be called in concurrency with any other method
		// except 'safety_refresh' or the case of 'init_new_thread' with 'good'
		
		inline cool::threads_init_result init_new_threads(
			cool::thread_count new_thread_count, // must be > 0
			cool::task_buffer_size new_task_buffer_size, // must be > 0
			cool::try_pop_count new_try_pop_count = cool::try_pop_count(1),
			cool::dispatch_interval = cool::dispatch_interval(0) // 0 sets value for 'new_dispatch_interval' to default_dispatch_interval('new_thread_count')
		) noexcept;
		inline bool good() const noexcept; // true if 'init_new_threads' has finished successfully, must not be relied upon if a 'delete_threads' concurrent call is imminent
		inline std::size_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline unsigned int try_pop_count() const noexcept;
		inline std::uint16_t dispatch_interval() const noexcept;
		inline void delete_threads() noexcept;

		// 'safety_refresh' produces a spurious wake up of all the potential waits on a condition and should only
		// be called for safety (possibly cyclically although not in a high frequency loop) if threads_mq could be suspected of deadlocks

		inline void safety_refresh() noexcept;

		// WARNING : 'thread_id' / 'thread_native_handle' does not check wether threads have been initialized beforehand

		inline std::thread::id thread_id(std::size_t thread_number) const noexcept;

		using thread_id_iterator = cool::_thread_iterator<cool::_thread_mq_id<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>;
		using thread_id_iterator_proxy = cool::_thread_const_iterator_proxy<thread_id_iterator>;

		inline thread_id_iterator_proxy thread_ids() const noexcept; // provides access to 'begin', 'end', 'cbegin', 'cend'

#ifdef COOL_THREADS_NATIVE_HANDLE
		inline std::thread::native_handle_type thread_native_handle(std::size_t thread_number);

		using thread_native_handle_iterator = cool::_thread_iterator<cool::_thread_mq_native_handle<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>;
		using thread_native_handle_iterator_proxy = cool::_thread_iterator_proxy<thread_native_handle_iterator>;

		inline thread_native_handle_iterator_proxy thread_native_handles() noexcept; // provides access to 'begin', 'end'
#endif // COOL_THREADS_NATIVE_HANDLE

		// 'default_dispatch_interval' returns a value 'new_dispatch_interval' close to sqrt('new_thread_count')
		// such that gcd('new_thread_count', 'new_dispatch_interval') is 1

		static inline std::uint16_t default_dispatch_interval(std::uint16_t new_thread_count) noexcept;
	};


	// threads_init_result

	class threads_init_result
	{

	public:

		static constexpr int success = 0;
		static constexpr int bad_align = 1;
		static constexpr int bad_parameters = 2;
		static constexpr int bad_alloc = 3;
		static constexpr int bad_thread_creation = 4;

		static constexpr int undefined = -1;

		threads_init_result() = default;

		inline operator bool() const noexcept; // returns true if threads init is successful
		inline bool good() const noexcept; // returns true if threads init is successful
		inline int value() const noexcept;
		inline const char* message() const noexcept;

	private:

		inline threads_init_result(int result) noexcept;

		int m_result = cool::threads_init_result::undefined;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
	};


	// threads_exception_handler

	// handles all exceptions from all instances of threads_sq and threads_mq

	class threads_exception_handler
	{

	public:

		// 'on_exception' arguments are :
		// > const std::exception& : exception thrown
		// > void(*)(void) : task function pointer that produced the exception
		// > std::thread::id : id of the thread that produced the exception
		// > void* : optional pointer to buffer 'exception_arg_ptr'

		// if 'on_exception' is a nullptr, then the exception handling will be the logical equivalent to a no-op

		// WARNING : 'on_exception' must not throw any exception or the program will terminate
		// WARNING : potential reads and writes to 'exception_arg_ptr' by 'on_exception' must be synchronized properly


		// 'on_delete_thread_exception' arguments are :
		// > const std::system_error& : exception thrown
		// > std::thread::id : id of the thread that produced the exception
		// > void* : optional pointer to buffer 'delete_thread_exception_arg_ptr'

		// if 'on_delete_thread_exception' is a nullptr, then the exception handling will be the logical equivalent to a no-op

		// WARNING : 'on_delete_thread_exception' must not throw any exception or the program will terminate
		// WARNING : potential reads and writes to 'delete_thread_exception_arg_ptr' by 'on_delete_thread_exception' must be synchronized properly
		// WARNING : 'on_delete_thread_exception' is called if a thread failed to join and the program should be considered to be unlikely to be left in a good state afterwards

		static inline void set_on_exception(void(*on_exception)(const std::exception&, void(*)(void), std::thread::id, void*), void* exception_arg_ptr = nullptr) noexcept;
		static inline void set_on_delete_thread_exception(void(*on_delete_thread_exception)(const std::system_error&, std::thread::id, void*), void* delete_thread_exception_arg_ptr = nullptr) noexcept;
		static inline void clear() noexcept;
	};


	// async_end

	// WARNNING : async_end object must outlive the completion of all function calls and notifications from
	// try_async or try_priority_async with it as a target argument

	class async_end
	{

	public:

		async_end() = default;
		async_end(const cool::async_end& rhs) = delete;
		cool::async_end& operator=(const cool::async_end& rhs) = delete;
		async_end(cool::async_end&& rhs) = delete;
		cool::async_end& operator=(cool::async_end&& rhs) = delete;
		~async_end() = default;

		inline void add_awaited(std::size_t number_of_tasks) noexcept;
		inline bool sub_awaited(std::size_t number_of_tasks) noexcept; // returns true iff notified
		inline bool set_awaited(std::ptrdiff_t number_of_tasks) noexcept; // returns true iff notified
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline bool decr_awaited() noexcept; // returns true iff notified
		inline void notify() noexcept;
		inline void finish() noexcept;
		inline bool finished() const noexcept;

		inline cool::_async_end_incr_proxy try_incr_awaited() noexcept;

		// 'safety_refresh' produces a spurious wake up of all the potential waits on a condition and should only
		// be called for safety (possibly cyclically although not in a high frequency loop) if async_end could be suspected of deadlocks

		inline void safety_refresh() noexcept;

	private:

		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
	};


	// async_result

	// WARNNING : async_result object must outlive the completion of all function calls and notifications from
	// try_async or try_priority_async with it as a target argument

	template <class return_Ty> class async_result
	{

	public:

		using value_type = return_Ty;
		using pointer = return_Ty*;
		using const_pointer = const return_Ty*;
		using reference = return_Ty&;
		using const_reference = const return_Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		async_result() = default;
		async_result(const cool::async_result<return_Ty>& rhs) = delete;
		cool::async_result<return_Ty>& operator=(const cool::async_result<return_Ty>& rhs) = delete;
		async_result(cool::async_result<return_Ty>&& rhs) = delete;
		cool::async_result<return_Ty>& operator=(cool::async_result<return_Ty>&& rhs) = delete;
		~async_result() = default;

		explicit inline async_result(return_Ty* storage_ptr) noexcept;

		inline void add_awaited(std::size_t number_of_tasks) noexcept;
		inline bool sub_awaited(std::size_t number_of_tasks) noexcept; // returns true iff notified
		inline bool set_awaited(std::ptrdiff_t number_of_tasks) noexcept; // returns true iff notified
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::_async_result_to_proxy<return_Ty> to(std::size_t offset) noexcept;
		inline bool decr_awaited() noexcept; // returns true iff notified
		inline void notify() noexcept;
		inline void finish() noexcept;
		inline bool finished() const noexcept;

		inline return_Ty& get(std::size_t offset) noexcept;
		inline return_Ty& get_unchecked(std::size_t offset) noexcept;
		inline const return_Ty& get_unchecked(std::size_t offset) const noexcept;
		inline void reset(std::size_t count);
		inline void reset(std::size_t count, const return_Ty& rhs);
		inline void reset_unchecked(std::size_t count);
		inline void reset_unchecked(std::size_t count, const return_Ty& rhs);

		inline return_Ty* data() noexcept;
		inline const return_Ty* data() const noexcept;

		inline void set_data(return_Ty* storage_ptr) noexcept;
		inline void set_data_unchecked(return_Ty* storage_ptr) noexcept;
		inline void clear() noexcept;
		inline void clear_unchecked() noexcept;

		// 'safety_refresh' produces a spurious wake up of all the potential waits on a condition and should only
		// be called for safety (possibly cyclically although not in a high frequency loop) if async_result could be suspected of deadlocks

		inline void safety_refresh() noexcept;

	private:

		return_Ty* m_stored_values_ptr = nullptr;
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::_async_result_to_proxy;
	};

	template <class return_Ty> class _async_result_to_proxy
	{

	public:

		_async_result_to_proxy() = delete;
		_async_result_to_proxy(const cool::_async_result_to_proxy<return_Ty>&) = delete;
		cool::_async_result_to_proxy<return_Ty>& operator=(const cool::_async_result_to_proxy<return_Ty>&) = delete;
		_async_result_to_proxy(cool::_async_result_to_proxy<return_Ty>&&) noexcept = default;
		cool::_async_result_to_proxy<return_Ty>& operator=(cool::_async_result_to_proxy<return_Ty>&&) noexcept = default;
		~_async_result_to_proxy() = default;

		inline cool::_async_result_incr_proxy<return_Ty> try_incr_awaited() noexcept;

	private:

		inline _async_result_to_proxy(cool::async_result<return_Ty>* target_ptr, std::size_t offset) noexcept : m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_result<return_Ty>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::async_result;
	};


	// safety_refresh_proxy

	class safety_refresh_proxy
	{

	public:

		safety_refresh_proxy() = delete;
		safety_refresh_proxy(const cool::safety_refresh_proxy&) noexcept = default;
		cool::safety_refresh_proxy& operator=(const cool::safety_refresh_proxy&) noexcept = default;
		safety_refresh_proxy(cool::safety_refresh_proxy&&) noexcept = default;
		cool::safety_refresh_proxy& operator=(cool::safety_refresh_proxy&&) noexcept = default;
		~safety_refresh_proxy() = default;

		inline void safety_refresh() const noexcept;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
		explicit inline safety_refresh_proxy(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& rhs) noexcept;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
		explicit inline safety_refresh_proxy(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& rhs) noexcept;
		explicit inline safety_refresh_proxy(cool::async_end& rhs) noexcept;
		template <class return_Ty> explicit inline safety_refresh_proxy(cool::async_result<return_Ty>& rhs) noexcept;

		inline bool operator==(const cool::safety_refresh_proxy& rhs) const noexcept;
		inline bool operator!=(const cool::safety_refresh_proxy& rhs) const noexcept;
		inline bool operator<=(const cool::safety_refresh_proxy& rhs) const noexcept;
		inline bool operator>=(const cool::safety_refresh_proxy& rhs) const noexcept;
		inline bool operator<(const cool::safety_refresh_proxy & rhs) const noexcept;
		inline bool operator>(const cool::safety_refresh_proxy& rhs) const noexcept;

		inline bool operator==(const void* rhs_ptr) const noexcept;
		inline bool operator!=(const void* rhs_ptr) const noexcept;
		inline bool operator<=(const void* rhs_ptr) const noexcept;
		inline bool operator>=(const void* rhs_ptr) const noexcept;
		inline bool operator<(const void* rhs_ptr) const noexcept;
		inline bool operator>(const void* rhs_ptr) const noexcept;

	private:

		void(*m_callable)(void*);
		void* m_object_ptr;
	};

	inline bool operator==(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
	inline bool operator!=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
	inline bool operator<=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
	inline bool operator>=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
	inline bool operator<(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
	inline bool operator>(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept;
}


namespace cool
{
	// _threads_base

	class _threads_base
	{

	private:

		friend class cool::threads_exception_handler;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::_threads_sq_data;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::_threads_mq_data;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;

		template <std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _base_task {
		private:
			static constexpr std::size_t _arg_buffer_size_padded = (_arg_buffer_size != 0) ? _arg_buffer_size : 1;
		public:
			class _address {
			public:
				void(*m_callable)(_base_task<_arg_buffer_size, _arg_buffer_align>*, _base_task<_arg_buffer_size, _arg_buffer_align>*);
				void(*m_function_ptr)(void);
			};
			class _address_with_target : public _address {
			public:
				void* m_target_ptr;
			};
			class _address_with_target_and_offset : public _address_with_target {
			public:
				std::size_t m_offset;
			};
			
			static constexpr std::size_t address_size = sizeof(_address);
			static constexpr std::size_t address_with_target_size = sizeof(_address_with_target);
			static constexpr std::size_t address_with_target_and_offset_size = sizeof(_address_with_target_and_offset);

			alignas(_arg_buffer_align) unsigned char m_arg_buffer[_arg_buffer_size_padded];
			_address_with_target_and_offset m_address_data;
		};

		template <std::size_t ... _indices> class indices {};
		template <std::size_t n, std::size_t ... _indices> class build_indices : public build_indices<n - 1, n - 1, _indices...> {};
		template <std::size_t ... _indices> class build_indices<0, _indices...> : public indices<_indices...> {};

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

		class exception_handler {
		public:
			inline exception_handler(void(*_function)(const std::exception&, void(*)(void), std::thread::id, void*), void* _arg_ptr) noexcept
				: m_function(_function), m_arg_ptr(_arg_ptr) {};
			void(*m_function)(const std::exception&, void(*)(void), std::thread::id, void*) = nullptr;
			void* m_arg_ptr = nullptr;
		};

		static inline std::atomic<exception_handler>& get_exception_handler() noexcept {
			static std::atomic<exception_handler> handler{ exception_handler{ nullptr, nullptr } };
			return handler;
		}
		static inline void catch_exception(const std::exception& excep, void(*task_function_ptr)(void)) noexcept {
			exception_handler handler = get_exception_handler().load(std::memory_order_seq_cst);
			if (handler.m_function != nullptr) {
				handler.m_function(excep, task_function_ptr, std::this_thread::get_id(), handler.m_arg_ptr);
			}
		}

		class delete_thread_exception_handler {
		public:
			inline delete_thread_exception_handler(void(*_function)(const std::system_error&, std::thread::id, void*), void* _arg_ptr) noexcept
				: m_function(_function), m_arg_ptr(_arg_ptr) {};
			void(*m_function)(const std::system_error&, std::thread::id, void*) = nullptr;
			void* m_arg_ptr = nullptr;
		};

		static inline std::atomic<delete_thread_exception_handler>& get_delete_thread_exception_handler() noexcept {
			static std::atomic<delete_thread_exception_handler> handler{ delete_thread_exception_handler{ nullptr, nullptr } };
			return handler;
		}
		static inline void catch_delete_thread_exception(const std::system_error& error, std::thread::id thread_id) noexcept {
			delete_thread_exception_handler handler = get_delete_thread_exception_handler().load(std::memory_order_seq_cst);
			if (handler.m_function != nullptr) {
				handler.m_function(error, thread_id, handler.m_arg_ptr);
			}
		}
	};

	// _threads_sq_data

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
	class alignas(_cache_line_size) _threads_sq_data : public cool::_threads_base
	{

	private:

		friend class cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, true>;
		friend class cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, false>;

		using _task = typename cool::_threads_base::_base_task<_arg_buffer_size, _arg_buffer_align>;

		_threads_sq_data() = default;
		_threads_sq_data(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(
			const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_sq_data(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(
			cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		~_threads_sq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed) noexcept;

		_task* m_task_buffer_data_ptr = nullptr;
		_task* m_task_buffer_end_ptr = nullptr;

		_task* m_last_task_ptr = nullptr;
		_task* m_next_task_ptr = nullptr;

		bool m_stop_threads = true;
		char* m_task_buffer_unaligned_data_ptr = nullptr;

		std::thread* m_threads_data_ptr = nullptr;
		std::size_t m_thread_count = 0;

		alignas(_cache_line_size) std::condition_variable m_condition_var;
		std::mutex m_mutex;

		std::atomic<bool> m_good{ false };
	};

	// _threads_mq_data

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
	class alignas(_cache_line_size) _threads_mq_data : public cool::_threads_base
	{

	private:

		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, true>;
		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, false>;
		friend class cool::_thread_mq_id<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;
		friend class cool::_thread_mq_native_handle<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;

		using _task = typename cool::_threads_base::_base_task<_arg_buffer_size, _arg_buffer_align>;

#ifdef UINT64_MAX
		using _uintX = std::uint32_t;
		using _uint2X = std::uint64_t;
#else // UINT64_MAX
		using _uintX = std::uint16_t;
		using _uint2X = std::uint32_t;
#endif // UINT64_MAX

		_threads_mq_data() = default;
		_threads_mq_data(const cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(
			const cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_mq_data(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(
			cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		~_threads_mq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept;

		class _thread_block;

		_uintX m_dispatch_interval = 1;
		_uintX m_mod_k = 0;
		_uint2X m_mod_a = static_cast<_uint2X>(1) << (sizeof(_uintX) * CHAR_BIT);
		_uint2X m_mod_D = 1;
		std::size_t m_thread_count = 0;
		_thread_block* m_thread_blocks_data_ptr = nullptr;

		std::atomic<bool> m_good{ false };
		unsigned int m_pop_rounds = 0;

		char* m_thread_blocks_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<_uintX> m_thread_dispatch{ 0 };

		alignas(_cache_line_size) std::atomic<bool> m_can_refresh{ false };

		class alignas(_cache_line_size) _thread_block
		{

		public:

			inline bool new_task_buffer(std::size_t new_task_buffer_size) noexcept;
			inline void delete_task_buffer() noexcept;

			_task* m_task_buffer_data_ptr = nullptr;
			_task* m_task_buffer_end_ptr = nullptr;

			_task* m_last_task_ptr = nullptr;
			_task* m_next_task_ptr = nullptr;

			bool m_stop_threads = true;

			std::thread m_thread;
			char* m_task_buffer_unaligned_data_ptr = nullptr;

			alignas(_cache_line_size) std::condition_variable m_condition_var;
			std::mutex m_mutex;
		};
	};

	// target incr proxy

	class _async_end_incr_proxy
	{

	public:

		_async_end_incr_proxy() = delete;
		_async_end_incr_proxy(const cool::_async_end_incr_proxy&) = delete;
		cool::_async_end_incr_proxy& operator=(const cool::_async_end_incr_proxy&) = delete;
		_async_end_incr_proxy(cool::_async_end_incr_proxy&&) noexcept = default;
		cool::_async_end_incr_proxy& operator=(cool::_async_end_incr_proxy&&) noexcept = default;
		~_async_end_incr_proxy() = default;

	private:

		_async_end_incr_proxy(cool::async_end* target_ptr) noexcept : m_parent_ptr(target_ptr) {}

		cool::async_end* m_parent_ptr;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
		friend class cool::async_end;
	};

	template <class return_Ty> class _async_result_incr_proxy
	{

	public:

		_async_result_incr_proxy() = delete;
		_async_result_incr_proxy(const cool::_async_result_incr_proxy<return_Ty>&) = delete;
		cool::_async_result_incr_proxy<return_Ty>& operator=(const cool::_async_result_incr_proxy<return_Ty>&) = delete;
		_async_result_incr_proxy(cool::_async_result_incr_proxy<return_Ty>&&) noexcept = default;
		cool::_async_result_incr_proxy<return_Ty>& operator=(cool::_async_result_incr_proxy<return_Ty>&&) noexcept = default;
		~_async_result_incr_proxy() = default;

	private:

		inline _async_result_incr_proxy(cool::async_result<return_Ty>* target_ptr, std::size_t offset) noexcept : m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_result<return_Ty>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::_async_result_to_proxy;
	};

	// iterators

	template <class data_Ty> class _thread_iterator : private data_Ty
	{

	public:

		using data_Ty::data_Ty;
		using value_type = typename data_Ty::value_type;
		using pointer_type = typename data_Ty::pointer_type;

		inline cool::_thread_iterator<data_Ty>& operator--() noexcept { this->m_ptr--; return *this; }
		inline cool::_thread_iterator<data_Ty>& operator++() noexcept { this->m_ptr++; return *this; }
		inline cool::_thread_iterator<data_Ty> operator--(int) noexcept { cool::_thread_iterator<data_Ty> ret = *this; this->m_ptr--; return ret; }
		inline cool::_thread_iterator<data_Ty> operator++(int) noexcept { cool::_thread_iterator<data_Ty> ret = *this; this->m_ptr++; return ret; }
		inline cool::_thread_iterator<data_Ty>& operator-=(std::ptrdiff_t offset) noexcept { this->m_ptr -= offset; return *this; }
		inline cool::_thread_iterator<data_Ty>& operator+=(std::ptrdiff_t offset) noexcept { this->m_ptr += offset; return *this; }
		inline cool::_thread_iterator<data_Ty> operator-(std::ptrdiff_t offset) const noexcept { return cool::_thread_iterator<data_Ty>(this->m_ptr - offset); }
		inline cool::_thread_iterator<data_Ty> operator+(std::ptrdiff_t offset) const noexcept { return cool::_thread_iterator<data_Ty>(this->m_ptr + offset); }
		inline std::ptrdiff_t operator-(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr - rhs.m_ptr; }

		inline bool operator==(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr == rhs.m_ptr; }
		inline bool operator!=(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr != rhs.m_ptr; }
		inline bool operator<=(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr <= rhs.m_ptr; }
		inline bool operator>=(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr >= rhs.m_ptr; }
		inline bool operator<(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr < rhs.m_ptr; }
		inline bool operator>(const cool::_thread_iterator<data_Ty>& rhs) const noexcept { return this->m_ptr > rhs.m_ptr; }

		value_type operator*() const noexcept(data_Ty::indirection_is_noexcept) { return this->indirection_impl(); }
		value_type operator[](std::ptrdiff_t offset) const noexcept(data_Ty::indirection_is_noexcept) { return this->indirection_impl(offset); }
	};

	template <class _thread_iterator_Ty> class _thread_iterator_proxy
	{

	public:

		using pointer_type = typename _thread_iterator_Ty::pointer_type;

		inline _thread_iterator_proxy(pointer_type begin_ptr, pointer_type end_ptr) noexcept : m_begin(begin_ptr), m_end(end_ptr) {}
		inline _thread_iterator_Ty begin() const noexcept { return m_begin; }
		inline _thread_iterator_Ty end() const noexcept { return m_end; }

	private:

		_thread_iterator_Ty m_begin;
		_thread_iterator_Ty m_end;
	};

	template <class _thread_iterator_Ty> class _thread_const_iterator_proxy
	{

	public:

		using pointer_type = typename _thread_iterator_Ty::pointer_type;

		inline _thread_const_iterator_proxy(pointer_type begin_ptr, pointer_type end_ptr) noexcept : m_begin(begin_ptr), m_end(end_ptr) {}
		inline _thread_iterator_Ty begin() const noexcept { return m_begin; }
		inline _thread_iterator_Ty end() const noexcept { return m_end; }
		inline _thread_iterator_Ty cbegin() const noexcept { return m_begin; }
		inline _thread_iterator_Ty cend() const noexcept { return m_end; }

	private:

		_thread_iterator_Ty m_begin;
		_thread_iterator_Ty m_end;
	};

	class _thread_sq_id {
	public:
		using value_type = std::thread::id;
		using pointer_type = const std::thread*;
		static constexpr bool indirection_is_noexcept = true;
		inline _thread_sq_id(pointer_type ptr) noexcept : m_ptr(ptr) {}
	protected:
		inline value_type indirection_impl() const noexcept { return m_ptr->get_id(); }
		inline value_type indirection_impl(std::ptrdiff_t offset) const noexcept { return (m_ptr + offset)->get_id(); }
		pointer_type m_ptr;
	};
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _thread_mq_id {
	public:
		using value_type = std::thread::id;
		using pointer_type = const typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block*;
		static constexpr bool indirection_is_noexcept = true;
		inline _thread_mq_id(pointer_type ptr) noexcept : m_ptr(ptr) {}
	protected:
		inline value_type indirection_impl() const noexcept { return m_ptr->m_thread.get_id(); }
		inline value_type indirection_impl(std::ptrdiff_t offset) const noexcept { return (m_ptr + offset)->m_thread.get_id(); }
		pointer_type m_ptr;
	};
#ifdef COOL_THREADS_NATIVE_HANDLE
	class _thread_sq_native_handle {
	public:
		using value_type = std::thread::native_handle_type;
		using pointer_type = std::thread*;
		static constexpr bool indirection_is_noexcept = false;
		inline _thread_sq_native_handle(pointer_type ptr) noexcept : m_ptr(ptr) {}
	protected:
		inline value_type indirection_impl() const { return m_ptr->native_handle(); }
		inline value_type indirection_impl(std::ptrdiff_t offset) const { return (m_ptr + offset)->native_handle(); }
		pointer_type m_ptr;
	};
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _thread_mq_native_handle {
	public:
		using value_type = std::thread::native_handle_type;
		using pointer_type = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block*;
		static constexpr bool indirection_is_noexcept = false;
		inline _thread_mq_native_handle(pointer_type ptr) noexcept : m_ptr(ptr) {}
	protected:
		inline value_type indirection_impl() const { return m_ptr->m_thread.native_handle(); }
		inline value_type indirection_impl(std::ptrdiff_t offset) const { return (m_ptr + offset)->m_thread.native_handle(); }
		pointer_type m_ptr;
	};
#endif // COOL_THREADS_NATIVE_HANDLE
}


// init types detail

inline constexpr cool::thread_count::thread_count(std::uint16_t new_thread_count) noexcept : m_value(new_thread_count) {}
inline constexpr std::uint16_t cool::thread_count::value() const noexcept { return m_value; }

inline constexpr cool::task_buffer_size::task_buffer_size(std::size_t new_task_buffer_size) noexcept : m_value(new_task_buffer_size) {}
inline constexpr std::size_t cool::task_buffer_size::value() const noexcept { return m_value; }

inline constexpr cool::try_pop_count::try_pop_count(unsigned int new_try_pop_count) noexcept : m_value(new_try_pop_count) {}
inline constexpr unsigned int cool::try_pop_count::value() const noexcept { return m_value; }

inline constexpr cool::dispatch_interval::dispatch_interval(std::uint16_t new_dispatch_interval) noexcept : m_value(new_dispatch_interval) {}
inline constexpr std::uint16_t cool::dispatch_interval::value() const noexcept { return m_value; }


// threads_sq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::~threads_sq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::priority_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_size);
					}
				};

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::priority_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		std::this_thread::yield();
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_priority_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_size);
					}
				};

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_priority_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_size);
					}
				};

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* last_task_ptr_p1 = (this->m_last_task_ptr + 1 != this->m_task_buffer_end_ptr) ? this->m_last_task_ptr + 1 : this->m_task_buffer_data_ptr;

			if (last_task_ptr_p1 != this->m_next_task_ptr)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_last_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				this->m_last_task_ptr = last_task_ptr_p1;

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_arg_type_static_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(this->m_mutex);

			_cool_thsq_task* next_task_ptr_m1 = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ? this->m_next_task_ptr - 1 : this->m_task_buffer_end_ptr - 1;

			if (this->m_last_task_ptr != next_task_ptr_m1)
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				this->m_next_task_ptr = next_task_ptr_m1;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::forward<arg_Ty>(args)...);
				std::memcpy(&(this->m_next_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_address_data.m_offset = target.m_offset;

				this->m_next_task_ptr->m_address_data.m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception & xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
						}

						target_ref.decr_awaited();

						reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thsq_pack();
					}
					else
					{
						new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::move(*reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)));
						reinterpret_cast<_cool_thsq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thsq_pack();

						std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thsq_task::address_with_target_and_offset_size);
					}
				};

				break;
			}
			else
			{
				return false;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class ... arg_Ty>
inline constexpr bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::arg_type_is_valid() noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	constexpr bool ret = (sizeof(_cool_thsq_pack) <= _arg_buffer_size) && (alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task));
	return ret;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::threads_init_result cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::init_new_threads(cool::thread_count new_thread_count, cool::task_buffer_size new_task_buffer_size) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

	assert((reinterpret_cast<std::uintptr_t>(this) % _cache_line_size == 0) && "cool::threads_sq<...> : object location must be aligned in memory");

	if (reinterpret_cast<std::uintptr_t>(this) % _cache_line_size != 0)
	{
		return cool::threads_init_result(cool::threads_init_result::bad_align);
	}
	else
	{
		delete_threads();
		this->m_stop_threads = false;
	}

	if ((new_thread_count.value() == 0) || (new_task_buffer_size.value() == 0))
	{
		return cool::threads_init_result(cool::threads_init_result::bad_parameters);
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count.value());
	this->m_thread_count = _new_thread_count;

	std::size_t threads_constructed = 0;

	this->m_threads_data_ptr = static_cast<std::thread*>(::operator new(
		_new_thread_count * sizeof(std::thread) + 2 * cache_line_size, std::nothrow));

	if (this->m_threads_data_ptr == nullptr)
	{
		this->delete_threads_detail(threads_constructed);
		std::atomic_signal_fence(std::memory_order_release);
		return cool::threads_init_result(cool::threads_init_result::bad_alloc);
	}
	else
	{
		this->m_threads_data_ptr = reinterpret_cast<std::thread*>(
			reinterpret_cast<char*>(this->m_threads_data_ptr) + cache_line_size);
	}

	constexpr std::size_t task_buffer_padding = (cache_line_size > arg_buffer_align) ? cache_line_size : arg_buffer_align;
	std::size_t new_task_buffer_size_p1 = new_task_buffer_size.value() + 1;

	this->m_task_buffer_unaligned_data_ptr = static_cast<char*>(::operator new(new_task_buffer_size_p1 * sizeof(_cool_thsq_task) + task_buffer_padding + cache_line_size, std::nothrow));

	if (this->m_task_buffer_unaligned_data_ptr == nullptr)
	{
		this->delete_threads_detail(threads_constructed);
		std::atomic_signal_fence(std::memory_order_release);
		return cool::threads_init_result(cool::threads_init_result::bad_alloc);
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
		xCOOL_THREADS_TRY
		{
			new (this->m_threads_data_ptr + k) std::thread([this]()
			{
				while (true)
				{
					xCOOL_THREADS_TRY
					{
						_cool_thsq_task current_task;

						{
							std::unique_lock<std::mutex> lock(this->m_mutex);

							this->m_condition_var.wait(lock, [this]() -> bool { return (this->m_last_task_ptr != this->m_next_task_ptr) || this->m_stop_threads; });

							if (this->m_last_task_ptr != this->m_next_task_ptr)
							{
								this->m_next_task_ptr->m_address_data.m_callable(&current_task, this->m_next_task_ptr);

								_cool_thsq_task* next_task_ptr_p1 = this->m_next_task_ptr + 1;

								this->m_next_task_ptr = (next_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
									next_task_ptr_p1 : this->m_task_buffer_data_ptr;
							}
							else
							{
								return;
							}
						}

						current_task.m_address_data.m_callable(&current_task, nullptr);
					}
					xCOOL_THREADS_CATCH(...) {}
				}
			});
		}
		xCOOL_THREADS_CATCH(...)
		{
			this->delete_threads_detail(threads_constructed);
			std::atomic_signal_fence(std::memory_order_release);
			return cool::threads_init_result(cool::threads_init_result::bad_thread_creation);
		}

		threads_constructed++;
	}

	std::atomic_signal_fence(std::memory_order_release);
	this->m_good.store(true, std::memory_order_seq_cst);
	return cool::threads_init_result(cool::threads_init_result::success);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::good() const noexcept
{
	return this->m_good.load(std::memory_order_seq_cst);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::size_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_count() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return this->m_thread_count;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::size_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::task_buffer_size() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);

	if (this->m_task_buffer_data_ptr != nullptr)
	{
		return static_cast<std::size_t>(this->m_task_buffer_end_ptr - this->m_task_buffer_data_ptr) - 1;
	}
	else
	{
		return 0;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::delete_threads() noexcept
{
	this->m_good.store(false, std::memory_order_seq_cst);
	this->delete_threads_detail(this->m_thread_count);
	std::atomic_signal_fence(std::memory_order_release);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::safety_refresh() noexcept
{
	this->m_condition_var.notify_all();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::thread::id cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_id(std::size_t thread_number) const noexcept
{
	return (this->m_threads_data_ptr + thread_number)->get_id();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::_thread_const_iterator_proxy<cool::_thread_iterator<cool::_thread_sq_id>> cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_ids() const noexcept
{
	return cool::_thread_const_iterator_proxy<cool::_thread_iterator<cool::_thread_sq_id>>(this->m_threads_data_ptr, this->m_threads_data_ptr + this->m_thread_count);
}

#ifdef COOL_THREADS_NATIVE_HANDLE
template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::thread::native_handle_type cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_native_handle(std::size_t thread_number)
{
	return (this->m_threads_data_ptr + thread_number)->native_handle();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::_thread_iterator_proxy<cool::_thread_iterator<cool::_thread_sq_native_handle>> cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_native_handles() noexcept
{
	return cool::_thread_iterator_proxy<cool::_thread_iterator<cool::_thread_sq_native_handle>>(this->m_threads_data_ptr, this->m_threads_data_ptr + this->m_thread_count);
}
#endif // COOL_THREADS_NATIVE_HANDLE

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed) noexcept
{
	constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);

	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(this->m_mutex);
			this->m_stop_threads = true;
			break;
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_all();

	if (this->m_threads_data_ptr != nullptr)
	{
		for (std::size_t k = 0; k < threads_constructed; k++)
		{
			if ((this->m_threads_data_ptr + k)->joinable())
			{
				std::thread::id thread_id = (this->m_threads_data_ptr + k)->get_id();

				xCOOL_THREADS_TRY
				{
					(this->m_threads_data_ptr + k)->join();
				}
				xCOOL_THREADS_CATCH(const std::system_error& xCOOL_THREADS_SYSTEM_ERROR)
				{
					cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_delete_thread_exception(xCOOL_THREADS_SYSTEM_ERROR, thread_id);
				}
			}
			(this->m_threads_data_ptr + k)->~thread();
		}

		::operator delete(reinterpret_cast<char*>(this->m_threads_data_ptr) - cache_line_size);
	}

	if (this->m_task_buffer_data_ptr != nullptr)
	{
		std::size_t task_buffer_size = static_cast<std::size_t>(this->m_task_buffer_end_ptr - this->m_task_buffer_data_ptr);
		for (std::size_t k = 0; k < task_buffer_size; k++)
		{
			(this->m_task_buffer_data_ptr + k)->~_task();
		}

		::operator delete(this->m_task_buffer_unaligned_data_ptr);
	}

	this->m_threads_data_ptr = nullptr;
	this->m_thread_count = 0;

	this->m_task_buffer_data_ptr = nullptr;
	this->m_task_buffer_end_ptr = nullptr;
	this->m_next_task_ptr = nullptr;
	this->m_last_task_ptr = nullptr;

	this->m_task_buffer_unaligned_data_ptr = nullptr;
}


// threads_mq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::~threads_mq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return;
				}
			}
			else
			{
				full_queue_counter--;

				if (full_queue_counter == 0)
				{
					std::this_thread::yield();
					full_queue_counter = this->m_thread_count;
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						std::this_thread::yield();
						full_queue_counter = this->m_thread_count;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;
			
		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						std::this_thread::yield();
						full_queue_counter = this->m_thread_count;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
					current_thread_ptr->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

							xCOOL_THREADS_TRY
							{
								*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							target_ref.decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_and_offset_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						std::this_thread::yield();
						full_queue_counter = this->m_thread_count;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
					current_thread_ptr->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

							xCOOL_THREADS_TRY
							{
								*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							target_ref.decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_and_offset_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						std::this_thread::yield();
						full_queue_counter = this->m_thread_count;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::no_target_t, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return true;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						return false;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::async_end& target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(&target);

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return true;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						return false;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							xCOOL_THREADS_TRY
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call_no_return(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							static_cast<cool::async_end*>(_task_ptr->m_address_data.m_target_ptr)->decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return true;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						return false;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
					current_thread_ptr->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

							xCOOL_THREADS_TRY
							{
								*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							target_ref.decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_and_offset_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return true;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						return false;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty&& ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_arg_type_static_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

	_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
	_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
	std::size_t thread_num = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);

	std::size_t full_queue_counter = this->m_thread_count;

	while (true)
	{
		_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + thread_num;

		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

			if (lock.owns_lock())
			{
				_cool_thmq_task* last_task_ptr_p1 = (current_thread_ptr->m_last_task_ptr + 1 != current_thread_ptr->m_task_buffer_end_ptr) ?
					current_thread_ptr->m_last_task_ptr + 1 : current_thread_ptr->m_task_buffer_data_ptr;

				if (last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
				{
					target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

					new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::forward<arg_Ty>(args)...);
					std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_address_data.m_function_ptr), &task, sizeof(void(*)(void)));
					current_thread_ptr->m_last_task_ptr->m_address_data.m_target_ptr = static_cast<void*>(target.m_parent_ptr);
					current_thread_ptr->m_last_task_ptr->m_address_data.m_offset = target.m_offset;

					current_thread_ptr->m_last_task_ptr->m_address_data.m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
					{
						if (_fetch_task_ptr == nullptr)
						{
							function_Ty function_ptr;
							std::memcpy(&function_ptr, &(_task_ptr->m_address_data.m_function_ptr), sizeof(void(*)(void)));

							cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_address_data.m_target_ptr);

							xCOOL_THREADS_TRY
							{
								*(target_ref.m_stored_values_ptr + _task_ptr->m_address_data.m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::call(
									function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
								);
							}
							xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
							{
								cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_address_data.m_function_ptr);
							}

							target_ref.decr_awaited();

							reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer)->~_cool_thmq_pack();
						}
						else
						{
							new (static_cast<void*>(_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::move(*reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)));
							reinterpret_cast<_cool_thmq_pack*>(_fetch_task_ptr->m_arg_buffer)->~_cool_thmq_pack();

							std::memcpy(&_task_ptr->m_address_data, &_fetch_task_ptr->m_address_data, _cool_thmq_task::address_with_target_and_offset_size);
						}
					};

					bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);
					current_thread_ptr->m_last_task_ptr = last_task_ptr_p1;
					lock.unlock();

					if (notification_required)
					{
						current_thread_ptr->m_condition_var.notify_one();
					}

					return true;
				}
				else
				{
					full_queue_counter--;

					if (full_queue_counter == 0)
					{
						return false;
					}
				}
			}
		}
		xCOOL_THREADS_CATCH(...) {}

		thread_num = (thread_num != 0) ? thread_num - 1 : this->m_thread_count - 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check> template <class ... arg_Ty>
inline constexpr bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::arg_type_is_valid() noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_pack = std::tuple<typename std::decay<arg_Ty>::type ...>;

	constexpr bool ret = (sizeof(_cool_thmq_pack) <= _arg_buffer_size) && (alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task));
	return ret;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::threads_init_result cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::init_new_threads(
	cool::thread_count new_thread_count,
	cool::task_buffer_size new_task_buffer_size,
	cool::try_pop_count new_try_pop_count,
	cool::dispatch_interval new_dispatch_interval) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	assert((reinterpret_cast<std::uintptr_t>(this) % _cache_line_size == 0) && "cool::threads_mq<...> : object location must be aligned in memory");

	if (reinterpret_cast<std::uintptr_t>(this) % _cache_line_size != 0)
	{
		return cool::threads_init_result(cool::threads_init_result::bad_align);
	}
	else
	{
		delete_threads();
	}

	if ((new_thread_count.value() == 0) || (new_task_buffer_size.value() == 0))
	{
		return cool::threads_init_result(cool::threads_init_result::bad_parameters);
	}

	if (new_dispatch_interval.value() != 0)
	{
		this->m_dispatch_interval = static_cast<_cool_thmq_uintX>(new_dispatch_interval.value());
	}
	else
	{
		this->m_dispatch_interval = static_cast<_cool_thmq_uintX>(default_dispatch_interval(new_thread_count.value()));
	}

	if (new_thread_count.value() != 1)
	{
		constexpr _cool_thmq_uintX uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X mod_D = static_cast<_cool_thmq_uint2X>(new_thread_count.value());

		_cool_thmq_uintX i = 0;
		while ((static_cast<_cool_thmq_uint2X>(1) << i) < static_cast<_cool_thmq_uint2X>(new_thread_count.value()))
		{
			i++;
		}

		_cool_thmq_uintX temp = uintX_bitcount + i;
		_cool_thmq_uint2X mod_a = (static_cast<_cool_thmq_uint2X>(1) << temp) / mod_D;
		if ((mod_a * mod_D) != (static_cast<_cool_thmq_uint2X>(1) << temp))
		{
			mod_a++;
		}

		this->m_mod_k = i - 1;
		this->m_mod_a = mod_a - (static_cast<_cool_thmq_uint2X>(1) << uintX_bitcount);
		this->m_mod_D = mod_D;
	}
	else
	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		this->m_mod_k = 0;
		this->m_mod_a = static_cast<_cool_thmq_uint2X>(1) << uintX_bitcount;
		this->m_mod_D = 1;
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count.value());
	std::size_t task_buffer_size_per_thread = new_task_buffer_size.value() / _new_thread_count;
	if (_new_thread_count * task_buffer_size_per_thread != new_task_buffer_size.value())
	{
		task_buffer_size_per_thread++;
	}

	this->m_thread_count = _new_thread_count;

	unsigned int _new_pop_rounds = new_try_pop_count.value() / static_cast<unsigned int>(new_thread_count.value());
	if ((static_cast<unsigned int>(new_thread_count.value()) * _new_pop_rounds != new_try_pop_count.value())
		&& (static_cast<unsigned int>(new_thread_count.value()) * (_new_pop_rounds + 1) >= new_try_pop_count.value()))
	{
		_new_pop_rounds++;
	}

	this->m_pop_rounds = _new_pop_rounds;
	

	std::size_t threads_constructed = 0;
	std::size_t threads_launched = 0;

	this->m_thread_blocks_unaligned_data_ptr = static_cast<char*>(::operator new(
		_new_thread_count * sizeof(_cool_thmq_tblk) + 2 * cache_line_size, std::nothrow));

	if (this->m_thread_blocks_unaligned_data_ptr == nullptr)
	{
		this->delete_threads_detail(threads_constructed, threads_launched);
		return cool::threads_init_result(cool::threads_init_result::bad_alloc);
	}
	else
	{
		std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_thread_blocks_unaligned_data_ptr) % static_cast<std::uintptr_t>(cache_line_size);

		this->m_thread_blocks_data_ptr = reinterpret_cast<_cool_thmq_tblk*>(this->m_thread_blocks_unaligned_data_ptr
			+ static_cast<std::size_t>(ptr_remainder != 0) * (cache_line_size - static_cast<std::size_t>(ptr_remainder)));
	}

	for (std::size_t thread_num = 0; thread_num < _new_thread_count; thread_num++)
	{
		new (this->m_thread_blocks_data_ptr + thread_num) _cool_thmq_tblk();
		if (!((this->m_thread_blocks_data_ptr + thread_num)->new_task_buffer(task_buffer_size_per_thread)))
		{
			this->delete_threads_detail(threads_constructed, threads_launched);
			return cool::threads_init_result(cool::threads_init_result::bad_alloc);
		}
		threads_constructed++;
	}

	for (std::size_t thread_num = 0; thread_num < _new_thread_count; thread_num++)
	{
		(this->m_thread_blocks_data_ptr + thread_num)->m_stop_threads = false;

		xCOOL_THREADS_TRY
		{
			if (_new_pop_rounds == 0)
			{
				_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + thread_num;

				ptr->m_thread = std::thread([ptr]()
				{
					while (true)
					{
						xCOOL_THREADS_TRY
						{
							_cool_thmq_task current_task;

							{
								std::unique_lock<std::mutex> lock(ptr->m_mutex);

								ptr->m_condition_var.wait(lock, [ptr]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

								if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
								{
									ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

									_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

									ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
										next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;
								}
								else
								{
									return;
								}
							}

							current_task.m_address_data.m_callable(&current_task, nullptr);
						}
						xCOOL_THREADS_CATCH(...) {}
					}
				});
			}
			else if (_new_pop_rounds == 1)
			{
				_cool_thmq_tblk* thread_blocks_data_ptr = this->m_thread_blocks_data_ptr;

				(thread_blocks_data_ptr + thread_num)->m_thread = std::thread([thread_blocks_data_ptr, thread_num, _new_thread_count]()
				{
					while (true)
					{
						xCOOL_THREADS_TRY
						{
							_cool_thmq_task current_task;

							bool ongoing = [&]() -> bool
							{
								for (std::size_t k = thread_num; k < _new_thread_count; k++)
								{
									_cool_thmq_tblk* ptr = thread_blocks_data_ptr + k;

									std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

									if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
									{
										ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

										_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

										ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
											next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

										return true; // return from lambda, sets variable 'ongoing' as true
									}
								}

								for (std::size_t k = 0; k < thread_num; k++)
								{
									_cool_thmq_tblk* ptr = thread_blocks_data_ptr + k;

									std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

									if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
									{
										ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

										_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

										ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
											next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

										return true; // return from lambda, sets variable 'ongoing' as true
									}
								}

								{
									_cool_thmq_tblk* ptr = thread_blocks_data_ptr + thread_num;

									std::unique_lock<std::mutex> lock(ptr->m_mutex);

									ptr->m_condition_var.wait(lock, [ptr]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

									if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
									{
										ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

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
								current_task.m_address_data.m_callable(&current_task, nullptr);
							}
							else
							{
								return;
							}
						}
						xCOOL_THREADS_CATCH(...) {}
					}
				});
			}
			else
			{
				_cool_thmq_tblk* thread_blocks_data_ptr = this->m_thread_blocks_data_ptr;

				(thread_blocks_data_ptr + thread_num)->m_thread = std::thread([thread_blocks_data_ptr, thread_num, _new_thread_count, _new_pop_rounds]()
				{
					while (true)
					{
						xCOOL_THREADS_TRY
						{
							_cool_thmq_task current_task;

							bool ongoing = [&]() -> bool
							{
								for (unsigned int n = _new_pop_rounds; n > 0; n--)
								{
									for (std::size_t k = thread_num; k < _new_thread_count; k++)
									{
										_cool_thmq_tblk* ptr = thread_blocks_data_ptr + k;

										std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

										if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
										{
											ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

											_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

											ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
												next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

											return true; // return from lambda, sets variable 'ongoing' as true
										}
									}

									for (std::size_t k = 0; k < thread_num; k++)
									{
										_cool_thmq_tblk* ptr = thread_blocks_data_ptr + k;

										std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

										if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
										{
											ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

											_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

											ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
												next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;

											return true; // return from lambda, sets variable 'ongoing' as true
										}
									}
								}

								{
									_cool_thmq_tblk* ptr = thread_blocks_data_ptr + thread_num;

									std::unique_lock<std::mutex> lock(ptr->m_mutex);

									ptr->m_condition_var.wait(lock, [ptr]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

									if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
									{
										ptr->m_next_task_ptr->m_address_data.m_callable(&current_task, ptr->m_next_task_ptr);

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
								current_task.m_address_data.m_callable(&current_task, nullptr);
							}
							else
							{
								return;
							}
						}
						xCOOL_THREADS_CATCH(...) {}
					}
				});
			}
		}
		xCOOL_THREADS_CATCH(...)
		{
			this->delete_threads_detail(threads_constructed, threads_launched);
			return cool::threads_init_result(cool::threads_init_result::bad_thread_creation);
		}

		threads_launched++;
	}

	std::atomic_signal_fence(std::memory_order_release);
	this->m_can_refresh.store(true, std::memory_order_release);
	this->m_good.store(true, std::memory_order_seq_cst);
	return cool::threads_init_result(cool::threads_init_result::success);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::good() const noexcept
{
	return this->m_good.load(std::memory_order_seq_cst);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::size_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_count() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return this->m_thread_count;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::size_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::task_buffer_size() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);

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

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline unsigned int cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::try_pop_count() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return this->m_pop_rounds * static_cast<unsigned int>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::dispatch_interval() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return static_cast<std::uint16_t>(this->m_dispatch_interval);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::delete_threads() noexcept
{
	this->delete_threads_detail(this->m_thread_count, this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::safety_refresh() noexcept
{
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	if (this->m_can_refresh.exchange(false, std::memory_order_acquire))
	{
		_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr;

		for (std::size_t k = 0; k < this->m_thread_count; k++)
		{
			(ptr + k)->m_condition_var.notify_one();
		}

		this->m_can_refresh.store(true, std::memory_order_release);
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::thread::id cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_id(std::size_t thread_number) const noexcept
{
	return (this->m_thread_blocks_data_ptr + thread_number)->m_thread.get_id();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::_thread_const_iterator_proxy<cool::_thread_iterator<cool::_thread_mq_id<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>> cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_ids() const noexcept
{
	return cool::_thread_const_iterator_proxy<cool::_thread_iterator<cool::_thread_mq_id<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>>(this->m_thread_blocks_data_ptr, this->m_thread_blocks_data_ptr + this->m_thread_count);
}

#ifdef COOL_THREADS_NATIVE_HANDLE
template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::thread::native_handle_type cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_native_handle(std::size_t thread_number)
{
	return (this->m_thread_blocks_data_ptr + thread_number)->m_thread.native_handle();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::_thread_iterator_proxy<cool::_thread_iterator<cool::_thread_mq_native_handle<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>> cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::thread_native_handles() noexcept
{
	return cool::_thread_iterator_proxy<cool::_thread_iterator<cool::_thread_mq_native_handle<_cache_line_size, _arg_buffer_size, _arg_buffer_align>>>(this->m_thread_blocks_data_ptr, this->m_thread_blocks_data_ptr + this->m_thread_count);
}
#endif // COOL_THREADS_NATIVE_HANDLE

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>::default_dispatch_interval(std::uint16_t new_thread_count) noexcept
{
	if (new_thread_count == 0)
	{
		return 0;
	}

	std::uint16_t max_dispatch_interval;

	{
		using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;

		_cool_thmq_uintX _max_dispatch_interval = static_cast<_cool_thmq_uintX>(-1) / static_cast<_cool_thmq_uintX>(new_thread_count);
		_cool_thmq_uintX _new_thread_count_d2 = static_cast<_cool_thmq_uintX>(new_thread_count) / 2;
		max_dispatch_interval = (_max_dispatch_interval < _new_thread_count_d2) ?
			static_cast<std::uint16_t>(_max_dispatch_interval) : static_cast<std::uint16_t>(_new_thread_count_d2);
		max_dispatch_interval = (max_dispatch_interval < 32) ? max_dispatch_interval : 32;
	}

	std::uint16_t sqr = 0;

	while ((sqr < max_dispatch_interval) && (sqr * sqr <= new_thread_count))
	{
		sqr++;
	}

	if (sqr == 0)
	{
		return 0;
	}
	else
	{
		for (std::uint16_t ret = sqr; ret > 2; ret--)
		{
			for (std::uint16_t divisor = ret; (ret % divisor != 0) || (new_thread_count % divisor != 0); divisor--)
			{
				if (divisor <= 2)
				{
					return ret;
				}
			}
		}

		for (std::uint16_t ret = sqr; ret < max_dispatch_interval; ret++)
		{
			for (std::uint16_t divisor = ret; (ret % divisor != 0) || (new_thread_count % divisor != 0); divisor--)
			{
				if (divisor <= 2)
				{
					return ret;
				}
			}
		}

		return ((new_thread_count % 2 != 0) && (2 <= max_dispatch_interval)) ? 2 : 1;
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept
{
	bool was_good = this->m_good.exchange(false, std::memory_order_seq_cst);

	_thread_block* ptr = this->m_thread_blocks_data_ptr;

	if (ptr != nullptr)
	{
		for (std::size_t k = 0; k < threads_launched; k++)
		{
			while (true)
			{
				xCOOL_THREADS_TRY
				{
					std::lock_guard<std::mutex> lock((ptr + k)->m_mutex);
					(ptr + k)->m_stop_threads = true;
					break;
				}
				xCOOL_THREADS_CATCH(...) {}
			}

			(ptr + k)->m_condition_var.notify_one();

			if ((ptr + k)->m_thread.joinable())
			{
				std::thread::id thread_id = (ptr + k)->m_thread.get_id();

				xCOOL_THREADS_TRY
				{
					(ptr + k)->m_thread.join();
				}
				xCOOL_THREADS_CATCH(const std::system_error& xCOOL_THREADS_SYSTEM_ERROR)
				{
					cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_delete_thread_exception(xCOOL_THREADS_SYSTEM_ERROR, thread_id);
				}
			}
		}

		if (was_good)
		{
			while (!(this->m_can_refresh.load(std::memory_order_relaxed)) || !(this->m_can_refresh.exchange(false, std::memory_order_acquire)))
			{
				std::this_thread::yield();
			}
		}

		for (std::size_t k = 0; k < threads_constructed; k++)
		{
			(ptr + k)->delete_task_buffer();
			(ptr + k)->~_thread_block();
		}

		::operator delete(this->m_thread_blocks_unaligned_data_ptr);
	}

	constexpr std::size_t uintX_bitcount = sizeof(_uintX) * CHAR_BIT;

	this->m_dispatch_interval = 1;
	this->m_mod_k = 0;
	this->m_mod_a = static_cast<_uint2X>(1) << uintX_bitcount;
	this->m_mod_D = 1;
	this->m_thread_count = 0;
	this->m_thread_blocks_data_ptr = nullptr;

	this->m_pop_rounds = 0;

	this->m_thread_blocks_unaligned_data_ptr = nullptr;

	std::atomic_signal_fence(std::memory_order_release);
}

inline cool::threads_init_result::operator bool() const noexcept
{
	return m_result == threads_init_result::success;
}

inline bool cool::threads_init_result::good() const noexcept
{
	return m_result == threads_init_result::success;
}

inline int cool::threads_init_result::value() const noexcept
{
	return m_result;
}

inline const char* cool::threads_init_result::message() const noexcept
{
	switch (m_result)
	{
	case threads_init_result::success: return "cool threads init success"; break;
	case threads_init_result::bad_align: return "cool threads init failed : bad alignment of threads_sq/threads_mq object"; break;
	case threads_init_result::bad_parameters: return "cool threads init failed : bad parameters"; break;
	case threads_init_result::bad_alloc: return "cool threads init failed : bad allocation"; break;
	case threads_init_result::bad_thread_creation: return "cool threads init failed : bad thread creation"; break;
	default: return "cool threads init result undefined"; break;
	}
}

inline cool::threads_init_result::threads_init_result(int result) noexcept : m_result(result) {}

inline void cool::threads_exception_handler::set_on_exception(void(*on_exception)(const std::exception&, void(*)(void), std::thread::id, void*), void* exception_arg_ptr) noexcept
{
	cool::_threads_base::exception_handler handler{ on_exception, exception_arg_ptr };
	cool::_threads_base::get_exception_handler().store(handler, std::memory_order_seq_cst);
}

inline void cool::threads_exception_handler::set_on_delete_thread_exception(void(*on_delete_thread_exception)(const std::system_error&, std::thread::id, void*), void* delete_thread_exception_arg_ptr) noexcept
{
	cool::_threads_base::delete_thread_exception_handler handler{ on_delete_thread_exception, delete_thread_exception_arg_ptr };
	cool::_threads_base::get_delete_thread_exception_handler().store(handler, std::memory_order_seq_cst);
}

inline void cool::threads_exception_handler::clear() noexcept
{
	cool::_threads_base::exception_handler handler{ nullptr, nullptr };
	cool::_threads_base::get_exception_handler().store(handler, std::memory_order_seq_cst);

	cool::_threads_base::delete_thread_exception_handler delete_thread_handler{ nullptr, nullptr };
	cool::_threads_base::get_delete_thread_exception_handler().store(delete_thread_handler, std::memory_order_seq_cst);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline bool cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block::new_task_buffer(std::size_t new_task_buffer_size) noexcept
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
			(m_task_buffer_data_ptr + k)->~_task();
		}

		::operator delete(m_task_buffer_unaligned_data_ptr);
	}
}


// async_end detail

inline void cool::async_end::add_awaited(std::size_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(static_cast<std::ptrdiff_t>(number_of_tasks), std::memory_order_seq_cst);
}

inline bool cool::async_end::sub_awaited(std::size_t number_of_tasks) noexcept
{
	std::ptrdiff_t _number_of_tasks = static_cast<std::ptrdiff_t>(number_of_tasks);
	std::ptrdiff_t prev = m_tasks_awaited.fetch_sub(_number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (prev <= _number_of_tasks))
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

inline bool cool::async_end::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	std::ptrdiff_t prev = m_tasks_awaited.exchange(number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (number_of_tasks <= 0))
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

inline std::ptrdiff_t cool::async_end::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

inline bool cool::async_end::decr_awaited() noexcept
{
	if (m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

inline void cool::async_end::notify() noexcept
{
	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(m_finish_mutex);
			m_finish_condition_var.notify_all();
			return;
		}
		xCOOL_THREADS_CATCH(...) {}
	}
}

inline void cool::async_end::finish() noexcept
{
	while (m_tasks_awaited.load(std::memory_order_seq_cst) > 0)
	{
		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(m_finish_mutex);

			if (m_tasks_awaited.load(std::memory_order_relaxed) > 0)
			{
				m_finish_condition_var.wait(lock);
			}
			else
			{
				return;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}
}

inline bool cool::async_end::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

inline cool::_async_end_incr_proxy cool::async_end::try_incr_awaited() noexcept
{
	return cool::_async_end_incr_proxy(this);
}

inline void cool::async_end::safety_refresh() noexcept
{
	m_finish_condition_var.notify_all();
}


// async_result detail

template <class return_Ty>
inline cool::async_result<return_Ty>::async_result(return_Ty* storage_ptr) noexcept
{
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::add_awaited(std::size_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
}

template <class return_Ty>
inline bool cool::async_result<return_Ty>::sub_awaited(std::size_t number_of_tasks) noexcept
{
	std::ptrdiff_t _number_of_tasks = static_cast<std::ptrdiff_t>(number_of_tasks);
	std::ptrdiff_t prev = m_tasks_awaited.fetch_sub(_number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (prev <= _number_of_tasks))
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline bool cool::async_result<return_Ty>::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	std::ptrdiff_t prev = m_tasks_awaited.exchange(number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (number_of_tasks <= 0))
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline std::ptrdiff_t cool::async_result<return_Ty>::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

template <class return_Ty>
inline cool::_async_result_to_proxy<return_Ty> cool::async_result<return_Ty>::to(std::size_t offset) noexcept
{
	return cool::_async_result_to_proxy<return_Ty>(this, offset);
}

template <class return_Ty>
inline bool cool::async_result<return_Ty>::decr_awaited() noexcept
{
	if (m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
	{
		while (true)
		{
			xCOOL_THREADS_TRY
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::notify() noexcept
{
	while (true)
	{
		xCOOL_THREADS_TRY
		{
			std::lock_guard<std::mutex> lock(m_finish_mutex);
			m_finish_condition_var.notify_all();
			return;
		}
		xCOOL_THREADS_CATCH(...) {}
	}
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::finish() noexcept
{
	while (m_tasks_awaited.load(std::memory_order_seq_cst) > 0)
	{
		xCOOL_THREADS_TRY
		{
			std::unique_lock<std::mutex> lock(m_finish_mutex);

			if (m_tasks_awaited.load(std::memory_order_relaxed) > 0)
			{
				m_finish_condition_var.wait(lock);
			}
			else
			{
				return;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}
}

template <class return_Ty>
inline bool cool::async_result<return_Ty>::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

template <class return_Ty>
inline return_Ty& cool::async_result<return_Ty>::get(std::size_t offset) noexcept
{
	finish();
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline return_Ty& cool::async_result<return_Ty>::get_unchecked(std::size_t offset) noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline const return_Ty& cool::async_result<return_Ty>::get_unchecked(std::size_t offset) const noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::reset(std::size_t count)
{
	finish();

	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty{};
	}
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::reset(std::size_t count, const return_Ty& rhs)
{
	finish();

	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = rhs;
	}
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::reset_unchecked(std::size_t count)
{
	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty{};
	}
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::reset_unchecked(std::size_t count, const return_Ty& rhs)
{
	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = rhs;
	}
}

template <class return_Ty>
inline return_Ty* cool::async_result<return_Ty>::data() noexcept
{
	return m_stored_values_ptr;
}

template <class return_Ty>
inline const return_Ty* cool::async_result<return_Ty>::data() const noexcept
{
	return m_stored_values_ptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::set_data(return_Ty* storage_ptr) noexcept
{
	finish();
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::set_data_unchecked(return_Ty* storage_ptr) noexcept
{
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::clear() noexcept
{
	finish();
	m_stored_values_ptr = nullptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::clear_unchecked() noexcept
{
	m_stored_values_ptr = nullptr;
}

template <class return_Ty>
inline void cool::async_result<return_Ty>::safety_refresh() noexcept
{
	m_finish_condition_var.notify_all();
}

template <class return_Ty>
inline cool::_async_result_incr_proxy<return_Ty> cool::_async_result_to_proxy<return_Ty>::try_incr_awaited() noexcept
{
	return cool::_async_result_incr_proxy<return_Ty>(m_parent_ptr, m_offset);
}

inline void cool::safety_refresh_proxy::safety_refresh() const noexcept
{
	m_callable(m_object_ptr);
}


// safety_refresh_proxy detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::safety_refresh_proxy::safety_refresh_proxy(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& rhs) noexcept : m_object_ptr(&rhs)
{
	m_callable = [](void* object_ptr)
	{
		reinterpret_cast<cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>*>(object_ptr)->safety_refresh();
	};
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _arg_type_static_check>
inline cool::safety_refresh_proxy::safety_refresh_proxy(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>& rhs) noexcept : m_object_ptr(&rhs)
{
	m_callable = [](void* object_ptr)
	{
		reinterpret_cast<cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _arg_type_static_check>*>(object_ptr)->safety_refresh();
	};
}

inline cool::safety_refresh_proxy::safety_refresh_proxy(cool::async_end& rhs) noexcept : m_object_ptr(&rhs)
{
	m_callable = [](void* object_ptr)
	{
		reinterpret_cast<cool::async_end*>(object_ptr)->safety_refresh();
	};
}

template <class return_Ty>
inline cool::safety_refresh_proxy::safety_refresh_proxy(cool::async_result<return_Ty>& rhs) noexcept : m_object_ptr(&rhs)
{
	m_callable = [](void* object_ptr)
	{
		reinterpret_cast<cool::async_result<return_Ty>*>(object_ptr)->safety_refresh();
	};
}

inline bool cool::safety_refresh_proxy::operator==(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr == rhs.m_object_ptr;
}
inline bool cool::safety_refresh_proxy::operator!=(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr != rhs.m_object_ptr;
}
inline bool cool::safety_refresh_proxy::operator<=(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr <= rhs.m_object_ptr;
}
inline bool cool::safety_refresh_proxy::operator>=(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr >= rhs.m_object_ptr;
}
inline bool cool::safety_refresh_proxy::operator<(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr < rhs.m_object_ptr;
}
inline bool cool::safety_refresh_proxy::operator>(const cool::safety_refresh_proxy& rhs) const noexcept {
	return m_object_ptr > rhs.m_object_ptr;
}

inline bool cool::safety_refresh_proxy::operator==(const void* rhs_ptr) const noexcept {
	return m_object_ptr == rhs_ptr;
}
inline bool cool::safety_refresh_proxy::operator!=(const void* rhs_ptr) const noexcept {
	return m_object_ptr != rhs_ptr;
}
inline bool cool::safety_refresh_proxy::operator<=(const void* rhs_ptr) const noexcept {
	return m_object_ptr <= rhs_ptr;
}
inline bool cool::safety_refresh_proxy::operator>=(const void* rhs_ptr) const noexcept {
	return m_object_ptr >= rhs_ptr;
}
inline bool cool::safety_refresh_proxy::operator<(const void* rhs_ptr) const noexcept {
	return m_object_ptr < rhs_ptr;
}
inline bool cool::safety_refresh_proxy::operator>(const void* rhs_ptr) const noexcept {
	return m_object_ptr > rhs_ptr;
}

inline bool cool::operator==(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs == lhs_ptr;
}
inline bool cool::operator!=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs != lhs_ptr;
}
inline bool cool::operator<=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs >= lhs_ptr;
}
inline bool cool::operator>=(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs <= lhs_ptr;
}
inline bool cool::operator<(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs > lhs_ptr;
}
inline bool cool::operator>(const void* lhs_ptr, const cool::safety_refresh_proxy& rhs) noexcept {
	return rhs < lhs_ptr;
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
