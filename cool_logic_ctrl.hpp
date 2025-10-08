// cool_logic_ctrl.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_LOGIC_CTRL_HPP
#define xCOOL_LOGIC_CTRL_HPP

#include <cstddef>
#include <initializer_list>
#include <functional>
#include <algorithm>
#include <cassert>


namespace cool
{
	// cmp_Ty must have the same interface as std::not_equal_to<Ty>
	// cmp_Ty must return true when equality is considered to not hold
	// logic_ctrl_cmp may also be used for cmp_Ty

	template <class Ty, class cmp_Ty = std::not_equal_to<Ty>> class logic_ctrl;

	template <class Ty, class cmp_Ty> class logic_ctrl_variable_info;
	template <class Ty> class logic_ctrl_observer_info;
	template <class Ty> class logic_ctrl_relation_info;

	class max_depth;
	class logic_ctrl_init_result;

	class logic_ctrl_cmp;

	// logic_ctrl_variable_info

	template <class Ty, class cmp_Ty> class logic_ctrl_variable_info
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using compare_type = cmp_Ty;

		// > 1st arg is observer variable index, observer variable gets assigned the return value
		// > 2nt arg points to array of all variables
		// > 3rd arg points to shared data
		using compute_func_type = Ty(*)(std::size_t, const Ty*, void*);

		logic_ctrl_variable_info() = default;
		inline logic_ctrl_variable_info(cmp_Ty _cmp);

		compute_func_type compute_func = nullptr;
		std::size_t observer_index_begin = 0;
		std::size_t observer_index_end = 0;
		cmp_Ty cmp{};
		bool affected = false;
	};

	// logic_ctrl_observer_info

	template <class cmp_Ty> class logic_ctrl_observer_info
	{

	public:

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using compare_type = cmp_Ty;

		logic_ctrl_observer_info() = default;
		inline logic_ctrl_observer_info(std::size_t _observer_index, cmp_Ty _cmp);

		std::size_t observer_index = 0;
		cmp_Ty cmp{};
	};

	// logic_ctrl_relation_info

	template <class cmp_Ty> class logic_ctrl_relation_info
	{

	public:

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using compare_type = cmp_Ty;

		logic_ctrl_relation_info() = default;
		inline logic_ctrl_relation_info(cmp_Ty _cmp);

		std::size_t observed_index = 0;
		std::size_t observer_index = 0;
		cmp_Ty cmp{};
	};

	// max_depth

	class max_depth {
	public:
		max_depth() = delete;
		explicit inline constexpr max_depth(int new_max_depth) noexcept;
		inline constexpr int value() const noexcept;
	private:
		int m_value;
	};

	// logic_ctrl_init_result

	class logic_ctrl_init_result
	{

	public:

		static constexpr int success = 0;
		static constexpr int ongoing = 1;
		static constexpr int bad_parameters = 2;
		static constexpr int bad_observer_variable_index = 3;
		static constexpr int bad_observed_variable_index = 4;
		static constexpr int bad_relation_count = 5;
		static constexpr int bad_compute_func_assignment = 6;

		static constexpr int undefined = -1;

		logic_ctrl_init_result() = default;

		inline operator bool() const noexcept; // returns true if logic ctrl init is successful
		inline bool good() const noexcept; // returns true if logic ctrl init is successful
		inline int value() const noexcept;
		inline const char* message() const noexcept;

	private:

		inline logic_ctrl_init_result(int result) noexcept;

		int m_result = cool::logic_ctrl_init_result::undefined;

		template <class Ty, class cmp_Ty> friend class cool::logic_ctrl;
	};

	// logic_ctrl

	template <class Ty, class cmp_Ty> class logic_ctrl
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;
		using reference = Ty&;
		using const_reference = const Ty&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		using compare_type = cmp_Ty;

		using variable_info_type = cool::logic_ctrl_variable_info<Ty, cmp_Ty>;
		using observer_info_type = cool::logic_ctrl_observer_info<cmp_Ty>;
		using relation_info_type = cool::logic_ctrl_relation_info<cmp_Ty>;

		using compute_func_type = typename variable_info_type::compute_func_type;

		// set

		// > set_variable writes and triggers observers if 'cmp(old_value, new_value)' is true

		void set_variable(std::size_t variable_index, Ty new_value);
		void set_variable(std::size_t variable_index, Ty new_value, cool::max_depth _max_depth);

		void set_variable_no_cmp(std::size_t variable_index, Ty new_value);
		void set_variable_no_cmp(std::size_t variable_index, Ty new_value, cool::max_depth _max_depth);

		// accessors

		inline Ty& operator[](std::size_t variable_index) noexcept;
		inline const Ty& operator[](std::size_t variable_index) const noexcept;

		inline bool good() const noexcept;
		inline bool init_ongoing() const noexcept;
		inline std::size_t variable_count() const noexcept;
		inline int default_max_depth() const noexcept;
		inline std::size_t relation_count() const noexcept;
		inline std::size_t max_relation_count() const noexcept;

		inline Ty* begin() noexcept;
		inline Ty* end() noexcept;
		inline const Ty* begin() const noexcept;
		inline const Ty* end() const noexcept;
		inline const Ty* cbegin() const noexcept;
		inline const Ty* cend() const noexcept;

		// setup and clear

		// > 'relation_info_ptr' does not need to have persistent ownership after 'end_init'

		cool::logic_ctrl_init_result begin_init(
			std::size_t new_variable_count,
			Ty* variables_ptr,
			variable_info_type* variable_info_ptr,
			std::size_t new_max_relation_count,
			observer_info_type* observer_info_ptr,
			relation_info_type* relation_info_ptr,
			cool::max_depth new_default_max_depth = cool::max_depth(64),
			cmp_Ty default_cmp = cmp_Ty{},
			void* shared_data_ptr = nullptr
		);

		cool::logic_ctrl_init_result init_add_compute_func(std::size_t variable_index, std::initializer_list<std::size_t> observed_variables, compute_func_type compute_func) noexcept;
		cool::logic_ctrl_init_result init_add_compute_func(std::size_t variable_index, const std::size_t* observed_variables_ptr, std::size_t observed_variable_count, compute_func_type compute_func) noexcept;
		cool::logic_ctrl_init_result init_add_compute_func(std::size_t variable_index, std::initializer_list<observer_info_type> observed_variables, compute_func_type compute_func);
		cool::logic_ctrl_init_result init_add_compute_func(std::size_t variable_index, const observer_info_type* observed_variables_ptr, std::size_t observed_variable_count, compute_func_type compute_func);
		cool::logic_ctrl_init_result init_add_cmp(std::size_t variable_index, cmp_Ty cmp);

		cool::logic_ctrl_init_result end_init();

		void clear() noexcept;

	private:

		Ty* m_variables_ptr = nullptr;
		variable_info_type* m_variable_info_ptr = nullptr;
		observer_info_type* m_observer_info_ptr = nullptr;

		void* m_shared_data_ptr = nullptr;
		int m_default_max_depth = 0;
		int m_init = cool::logic_ctrl_init_result::undefined;

		std::size_t m_variable_count = 0;

		relation_info_type* m_relation_info_ptr = nullptr;
		std::size_t m_relation_count = 0;
		std::size_t m_max_relation_count = 0;
	};

	// logic_ctrl_cmp

	class logic_ctrl_cmp
	{

	public:

		static constexpr char not_equal = 0;
		static constexpr char rising = 1;
		static constexpr char falling = 2;
		static constexpr char always_false = 3;
		static constexpr char always_true = 4;

		char detect = logic_ctrl_cmp::not_equal;

		constexpr logic_ctrl_cmp() = default;
		inline constexpr logic_ctrl_cmp(char _detect) noexcept;
		constexpr logic_ctrl_cmp(const cool::logic_ctrl_cmp&) noexcept = default;
		cool::logic_ctrl_cmp& operator=(const cool::logic_ctrl_cmp&) noexcept = default;

		template <class Ty>
		inline constexpr bool operator()(const Ty& old_value, const Ty& new_value) const noexcept;
	};
}


// detail

template <class Ty, class cmp_Ty>
inline cool::logic_ctrl_variable_info<Ty, cmp_Ty>::logic_ctrl_variable_info(cmp_Ty _cmp) : cmp(_cmp) {}

template <class cmp_Ty>
inline cool::logic_ctrl_observer_info<cmp_Ty>::logic_ctrl_observer_info(std::size_t _observer_index, cmp_Ty _cmp) : observer_index(_observer_index), cmp(_cmp) {}

template <class cmp_Ty>
inline cool::logic_ctrl_relation_info<cmp_Ty>::logic_ctrl_relation_info(cmp_Ty _cmp) : cmp(_cmp) {}

inline constexpr cool::max_depth::max_depth(int new_max_depth) noexcept : m_value(new_max_depth) {}

inline constexpr int cool::max_depth::value() const noexcept { return m_value; }

inline cool::logic_ctrl_init_result::operator bool() const noexcept
{
	return m_result == logic_ctrl_init_result::success;
}

inline bool cool::logic_ctrl_init_result::good() const noexcept
{
	return m_result == logic_ctrl_init_result::success;
}

inline int cool::logic_ctrl_init_result::value() const noexcept
{
	return m_result;
}

inline const char* cool::logic_ctrl_init_result::message() const noexcept
{
	switch (m_result)
	{
	case logic_ctrl_init_result::success: return "cool logic ctrl init success"; break;
	case logic_ctrl_init_result::bad_parameters: return "cool logic ctrl init failed : bad parameters"; break;
	case logic_ctrl_init_result::bad_observer_variable_index: return "cool logic ctrl init failed : observer variable index out of bound"; break;
	case logic_ctrl_init_result::bad_observed_variable_index: return "cool logic ctrl init failed : observed variable index out of bound"; break;
	case logic_ctrl_init_result::bad_relation_count: return "cool logic ctrl init failed : maximum relation count is too low"; break;
	case logic_ctrl_init_result::bad_compute_func_assignment: return "cool logic ctrl init failed : double assignement of compute functions for one variable"; break;
	default: return "cool logic ctrl init result undefined"; break;
	}
}

inline cool::logic_ctrl_init_result::logic_ctrl_init_result(int result) noexcept : m_result(result) {}

template <class Ty, class cmp_Ty>
void cool::logic_ctrl<Ty, cmp_Ty>::set_variable(std::size_t variable_index, Ty new_value)
{
	set_variable(variable_index, new_value, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty>
void cool::logic_ctrl<Ty, cmp_Ty>::set_variable(std::size_t variable_index, Ty new_value, cool::max_depth _max_depth)
{
	assert(good());
	assert(variable_index < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + variable_index);
	Ty old_value = *(m_variables_ptr + variable_index);

	if (!variable_info_ref.affected && variable_info_ref.cmp(old_value, new_value))
	{
		variable_info_ref.affected = true;
		*(m_variables_ptr + variable_index) = new_value;
		int _max_depth_value = _max_depth.value();

		if (_max_depth_value > 0)
		{
			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				std::size_t observer_variable_index = (m_observer_info_ptr + observer_index)->observer_index;
				cmp_Ty& cmp_ref = (m_observer_info_ptr + observer_index)->cmp;

				if (cmp_ref(old_value, new_value))
				{
					compute_func_type observer_compute_func = (m_variable_info_ptr + observer_variable_index)->compute_func;

					set_variable(observer_variable_index,
						observer_compute_func(observer_variable_index, m_variables_ptr, m_shared_data_ptr),
						cool::max_depth(_max_depth_value - 1));
				}
			}
		}

		variable_info_ref.affected = false;
	}
}

template <class Ty, class cmp_Ty>
void cool::logic_ctrl<Ty, cmp_Ty>::set_variable_no_cmp(std::size_t variable_index, Ty new_value)
{
	set_variable_no_cmp(variable_index, new_value, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty>
void cool::logic_ctrl<Ty, cmp_Ty>::set_variable_no_cmp(std::size_t variable_index, Ty new_value, cool::max_depth _max_depth)
{
	assert(good());
	assert(variable_index < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + variable_index);

	if (!variable_info_ref.affected)
	{
		variable_info_ref.affected = true;
		*(m_variables_ptr + variable_index) = new_value;
		int _max_depth_value = _max_depth.value();

		if (_max_depth_value > 0)
		{
			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				std::size_t observer_variable_index = (m_observer_info_ptr + observer_index)->observer_index;
				compute_func_type observer_compute_func = (m_variable_info_ptr + observer_variable_index)->compute_func;

				set_variable_no_cmp(observer_variable_index,
					observer_compute_func(observer_variable_index, m_variables_ptr, m_shared_data_ptr),
					cool::max_depth(_max_depth_value - 1));
			}
		}

		variable_info_ref.affected = false;
	}
}

template <class Ty, class cmp_Ty>
inline Ty& cool::logic_ctrl<Ty, cmp_Ty>::operator[](std::size_t variable_index) noexcept { return *(m_variables_ptr + variable_index); }

template <class Ty, class cmp_Ty>
inline const Ty& cool::logic_ctrl<Ty, cmp_Ty>::operator[](std::size_t variable_index) const noexcept { return *(m_variables_ptr + variable_index); }

template <class Ty, class cmp_Ty>
inline bool cool::logic_ctrl<Ty, cmp_Ty>::good() const noexcept { return m_init == logic_ctrl_init_result::success; }

template <class Ty, class cmp_Ty>
inline bool cool::logic_ctrl<Ty, cmp_Ty>::init_ongoing() const noexcept { return m_init == logic_ctrl_init_result::ongoing; }

template <class Ty, class cmp_Ty>
inline std::size_t cool::logic_ctrl<Ty, cmp_Ty>::variable_count() const noexcept { return m_variable_count; }

template <class Ty, class cmp_Ty>
inline int cool::logic_ctrl<Ty, cmp_Ty>::default_max_depth() const noexcept { return m_default_max_depth; }

template <class Ty, class cmp_Ty>
inline std::size_t cool::logic_ctrl<Ty, cmp_Ty>::relation_count() const noexcept { return m_relation_count; }

template <class Ty, class cmp_Ty>
inline std::size_t cool::logic_ctrl<Ty, cmp_Ty>::max_relation_count() const noexcept { return m_max_relation_count; }

template <class Ty, class cmp_Ty>
inline Ty* cool::logic_ctrl<Ty, cmp_Ty>::begin() noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty>
inline Ty* cool::logic_ctrl<Ty, cmp_Ty>::end() noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty>
inline const Ty* cool::logic_ctrl<Ty, cmp_Ty>::begin() const noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty>
inline const Ty* cool::logic_ctrl<Ty, cmp_Ty>::end() const noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty>
inline const Ty* cool::logic_ctrl<Ty, cmp_Ty>::cbegin() const noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty>
inline const Ty* cool::logic_ctrl<Ty, cmp_Ty>::cend() const noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::begin_init(
	std::size_t new_variable_count,
	Ty* variables_ptr,
	variable_info_type* variable_info_ptr,
	std::size_t new_max_relation_count,
	observer_info_type* observer_info_ptr,
	relation_info_type* relation_info_ptr,
	cool::max_depth new_default_max_depth,
	cmp_Ty default_cmp,
	void* shared_data_ptr)
{
	assert(variables_ptr != nullptr);
	assert(variable_info_ptr != nullptr);
	assert(observer_info_ptr != nullptr);
	assert(relation_info_ptr != nullptr);

	if ((new_variable_count == 0) || (variables_ptr == nullptr) || (variable_info_ptr == nullptr)
		|| (observer_info_ptr == nullptr) || (relation_info_ptr == nullptr))
	{
		m_init = cool::logic_ctrl_init_result::bad_parameters;
	}
	else
	{
		m_variables_ptr = variables_ptr;
		m_variable_info_ptr = variable_info_ptr;
		m_observer_info_ptr = observer_info_ptr;

		m_shared_data_ptr = shared_data_ptr;
		m_default_max_depth = new_default_max_depth.value();
		m_init = cool::logic_ctrl_init_result::ongoing;

		m_variable_count = new_variable_count;

		m_relation_info_ptr = relation_info_ptr;
		m_relation_count = 0;
		m_max_relation_count = new_max_relation_count;

		for (std::size_t n = 0; n < new_variable_count; n++)
		{
			*(m_variables_ptr + n) = Ty{};
			*(m_variable_info_ptr + n) = variable_info_type(default_cmp);
		}

		for (std::size_t n = 0; n < new_max_relation_count; n++)
		{
			*(m_observer_info_ptr + n) = observer_info_type(0, default_cmp);
			*(m_relation_info_ptr + n) = relation_info_type(default_cmp);
		}
	}

	return cool::logic_ctrl_init_result(m_init);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::init_add_compute_func(std::size_t variable_index, std::initializer_list<std::size_t> observed_variables, compute_func_type compute_func) noexcept
{
	return init_add_compute_func(variable_index, observed_variables.begin(), observed_variables.size(), compute_func);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::init_add_compute_func(std::size_t variable_index, const std::size_t* observed_variables_ptr, std::size_t observed_variable_count, compute_func_type compute_func) noexcept
{
	assert(compute_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	if (m_init == cool::logic_ctrl_init_result::ongoing)
	{
		if ((compute_func == nullptr) || ((observed_variables_ptr == nullptr) && (observed_variable_count != 0)))
		{
			m_init = cool::logic_ctrl_init_result::bad_parameters;
		}
		else if (!(variable_index < m_variable_count))
		{
			m_init = cool::logic_ctrl_init_result::bad_observer_variable_index;
		}
		else if ((m_variable_info_ptr + variable_index)->compute_func != nullptr)
		{
			m_init = cool::logic_ctrl_init_result::bad_compute_func_assignment;
		}
		else
		{
			(m_variable_info_ptr + variable_index)->compute_func = compute_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				std::size_t observed_variable = *(observed_variables_ptr + n);

				if (!(observed_variable < m_variable_count))
				{
					m_init = cool::logic_ctrl_init_result::bad_observed_variable_index;
					break;
				}
				else if (!(m_relation_count < m_max_relation_count))
				{
					m_init = cool::logic_ctrl_init_result::bad_relation_count;
					break;
				}
				else if (observed_variable != variable_index)
				{
					(m_relation_info_ptr + m_relation_count)->observed_index = observed_variable;
					(m_relation_info_ptr + m_relation_count)->observer_index = variable_index;
					m_relation_count++;
				}
			}
		}
	}

	return cool::logic_ctrl_init_result(m_init);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::init_add_compute_func(std::size_t variable_index, std::initializer_list<observer_info_type> observed_variables, compute_func_type compute_func)
{
	return init_add_compute_func(variable_index, observed_variables.begin(), observed_variables.size(), compute_func);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::init_add_compute_func(std::size_t variable_index, const observer_info_type* observed_variables_ptr, std::size_t observed_variable_count, compute_func_type compute_func)
{
	assert(compute_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	if (m_init == cool::logic_ctrl_init_result::ongoing)
	{
		if (compute_func == nullptr)
		{
			m_init = cool::logic_ctrl_init_result::bad_parameters;
		}
		else if (!(variable_index < m_variable_count))
		{
			m_init = cool::logic_ctrl_init_result::bad_observer_variable_index;
		}
		else if ((m_variable_info_ptr + variable_index)->compute_func != nullptr)
		{
			m_init = cool::logic_ctrl_init_result::bad_compute_func_assignment;
		}
		else
		{
			(m_variable_info_ptr + variable_index)->compute_func = compute_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				const observer_info_type& observed_variable = *(observed_variables_ptr + n);

				if (!(observed_variable.observer_index < m_variable_count))
				{
					m_init = cool::logic_ctrl_init_result::bad_observed_variable_index;
					break;
				}
				else if (!(m_relation_count < m_max_relation_count))
				{
					m_init = cool::logic_ctrl_init_result::bad_relation_count;
					break;
				}
				else if (observed_variable.observer_index != variable_index)
				{
					(m_relation_info_ptr + m_relation_count)->observed_index = observed_variable.observer_index;
					(m_relation_info_ptr + m_relation_count)->observer_index = variable_index;
					(m_relation_info_ptr + m_relation_count)->cmp = observed_variable.cmp;
					m_relation_count++;
				}
			}
		}
	}

	return cool::logic_ctrl_init_result(m_init);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::init_add_cmp(std::size_t variable_index, cmp_Ty cmp)
{
	if (m_init == cool::logic_ctrl_init_result::ongoing)
	{
		if (variable_index < m_variable_count)
		{
			(m_variable_info_ptr + variable_index)->cmp = cmp;
		}
		else
		{
			m_init = cool::logic_ctrl_init_result::bad_observed_variable_index;
		}
	}

	return cool::logic_ctrl_init_result(m_init);
}

template <class Ty, class cmp_Ty>
cool::logic_ctrl_init_result cool::logic_ctrl<Ty, cmp_Ty>::end_init()
{
	if (m_init == cool::logic_ctrl_init_result::ongoing)
	{
		m_init = cool::logic_ctrl_init_result::success;

		std::sort(m_relation_info_ptr, m_relation_info_ptr + m_relation_count,
			[](const relation_info_type& a, const relation_info_type& b) { return a.observed_index < b.observed_index; });

		std::size_t current_rel = 0;

		for (std::size_t variable_index = 0; variable_index < m_variable_count; variable_index++)
		{
			(m_variable_info_ptr + variable_index)->observer_index_begin = current_rel;

			while ((current_rel < m_relation_count) && ((m_relation_info_ptr + current_rel)->observed_index == variable_index))
			{
				(m_observer_info_ptr + current_rel)->observer_index = (m_relation_info_ptr + current_rel)->observer_index;
				(m_observer_info_ptr + current_rel)->cmp = (m_relation_info_ptr + current_rel)->cmp;
				current_rel++;
			}

			(m_variable_info_ptr + variable_index)->observer_index_end = current_rel;
		}
	}

	return cool::logic_ctrl_init_result(m_init);
}

template <class Ty, class cmp_Ty>
void cool::logic_ctrl<Ty, cmp_Ty>::clear() noexcept
{
	m_variables_ptr = nullptr;
	m_variable_info_ptr = nullptr;
	m_observer_info_ptr = nullptr;

	m_shared_data_ptr = nullptr;
	m_default_max_depth = 0;

	m_variable_count = 0;
	m_init = logic_ctrl_init_result::undefined;

	m_relation_info_ptr = nullptr;
	m_relation_count = 0;
	m_max_relation_count = 0;
}

inline constexpr cool::logic_ctrl_cmp::logic_ctrl_cmp(char _detect) noexcept : detect(_detect) {}

template <class Ty>
inline constexpr bool cool::logic_ctrl_cmp::operator()(const Ty& old_value, const Ty& new_value) const noexcept
{
	switch (detect)
	{
	case logic_ctrl_cmp::not_equal: return old_value != new_value; break;
	case logic_ctrl_cmp::rising: return old_value < new_value; break;
	case logic_ctrl_cmp::falling: return old_value > new_value; break;
	default: return (detect == logic_ctrl_cmp::always_true); break;
	}
}

#endif // xCOOL_LOGIC_CTRL_HPP


// cool_logic_ctrl.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.
