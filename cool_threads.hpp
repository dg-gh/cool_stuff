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


namespace cool
{
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class threads_sq;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class threads_mq;
	class threads_exception_handler;

#ifndef xCOOL_NOT_TARGET_ENUM
#define xCOOL_NOT_TARGET_ENUM
	enum no_target_t { no_target };
#endif // xCOOL_NOT_TARGET_ENUM

	class async_task_end;
	template <class return_Ty> class async_task_result;

	class _threads_base;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_sq_data;
	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> class _threads_mq_data;
	class _async_task_end_incr_proxy;
	template <class Ty> class _async_task_result_at_proxy;
	template <class Ty> class _async_task_result_incr_proxy;

	// threads_sq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t)>
	class threads_sq : private cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_sq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_sq() = default;
		threads_sq(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		threads_sq(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		inline ~threads_sq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'args_Ty ... args' must be movable or copyable without throwing exceptions

		// if 'function_Ty task' throws an exception, the thread will call the exception handler and jump to the next task
		// all methods of threads_mq deal with potential exceptions internally and will not pass exceptions to the caller, even if not marked 'noexcept'

		// WARNING : queuing tasks with 'async' / 'priority_async' / 'try_async' / 'try_priority_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void priority_async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void priority_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void priority_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_priority_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		inline bool init_new_threads(std::uint16_t new_thread_count, std::size_t new_task_buffer_size);
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline void delete_threads();
	};

	// threads_mq

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align = alignof(std::max_align_t)>
	class threads_mq : private cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>
	{

	public:

		static constexpr std::size_t cache_line_size = alignof(cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);
		static constexpr std::size_t arg_buffer_size = _arg_buffer_size;
		static constexpr std::size_t arg_buffer_align = _arg_buffer_align;

		// 64 (bytes) is the most common value for _cache_line_size

		static_assert((_cache_line_size & (_cache_line_size - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : cache_line_size must be a power of 2");
		static_assert((_arg_buffer_align & (_arg_buffer_align - 1)) == 0,
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be a power of 2");
		static_assert(_arg_buffer_align >= alignof(std::max_align_t),
			"cool::threads_mq<cache_line_size, arg_buffer_size, arg_buffer_align> requirement : arg_buffer_align must be greater or equal to alignof(std::max_align_t)");

		threads_mq() = default;
		threads_mq(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		threads_mq(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		inline ~threads_mq();

		// 'function_Ty task' must be a function pointer
		// 'function_Ty task' must pass arguments by copy
		// 'args_Ty ... args' must be movable or copyable without throwing exceptions

		// if 'function_Ty task' throws an exception, the thread will call the exception handler and jump to the next task
		// all methods of threads_mq deal with potential exceptions internally and will not pass exceptions to the caller, even if not marked 'noexcept'

		// WARNING : queuing tasks with 'async' / 'try_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline void async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::no_target_t, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args);

		template <class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		template <class return_Ty, class function_Ty, class ... args_Ty>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args);

		inline bool init_new_threads(
			std::uint16_t new_thread_count,
			std::size_t new_task_buffer_size,
			unsigned int _pop_tries = 1,
			unsigned int _push_tries = 32,
			std::uint16_t dispatch_interval = 1
		);
		inline std::uint16_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline unsigned int pop_tries() const noexcept;
		inline unsigned int push_tries() const noexcept;
		inline std::uint16_t dispatch_interval() const noexcept;
		inline void delete_threads();
	};


	// threads_exception_handler

	class threads_exception_handler
	{

	public:

		// 'on_exception' must not throw
		// 'on_exception' arguments are : exception thrown, task function pointer that produced the exception, optional pointer to buffer 'exception_arg_ptr'
		// potential reads and writes to 'exception_arg_ptr' by 'on_exception' must be synchronized properly

		static inline void set(void(*on_exception)(const std::exception&, void(*)(void), void*), void* exception_arg_ptr = nullptr) noexcept;
		static inline void clear() noexcept;
	};


	// async_task_end

	// WARNNING : async_task_end object must outlive the threads of threads_sq/threads_mq object that calls
	// try_async or try_priority_async with it as a target argument

	class async_task_end
	{

	public:

		async_task_end() = default;
		async_task_end(const cool::async_task_end& rhs) = delete;
		cool::async_task_end& operator=(const cool::async_task_end& rhs) = delete;
		async_task_end(cool::async_task_end&& rhs) = delete;
		cool::async_task_end& operator=(cool::async_task_end&& rhs) = delete;
		~async_task_end() = default;

		inline void add_awaited(std::size_t number_of_tasks) noexcept;
		inline bool sub_awaited(std::size_t number_of_tasks); // returns true iff notified
		inline bool set_awaited(std::ptrdiff_t number_of_tasks); // returns true iff notified
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline bool decr_awaited(); // returns true iff notified
		inline void notify();
		inline void finish();
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

	// WARNNING: async_task_result object must outlive the threads of threads_sq/threads_mq object that calls
	// try_async or try_priority_async with it as a target argument

	template <class return_Ty> class async_task_result
	{

	public:

		using value_type = return_Ty;
		using pointer = return_Ty*;
		using const_pointer = const return_Ty*;
		using reference = return_Ty&;
		using const_reference = const return_Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		async_task_result() = default;
		async_task_result(const cool::async_task_result<return_Ty>& rhs) = delete;
		cool::async_task_result<return_Ty>& operator=(const cool::async_task_result<return_Ty>& rhs) = delete;
		async_task_result(cool::async_task_result<return_Ty>&& rhs) = delete;
		cool::async_task_result<return_Ty>& operator=(cool::async_task_result<return_Ty>&& rhs) = delete;
		~async_task_result() = default;

		explicit inline async_task_result(return_Ty* storage_ptr);
		explicit inline async_task_result(return_Ty* storage_ptr, const return_Ty& rhs);

		inline void add_awaited(std::size_t number_of_tasks) noexcept;
		inline bool sub_awaited(std::size_t number_of_tasks); // returns true iff notified
		inline bool set_awaited(std::ptrdiff_t number_of_tasks); // returns true iff notified
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::_async_task_result_at_proxy<return_Ty> to(std::size_t offset) noexcept;
		inline bool decr_awaited(); // returns true iff notified
		inline void notify();
		inline void finish();
		inline bool finished() const noexcept;

		inline return_Ty& get(std::size_t offset);
		inline return_Ty& get_unchecked(std::size_t offset) noexcept;
		inline const return_Ty& get_unchecked(std::size_t offset) const noexcept;
		inline void reset(std::size_t count);
		inline void reset(std::size_t count, const return_Ty& rhs);
		inline void reset_unchecked(std::size_t count);
		inline void reset_unchecked(std::size_t count, const return_Ty& rhs);

		inline return_Ty* data() noexcept;
		inline const return_Ty* data() const noexcept;

		inline void set_data(return_Ty* storage_ptr);
		inline void set_data_unchecked(return_Ty* storage_ptr) noexcept;
		inline void clear();
		inline void clear_unchecked() noexcept;

	private:

		return_Ty* m_stored_values_ptr = nullptr;
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::_async_task_result_at_proxy;
	};

	template <class return_Ty> class _async_task_result_at_proxy
	{

	public:

		_async_task_result_at_proxy() = delete;
		_async_task_result_at_proxy(const cool::_async_task_result_at_proxy<return_Ty>&) = delete;
		cool::_async_task_result_at_proxy<return_Ty>& operator=(const cool::_async_task_result_at_proxy<return_Ty>&) = delete;
		_async_task_result_at_proxy(cool::_async_task_result_at_proxy<return_Ty>&&) noexcept = default;
		cool::_async_task_result_at_proxy<return_Ty>& operator=(cool::_async_task_result_at_proxy<return_Ty>&&) noexcept = default;
		~_async_task_result_at_proxy() = default;

		inline cool::_async_task_result_incr_proxy<return_Ty> try_incr_awaited() noexcept;

	private:

		inline _async_task_result_at_proxy(cool::async_task_result<return_Ty>* target_ptr, std::size_t offset) noexcept : m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::async_task_result;
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
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::_threads_mq_data;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;

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
	};

	// _threads_sq_data

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
	class alignas(_cache_line_size) _threads_sq_data : public cool::_threads_base
	{

	private:

		friend class cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;

		using _task = typename cool::_threads_base::_base_task<_arg_buffer_size, _arg_buffer_align>;

		_threads_sq_data() = default;
		_threads_sq_data(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(const cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&) = delete;
		_threads_sq_data(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>& operator=(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>&&) = delete;
		~_threads_sq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed);

		_task* m_task_buffer_data_ptr = nullptr;
		_task* m_task_buffer_end_ptr = nullptr;

		_task* m_last_task_ptr = nullptr;
		_task* m_next_task_ptr = nullptr;

		std::thread* m_threads_data_ptr = nullptr;
		std::size_t m_thread_count = 0;

		std::condition_variable m_condition_var;
		std::mutex m_mutex;

		bool m_stop_threads = true;

		char* m_task_buffer_unaligned_data_ptr = nullptr;
	};

	// _threads_mq_data

	template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
	class alignas(_cache_line_size) _threads_mq_data : public cool::_threads_base
	{

	private:

		friend class cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>;

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

		inline void delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched);

		class _thread_block;

		_thread_block* m_thread_blocks_data_ptr = nullptr;
		std::size_t m_thread_count = 0;
		unsigned int m_pop_rounds = 0;
		unsigned int m_push_rounds = 0;
		
		_uint2X m_mod_D = 1;
		_uint2X m_mod_a = static_cast<_uint2X>(1) << (sizeof(_uintX) * CHAR_BIT);
		_uintX m_mod_k = 0;
		_uintX m_dispatch_interval = 1;
		char* m_thread_blocks_unaligned_data_ptr = nullptr;

		alignas(_cache_line_size) std::atomic<_uintX> m_thread_dispatch{ 0 };


		class alignas(_cache_line_size) _thread_block
		{

		public:

			inline bool new_task_buffer(std::size_t new_task_buffer_size);
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

	class _async_task_end_incr_proxy
	{

	public:

		_async_task_end_incr_proxy() = delete;
		_async_task_end_incr_proxy(const cool::_async_task_end_incr_proxy&) = delete;
		cool::_async_task_end_incr_proxy& operator=(const cool::_async_task_end_incr_proxy&) = delete;
		_async_task_end_incr_proxy(cool::_async_task_end_incr_proxy&&) noexcept = default;
		cool::_async_task_end_incr_proxy& operator=(cool::_async_task_end_incr_proxy&&) noexcept = default;
		~_async_task_end_incr_proxy() = default;

	private:

		_async_task_end_incr_proxy(cool::async_task_end* target_ptr) noexcept : m_parent_ptr(target_ptr) {}

		cool::async_task_end* m_parent_ptr;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		friend class cool::async_task_end;
	};

	template <class return_Ty> class _async_task_result_incr_proxy
	{

	public:

		_async_task_result_incr_proxy() = delete;
		_async_task_result_incr_proxy(const cool::_async_task_result_incr_proxy<return_Ty>&) = delete;
		cool::_async_task_result_incr_proxy<return_Ty>& operator=(const cool::_async_task_result_incr_proxy<return_Ty>&) = delete;
		_async_task_result_incr_proxy(cool::_async_task_result_incr_proxy<return_Ty>&&) noexcept = default;
		cool::_async_task_result_incr_proxy<return_Ty>& operator=(cool::_async_task_result_incr_proxy<return_Ty>&&) noexcept = default;
		~_async_task_result_incr_proxy() = default;

	private:

		inline _async_task_result_incr_proxy(cool::async_task_result<return_Ty>* target_ptr, std::size_t offset) noexcept : m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_sq;
		template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> friend class cool::threads_mq;
		template <class return_Ty2> friend class cool::_async_task_result_at_proxy;
	};
}


// threads_sq detail

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::~threads_sq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::priority_async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	while (true)
	{
		try
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

						try
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
						}

						static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	while (true)
	{
		try
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

						cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

						try
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::priority_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	while (true)
	{
		try
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

						cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

						try
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	while (true)
	{
		try
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

						cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

						try
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::priority_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	while (true)
	{
		try
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

						cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

						try
						{
							*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
								function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
							);
						}
						catch (std::exception excep)
						{
							cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
		catch (...) {}
	}

	this->m_condition_var.notify_one();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	try
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

					try
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
					}

					static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	try
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

					cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

					try
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	try
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

					cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

					try
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_async : task arguments alignment too large");

	try
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

					cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

					try
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_priority_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thsq_pack = decltype(std::make_tuple(std::move(args)...));

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_priority_async : task must be a function pointer");
	static_assert(sizeof(_cool_thsq_pack) <= _arg_buffer_size, "cool::threads_sq<...>::try_priority_async : task arguments too large");
	static_assert(alignof(_cool_thsq_pack) <= alignof(_cool_thsq_task), "cool::threads_sq<...>::try_priority_async : task arguments alignment too large");

	try
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

					cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

					try
					{
						*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
							function_ptr, std::move(*reinterpret_cast<_cool_thsq_pack*>(_task_ptr->m_arg_buffer))
						);
					}
					catch (std::exception excep)
					{
						cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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
	catch (...)
	{
		return false;
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
					try
					{
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
					catch (...) {}
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
inline void cool::threads_sq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads()
{
	this->delete_threads_detail(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed)
{
	constexpr std::size_t cache_line_size = alignof(cool::_threads_sq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>);

	while (true)
	{
		try
		{
			std::unique_lock<std::mutex> lock(this->m_mutex);
			this->m_stop_threads = true;
			break;
		}
		catch (...) {}
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

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::~threads_mq()
{
	delete_threads();
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;
			
			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return;
			}
		}
	}
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::no_target_t, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

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
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::async_task_end& target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

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
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_end_incr_proxy target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								try
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call_no_return(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
								}

								static_cast<cool::async_task_end*>(_task_ptr->m_target_ptr)->decr_awaited();

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
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_at_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}
	}

	return false;
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align> template <class return_Ty, class function_Ty, class ... args_Ty>
inline bool cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::try_async(cool::_async_task_result_incr_proxy<return_Ty> target, function_Ty task, args_Ty ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;
	using _cool_thmq_pack = decltype(std::make_tuple(std::move(args)...));

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	static_assert(std::is_pointer<function_Ty>::value && std::is_function<typename std::remove_pointer<function_Ty>::type>::value,
		"cool::threads_sq<...>::try_async : task must be a function pointer");
	static_assert(sizeof(_cool_thmq_pack) <= _arg_buffer_size, "cool::threads_mq<...>::try_async : task arguments too large");
	static_assert(alignof(_cool_thmq_pack) <= alignof(_cool_thmq_task), "cool::threads_mq<...>::try_async : task arguments alignment too large");

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

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

			if (success)
			{
				current_thread_ptr->m_condition_var.notify_one();
				return true;
			}
		}

		for (std::size_t k = this->m_thread_count - 1; k > first_thread; k--)
		{
			_cool_thmq_tblk* current_thread_ptr = this->m_thread_blocks_data_ptr + k;

			bool success = false;

			try
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

								cool::async_task_result<return_Ty>& target_ref = *static_cast<cool::async_task_result<return_Ty>*>(_task_ptr->m_target_ptr);

								try
								{
									*(static_cast<return_Ty*>(target_ref.m_stored_values_ptr) + _task_ptr->m_offset) = cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::call(
										function_ptr, std::move(*reinterpret_cast<_cool_thmq_pack*>(_task_ptr->m_arg_buffer))
									);
								}
								catch (std::exception excep)
								{
									cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::catch_exception(excep, _task_ptr->m_function_ptr);
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

						current_thread_ptr->m_last_task_ptr = (last_task_ptr_p1 != current_thread_ptr->m_task_buffer_end_ptr) ?
							last_task_ptr_p1 : current_thread_ptr->m_task_buffer_data_ptr;

						success = true;
					}
				}
			}
			catch (...) {}

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
	unsigned int _pop_tries,
	unsigned int _push_tries,
	std::uint16_t dispatch_interval)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_thread_block;

	using _cool_thmq_uintX = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uintX;
	using _cool_thmq_uint2X = typename cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::_uint2X;

	delete_threads();

	if ((new_thread_count == 0) || (new_task_buffer_size == 0))
	{
		return false;
	}

	std::size_t _new_thread_count = static_cast<std::size_t>(new_thread_count);
	std::size_t task_buffer_size_per_thread = new_task_buffer_size / _new_thread_count;
	if (_new_thread_count * task_buffer_size_per_thread != new_task_buffer_size)
	{
		task_buffer_size_per_thread++;
	}

	this->m_thread_count = _new_thread_count;

	unsigned int _new_pop_rounds = _pop_tries / static_cast<unsigned int>(new_thread_count);
	if ((_new_pop_rounds == 0) || (static_cast<unsigned int>(new_thread_count) * _new_pop_rounds != _pop_tries
		&& (static_cast<unsigned int>(new_thread_count) * (_new_pop_rounds + 1) >= _pop_tries)))
	{
		_new_pop_rounds++;
	}

	this->m_pop_rounds = _new_pop_rounds;

	unsigned int _new_push_rounds = _push_tries / static_cast<unsigned int>(new_thread_count);
	if ((_new_push_rounds == 0) || ((static_cast<unsigned int>(new_thread_count) * _new_push_rounds != _push_tries)
		&& (static_cast<unsigned int>(new_thread_count) * (_new_push_rounds + 1) >= _push_tries)))
	{
		_new_push_rounds++;
	}

	this->m_push_rounds = _new_push_rounds;
	
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

			(this->m_thread_blocks_data_ptr + thread_num)->m_thread = std::thread([this, thread_num, _new_thread_count, _new_pop_rounds]()
			{
				while (true)
				{
					try
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
										current_task = std::move(*(ptr->m_next_task_ptr));

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
					catch (...) {}
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
inline unsigned int cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::pop_tries() const noexcept
{
	return this->m_pop_rounds * static_cast<unsigned int>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline unsigned int cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::push_tries() const noexcept
{
	return this->m_push_rounds * static_cast<unsigned int>(this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline std::uint16_t cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::dispatch_interval() const noexcept
{
	return static_cast<std::uint16_t>(this->m_dispatch_interval);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::threads_mq<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads()
{
	this->delete_threads_detail(this->m_thread_count, this->m_thread_count);
}

template <std::size_t _cache_line_size, std::size_t _arg_buffer_size, std::size_t _arg_buffer_align>
inline void cool::_threads_mq_data<_cache_line_size, _arg_buffer_size, _arg_buffer_align>::delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched)
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
					try
					{
						std::lock_guard<std::mutex> lock(ptr->m_mutex);
						ptr->m_stop_threads = true;
						break;
					}
					catch (...) {}
				}

				ptr->m_condition_var.notify_one();

				if (ptr->m_thread.joinable())
				{
					ptr->m_thread.join();
				}
			}

			if (k < threads_constructed)
			{
				ptr->~_thread_block();
			}

			ptr->delete_task_buffer();
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

inline void cool::threads_exception_handler::set(void(*on_exception)(const std::exception&, void(*)(void), void*), void* exception_arg_ptr) noexcept
{
	cool::_threads_base::exception_handler handler{ on_exception, exception_arg_ptr };
	cool::_threads_base::get_exception_handler().store(handler, std::memory_order_seq_cst);
}

inline void cool::threads_exception_handler::clear() noexcept
{
	cool::_threads_base::exception_handler handler{ nullptr, nullptr };
	cool::_threads_base::get_exception_handler().store(handler, std::memory_order_seq_cst);
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
			(m_task_buffer_data_ptr + k)->~_task();
		}

		::operator delete(m_task_buffer_unaligned_data_ptr);
	}
}


// async_task_end detail

inline void cool::async_task_end::add_awaited(std::size_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(static_cast<std::ptrdiff_t>(number_of_tasks), std::memory_order_seq_cst);
}

inline bool cool::async_task_end::sub_awaited(std::size_t number_of_tasks)
{
	std::ptrdiff_t _number_of_tasks = static_cast<std::ptrdiff_t>(number_of_tasks);
	std::ptrdiff_t prev = m_tasks_awaited.fetch_sub(_number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (prev - _number_of_tasks <= 0))
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

inline bool cool::async_task_end::set_awaited(std::ptrdiff_t number_of_tasks)
{
	std::ptrdiff_t prev = m_tasks_awaited.exchange(number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (number_of_tasks <= 0))
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

inline std::ptrdiff_t cool::async_task_end::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

inline bool cool::async_task_end::decr_awaited()
{
	if (m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

inline void cool::async_task_end::notify()
{
	while (true)
	{
		try
		{
			std::lock_guard<std::mutex> lock(m_finish_mutex);
			m_finish_condition_var.notify_all();
			return;
		}
		catch (...) {}
	}
}

inline void cool::async_task_end::finish()
{
	while (m_tasks_awaited.load(std::memory_order_seq_cst) > 0)
	{
		try
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
		catch (...) {}
	}
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

template <class return_Ty>
inline cool::async_task_result<return_Ty>::async_task_result(return_Ty* storage_ptr)
{
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline cool::async_task_result<return_Ty>::async_task_result(return_Ty* storage_ptr, const return_Ty& rhs)
{
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::add_awaited(std::size_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
}

template <class return_Ty>
inline bool cool::async_task_result<return_Ty>::sub_awaited(std::size_t number_of_tasks)
{
	std::ptrdiff_t _number_of_tasks = static_cast<std::ptrdiff_t>(number_of_tasks);
	std::ptrdiff_t prev = m_tasks_awaited.fetch_sub(_number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (prev - _number_of_tasks <= 0))
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline bool cool::async_task_result<return_Ty>::set_awaited(std::ptrdiff_t number_of_tasks)
{
	std::ptrdiff_t prev = m_tasks_awaited.exchange(number_of_tasks, std::memory_order_seq_cst);

	if ((prev > 0) && (number_of_tasks <= 0))
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline std::ptrdiff_t cool::async_task_result<return_Ty>::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

template <class return_Ty>
inline cool::_async_task_result_at_proxy<return_Ty> cool::async_task_result<return_Ty>::to(std::size_t offset) noexcept
{
	return cool::_async_task_result_at_proxy<return_Ty>(this, offset);
}

template <class return_Ty>
inline bool cool::async_task_result<return_Ty>::decr_awaited()
{
	if (m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
	{
		while (true)
		{
			try
			{
				std::lock_guard<std::mutex> lock(m_finish_mutex);
				m_finish_condition_var.notify_all();
				return true;
			}
			catch (...) {}
		}
	}
	else
	{
		return false;
	}
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::notify()
{
	while (true)
	{
		try
		{
			std::lock_guard<std::mutex> lock(m_finish_mutex);
			m_finish_condition_var.notify_all();
			return;
		}
		catch (...) {}
	}
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::finish()
{
	while (m_tasks_awaited.load(std::memory_order_seq_cst) > 0)
	{
		try
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
		catch (...) {}
	}
}

template <class return_Ty>
inline bool cool::async_task_result<return_Ty>::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

template <class return_Ty>
inline return_Ty& cool::async_task_result<return_Ty>::get(std::size_t offset)
{
	finish();
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline return_Ty& cool::async_task_result<return_Ty>::get_unchecked(std::size_t offset) noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline const return_Ty& cool::async_task_result<return_Ty>::get_unchecked(std::size_t offset) const noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::reset(std::size_t count)
{
	finish();

	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty();
	}
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::reset(std::size_t count, const return_Ty& rhs)
{
	finish();

	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = rhs;
	}
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::reset_unchecked(std::size_t count)
{
	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty();
	}
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::reset_unchecked(std::size_t count, const return_Ty& rhs)
{
	for (std::size_t k = 0; k < count; k++)
	{
		*(m_stored_values_ptr + k) = rhs;
	}
}

template <class return_Ty>
inline return_Ty* cool::async_task_result<return_Ty>::data() noexcept
{
	return m_stored_values_ptr;
}

template <class return_Ty>
inline const return_Ty* cool::async_task_result<return_Ty>::data() const noexcept
{
	return m_stored_values_ptr;
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::set_data(return_Ty* storage_ptr)
{
	finish();
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::set_data_unchecked(return_Ty* storage_ptr) noexcept
{
	m_stored_values_ptr = storage_ptr;
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::clear()
{
	finish();
	m_stored_values_ptr = nullptr;
}

template <class return_Ty>
inline void cool::async_task_result<return_Ty>::clear_unchecked() noexcept
{
	m_stored_values_ptr = nullptr;
}

template <class return_Ty>
inline cool::_async_task_result_incr_proxy<return_Ty> cool::_async_task_result_at_proxy<return_Ty>::try_incr_awaited() noexcept
{
	return cool::_async_task_result_incr_proxy<return_Ty>(m_parent_ptr, m_offset);
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