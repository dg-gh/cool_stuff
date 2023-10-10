// cool_threads.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef _COOL_THREADS_HPP
#define _COOL_THREADS_HPP

#include <cstddef>
#include <cstdint>
#include <new>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>


namespace cool
{
	// 64 (bytes) is the most usual value for cache_line_size
	template <std::size_t cache_line_size> class threads_sq;
	template <std::size_t cache_line_size> class threads_mq;

	enum no_target_t { no_target };
	class async_task_end;
	template <class Ty, class _allocator = std::allocator<Ty>> class async_task_result;

	template <std::size_t cache_line_size> class _threads_sq_data;
	template <std::size_t cache_line_size> class _threads_mq_data;
	class _async_task_end_incr_proxy;
	template <class Ty, class _allocator> class _async_task_result_proxy;
	template <class Ty, class _allocator> class _async_task_result_incr_proxy;

	// threads_sq

	template <std::size_t cache_line_size> class threads_sq
		: private cool::_threads_sq_data<cache_line_size>
	{

	public:

		threads_sq() = default;
		threads_sq(const cool::threads_sq<cache_line_size>&) = delete;
		cool::threads_sq<cache_line_size>& operator=(const cool::threads_sq<cache_line_size>&) = delete;
		threads_sq(cool::threads_sq<cache_line_size>&&) = delete;
		cool::threads_sq<cache_line_size>& operator=(cool::threads_sq<cache_line_size>&&) = delete;
		virtual ~threads_sq() { delete_threads(); }

		// WARNING: in 'try_async' and 'try_priority_async' methods, 'function_Ty task' must have a persistent lifetime
		// and must not be relocated elsewhere until its execution is completed

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::no_target_t, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_priority_async(cool::no_target_t, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_priority_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_priority_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_priority_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_priority_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		inline bool init_new_threads(std::size_t new_thread_count, std::size_t new_task_buffer_size);
		inline std::size_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline void delete_threads() noexcept;
	};

	// threads_mq

	template <std::size_t cache_line_size> class threads_mq
		: private cool::_threads_mq_data<cache_line_size>
	{

	public:

		threads_mq() = default;
		threads_mq(const cool::threads_mq<cache_line_size>&) = delete;
		cool::threads_mq<cache_line_size>& operator=(const cool::threads_mq<cache_line_size>&) = delete;
		threads_mq(cool::threads_mq<cache_line_size>&&) = delete;
		cool::threads_mq<cache_line_size>& operator=(cool::threads_mq<cache_line_size>&&) = delete;
		virtual ~threads_mq() { delete_threads(); }

		// WARNING: in 'try_async' method, 'function_Ty task' must have a persistent lifetime
		// and must not be relocated elsewhere until its execution is completed

		// WARNING: queuing tasks with 'try_async' does not check wether threads have been initialized beforehand

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::no_target_t, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args);

		template <class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
		inline bool try_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args);

		inline bool init_new_threads(std::size_t new_thread_count, std::size_t new_task_buffer_size,
			unsigned int push_rounds = 1, unsigned int pop_rounds = 1,
			unsigned int dispatch_interval = 1);
		inline std::size_t thread_count() const noexcept;
		inline std::size_t task_buffer_size() const noexcept;
		inline unsigned int push_rounds() const noexcept;
		inline unsigned int pop_rounds() const noexcept;
		inline unsigned int dispatch_interval() const noexcept;
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
		~async_task_end();

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

		template <std::size_t cache_line_size> friend class cool::threads_sq;
		template <std::size_t cache_line_size> friend class cool::threads_mq;
	};

	// async_task_result

	template <class return_Ty, class _allocator> class async_task_result : private _allocator
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
		async_task_result(const cool::async_task_result<return_Ty, _allocator>& rhs) = delete;
		cool::async_task_result<return_Ty, _allocator>& operator=(const cool::async_task_result<return_Ty, _allocator>& rhs) = delete;
		async_task_result(cool::async_task_result<return_Ty, _allocator>&& rhs) = delete;
		cool::async_task_result<return_Ty, _allocator>& operator=(cool::async_task_result<return_Ty, _allocator>&& rhs) = delete;
		~async_task_result();

		template <class ... _Args> explicit async_task_result(std::size_t new_size, _Args&& ... args);

		inline cool::async_task_result<return_Ty, _allocator>& add_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, _allocator>& sub_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline cool::async_task_result<return_Ty, _allocator>& set_awaited(std::ptrdiff_t number_of_tasks) noexcept;
		inline std::ptrdiff_t get_awaited() const noexcept;
		inline cool::_async_task_result_proxy<return_Ty, _allocator> at(std::size_t offset) noexcept;
		inline cool::async_task_result<return_Ty, _allocator>& finish() noexcept;
		inline bool finished() const noexcept;
		inline return_Ty& get(std::size_t offset) noexcept;
		inline return_Ty& get_unchecked(std::size_t offset) noexcept;
		inline const return_Ty& get_unchecked(std::size_t offset) const noexcept;
		template <class ... _Args>
		inline cool::async_task_result<return_Ty, _allocator>& reset(_Args&& ... args);
		template <class ... _Args>
		inline cool::async_task_result<return_Ty, _allocator>& reset_unchecked(_Args&& ... args);

		inline void resize(std::size_t new_size);
		inline std::size_t size() const noexcept;
		inline void clear() noexcept;

	private:

		return_Ty* m_stored_values_ptr = nullptr;
		std::size_t m_size = 0;
		std::condition_variable m_finish_condition_var;
		std::mutex m_finish_mutex;
		std::atomic<std::ptrdiff_t> m_tasks_awaited{ 0 };

		template <std::size_t cache_line_size> friend class cool::threads_sq;
		template <std::size_t cache_line_size> friend class cool::threads_mq;
	};

	// _async_task_result_proxy

	template <class return_Ty, class _allocator> class _async_task_result_proxy
	{

	public:

		_async_task_result_proxy() = delete;
		_async_task_result_proxy(const cool::_async_task_result_proxy<return_Ty, _allocator>&) = delete;
		cool::_async_task_result_proxy<return_Ty, _allocator>& operator=(const cool::_async_task_result_proxy<return_Ty, _allocator>&) = delete;
		_async_task_result_proxy(cool::_async_task_result_proxy<return_Ty, _allocator>&&) noexcept = default;
		cool::_async_task_result_proxy<return_Ty, _allocator>& operator=(cool::_async_task_result_proxy<return_Ty, _allocator>&&) noexcept = default;
		~_async_task_result_proxy() {}

		inline cool::_async_task_result_incr_proxy<return_Ty, _allocator> try_incr_awaited() noexcept;

	private:

		_async_task_result_proxy(cool::async_task_result<return_Ty, _allocator>* target_ptr, std::size_t offset) noexcept
			: m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty, _allocator>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t cache_line_size> friend class cool::threads_sq;
		template <std::size_t cache_line_size> friend class cool::threads_mq;
		template <class return_Ty2, class _allocator2> friend class cool::async_task_result;
	};
}


namespace cool
{
	// _threads_sq_data detail

	template <std::size_t cache_line_size> class alignas(cache_line_size) _threads_sq_data
	{

	private:

		friend class cool::threads_sq<cache_line_size>;

		_threads_sq_data() = default;
		_threads_sq_data(const cool::_threads_sq_data<cache_line_size>&) = delete;
		cool::_threads_sq_data<cache_line_size>& operator=(const cool::_threads_sq_data<cache_line_size>&) = delete;
		_threads_sq_data(cool::_threads_sq_data<cache_line_size>&&) = delete;
		cool::_threads_sq_data<cache_line_size>& operator=(cool::_threads_sq_data<cache_line_size>&&) = delete;
		virtual ~_threads_sq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed) noexcept;

		class _task;
		using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

		_cool_thsq_task* m_task_buffer_data_ptr = nullptr;
		_cool_thsq_task* m_task_buffer_end_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;

		_cool_thsq_task* m_last_task_ptr = nullptr;
		_cool_thsq_task* m_next_task_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;

		std::thread* m_threads_data_ptr = nullptr;
		std::size_t m_thread_count = 0;

		std::condition_variable m_condition_var;
		std::mutex m_mutex;

		bool m_stop_threads = true;

		class _task
		{

		public:

			std::function<void(_cool_thsq_task*)> m_callable;
			const void* m_function_ptr;
			void* m_target_ptr;
			std::size_t m_offset;
		};
	};

	// _threads_mq_data detail

	template <std::size_t cache_line_size> class alignas(cache_line_size) _threads_mq_data
	{

	private:

		friend class cool::threads_mq<cache_line_size>;

		_threads_mq_data() = default;
		_threads_mq_data(const cool::_threads_mq_data<cache_line_size>&) = delete;
		cool::_threads_mq_data<cache_line_size>& operator=(const cool::_threads_mq_data<cache_line_size>&) = delete;
		_threads_mq_data(cool::_threads_mq_data<cache_line_size>&&) = delete;
		cool::_threads_mq_data<cache_line_size>& operator=(cool::_threads_mq_data<cache_line_size>&&) = delete;
		virtual ~_threads_mq_data() = default;

		inline void delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept;

		class _task;
		using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;

		class _thread_block;
		using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

		_cool_thmq_tblk* m_thread_blocks_data_ptr = nullptr;
		_cool_thmq_tblk* m_thread_blocks_unaligned_data_ptr = nullptr;
		std::size_t m_thread_count = 0;
		std::size_t m_push_rounds = 1;
		std::size_t m_pop_rounds = 1;

		unsigned int m_thread_count_uint = 0;
		unsigned int m_dispatch_interval = 1;

		alignas(cache_line_size) std::atomic<unsigned int> m_thread_dispatch{ 0 };

		class _task
		{

		public:

			std::function<void(_cool_thmq_task*)> m_callable;
			const void* m_function_ptr;
			void* m_target_ptr;
			std::size_t m_offset;
		};

		class alignas(cache_line_size) _thread_block
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
		~_async_task_end_incr_proxy() {}

	private:

		_async_task_end_incr_proxy(cool::async_task_end* target_ptr) noexcept
			: m_parent_ptr(target_ptr) {}

		cool::async_task_end* m_parent_ptr;

		template <std::size_t cache_line_size> friend class cool::threads_sq;
		template <std::size_t cache_line_size> friend class cool::threads_mq;
		friend class cool::async_task_end;
	};

	template <class return_Ty, class _allocator> class _async_task_result_incr_proxy
	{

	public:

		_async_task_result_incr_proxy() = delete;
		_async_task_result_incr_proxy(const cool::_async_task_result_incr_proxy<return_Ty, _allocator>&) = delete;
		cool::_async_task_result_incr_proxy<return_Ty, _allocator>& operator=(const cool::_async_task_result_incr_proxy<return_Ty, _allocator>&) = delete;
		_async_task_result_incr_proxy(cool::_async_task_result_incr_proxy<return_Ty, _allocator>&&) noexcept = default;
		cool::_async_task_result_incr_proxy<return_Ty, _allocator>& operator=(cool::_async_task_result_incr_proxy<return_Ty, _allocator>&&) noexcept = default;
		~_async_task_result_incr_proxy() {}

	private:

		_async_task_result_incr_proxy(cool::async_task_result<return_Ty, _allocator>* target_ptr, std::size_t offset) noexcept
			: m_parent_ptr(target_ptr), m_offset(offset) {}

		cool::async_task_result<return_Ty, _allocator>* m_parent_ptr;
		std::size_t m_offset;

		template <std::size_t cache_line_size> friend class cool::threads_sq;
		template <std::size_t cache_line_size> friend class cool::threads_mq;
		template <class return_Ty2, class _allocator2> friend class cool::_async_task_result_proxy;
	};
}


// threads_sq detail

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_async(cool::no_target_t, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
	};

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);

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

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_priority_async(cool::no_target_t, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
	};

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

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

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

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_priority_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

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

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

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

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_priority_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

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

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

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

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_priority_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

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

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	{
		std::lock_guard<std::mutex> lock(this->m_mutex);

		_cool_thsq_task* last_task_ptr_p1 = this->m_last_task_ptr + 1;

		if ((last_task_ptr_p1 != this->m_next_task_ptr)
			&& ((this->m_task_buffer_data_ptr != this->m_next_task_ptr)
				|| (last_task_ptr_p1 != this->m_task_buffer_end_ptr)))
		{
			target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;

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

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_sq<cache_line_size>::try_priority_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	std::function<void(_cool_thsq_task*)> callable = [args...](_cool_thsq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

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

			this->m_last_task_ptr->m_callable = std::move(callable);
			this->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
			this->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
			this->m_last_task_ptr->m_offset = target.m_offset;
		}
		else
		{
			return false;
		}
	}

	this->m_condition_var.notify_one();

	return true;
}


template <std::size_t cache_line_size>
inline bool cool::threads_sq<cache_line_size>::init_new_threads(std::size_t new_thread_count, std::size_t new_task_buffer_size)
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

	delete_threads();
	this->m_stop_threads = false;

	if ((new_thread_count == 0) || (new_task_buffer_size == 0))
	{
		return false;
	}

	new_task_buffer_size = (new_thread_count > new_task_buffer_size) ? new_thread_count : new_task_buffer_size;

	std::size_t threads_constructed = 0;

	try
	{
		this->m_threads_data_ptr = static_cast<std::thread*>(::operator new(
			new_thread_count * sizeof(std::thread) + 2 * cache_line_size, std::nothrow));

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

		this->m_thread_count = new_thread_count;

		this->m_task_buffer_data_ptr = static_cast<_cool_thsq_task*>(::operator new(
			(new_task_buffer_size + 1) * sizeof(_cool_thsq_task) + 2 * cache_line_size,
			std::nothrow));
		if (this->m_task_buffer_data_ptr == nullptr)
		{
			this->delete_threads_detail(threads_constructed);
			return false;
		}
		else
		{
			this->m_task_buffer_data_ptr = reinterpret_cast<_cool_thsq_task*>(
				reinterpret_cast<char*>(this->m_task_buffer_data_ptr) + cache_line_size);
		}

		std::size_t new_task_buffer_size_p1 = new_task_buffer_size + 1;
		this->m_task_buffer_end_ptr = this->m_task_buffer_data_ptr + new_task_buffer_size_p1;
		this->m_next_task_ptr = this->m_task_buffer_data_ptr;
		this->m_last_task_ptr = this->m_task_buffer_data_ptr;

		for (std::size_t k = 0; k < new_task_buffer_size_p1; k++)
		{
			new (this->m_task_buffer_data_ptr + k) _cool_thsq_task();
		}

		for (std::size_t k = 0; k < new_thread_count; k++)
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
								current_task = std::move(*this->m_next_task_ptr);

								_cool_thsq_task* next_task_ptr_p1 = this->m_next_task_ptr + 1;

								this->m_next_task_ptr = (next_task_ptr_p1 != this->m_task_buffer_end_ptr) ?
									next_task_ptr_p1 : this->m_task_buffer_data_ptr;
							}
							else
							{
								return;
							}
						}

						current_task.m_callable(&current_task);
					}
				}
			);

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

template <std::size_t cache_line_size>
inline std::size_t cool::threads_sq<cache_line_size>::thread_count() const noexcept
{
	return this->m_thread_count;
}

template <std::size_t cache_line_size>
inline std::size_t cool::threads_sq<cache_line_size>::task_buffer_size() const noexcept
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

template <std::size_t cache_line_size>
inline void cool::threads_sq<cache_line_size>::delete_threads() noexcept
{
	this->delete_threads_detail(this->m_thread_count);
}

template <std::size_t cache_line_size>
inline void cool::_threads_sq_data<cache_line_size>::delete_threads_detail(std::size_t threads_constructed) noexcept
{
	using _cool_thsq_task = typename cool::_threads_sq_data<cache_line_size>::_task;

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

		::operator delete(reinterpret_cast<char*>(this->m_task_buffer_data_ptr) - cache_line_size);
	}

	this->m_threads_data_ptr = nullptr;
	this->m_thread_count = 0;

	this->m_task_buffer_data_ptr = nullptr;
	this->m_task_buffer_end_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;
	this->m_next_task_ptr = nullptr;
	this->m_last_task_ptr = static_cast<_cool_thsq_task*>(nullptr) + 1;
}


// threads_mq detail

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_mq<cache_line_size>::try_async(cool::no_target_t, const function_Ty& task, _Args&& ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	std::function<void(_cool_thmq_task*)> callable = [args...](_cool_thmq_task* _task_ptr)
	{
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
	};

	std::size_t first_thread = static_cast<std::size_t>(this->m_thread_dispatch.fetch_add(
		this->m_dispatch_interval, std::memory_order_relaxed)
		% this->m_thread_count_uint);

	bool success = false;

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);

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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);

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

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_mq<cache_line_size>::try_async(cool::async_task_end& target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	std::function<void(_cool_thmq_task*)> callable = [args...](_cool_thmq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	std::size_t first_thread = static_cast<std::size_t>(this->m_thread_dispatch.fetch_add(
		this->m_dispatch_interval, std::memory_order_relaxed)
		% this->m_thread_count_uint);

	bool success = false;

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(&target);

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

template <std::size_t cache_line_size> template <class function_Ty, class ... _Args>
inline bool cool::threads_mq<cache_line_size>::try_async(cool::_async_task_end_incr_proxy target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	std::function<void(_cool_thmq_task*)> callable = [args...](_cool_thmq_task* _task_ptr)
	{
		cool::async_task_end& target_ref = *reinterpret_cast<cool::async_task_end*>(_task_ptr->m_target_ptr);
		(*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	std::size_t first_thread = static_cast<std::size_t>(this->m_thread_dispatch.fetch_add(
		this->m_dispatch_interval, std::memory_order_relaxed)
		% this->m_thread_count_uint);

	bool success = false;

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
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
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

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
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);

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

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_mq<cache_line_size>::try_async(cool::_async_task_result_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	std::function<void(_cool_thmq_task*)> callable = [args...](_cool_thmq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	std::size_t first_thread = static_cast<std::size_t>(this->m_thread_dispatch.fetch_add(
		this->m_dispatch_interval, std::memory_order_relaxed)
		% this->m_thread_count_uint);

	bool success = false;

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

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
						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

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

template <std::size_t cache_line_size> template <class return_Ty, class _allocator, class function_Ty, class ... _Args>
inline bool cool::threads_mq<cache_line_size>::try_async(cool::_async_task_result_incr_proxy<return_Ty, _allocator> target, const function_Ty& task, _Args&& ... args)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	std::function<void(_cool_thmq_task*)> callable = [args...](_cool_thmq_task* _task_ptr)
	{
		cool::async_task_result<return_Ty, _allocator>& target_ref = *reinterpret_cast<cool::async_task_result<return_Ty, _allocator>*>(_task_ptr->m_target_ptr);
		*(target_ref.m_stored_values_ptr + _task_ptr->m_offset) = (*reinterpret_cast<function_Ty*>(const_cast<void*>(_task_ptr->m_function_ptr)))(args...);
		if (target_ref.m_tasks_awaited.fetch_sub(1, std::memory_order_seq_cst) == 1)
		{
			target_ref.m_finish_condition_var.notify_one();
		}
	};

	std::size_t first_thread = static_cast<std::size_t>(this->m_thread_dispatch.fetch_add(
		this->m_dispatch_interval, std::memory_order_relaxed)
		% this->m_thread_count_uint);

	bool success = false;

	for (unsigned int n = this->m_push_rounds; n > 0; n--)
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
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

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
						target.m_parent_ptr->m_tasks_awaited.fetch_add(1, std::memory_order_relaxed);

						current_thread_ptr->m_last_task_ptr->m_callable = std::move(callable);
						current_thread_ptr->m_last_task_ptr->m_function_ptr = reinterpret_cast<const void*>(&task);
						current_thread_ptr->m_last_task_ptr->m_target_ptr = static_cast<void*>(target.m_parent_ptr);
						current_thread_ptr->m_last_task_ptr->m_offset = target.m_offset;

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

template <std::size_t cache_line_size>
inline bool cool::threads_mq<cache_line_size>::init_new_threads(std::size_t new_thread_count, std::size_t new_task_buffer_size,
	unsigned int push_rounds, unsigned int pop_rounds, unsigned int dispatch_interval)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

	delete_threads();

	if ((new_thread_count == 0) || (new_task_buffer_size == 0)
		|| (push_rounds == 0) || (pop_rounds == 0))
	{
		return false;
	}

	std::size_t task_buffer_size_per_thread = new_task_buffer_size / new_thread_count;
	if (new_thread_count * task_buffer_size_per_thread != new_task_buffer_size)
	{
		task_buffer_size_per_thread++;
	}

	this->m_push_rounds = push_rounds;
	this->m_pop_rounds = pop_rounds;
	this->m_dispatch_interval = dispatch_interval;

	std::size_t threads_constructed = 0;
	std::size_t threads_launched = 0;

	try
	{
		this->m_thread_blocks_unaligned_data_ptr = static_cast<_cool_thmq_tblk*>(::operator new(
			new_thread_count * sizeof(_cool_thmq_tblk) + 2 * cache_line_size, std::nothrow));

		if (this->m_thread_blocks_unaligned_data_ptr == nullptr)
		{
			this->delete_threads_detail(threads_constructed, threads_launched);
			return false;
		}
		else
		{
			std::uintptr_t ptr_remainder = reinterpret_cast<std::uintptr_t>(this->m_thread_blocks_unaligned_data_ptr) % static_cast<std::uintptr_t>(cache_line_size);

			this->m_thread_blocks_data_ptr = reinterpret_cast<_cool_thmq_tblk*>(reinterpret_cast<char*>(this->m_thread_blocks_unaligned_data_ptr)
				+ static_cast<std::size_t>(ptr_remainder != 0) * (cache_line_size - static_cast<std::size_t>(ptr_remainder)));
		}

		this->m_thread_count = new_thread_count;
		this->m_thread_count_uint = static_cast<unsigned int>(new_thread_count);

		for (std::size_t thread_num = 0; thread_num < new_thread_count; thread_num++)
		{
			new (this->m_thread_blocks_data_ptr + thread_num) _cool_thmq_tblk();
			if (!((this->m_thread_blocks_data_ptr + thread_num)->new_task_buffer(task_buffer_size_per_thread)))
			{
				this->delete_threads_detail(threads_constructed, threads_launched);
				return false;
			}
			threads_constructed++;
		}

		for (std::size_t thread_num = 0; thread_num < new_thread_count; thread_num++)
		{
			(this->m_thread_blocks_data_ptr + thread_num)->m_stop_threads = false;
			(this->m_thread_blocks_data_ptr + thread_num)->m_thread = std::thread([this, thread_num, new_thread_count, pop_rounds]()
				{
					_cool_thmq_task current_task;
					_cool_thmq_tblk* ptr;

					while (true)
					{
						bool ongoing = [&]() -> bool
						{
							for (unsigned int n = pop_rounds; n > 0; n--)
							{
								for (std::size_t k = thread_num; k < new_thread_count; k++)
								{
									ptr = this->m_thread_blocks_data_ptr + k;

									std::unique_lock<std::mutex> lock(ptr->m_mutex, std::try_to_lock);

									if (lock.owns_lock() && (ptr->m_last_task_ptr != ptr->m_next_task_ptr))
									{
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
										current_task = std::move(*(ptr->m_next_task_ptr));

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

								ptr->m_condition_var.wait(lock, [=]() -> bool
									{
										return (ptr->m_last_task_ptr != ptr->m_next_task_ptr) || ptr->m_stop_threads;
									}
								);

								if (ptr->m_last_task_ptr != ptr->m_next_task_ptr)
								{
									current_task = std::move(*(ptr->m_next_task_ptr));

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
							current_task.m_callable(&current_task);
						}
						else
						{
							return;
						}
					}
				}
			);

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

template <std::size_t cache_line_size>
inline std::size_t cool::threads_mq<cache_line_size>::thread_count() const noexcept
{
	return this->m_thread_count;
}

template <std::size_t cache_line_size>
inline std::size_t cool::threads_mq<cache_line_size>::task_buffer_size() const noexcept
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

template <std::size_t cache_line_size>
inline unsigned int cool::threads_mq<cache_line_size>::push_rounds() const noexcept
{
	return this->m_push_rounds;
}

template <std::size_t cache_line_size>
inline unsigned int cool::threads_mq<cache_line_size>::pop_rounds() const noexcept
{
	return this->m_pop_rounds;
}

template <std::size_t cache_line_size>
inline unsigned int cool::threads_mq<cache_line_size>::dispatch_interval() const noexcept
{
	return this->m_dispatch_interval;
}

template <std::size_t cache_line_size>
inline void cool::threads_mq<cache_line_size>::delete_threads() noexcept
{
	this->delete_threads_detail(this->m_thread_count, this->m_thread_count);
}

template <std::size_t cache_line_size>
inline void cool::_threads_mq_data<cache_line_size>::delete_threads_detail(std::size_t threads_constructed, std::size_t threads_launched) noexcept
{
	using _cool_thmq_tblk = typename cool::_threads_mq_data<cache_line_size>::_thread_block;

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
	this->m_thread_count_uint = 0;
	this->m_dispatch_interval = 1;
}

template <std::size_t cache_line_size>
inline bool cool::_threads_mq_data<cache_line_size>::_thread_block::new_task_buffer(std::size_t new_task_buffer_size)
{
	using _cool_thmq_task = typename cool::_threads_mq_data<cache_line_size>::_task;

	std::size_t new_task_buffer_size_p1 = new_task_buffer_size + 1;

	m_task_buffer_data_ptr = static_cast<_cool_thmq_task*>(::operator new(
		new_task_buffer_size_p1 * sizeof(_cool_thmq_task) + 2 * cache_line_size, std::nothrow));

	if (m_task_buffer_data_ptr == nullptr)
	{
		return false;
	}
	else
	{
		m_task_buffer_data_ptr = reinterpret_cast<_cool_thmq_task*>(
			reinterpret_cast<char*>(m_task_buffer_data_ptr) + cache_line_size);
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

template <std::size_t cache_line_size>
inline void cool::_threads_mq_data<cache_line_size>::_thread_block::delete_task_buffer() noexcept
{
	if (m_task_buffer_data_ptr != nullptr)
	{
		std::size_t task_buffer_size_p1 = static_cast<std::size_t>(m_task_buffer_end_ptr - m_task_buffer_data_ptr);

		for (std::size_t k = 0; k < task_buffer_size_p1; k++)
		{
			(m_task_buffer_data_ptr + k)->~_cool_thmq_task();
		}

		::operator delete(reinterpret_cast<char*>(m_task_buffer_data_ptr) - cache_line_size);
	}
}


// async_task_end detail

cool::async_task_end::~async_task_end()
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

template <class return_Ty, class _allocator>
cool::async_task_result<return_Ty, _allocator>::~async_task_result()
{
	finish();

	if (m_stored_values_ptr != nullptr)
	{
		for (std::size_t k = m_size; k > 0; )
		{
			k--;
			(m_stored_values_ptr + k)->~return_Ty();
		}

		this->deallocate(m_stored_values_ptr, m_size);
	}
}

template <class return_Ty, class _allocator> template <class ... _Args>
cool::async_task_result<return_Ty, _allocator>::async_task_result(std::size_t new_size, _Args&& ... args)
{
	m_stored_values_ptr = this->allocate(new_size);
	for (std::size_t k = 0; k < new_size; k++)
	{
		new (m_stored_values_ptr + k) return_Ty(std::forward<_Args>(args)...);
	}
	m_size = new_size;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::add_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::sub_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_sub(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::set_awaited(std::ptrdiff_t number_of_tasks) noexcept
{
	m_tasks_awaited.fetch_add(number_of_tasks, std::memory_order_seq_cst);
	return *this;
}

template <class return_Ty, class _allocator>
inline std::ptrdiff_t cool::async_task_result<return_Ty, _allocator>::get_awaited() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst);
}

template <class return_Ty, class _allocator>
inline cool::_async_task_result_proxy<return_Ty, _allocator> cool::async_task_result<return_Ty, _allocator>::at(std::size_t offset) noexcept
{
	return cool::_async_task_result_proxy<return_Ty, _allocator>(this, offset);
}

template <class return_Ty, class _allocator>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::finish() noexcept
{
	std::unique_lock<std::mutex> lock(m_finish_mutex);
	m_finish_condition_var.wait(lock, [&]() -> bool { return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0; });
	return *this;
}

template <class return_Ty, class _allocator>
inline bool cool::async_task_result<return_Ty, _allocator>::finished() const noexcept
{
	return m_tasks_awaited.load(std::memory_order_seq_cst) <= 0;
}

template <class return_Ty, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, _allocator>::get(std::size_t offset) noexcept
{
	finish();
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty, class _allocator>
inline return_Ty& cool::async_task_result<return_Ty, _allocator>::get_unchecked(std::size_t offset) noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty, class _allocator>
inline const return_Ty& cool::async_task_result<return_Ty, _allocator>::get_unchecked(std::size_t offset) const noexcept
{
	return *(m_stored_values_ptr + offset);
}

template <class return_Ty, class _allocator> template <class ... _Args>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::reset(_Args&& ... args)
{
	finish();

	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty(std::forward<_Args>(args)...);
	}

	return *this;
}

template <class return_Ty, class _allocator> template <class ... _Args>
inline cool::async_task_result<return_Ty, _allocator>& cool::async_task_result<return_Ty, _allocator>::reset_unchecked(_Args&& ... args)
{
	for (std::size_t k = 0; k < m_size; k++)
	{
		*(m_stored_values_ptr + k) = return_Ty(std::forward<_Args>(args)...);
	}

	return *this;
}

template <class return_Ty, class _allocator>
inline std::size_t cool::async_task_result<return_Ty, _allocator>::size() const noexcept
{
	return m_size;
}

template <class return_Ty, class _allocator>
inline void cool::async_task_result<return_Ty, _allocator>::resize(std::size_t new_size)
{
	clear();

	m_stored_values_ptr = this->allocate(new_size);
	if (m_stored_values_ptr != nullptr)
	{
		for (std::size_t k = 0; k < new_size; k++)
		{
			new (m_stored_values_ptr + k) return_Ty();
		}
		m_size = new_size;
	}
}

template <class return_Ty, class _allocator>
inline void cool::async_task_result<return_Ty, _allocator>::clear() noexcept
{
	finish();

	if (m_stored_values_ptr != nullptr)
	{
		for (std::size_t k = m_size; k > 0; )
		{
			k--;
			(m_stored_values_ptr + k)->~return_Ty();
		}

		this->deallocate(m_stored_values_ptr, m_size);
	}

	m_stored_values_ptr = nullptr;
	m_size = 0;
	m_tasks_awaited.store(0, std::memory_order_relaxed);
}

template <class return_Ty, class _allocator>
inline cool::_async_task_result_incr_proxy<return_Ty, _allocator> cool::_async_task_result_proxy<return_Ty, _allocator>::try_incr_awaited() noexcept
{
	return cool::_async_task_result_incr_proxy<return_Ty, _allocator>(m_parent_ptr, m_offset);
}

#endif // _COOL_THREADS_HPP


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
