// cool_threads.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_THREADS_HPP
#define xCOOL_THREADS_HPP

#include <cstddef>
#include <cstdint>
#include <climits>
#include <cstring>
#include <new>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>
#include <exception>
#include <system_error>
#include <cassert>


// to disable exceptions, #define COOL_THREADS_NOEXCEPTIONS

#if !defined(xCOOL_THREADS_TRY) && !defined(xCOOL_THREADS_CATCH) && !defined(xCOOL_THREADS_EXCEPTION)
#ifndef COOL_THREADS_NOEXCEPTIONS
#define xCOOL_THREADS_TRY try
#define xCOOL_THREADS_CATCH(expr) catch (expr)
#define xCOOL_THREADS_EXCEPTION excep
#define xCOOL_THREADS_SYSTEM_ERROR error
#else // COOL_THREADS_NOEXCEPTIONS
#define xCOOL_THREADS_TRY
#define xCOOL_THREADS_CATCH(expr) if (false)
#define xCOOL_THREADS_EXCEPTION std::exception{}
#define xCOOL_THREADS_SYSTEM_ERROR std::system_error{}
#endif // COOL_THREADS_NOEXCEPTIONS
#endif // !defined(xCOOL_THREADS_TRY) && !defined(xCOOL_THREADS_CATCH) && !defined(xCOOL_THREADS_EXCEPTION)


namespace cool
{
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> class threads_sq;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> class threads_mq;
	class threads_init_result;
	class threads_exception_handler;

#ifndef xCOOL_NOT_TARGET_ENUM
#define xCOOL_NOT_TARGET_ENUM
	enum no_target_t { no_target };
#endif // xCOOL_NOT_TARGET_ENUM

	class async_end;
	template <class return_Ty> class async_result;

	class _threads_base;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_sq_data;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_mq_data;
	class _async_end_incr_proxy;
	template <class Ty> class _async_result_to_proxy;
	template <class Ty> class _async_result_incr_proxy;

	// threads_sq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t), bool _static_arg_type_check = true>
	class threads_sq : private cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;
		static constexpr bool static_arg_type_check = _static_arg_type_check;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_sq() = default;
		threads_sq(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>& operator=(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&) = delete;
		threads_sq(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>& operator=(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&&) = delete;
		inline ~threads_sq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'arg_Ty ... args' must be movable or copyable without throwing exceptions

		// if 'function_Ty task' throws an exception, the thread will call the exception handler and jump to the next task

		// WARNING : queuing tasks with 'async' / 'priority_async' / 'try_async' / 'try_priority_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class ... arg_Ty>
		static inline constexpr bool arg_type_is_valid() noexcept;

		// WARNING : 'init_new_threads' and 'delete_threads' must not be called in concurrency with any other method
		// except the case of 'init_new_thread' with 'good'

		inline cool::threads_init_result init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size) noexcept; // arguments must be > 0
		inline bool good() const noexcept; // true if 'init_new_threads' has finished successfully, must not be relied upon if a 'delete_threads' concurrent call is imminent
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline void delete_threads() noexcept;
	};

	// threads_mq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t), bool _static_arg_type_check = true>
	class threads_mq : private cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;
		static constexpr bool static_arg_type_check = _static_arg_type_check;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_mq() = default;
		threads_mq(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>& operator=(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&) = delete;
		threads_mq(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>& operator=(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>&&) = delete;
		inline ~threads_mq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'arg_Ty ... args' must be movable or copyable without throwing exceptions

		// if 'function_Ty task' throws an exception, the thread will call the exception handler and jump to the next task

		// WARNING : queuing tasks with 'async' / 'try_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline void async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept;

		template <class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class return_Ty, class function_Ty, class ... arg_Ty>
		inline bool try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept;

		template <class ... arg_Ty>
		static inline constexpr bool arg_type_is_valid() noexcept;

		// WARNING : 'init_new_threads' and 'delete_threads' must not be called in concurrency with any other method
		// except the case of 'init_new_thread' with 'good'

		inline cool::threads_init_result init_new_threads(
			std::uint16_t new_thread_count, // must be > 0
			std::size_t new_task_buffer_size, // must be > 0
			unsigned int _pop_tries = 1,
			unsigned int _push_tries = 32, // must be > 0
			std::uint16_t dispatch_interval = 1 // must be > 0
		) noexcept;
		inline bool good() const noexcept; // true if 'init_new_threads' has finished successfully, must not be relied upon if a 'delete_threads' concurrent call is imminent
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline unsigned int pop_tries() const noexcept;
		inline unsigned int push_tries() const noexcept;
		inline std::uint16_t dispatch_interval() const noexcept;
		inline void delete_threads() noexcept;
	};


	// threads_init_result

	class threads_init_result
	{

	public:

		static constexpr int success = 0;
		static constexpr int bad_align = 1;
		static constexpr int bad_parameters = 2;
		static constexpr int bad_alloc = 3;
		static constexpr int bad_thread = 4;

		static constexpr int undefined = -1;

		threads_init_result() = default;

		inline operator bool() const noexcept; // returns true if threads init is successful
		inline bool good() const noexcept; // returns true if threads init is successful
		inline int value() const noexcept;
		inline const char* message() const noexcept;

	private:

		inline threads_init_result(int result) noexcept;

		int m_result = cool::threads_init_result::undefined;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
	};


	// threads_exception_handler

	// handles all exceptions from all instances of threads_sq and threads_mq

	class threads_exception_handler
	{

	public:

		// 'on_exception' arguments are :
		// > const std::exception& : exception thrown
		// > void(*)(void) : task function pointer that produced the exception
		// > void* : optional pointer to buffer 'exception_arg_ptr'

		// if 'on_exception' is a nullptr, then the exception handling will be the logical equivalent to a no-op

		// WARNING : 'on_exception' must not throw any exception or the program will terminate
		// WARNING : potential reads and writes to 'exception_arg_ptr' by 'on_exception' must be synchronized properly


		// 'on_delete_thread_exception' arguments are :
		// > const std::system_error& : exception thrown
		// > void* : pointer to instance of object threads_sq/threads_mq that produced the exception
		// > void* : optional pointer to buffer 'delete_thread_exception_arg_ptr'

		// if 'on_delete_thread_exception' is a nullptr, then the exception handling will be the logical equivalent to a no-op

		// WARNING : 'on_delete_thread_exception' must not throw any exception or the program will terminate
		// WARNING : potential reads and writes to 'delete_thread_exception_arg_ptr' by 'on_delete_thread_exception' must be synchronized properly
		// WARNING : 'on_delete_thread_exception' is called if a thread failed to join and the program should be considered to be unlikely to be left in a good state afterwards

		static inline void set_on_exception(void(*on_exception)(const std::exception&, void(*)(void), void*), void* exception_arg_ptr = nullptr) noexcept;
		static inline void set_on_delete_thread_exception(void(*on_delete_thread_exception)(const std::system_error&, void*, void*), void* delete_thread_exception_arg_ptr = nullptr) noexcept;
		static inline void clear() noexcept;
	};


	// async_end

	// WARNNING : async_end object must outlive the threads of threads_sq/threads_mq object that calls
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

	private:

		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
	};


	// async_result

	// WARNNING: async_result object must outlive the threads of threads_sq/threads_mq object that calls
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

	private:

		return_Ty* m_stored_values_ptr = nullptr;
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
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

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::async_result;
	};
}


namespace cool
{
	// _threads_base

	class _threads_base
	{

	private:

		friend class cool::threads_exception_handler;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::_threads_sq_data;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::_threads_mq_data;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;

		template <std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _base_task {
		public:
			static constexpr std::size_t _arg_buffer_size_padded = (_arg_buffer_size != 0) ? _arg_buffer_size : 1;
			alignas(_arg_buffer_align) unsigned char m_arg_buffer[_arg_buffer_size_padded];
			void(*m_callable)(_base_task<_arg_buffer_size, _arg_buffer_align>*, _base_task<_arg_buffer_size, _arg_buffer_align>*);
			void(*m_function_ptr)(void);
			void* m_target_ptr;
			std::size_t m_offset;
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
			inline exception_handler(void(*_function)(const std::exception&, void(*)(void), void*), void* _arg_ptr) noexcept
				: m_function(_function), m_arg_ptr(_arg_ptr) {};
			void(*m_function)(const std::exception&, void(*)(void), void*) = nullptr;
			void* m_arg_ptr = nullptr;
		};

		static inline std::atomic<exception_handler>& get_exception_handler() noexcept {
			static std::atomic<exception_handler> handler{ exception_handler{ nullptr, nullptr } };
			return handler;
		}
		static inline void catch_exception(const std::exception& excep, void(*task_function_ptr)(void)) noexcept {
			exception_handler handler = get_exception_handler().load(std::memory_order_seq_cst);
			if (handler.m_function != nullptr) {
				handler.m_function(excep, task_function_ptr, handler.m_arg_ptr);
			}
		}

		class delete_thread_exception_handler {
		public:
			inline delete_thread_exception_handler(void(*_function)(const std::system_error&, void*, void*), void* _arg_ptr) noexcept
				: m_function(_function), m_arg_ptr(_arg_ptr) {};
			void(*m_function)(const std::system_error&, void*, void*) = nullptr;
			void* m_arg_ptr = nullptr;
		};

		static inline std::atomic<delete_thread_exception_handler>& get_delete_thread_exception_handler() noexcept {
			static std::atomic<delete_thread_exception_handler> handler{ delete_thread_exception_handler{ nullptr, nullptr } };
			return handler;
		}
		static inline void catch_delete_thread_exception(const std::system_error& error, void* threads_xq_instance) noexcept {
			delete_thread_exception_handler handler = get_delete_thread_exception_handler().load(std::memory_order_seq_cst);
			if (handler.m_function != nullptr) {
				handler.m_function(error, threads_xq_instance, handler.m_arg_ptr);
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
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_sq_data(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		~_threads_sq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed) noexcept;

		_task* m_task_buffer_data_ptr = nullptr;
		_task* m_task_buffer_end_ptr = nullptr;

		_task* m_last_task_ptr = nullptr;
		_task* m_next_task_ptr = nullptr;

		std::thread* m_threads_data_ptr = nullptr;
		std::size_t m_thread_count = 0;

		std::condition_variable m_condition_var;
		std::mutex m_mutex;

		bool m_stop_threads = true;
		std::atomic<bool> m_good{ false };

		char* m_task_buffer_unaligned_data_ptr = nullptr;
	};

	// _threads_mq_data

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
	class alignas(_cache_line_size) _threads_mq_data : public cool::_threads_base
	{

	private:

		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, true>;
		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, false>;

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
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_mq_data(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		~_threads_mq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept;

		class _thread_block;

		_thread_block* m_thread_blocks_data_ptr = nullptr;
		std::size_t m_thread_count = 0;
		unsigned int m_pop_rounds = 0;
		unsigned int m_push_rounds = 0;
		std::atomic<bool> m_good{ false };
		
		_uint2X m_mod_D = 1;
		_uint2X m_mod_a = static_cast<_uint2X>(1) << (sizeof(_uintX) * CHAR_BIT);
		_uintX m_mod_k = 0;
		_uintX m_dispatch_interval = 1;
		char* m_thread_blocks_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<_uintX> m_thread_dispatch{ 0 };


		class alignas(_cache_line_size) _thread_block
		{

		public:

			inline bool new_task_buffer(std::size_t new_task_buffer_size) noexcept;
			inline void delete_task_buffer() noexcept;

			_task* m_task_buffer_data_ptr = nullptr;
			_task* m_task_buffer_end_ptr = nullptr;

			_task* m_last_task_ptr = nullptr;
			_task* m_next_task_ptr = nullptr;

			std::condition_variable m_condition_var;
			std::mutex m_mutex;
			std::thread m_thread;

			bool m_stop_threads = true;

			char* m_task_buffer_unaligned_data_ptr = nullptr;
		};
	};

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

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
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

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::_async_result_to_proxy;
	};
}


// threads_sq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::~threads_sq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::priority_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
					this->m_next_task_ptr : this->m_task_buffer_end_ptr;
				(this->m_next_task_ptr)--;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

				this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

				this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::priority_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
					this->m_next_task_ptr : this->m_task_buffer_end_ptr;
				(this->m_next_task_ptr)--;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_target_ptr = static_cast<void*>(&target);

				this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

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
				std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

				this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						xCOOL_THREADS_TRY
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_offset = target.m_offset;

				this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						target_ref.decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				this->m_next_task_ptr = (this->m_next_task_ptr != this->m_task_buffer_data_ptr) ?
					this->m_next_task_ptr : this->m_task_buffer_end_ptr;
				(this->m_next_task_ptr)--;

				new (static_cast<void*>(this->m_next_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_offset = target.m_offset;

				this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						target_ref.decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::async : task arguments alignment too large");

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

			_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

			if ((last_task_ptr_p1 != this->m_next_task_ptr)
				&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
					|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
			{
				target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

				new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
				std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_last_task_ptr->m_offset = target.m_offset;

				this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						target_ref.decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::priority_async : task arguments alignment too large");

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
				std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
				this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
				this->m_next_task_ptr->m_offset = target.m_offset;

				this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
				{
					if (_fetch_task_ptr == nullptr)
					{
						function_Ty function_ptr;
						std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

						cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

						xCOOL_THREADS_TRY
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
						}

						target_ref.decr_awaited();

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

				break;
			}
		}
		xCOOL_THREADS_CATCH(...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_priority_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
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
			std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_priority_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
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
			std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_next_task_ptr->m_target_ptr = static_cast<void*>(&target);

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_priority_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
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
			std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					xCOOL_THREADS_TRY
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

					xCOOL_THREADS_TRY
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					target_ref.decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_priority_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
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
			std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_next_task_ptr->m_offset = target.m_offset;

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

					xCOOL_THREADS_TRY
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					target_ref.decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

			new (static_cast<void*>(this->m_last_task_ptr->m_arg_buffer)) _cool_thsq_pack(std::make_tuple(std::move(args)...));
			std::memcpy(&(this->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

			this->m_last_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

					xCOOL_THREADS_TRY
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					target_ref.decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_priority_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert((sizeof(_cool_thsq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments size too large");
	static_assert((alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task)) || !_static_arg_type_check, "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_sq<...>::try_priority_async : task arguments size or alignment too large");
		std::terminate();
	}

	xCOOL_THREADS_TRY
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
			std::memcpy(&(this->m_next_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
			this->m_next_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_next_task_ptr->m_offset = target.m_offset;

			this->m_next_task_ptr->m_callable = [](_cool_thsq_task* _task_ptr, _cool_thsq_task* _fetch_task_ptr)
			{
				if (_fetch_task_ptr == nullptr)
				{
					function_Ty function_ptr;
					std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

					cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

					xCOOL_THREADS_TRY
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
					}

					target_ref.decr_awaited();

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
	xCOOL_THREADS_CATCH(...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class ... arg_Ty>
inline constexpr bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::arg_type_is_valid() noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = std::tuple<arg_Ty ...>;

	constexpr bool ret = (sizeof(_cool_thsq_pack) <= _arg_buffer_size) && (alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task));
	return ret;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline cool::threads_init_result cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;

	if (reinterpret_cast<std::uintptr_t>(this) % _cache_line_size != 0)
	{
		return cool::threads_init_result(cool::threads_init_result::bad_align);
	}
	else
	{
		delete_threads();
		this->m_stop_threads = false;
	}

	if ((new_thread_count == 0) || (new_task_buffer_size == 0))
	{
		return cool::threads_init_result(cool::threads_init_result::bad_parameters);
	}


	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count);
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
	std::size_t new_task_buffer_size_p1 = new_task_buffer_size + 1;

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
					xCOOL_THREADS_CATCH(...) {}
				}
			});
		}
		xCOOL_THREADS_CATCH(...)
		{
			this->delete_threads_detail(threads_constructed);
			std::atomic_signal_fence(std::memory_order_release);
			return cool::threads_init_result(cool::threads_init_result::bad_thread);
		}

		threads_constructed++;
	}

	std::atomic_signal_fence(std::memory_order_release);
	this->m_good.store(true, std::memory_order_seq_cst);
	return cool::threads_init_result(cool::threads_init_result::success);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::good() const noexcept
{
	return this->m_good.load(std::memory_order_seq_cst);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline std::uint16_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::thread_count() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return static_cast<std::uint16_t>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline std::size_t cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::task_buffer_size() const noexcept
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

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::delete_threads() noexcept
{
	this->m_good.store(false, std::memory_order_seq_cst);
	this->delete_threads_detail(this->m_thread_count);
	std::atomic_signal_fence(std::memory_order_release);
}

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
		for (size_t k = 0; k < threads_constructed; k++)
		{
			if ((this->m_threads_data_ptr + k)->joinable())
			{
				xCOOL_THREADS_TRY
				{
					(this->m_threads_data_ptr + k)->join();
				}
				xCOOL_THREADS_CATCH(const std::system_error& xCOOL_THREADS_SYSTEM_ERROR)
				{
					cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_delete_thread_exception(xCOOL_THREADS_SYSTEM_ERROR, this);
				}
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

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::~threads_mq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	while (true)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	while (true)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	while (true)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;
			
			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	while (true)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	while (true)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::no_target_t, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::async_end& target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_end_incr_proxy target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								xCOOL_THREADS_TRY
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_result_to_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
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
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class return_Ty, class function_Ty, class ... arg_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::try_async(cool::_async_result_incr_proxy<return_Ty> target, function_Ty task, arg_Ty ... args) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_mq<...>::try_async : task must be a function pointer");
	static_assert((sizeof(_cool_thmq_pack) <= _arg_buffer_size) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments size too large");
	static_assert((alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task)) || !_static_arg_type_check, "cool::threads_mq<...>::try_async : task arguments alignment too large");

	constexpr bool doom = !arg_type_is_valid<arg_Ty ...>();
	if (doom)
	{
		assert(false && "cool::threads_mq<...>::try_async : task arguments size or alignment too large");
		std::terminate();
	}

	std::size_t first_thread;

	{
		constexpr std::size_t uintX_bitcount = sizeof(_cool_thmq_uintX) * CHAR_BIT;

		_cool_thmq_uint2X N = static_cast<_cool_thmq_uint2X>(this->m_thread_dispatch.fetch_add(this->m_dispatch_interval, std::memory_order_relaxed));
		_cool_thmq_uint2X b = (N * this->m_mod_a) >> uintX_bitcount;
		first_thread = static_cast<std::size_t>(N - ((((N - b) >> 1) + b) >> this->m_mod_k) * this->m_mod_D);
	}

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
	{
		for (std::size_t k = first_thread + 1; k > 0; )
		{
			k--;

			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			xCOOL_THREADS_TRY
			{
				std::unique_lock<std::mutex> lock(current_thread_ptr->m_mutex, std::try_to_lock);

				if (lock.owns_lock())
				{
					_cool_thmq_task* last_task_ptr_p1 = current_thread_ptr->m_last_task_ptr + 1;

					if ((last_task_ptr_p1 != current_thread_ptr->m_next_task_ptr)
						&& ((current_thread_ptr->m_task_buffer_data_ptr != current_thread_ptr->m_next_task_ptr)
							|| (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr)))
					{
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						new (static_cast<void*>(current_thread_ptr->m_last_task_ptr->m_arg_buffer)) _cool_thmq_pack(std::make_tuple(std::move(args)...));
						std::memcpy(&(current_thread_ptr->m_last_task_ptr->m_function_ptr), &task, sizeof(void(*)(void)));
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

						current_thread_ptr->m_last_task_ptr->m_callable = [](_cool_thmq_task* _task_ptr, _cool_thmq_task* _fetch_task_ptr)
						{
							if (_fetch_task_ptr == nullptr)
							{
								function_Ty function_ptr;
								std::memcpy(&function_ptr, &(_task_ptr->m_function_ptr), sizeof(void(*)(void)));

								cool::async_result<return_Ty>& target_ref = *static_cast<cool::async_result<return_Ty>*>(_task_ptr->m_target_ptr);

								xCOOL_THREADS_TRY
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								xCOOL_THREADS_CATCH(const std::exception& xCOOL_THREADS_EXCEPTION)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::catch_exception(xCOOL_THREADS_EXCEPTION, _task_ptr->m_function_ptr);
								}

								target_ref.decr_awaited();

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

						bool notification_required = (current_thread_ptr->m_last_task_ptr == current_thread_ptr->m_next_task_ptr);

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						lock.unlock();

						if (notification_required)
						{
							current_thread_ptr->m_condition_var.notify_one();
						}

						return true;
					}
				}
			}
			xCOOL_THREADS_CATCH(...) {}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check> template <class ... arg_Ty>
inline constexpr bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::arg_type_is_valid() noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_pack = std::tuple<arg_Ty ...>;

	constexpr bool ret = (sizeof(_cool_thmq_pack) <= _arg_buffer_size) && (alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task));
	return ret;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline cool::threads_init_result cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::init_new_threads(
	std::uint16_t new_thread_count,
	std::size_t new_task_buffer_size,
	unsigned int _pop_tries,
	unsigned int _push_tries,
	std::uint16_t dispatch_interval) noexcept
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	if (reinterpret_cast<std::uintptr_t>(this) % _cache_line_size != 0)
	{
		return cool::threads_init_result(cool::threads_init_result::bad_align);
	}
	else
	{
		delete_threads();
	}

	if ((new_thread_count == 0) || (new_task_buffer_size == 0) || (_push_tries == 0) || (dispatch_interval == 0))
	{
		return cool::threads_init_result(cool::threads_init_result::bad_parameters);
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count);
	std::size_t task_buffer_size_per_thread = new_task_buffer_size / _new_thread_count;
	if (_new_thread_count * task_buffer_size_per_thread != new_task_buffer_size)
	{
		task_buffer_size_per_thread++;
	}

	this->m_thread_count = _new_thread_count;

	unsigned int _new_pop_rounds = _pop_tries / static_cast<unsigned int>(new_thread_count);
	if ((static_cast<unsigned int>(new_thread_count) * _new_pop_rounds != _pop_tries)
		&& (static_cast<unsigned int>(new_thread_count) * (_new_pop_rounds + 1) >= _pop_tries))
	{
		_new_pop_rounds++;
	}

	this->m_pop_rounds = _new_pop_rounds;

	unsigned int _new_push_rounds = _push_tries / static_cast<unsigned int>(new_thread_count);
	if ((static_cast<unsigned int>(new_thread_count) * _new_push_rounds != _push_tries)
		&& (static_cast<unsigned int>(new_thread_count) * (_new_push_rounds + 1) >= _push_tries))
	{
		_new_push_rounds++;
	}

	this->m_push_rounds = _new_push_rounds;
	
	std::size_t threads_constructed = 0;
	std::size_t threads_launched = 0;


	this->m_thread_blocks_unaligned_data_ptr = static_cast<char*>(::operator new(
		_new_thread_count * sizeof(_cool_thmq_tblk) + 2 * cache_line_size, std::nothrow));

	if (this->m_thread_blocks_unaligned_data_ptr == nullptr)
	{
		this->delete_threads_detail(threads_constructed, threads_launched);
		std::atomic_signal_fence(std::memory_order_release);
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
			std::atomic_signal_fence(std::memory_order_release);
			return cool::threads_init_result(cool::threads_init_result::bad_alloc);
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

		xCOOL_THREADS_TRY
		{
			if (_new_pop_rounds > 0)
			{
				(this->m_thread_blocks_data_ptr + thread_num)->m_thread = std::thread([this, thread_num, _new_thread_count, _new_pop_rounds]()
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
										_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + k;

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

									for (std::size_t k = 0; k < thread_num; k++)
									{
										_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + k;

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
									_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + thread_num;

									std::unique_lock<std::mutex> lock(ptr->m_mutex);

									ptr->m_condition_var.wait(lock, [ptr]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

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
						xCOOL_THREADS_CATCH(...) {}
					}
				});
			}
			else
			{
				(this->m_thread_blocks_data_ptr + thread_num)->m_thread = std::thread([this, thread_num]()
				{
					while (true)
					{
						xCOOL_THREADS_TRY
						{
							_cool_thmq_task current_task;

							{
								_cool_thmq_tblk* ptr = this->m_thread_blocks_data_ptr + thread_num;

								std::unique_lock<std::mutex> lock(ptr->m_mutex);

								ptr->m_condition_var.wait(lock, [ptr]() -> bool { return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads; });

								if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
								{
									ptr->m_next_task_ptr->m_callable(&current_task, ptr->m_next_task_ptr);

									_cool_thmq_task* next_task_ptr_p1 = ptr->m_next_task_ptr + 1;

									ptr->m_next_task_ptr = (next_task_ptr_p1 != ptr->m_task_buffer_end_ptr) ?
										next_task_ptr_p1 : ptr->m_task_buffer_data_ptr;
								}
								else
								{
									return;
								}
							}

							current_task.m_callable(&current_task, nullptr);
						}
						xCOOL_THREADS_CATCH(...) {}
					}
				});
			}
		}
		xCOOL_THREADS_CATCH(...)
		{
			this->delete_threads_detail(threads_constructed, threads_launched);
			std::atomic_signal_fence(std::memory_order_release);
			return cool::threads_init_result(cool::threads_init_result::bad_thread);
		}

		threads_launched++;
	}

	std::atomic_signal_fence(std::memory_order_release);
	this->m_good.store(true, std::memory_order_seq_cst);
	return cool::threads_init_result(cool::threads_init_result::success);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::good() const noexcept
{
	return this->m_good.load(std::memory_order_seq_cst);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::thread_count() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return static_cast<std::uint16_t>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline std::size_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::task_buffer_size() const noexcept
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

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline unsigned int cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::pop_tries() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return this->m_pop_rounds * static_cast<unsigned int>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline unsigned int cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::push_tries() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return this->m_push_rounds * static_cast<unsigned int>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::dispatch_interval() const noexcept
{
	std::atomic_signal_fence(std::memory_order_acquire);
	return static_cast<std::uint16_t>(this->m_dispatch_interval);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align, bool _static_arg_type_check>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align, _static_arg_type_check>::delete_threads() noexcept
{
	this->m_good.store(false, std::memory_order_seq_cst);
	this->delete_threads_detail(this->m_thread_count, this->m_thread_count);
	std::atomic_signal_fence(std::memory_order_release);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept
{
	if (this->m_thread_blocks_data_ptr != nullptr)
	{
		for (size_t k = 0; k < this->m_thread_count; k++)
		{
			_thread_block* ptr = this->m_thread_blocks_data_ptr + k;

			if (k < threads_launched)
			{
				while (true)
				{
					xCOOL_THREADS_TRY
					{
						std::lock_guard<std::mutex> lock(ptr->m_mutex);
						ptr->m_stop_threads = true;
						break;
					}
					xCOOL_THREADS_CATCH(...) {}
				}

				ptr->m_condition_var.notify_one();

				if (ptr->m_thread.joinable())
				{
					xCOOL_THREADS_TRY
					{
						ptr->m_thread.join();
					}
					xCOOL_THREADS_CATCH(const std::system_error& xCOOL_THREADS_SYSTEM_ERROR)
					{
						cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_delete_thread_exception(xCOOL_THREADS_SYSTEM_ERROR, this);
					}
				}
			}

			if (k < threads_constructed)
			{
				ptr->delete_task_buffer();
				ptr->~_thread_block();
			}
		}

		::operator delete(this->m_thread_blocks_unaligned_data_ptr);
	}

	this->m_thread_blocks_data_ptr = nullptr;
	this->m_thread_count = 0;
	this->m_pop_rounds = 0;
	this->m_push_rounds = 0;

	constexpr std::size_t uintX_bitcount = sizeof(_uintX) * CHAR_BIT;

	this->m_mod_D = 1;
	this->m_mod_a = static_cast<_uint2X>(1) << uintX_bitcount;
	this->m_mod_k = 0;
	this->m_dispatch_interval = 1;
	this->m_thread_blocks_unaligned_data_ptr = nullptr;
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
	case threads_init_result::bad_thread: return "cool threads init failed : bad thread creation"; break;
	default: return "cool threads init result undefined"; break;
	}
}

inline cool::threads_init_result::threads_init_result(int result) noexcept : m_result(result) {}

inline void cool::threads_exception_handler::set_on_exception(void(*on_exception)(const std::exception&, void(*)(void), void*), void* exception_arg_ptr) noexcept
{
	cool::_threads_base::exception_handler handler{ on_exception, exception_arg_ptr };
	cool::_threads_base::get_exception_handler().store(handler, std::memory_order_seq_cst);
}

inline void cool::threads_exception_handler::set_on_delete_thread_exception(void(*on_delete_thread_exception)(const std::system_error&, void*, void*), void* delete_thread_exception_arg_ptr) noexcept
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
inline cool::_async_result_incr_proxy<return_Ty> cool::_async_result_to_proxy<return_Ty>::try_incr_awaited() noexcept
{
	return cool::_async_result_incr_proxy<return_Ty>(m_parent_ptr, m_offset);
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