// cool_logic_ctrl.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_LOGIC_CTRL_HPP
#define xCOOL_LOGIC_CTRL_HPP

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <functional>
#include <algorithm>
#include <cassert>


// to allow the use of std::vector to enable cool::logic_ctrl_vec : #define COOL_LOGIC_CTRL_VECTOR

#ifdef COOL_LOGIC_CTRL_VECTOR
#endif // COOL_LOGIC_CTRL_VECTOR


// to include variable count in variable_view : #define COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT

#ifdef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT


#ifdef COOL_LOGIC_CTRL_VECTOR
#include <vector>
#include <new>

#endif // COOL_LOGIC_CTRL_VECTOR


namespace cool
{
	// > Ty must be copy assignable from refresh_result_Ty
	// > refresh_result_Ty must be copy constructible from Ty

	// > 'cmp_Ty' must have the same interface as 'std::not_equal_to<refresh_result_Ty>'
	// > 'cmp_Ty' should return true when equality is considered to not hold
	// > 'cool::logic_ctrl_cmp<refresh_result_Ty>' may also be used for 'cmp_Ty' and provides more options
	// > 'lhs' is the new value and 'rhs' is the previous value in expression 'cmp(lhs, rhs)' with 'cmp' of type 'cmp_Ty'

	// > 'static_cast<index_Ty>(i)' where 'std::size_t i' ranges from 0 to 'variable_count() - 1' must be a valid expression

	// > member functions set_variable/refresh_variable will favor passing by value if 'small_Ty' is true

	template <class Ty, class cmp_Ty = std::not_equal_to<Ty>, class index_Ty = std::size_t, class refresh_result_Ty = Ty, bool small_Ty = (sizeof(refresh_result_Ty) <= 2 * sizeof(refresh_result_Ty*))> class _logic_ctrl_base;
	template <class Ty, class cmp_Ty = std::not_equal_to<Ty>, class index_Ty = std::size_t, class refresh_result_Ty = Ty, bool small_Ty = (sizeof(refresh_result_Ty) <= 2 * sizeof(refresh_result_Ty*))> class logic_ctrl;
#ifdef COOL_LOGIC_CTRL_VECTOR
	template <class Ty, class cmp_Ty = std::not_equal_to<Ty>, class index_Ty = std::size_t, class refresh_result_Ty = Ty, bool small_Ty = (sizeof(refresh_result_Ty) <= 2 * sizeof(refresh_result_Ty*))> class logic_ctrl_vec;
#endif // COOL_LOGIC_CTRL_VECTOR

	template <class cmp_Ty, class index_Ty> class logic_ctrl_observed_info;
	template <class refresh_result_Ty> class logic_ctrl_cmp;
	template <class index_Ty> class logic_ctrl_init_result;
	class max_depth;

	template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty = Ty, bool small_Ty = (sizeof(Ty) <= 2 * sizeof(Ty*))> class logic_ctrl_variable_info;
	template <class cmp_Ty, class index_Ty> class logic_ctrl_observer_info;
	template <class cmp_Ty, class index_Ty> class logic_ctrl_relation_info;


	// init types

	class variable_count {
	public:
		variable_count() = delete;
		explicit inline constexpr variable_count(std::size_t new_variable_count) noexcept;
		inline constexpr std::size_t value() const noexcept;
	private:
		std::size_t m_value;
	};

	class max_relation_count {
	public:
		max_relation_count() = delete;
		explicit inline constexpr max_relation_count(std::size_t _max_relation_count) noexcept;
		inline constexpr std::size_t value() const noexcept;
	private:
		std::size_t m_value;
	};


	// logic_ctrl_observed_info

	template <class cmp_Ty, class index_Ty> class logic_ctrl_observed_info
	{

	public:

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using compare_type = cmp_Ty;
		using index_type = index_Ty;

		logic_ctrl_observed_info() = default;
		inline logic_ctrl_observed_info(index_Ty _observed_index, cmp_Ty _cmp) noexcept;

		index_Ty observed_index = static_cast<index_Ty>(0);
		cmp_Ty cmp{};
	};

	// logic_ctrl_cmp

	template <class refresh_result_Ty> class logic_ctrl_cmp
	{

	public:

		static constexpr unsigned char change = 0;
		static constexpr unsigned char rising = 1;
		static constexpr unsigned char falling = 2;
		static constexpr unsigned char always = 3;
		static constexpr unsigned char never = 4;

		unsigned char trigger = logic_ctrl_cmp::change;

		constexpr logic_ctrl_cmp() = default;
		inline constexpr logic_ctrl_cmp(unsigned char _trigger) noexcept;
		constexpr logic_ctrl_cmp(const cool::logic_ctrl_cmp<refresh_result_Ty>&) noexcept = default;
		cool::logic_ctrl_cmp<refresh_result_Ty>& operator=(const cool::logic_ctrl_cmp<refresh_result_Ty>&) noexcept = default;

		inline constexpr bool operator()(const refresh_result_Ty& new_value, const refresh_result_Ty& previous_value) const noexcept;
	};

	// logic_ctrl_init_result

	template <class index_Ty> class logic_ctrl_init_result
	{

	public:

		using index_type = index_Ty;

		static constexpr int success = 0;
		static constexpr int init_ongoing = 1;
		static constexpr int bad_parameters = 2;
		static constexpr int bad_alloc = 3;
		static constexpr int bad_observer_variable_index = 4;
		static constexpr int bad_observed_variable_index = 5;
		static constexpr int bad_relation_count = 6;
		static constexpr int bad_refresh_func_assignment = 7;
		static constexpr int modification_after_init_end = 8;

		static constexpr int undefined = -1;

		logic_ctrl_init_result() = default;

		inline operator bool() const noexcept; // returns true if logic ctrl init is successful
		inline bool good() const noexcept; // returns true if logic ctrl init is successful
		inline int value() const noexcept;
		inline const char* message() const noexcept;
		inline index_Ty index_if_init_failed() const noexcept;

		cool::logic_ctrl_init_result<index_Ty>& operator&=(const cool::logic_ctrl_init_result<index_Ty>& rhs) noexcept;
		cool::logic_ctrl_init_result<index_Ty> operator&(const cool::logic_ctrl_init_result<index_Ty>& rhs) noexcept;

	private:

		inline logic_ctrl_init_result(int result) noexcept;
		inline logic_ctrl_init_result(int result, index_Ty observer_index) noexcept;

		int m_result = logic_ctrl_init_result::undefined;
		index_Ty m_index = static_cast<index_Ty>(0);

		template <class Ty, class cmp_Ty, class index_Ty2, class refresh_result_Ty2, bool small_Ty> friend class cool::_logic_ctrl_base;
		template <class Ty, class cmp_Ty, class index_Ty2, class refresh_result_Ty2, bool small_Ty> friend class cool::logic_ctrl;
#ifdef COOL_LOGIC_CTRL_VECTOR
		template <class Ty, class cmp_Ty, class index_Ty2, class refresh_result_Ty2, bool small_Ty> friend class cool::logic_ctrl_vec;
#endif // COOL_LOGIC_CTRL_VECTOR
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


	// logic_ctrl_get_observers_result

	template <class index_Ty> class logic_ctrl_get_observers_result
	{

	public:

		using index_type = index_Ty;

		std::size_t observer_count = 0;

		int depth_reached = 0;
		bool max_depth_is_reached = false;
		bool lock_is_reached = false;
		index_Ty index_if_lock_is_reached = static_cast<index_Ty>(0);
	};

#ifdef COOL_LOGIC_CTRL_VECTOR

	// logic_ctrl_get_observers_vec_result

	template <class index_Ty> class logic_ctrl_get_observers_vec_result : public logic_ctrl_get_observers_result<index_Ty>
	{

	public:

		using typename cool::logic_ctrl_get_observers_result<index_Ty>::index_type;

		std::vector<index_Ty> observers;
	};
#endif // COOL_LOGIC_CTRL_VECTOR

	// _logic_ctrl_base

	template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty> class _logic_ctrl_base
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
		using index_type = index_Ty;
		using refresh_result_value_type = refresh_result_Ty;

		static constexpr bool small_value_type = small_Ty;

		using variable_info_type = cool::logic_ctrl_variable_info<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>;
		using observed_info_type = cool::logic_ctrl_observed_info<cmp_Ty, index_Ty>;
		using observer_info_type = cool::logic_ctrl_observer_info<cmp_Ty, index_Ty>;
		using relation_info_type = cool::logic_ctrl_relation_info<cmp_Ty, index_Ty>;
		using init_result_type = cool::logic_ctrl_init_result<index_Ty>;

		using arg_value_type = typename std::conditional<small_Ty, refresh_result_Ty, const refresh_result_Ty&>::type;

		// provides interface to all variables as a const contiguous array
		class variable_view;

		// > 1st arg is observer/refreshed variable index, observer variable gets assigned the return value
		// > 2nt arg provides a view/array of all variables
		// > 3rd arg points to shared data of observer
		using refresh_func_type = refresh_result_Ty(*)(index_Ty, typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view, void*);

		using get_observers_result_type = cool::logic_ctrl_get_observers_result<index_Ty>;
#ifdef COOL_LOGIC_CTRL_VECTOR
		using get_observers_vec_result_type = cool::logic_ctrl_get_observers_vec_result<index_Ty>;
#endif // COOL_LOGIC_CTRL_VECTOR

		// set

		// > set_variable writes and triggers observers if 'cmp(new_value, previous_value)' returns true

		void set_variable(index_Ty variable_index, arg_value_type new_value);
		void set_variable(index_Ty variable_index, arg_value_type new_value, cool::max_depth _max_depth);

		// > set_variable_no_cmp writes and triggers observers

		void set_variable_no_cmp(index_Ty variable_index, arg_value_type new_value);
		void set_variable_no_cmp(index_Ty variable_index, arg_value_type new_value, cool::max_depth _max_depth);

		// > refresh_variable runs the refresh function that returns the 'new_value' and triggers observers if 'cmp(new_value, previous_value)' returns true

		void refresh_variable(index_Ty variable_index);
		void refresh_variable(index_Ty variable_index, cool::max_depth _max_depth);

		// > refresh_variable_no_cmp runs the refresh function and triggers observers

		void refresh_variable_no_cmp(index_Ty variable_index);
		void refresh_variable_no_cmp(index_Ty variable_index, cool::max_depth _max_depth);

		// accessors

		inline Ty& operator[](index_Ty variable_index) noexcept;
		inline const Ty& operator[](index_Ty variable_index) const noexcept;

		inline Ty* data() noexcept;
		inline const Ty* data() const noexcept;

		inline Ty* begin() noexcept;
		inline Ty* end() noexcept;
		inline const Ty* begin() const noexcept;
		inline const Ty* end() const noexcept;
		inline const Ty* cbegin() const noexcept;
		inline const Ty* cend() const noexcept;

		inline bool& locked(index_Ty variable_index) noexcept;
		inline const bool& locked(index_Ty variable_index) const noexcept;
		inline void lock_all(bool _locked) noexcept;

		inline void* get_shared_data_ptr(index_Ty variable_index) noexcept;
		inline const void* get_shared_data_ptr(index_Ty variable_index) const noexcept;

		inline bool good() const noexcept;
		inline bool init_ongoing() const noexcept;
		inline std::size_t variable_count() const noexcept;
		inline int default_max_depth() const noexcept;
		inline std::size_t relation_count() const noexcept;
		inline std::size_t max_relation_count() const noexcept;
		cool::logic_ctrl_get_observers_result<index_Ty> get_observers(index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size) const noexcept;
		cool::logic_ctrl_get_observers_result<index_Ty> get_observers(index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size, cool::max_depth _max_depth) const noexcept;
#ifdef COOL_LOGIC_CTRL_VECTOR
		cool::logic_ctrl_get_observers_vec_result<index_Ty> get_observers_vec(index_Ty variable_index) const;
		cool::logic_ctrl_get_observers_vec_result<index_Ty> get_observers_vec(index_Ty variable_index, cool::max_depth _max_depth) const;
#endif // COOL_LOGIC_CTRL_VECTOR

		init_result_type init_set_cmp(index_Ty variable_index, cmp_Ty cmp) noexcept;
		init_result_type init_set_shared_data_ptr(index_Ty variable_index, void* shared_data_ptr) noexcept;

		class variable_view
		{

		public:

			using value_type = Ty;
			using pointer = Ty*;
			using const_pointer = const Ty*;
			using reference = Ty&;
			using const_reference = const Ty&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using refresh_result_value_type = refresh_result_Ty;

			using index_type = index_Ty;

			variable_view() = delete;
#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
			explicit inline variable_view(const Ty* ptr) noexcept;
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
			explicit inline variable_view(const Ty* ptr, std::size_t _variable_count) noexcept;
			inline std::size_t variable_count() const noexcept;
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
			inline const Ty& operator[](index_Ty variable_index) const noexcept;
			inline const Ty* data() const noexcept;

		private:

			const Ty* m_ptr;
#ifdef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
			std::size_t m_variable_count;
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
		};

	private:

		friend class cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>;
#ifdef COOL_LOGIC_CTRL_VECTOR
		friend class cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>;
#endif // COOL_LOGIC_CTRL_VECTOR

		class _lock_guard
		{

		public:

			inline _lock_guard(bool& lock_ref) noexcept;
			_lock_guard(const cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard&) = delete;
			cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard& operator=(const cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard&) = delete;
			_lock_guard(cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard&&) = delete;
			cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard& operator=(cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard&&) = delete;
			inline ~_lock_guard();

		private:

			bool* m_ptr;
		};

		void _get_observers_sub(index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size, int _max_depth_value, int _max_depth_initial_value,
			cool::logic_ctrl_get_observers_result<index_Ty>& obs_result_ref) const noexcept;
#ifdef COOL_LOGIC_CTRL_VECTOR
		void _get_observers_vec_sub(index_Ty variable_index, int _max_depth_value, int _max_depth_initial_value,
			cool::logic_ctrl_get_observers_vec_result<index_Ty>& obs_result_ref) const;
#endif // COOL_LOGIC_CTRL_VECTOR
		void _delete_logic_ctrl_sub() noexcept;

		Ty* m_variables_ptr = nullptr;
		variable_info_type* m_variable_info_ptr = nullptr;
		observer_info_type* m_observer_info_ptr = nullptr;

		int m_default_max_depth = 0;
		int m_init = init_result_type::undefined;

		std::size_t m_variable_count = 0;

		relation_info_type* m_relation_info_ptr = nullptr;
		std::size_t m_relation_count = 0;
		std::size_t m_max_relation_count = 0;
	};

	// logic_ctrl

	template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty> class logic_ctrl
		: public cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>
	{

	public:

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::value_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::pointer;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::const_pointer;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::reference;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::const_reference;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::size_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::difference_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::compare_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::index_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_result_value_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::observed_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::observer_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::relation_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::arg_value_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_func_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_result_type;
#ifdef COOL_LOGIC_CTRL_VECTOR
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_vec_result_type;
#endif // COOL_LOGIC_CTRL_VECTOR

		logic_ctrl() noexcept = default;
		logic_ctrl(const cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&) = delete;
		cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& operator=(const cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&) = delete;
		logic_ctrl(cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept;
		cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& operator=(cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept;
		~logic_ctrl() = default;

		// setup and clear

		// > 'variables_ptr' must have persistent ownership of 'new_variable_count' contiguous elements
		// > 'variables_info_ptr' must have persistent ownership of 'new_variable_count' contiguous elements
		// > 'observer_info_ptr' must have persistent ownership of 'new_max_relation_count' contiguous elements
		// > 'relation_info_ptr' needs to own 'new_max_relation_count' elements and does not need to have persistent ownership after 'init_end'

		init_result_type init_logic_ctrl_begin(
			Ty* variables_ptr,
			variable_info_type* variable_info_ptr,
			cool::variable_count new_variable_count,
			observer_info_type* observer_info_ptr,
			relation_info_type* relation_info_ptr,
			cool::max_relation_count new_max_relation_count,
			cool::max_depth new_default_max_depth = cool::max_depth(64),
			cmp_Ty default_cmp = cmp_Ty{}
		);

		init_result_type init_add_relations(index_Ty observer_variable_index, std::initializer_list<index_Ty> observed_variables, refresh_func_type refresh_func) noexcept;
		init_result_type init_add_relations(index_Ty observer_variable_index, const index_Ty* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func) noexcept;
		init_result_type init_add_relations(index_Ty observer_variable_index, std::initializer_list<observed_info_type> observed_variables, refresh_func_type refresh_func) noexcept(std::is_nothrow_copy_assignable<cmp_Ty>::value);
		init_result_type init_add_relations(index_Ty variable_index, const observed_info_type* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func) noexcept(std::is_nothrow_copy_assignable<cmp_Ty>::value);
		init_result_type init_add_relations(index_Ty observer_variable_index, refresh_func_type refresh_func) noexcept;

		init_result_type init_logic_ctrl_end();

		void delete_logic_ctrl() noexcept;
	};

#ifdef COOL_LOGIC_CTRL_VECTOR

	// logic_ctrl_vec

	template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty> class logic_ctrl_vec
		: public cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>
	{

	public:

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::value_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::pointer;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::const_pointer;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::reference;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::const_reference;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::size_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::difference_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::compare_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::index_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_result_value_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::observed_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::observer_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::relation_info_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::arg_value_type;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view;
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_func_type;

		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_result_type;
#ifdef COOL_LOGIC_CTRL_VECTOR
		using typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_vec_result_type;
#endif // COOL_LOGIC_CTRL_VECTOR

		logic_ctrl_vec() noexcept = default;
		logic_ctrl_vec(const cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&) = delete;
		cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& operator=(const cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&) = delete;
		logic_ctrl_vec(cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&&) noexcept;
		cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& operator=(cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&&) noexcept;
		~logic_ctrl_vec();

		// setup and clear

		init_result_type init_new_logic_ctrl_begin(
			cool::variable_count new_variable_count,
			cool::max_depth new_default_max_depth = cool::max_depth(64),
			cmp_Ty default_cmp = cmp_Ty{}
		);

		init_result_type init_add_relations(index_Ty observer_variable_index, std::initializer_list<index_Ty> observed_variables, refresh_func_type refresh_func);
		init_result_type init_add_relations(index_Ty observer_variable_index, const index_Ty* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func);
		init_result_type init_add_relations(index_Ty observer_variable_index, std::initializer_list<observed_info_type> observed_variables, refresh_func_type refresh_func);
		init_result_type init_add_relations(index_Ty variable_index, const observed_info_type* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func);
		init_result_type init_add_relations(index_Ty observer_variable_index, refresh_func_type refresh_func) noexcept;

		init_result_type init_logic_ctrl_end();

		void delete_logic_ctrl() noexcept;

	private:

		std::vector<observer_info_type> m_observer_info_vec;
		std::vector<relation_info_type> m_relation_info_vec;
	};
#endif // COOL_LOGIC_CTRL_VECTOR

	// logic_ctrl_variable_info

	template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty> class logic_ctrl_variable_info
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
		using index_type = index_Ty;
		using refresh_result_value_type = refresh_result_Ty;

		static constexpr bool small_value_type = cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::small_value_type;
		using arg_value_type = typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::arg_value_type;
		using refresh_func_type = typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_func_type;

		logic_ctrl_variable_info() = default;
		inline logic_ctrl_variable_info(cmp_Ty _cmp) noexcept;

		refresh_func_type refresh_func = nullptr;
		void* shared_data_ptr = nullptr;
		std::size_t observer_index_begin = 0;
		std::size_t observer_index_end = 0;
		cmp_Ty cmp{};
		bool locked = false;
	};

	// logic_ctrl_observer_info

	template <class cmp_Ty, class index_Ty> class logic_ctrl_observer_info
	{

	public:

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using compare_type = cmp_Ty;
		using index_type = index_Ty;

		logic_ctrl_observer_info() = default;
		inline logic_ctrl_observer_info(index_Ty _observer_index, cmp_Ty _cmp) noexcept;

		index_Ty observer_index = static_cast<index_Ty>(0);
		cmp_Ty cmp{};
	};

	// logic_ctrl_relation_info

	template <class cmp_Ty, class index_Ty> class logic_ctrl_relation_info
	{

	public:

		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using compare_type = cmp_Ty;
		using index_type = index_Ty;

		logic_ctrl_relation_info() = default;
		inline logic_ctrl_relation_info(cmp_Ty _cmp) noexcept;

		index_Ty observed_index = static_cast<index_Ty>(0);
		index_Ty observer_index = static_cast<index_Ty>(0);
		cmp_Ty cmp{};
	};
}


// detail

inline constexpr cool::variable_count::variable_count(std::size_t new_variable_count) noexcept : m_value(new_variable_count) {}

inline constexpr std::size_t cool::variable_count::value() const noexcept { return m_value; }

inline constexpr cool::max_relation_count::max_relation_count(std::size_t _max_relation_count) noexcept : m_value(_max_relation_count) {}

inline constexpr std::size_t cool::max_relation_count::value() const noexcept { return m_value; }

template <class cmp_Ty, class index_Ty>
inline cool::logic_ctrl_observed_info<cmp_Ty, index_Ty>::logic_ctrl_observed_info(index_Ty _observed_index, cmp_Ty _cmp) noexcept
	: observed_index(_observed_index), cmp(std::move(_cmp)) {}

template <class refresh_result_Ty>
inline constexpr cool::logic_ctrl_cmp<refresh_result_Ty>::logic_ctrl_cmp(unsigned char _trigger) noexcept : trigger(_trigger) {}

template <class refresh_result_Ty>
inline constexpr bool cool::logic_ctrl_cmp<refresh_result_Ty>::operator()(const refresh_result_Ty& new_value, const refresh_result_Ty& previous_value) const noexcept
{
	switch (trigger)
	{
	case logic_ctrl_cmp::change: return new_value != previous_value; break;
	case logic_ctrl_cmp::rising: return new_value > previous_value; break;
	case logic_ctrl_cmp::falling: return new_value < previous_value; break;
	default: return (trigger == logic_ctrl_cmp::always); break;
	}
}

template <class index_Ty>
inline cool::logic_ctrl_init_result<index_Ty>::operator bool() const noexcept
{
	return m_result == logic_ctrl_init_result::success;
}

template <class index_Ty>
inline bool cool::logic_ctrl_init_result<index_Ty>::good() const noexcept
{
	return m_result == logic_ctrl_init_result::success;
}

template <class index_Ty>
inline int cool::logic_ctrl_init_result<index_Ty>::value() const noexcept
{
	return m_result;
}

template <class index_Ty>
inline const char* cool::logic_ctrl_init_result<index_Ty>::message() const noexcept
{
	switch (m_result)
	{
	case logic_ctrl_init_result::success: return "cool logic_ctrl init success"; break;
	case logic_ctrl_init_result::init_ongoing: return "cool logic_ctrl init is ongoing"; break;
	case logic_ctrl_init_result::bad_parameters: return "cool logic_ctrl init failed : bad parameters"; break;
	case logic_ctrl_init_result::bad_alloc: return "cool logic_ctrl init failed : bad allocation"; break;
	case logic_ctrl_init_result::bad_observer_variable_index: return "cool logic_ctrl init failed : observer variable index out of bound"; break;
	case logic_ctrl_init_result::bad_observed_variable_index: return "cool logic_ctrl init failed : observed variable index out of bound"; break;
	case logic_ctrl_init_result::bad_relation_count: return "cool logic_ctrl init failed : maximum relation count is too low"; break;
	case logic_ctrl_init_result::bad_refresh_func_assignment: return "cool logic_ctrl init failed : double assignement of refresh functions on one variable"; break;
	case logic_ctrl_init_result::modification_after_init_end: return "cool logic_ctrl warning : modification added after init_end"; break;
	default: return "cool logic_ctrl init result undefined"; break;
	}
}

template <class index_Ty>
inline index_Ty cool::logic_ctrl_init_result<index_Ty>::index_if_init_failed() const noexcept
{
	return m_index;
}

template <class index_Ty>
cool::logic_ctrl_init_result<index_Ty>& cool::logic_ctrl_init_result<index_Ty>::operator&=(const cool::logic_ctrl_init_result<index_Ty>& rhs) noexcept
{
	if ((m_result == logic_ctrl_init_result::success) || (m_result == logic_ctrl_init_result::init_ongoing) || (m_result == logic_ctrl_init_result::undefined))
	{
		m_result = rhs.m_result;
		m_index = rhs.m_index;
	}

	return *this;
}

template <class index_Ty>
cool::logic_ctrl_init_result<index_Ty> cool::logic_ctrl_init_result<index_Ty>::operator&(const cool::logic_ctrl_init_result<index_Ty>& rhs) noexcept
{
	return ((m_result == logic_ctrl_init_result::success) || (m_result == logic_ctrl_init_result::init_ongoing) || (m_result == logic_ctrl_init_result::undefined)) ? rhs : *this;
}

template <class index_Ty>
inline cool::logic_ctrl_init_result<index_Ty>::logic_ctrl_init_result(int result) noexcept : m_result(result) {}

template <class index_Ty>
inline cool::logic_ctrl_init_result<index_Ty>::logic_ctrl_init_result(int result, index_Ty index) noexcept : m_result(result), m_index(index) {}

inline constexpr cool::max_depth::max_depth(int new_max_depth) noexcept : m_value(new_max_depth) {}

inline constexpr int cool::max_depth::value() const noexcept { return m_value; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::set_variable(index_Ty variable_index, arg_value_type new_value)
{
	set_variable(variable_index, new_value, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::set_variable(index_Ty variable_index, arg_value_type new_value, cool::max_depth _max_depth)
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));
	refresh_result_Ty previous_value = *(m_variables_ptr + static_cast<std::size_t>(variable_index));

	if (variable_info_ref.cmp(new_value, previous_value) && !variable_info_ref.locked)
	{
		*(m_variables_ptr + static_cast<std::size_t>(variable_index)) = new_value;
		int _max_depth_value = _max_depth.value();

		if (_max_depth_value > 0)
		{
			_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);

				if (observer_info_ref.cmp(new_value, previous_value))
				{
					variable_info_type& observer_variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(observer_info_ref.observer_index));

					set_variable(observer_info_ref.observer_index,
						observer_variable_info_ref.refresh_func(observer_info_ref.observer_index,
#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
							_logic_ctrl_base::variable_view(m_variables_ptr),
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
							_logic_ctrl_base::variable_view(m_variables_ptr, m_variable_count),
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
							observer_variable_info_ref.shared_data_ptr
						),
						cool::max_depth(_max_depth_value - 1)
					);
				}
			}
		}
	}
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::set_variable_no_cmp(index_Ty variable_index, arg_value_type new_value)
{
	set_variable_no_cmp(variable_index, new_value, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::set_variable_no_cmp(index_Ty variable_index, arg_value_type new_value, cool::max_depth _max_depth)
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (!variable_info_ref.locked)
	{
		*(m_variables_ptr + static_cast<std::size_t>(variable_index)) = new_value;
		int _max_depth_value = _max_depth.value();

		if (_max_depth_value > 0)
		{
			_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
				variable_info_type& observer_variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(observer_info_ref.observer_index));

				set_variable_no_cmp(observer_info_ref.observer_index,
					observer_variable_info_ref.refresh_func(observer_info_ref.observer_index,
#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						_logic_ctrl_base::variable_view(m_variables_ptr),
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						_logic_ctrl_base::variable_view(m_variables_ptr, m_variable_count),
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						observer_variable_info_ref.shared_data_ptr
					),
					cool::max_depth(_max_depth_value - 1)
				);
			}
		}
	}
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_variable(index_Ty variable_index)
{
	refresh_variable(variable_index, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_variable(index_Ty variable_index, cool::max_depth _max_depth)
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (!variable_info_ref.locked && (variable_info_ref.refresh_func != nullptr))
	{
		refresh_result_Ty new_value = variable_info_ref.refresh_func(variable_index,
			_logic_ctrl_base::variable_view(m_variables_ptr),
			variable_info_ref.shared_data_ptr
		);
		refresh_result_Ty previous_value = *(m_variables_ptr + static_cast<std::size_t>(variable_index));

		if (variable_info_ref.cmp(new_value, previous_value))
		{
			*(m_variables_ptr + static_cast<std::size_t>(variable_index)) = new_value;
			int _max_depth_value = _max_depth.value();

			if (_max_depth_value > 0)
			{
				_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

				for (std::size_t observer_index = variable_info_ref.observer_index_begin;
					observer_index < variable_info_ref.observer_index_end;
					observer_index++)
				{
					observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);

					if (observer_info_ref.cmp(new_value, previous_value))
					{
						variable_info_type& observer_variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(observer_info_ref.observer_index));

						set_variable(observer_info_ref.observer_index,
							observer_variable_info_ref.refresh_func(observer_info_ref.observer_index,
#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
								_logic_ctrl_base::variable_view(m_variables_ptr),
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
								_logic_ctrl_base::variable_view(m_variables_ptr, m_variable_count),
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
								observer_variable_info_ref.shared_data_ptr
							),
							cool::max_depth(_max_depth_value - 1)
						);
					}
				}
			}
		}
	}
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_variable_no_cmp(index_Ty variable_index)
{
	refresh_variable_no_cmp(variable_index, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::refresh_variable_no_cmp(index_Ty variable_index, cool::max_depth _max_depth)
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (!variable_info_ref.locked)
	{
		if (variable_info_ref.refresh_func != nullptr)
		{
			*(m_variables_ptr + static_cast<std::size_t>(variable_index)) = variable_info_ref.refresh_func(variable_index,
				_logic_ctrl_base::variable_view(m_variables_ptr),
				variable_info_ref.shared_data_ptr
			);
		}

		int _max_depth_value = _max_depth.value();

		if (_max_depth_value > 0)
		{
			_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
				variable_info_type& observer_variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(observer_info_ref.observer_index));

				set_variable_no_cmp(observer_info_ref.observer_index,
					observer_variable_info_ref.refresh_func(observer_info_ref.observer_index,
#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						_logic_ctrl_base::variable_view(m_variables_ptr),
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						_logic_ctrl_base::variable_view(m_variables_ptr, m_variable_count),
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
						observer_variable_info_ref.shared_data_ptr
					),
					cool::max_depth(_max_depth_value - 1)
				);
			}
		}
	}
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline Ty& cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::operator[](index_Ty variable_index) noexcept
{
	assert(m_variables_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return *(m_variables_ptr + static_cast<std::size_t>(variable_index));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty& cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::operator[](index_Ty variable_index) const noexcept
{
	assert(m_variables_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return *(m_variables_ptr + static_cast<std::size_t>(variable_index));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::data() noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::data() const noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::begin() noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::end() noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::begin() const noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::end() const noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::cbegin() const noexcept { return m_variables_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::cend() const noexcept { return m_variables_ptr + m_variable_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline bool& cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::locked(index_Ty variable_index) noexcept
{
	assert(m_variable_info_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return (m_variable_info_ptr + static_cast<std::size_t>(variable_index))->locked;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const bool& cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::locked(index_Ty variable_index) const noexcept
{
	assert(m_variable_info_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return (m_variable_info_ptr + static_cast<std::size_t>(variable_index))->locked;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void inline cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::lock_all(bool _locked) noexcept
{
	assert(m_variable_info_ptr != nullptr);

	for (std::size_t variable_index = 0; variable_index < m_variable_count; variable_index++)
	{
		(m_variable_info_ptr + variable_index)->locked = _locked;
	}
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline void* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_shared_data_ptr(index_Ty variable_index) noexcept
{
	assert(m_variable_info_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return (m_variable_info_ptr + static_cast<std::size_t>(variable_index))->shared_data_ptr;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const void* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_shared_data_ptr(index_Ty variable_index) const noexcept
{
	assert(m_variable_info_ptr != nullptr);
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	return (m_variable_info_ptr + static_cast<std::size_t>(variable_index))->shared_data_ptr;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline bool cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::good() const noexcept { return m_init == init_result_type::success; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline bool cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_ongoing() const noexcept { return m_init == init_result_type::init_ongoing; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline std::size_t cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_count() const noexcept { return m_variable_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline int cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::default_max_depth() const noexcept { return m_default_max_depth; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline std::size_t cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::relation_count() const noexcept { return m_relation_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline std::size_t cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::max_relation_count() const noexcept { return m_max_relation_count; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_get_observers_result<index_Ty> cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers(
	index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size) const noexcept
{
	return get_observers(variable_index, observer_list_optional_ptr, max_observer_list_size, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_get_observers_result<index_Ty> cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers(
	index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size, cool::max_depth _max_depth) const noexcept
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	cool::logic_ctrl_get_observers_result<index_Ty> obs_result;
	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (!variable_info_ref.locked)
	{
		_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

		for (std::size_t observer_index = variable_info_ref.observer_index_begin;
			observer_index < variable_info_ref.observer_index_end;
			observer_index++)
		{
			observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
			_get_observers_sub(observer_info_ref.observer_index, observer_list_optional_ptr, max_observer_list_size, _max_depth.value(), _max_depth.value(), obs_result);
		}
	}
	else
	{
		obs_result.lock_is_reached = true;
		obs_result.index_if_lock_is_reached = variable_index;
	}

	return obs_result;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_get_observers_sub(
	index_Ty variable_index, index_Ty* observer_list_optional_ptr, std::size_t max_observer_list_size, int _max_depth_value, int _max_depth_initial_value,
	cool::logic_ctrl_get_observers_result<index_Ty>& obs_result_ref) const noexcept
{
	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (_max_depth_value > 0)
	{
		if (!variable_info_ref.locked)
		{
			if ((observer_list_optional_ptr != nullptr) && (obs_result_ref.observer_count < max_observer_list_size))
			{
				*(observer_list_optional_ptr + obs_result_ref.observer_count) = variable_index;
			}

			obs_result_ref.observer_count++;

			{
				int new_depth = _max_depth_initial_value - _max_depth_value + 1;
				if (obs_result_ref.depth_reached < new_depth)
				{
					obs_result_ref.depth_reached = new_depth;
				}
			}

			_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
				_get_observers_sub(observer_info_ref.observer_index, observer_list_optional_ptr, max_observer_list_size, _max_depth_value - 1, _max_depth_initial_value, obs_result_ref);
			}
		}
		else if (!obs_result_ref.lock_is_reached)
		{
			obs_result_ref.lock_is_reached = true;
			obs_result_ref.index_if_lock_is_reached = variable_index;
		}
	}
	else
	{
		obs_result_ref.max_depth_is_reached = true;
	}
}

#ifdef COOL_LOGIC_CTRL_VECTOR
template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_get_observers_vec_result<index_Ty> cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_vec(index_Ty variable_index) const
{
	return get_observers_vec(variable_index, cool::max_depth(m_default_max_depth));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_get_observers_vec_result<index_Ty> cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::get_observers_vec(index_Ty variable_index, cool::max_depth _max_depth) const
{
	assert(good());
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);

	cool::logic_ctrl_get_observers_vec_result<index_Ty> obs_result;
	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (!variable_info_ref.locked)
	{
		_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

		for (std::size_t observer_index = variable_info_ref.observer_index_begin;
			observer_index < variable_info_ref.observer_index_end;
			observer_index++)
		{
			observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
			_get_observers_vec_sub(observer_info_ref.observer_index, _max_depth.value(), _max_depth.value(), obs_result);
		}
	}
	else
	{
		obs_result.lock_is_reached = true;
		obs_result.index_if_lock_is_reached = variable_index;
	}

	obs_result.observer_count = obs_result.observers.size();
	return obs_result;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_get_observers_vec_sub(
	index_Ty variable_index, int _max_depth_value, int _max_depth_initial_value,
	cool::logic_ctrl_get_observers_vec_result<index_Ty>& obs_result_ref) const
{
	variable_info_type& variable_info_ref = *(m_variable_info_ptr + static_cast<std::size_t>(variable_index));

	if (_max_depth_value > 0)
	{
		if (!variable_info_ref.locked)
		{
			obs_result_ref.observers.push_back(variable_index);

			{
				int new_depth = _max_depth_initial_value - _max_depth_value + 1;
				if (obs_result_ref.depth_reached < new_depth)
				{
					obs_result_ref.depth_reached = new_depth;
				}
			}

			_logic_ctrl_base::_lock_guard lock(variable_info_ref.locked);

			for (std::size_t observer_index = variable_info_ref.observer_index_begin;
				observer_index < variable_info_ref.observer_index_end;
				observer_index++)
			{
				observer_info_type& observer_info_ref = *(m_observer_info_ptr + observer_index);
				_get_observers_vec_sub(observer_info_ref.observer_index, _max_depth_value - 1, _max_depth_initial_value, obs_result_ref);
			}
		}
		else if (!obs_result_ref.lock_is_reached)
		{
			obs_result_ref.lock_is_reached = true;
			obs_result_ref.index_if_lock_is_reached = variable_index;
		}
	}
	else
	{
		obs_result_ref.max_depth_is_reached = true;
	}
}
#endif // COOL_LOGIC_CTRL_VECTOR

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_set_cmp(
	index_Ty variable_index, cmp_Ty cmp) noexcept
{
	index_Ty return_index = variable_index;

	if (m_init == init_result_type::init_ongoing)
	{
		if (static_cast<std::size_t>(variable_index) < m_variable_count)
		{
			(m_variable_info_ptr + static_cast<std::size_t>(variable_index))->cmp = std::move(cmp);
			return_index = static_cast<index_Ty>(0);
		}
		else
		{
			m_init = init_result_type::bad_observed_variable_index;
		}
	}
	else if (m_init == init_result_type::success)
	{
		m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	return init_result_type(m_init, return_index);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_set_shared_data_ptr(
	index_Ty variable_index, void* shared_data_ptr) noexcept
{
	index_Ty return_index = variable_index;

	if (m_init == init_result_type::init_ongoing)
	{
		if (static_cast<std::size_t>(variable_index) < m_variable_count)
		{
			(m_variable_info_ptr + static_cast<std::size_t>(variable_index))->shared_data_ptr = shared_data_ptr;
			return_index = static_cast<index_Ty>(0);
		}
		else
		{
			m_init = init_result_type::bad_observed_variable_index;
		}
	}
	else if (m_init == init_result_type::success)
	{
		m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	return init_result_type(m_init, return_index);
}

#ifndef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view::variable_view(const Ty* ptr) noexcept : m_ptr(ptr) {}
#else // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view::variable_view(const Ty* ptr, std::size_t _variable_count) noexcept : m_ptr(ptr), m_variable_count(_variable_count) {}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline std::size_t cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view::variable_count() const noexcept { return m_variable_count; }
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty& cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view::operator[](index_Ty variable_index) const noexcept
{
	assert(m_ptr != nullptr);
#ifdef COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT
	assert(static_cast<std::size_t>(variable_index) < m_variable_count);
#endif // COOL_LOGIC_CTRL_VIEW_VARIABLE_COUNT

	return *(m_ptr + static_cast<std::size_t>(variable_index));
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline const Ty* cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::variable_view::data() const noexcept { return m_ptr; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_delete_logic_ctrl_sub() noexcept
{
	this->m_variables_ptr = nullptr;
	this->m_variable_info_ptr = nullptr;
	this->m_observer_info_ptr = nullptr;

	this->m_default_max_depth = 0;

	this->m_variable_count = 0;
	this->m_init = init_result_type::undefined;

	this->m_relation_info_ptr = nullptr;
	this->m_relation_count = 0;
	this->m_max_relation_count = 0;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard::_lock_guard(bool& lock_ref) noexcept : m_ptr(&lock_ref) { lock_ref = true; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::_lock_guard::~_lock_guard() { *m_ptr = false; }

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::logic_ctrl(cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept
	: cool::_logic_ctrl_base<Ty, cmp_Ty, refresh_result_Ty, index_Ty, small_Ty>(rhs)
{
	rhs._delete_logic_ctrl_sub();
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::operator=(
	cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept
{
	this->m_variables_ptr = rhs.m_variables_ptr;
	this->m_variable_info_ptr = rhs.m_variable_info_ptr;
	this->m_observer_info_ptr = rhs.m_observer_info_ptr;

	this->m_default_max_depth = rhs.m_default_max_depth;
	this->m_init = rhs.m_init;

	this->m_variable_count = rhs.m_variable_count;

	this->m_relation_info_ptr = rhs.m_relation_info_ptr;
	this->m_relation_count = rhs.m_relation_count;
	this->m_max_relation_count = rhs.m_max_relation_count;

	rhs._delete_logic_ctrl_sub();

	return *this;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_logic_ctrl_begin(
	Ty* variables_ptr,
	variable_info_type* variable_info_ptr,
	cool::variable_count new_variable_count,
	observer_info_type* observer_info_ptr,
	relation_info_type* relation_info_ptr,
	max_relation_count new_max_relation_count,
	cool::max_depth new_default_max_depth,
	cmp_Ty default_cmp)
{
	assert(variables_ptr != nullptr);
	assert(variable_info_ptr != nullptr);
	assert(observer_info_ptr != nullptr);
	assert(relation_info_ptr != nullptr);

	std::size_t _new_variable_count = new_variable_count.value();
	std::size_t _new_max_relation_count = new_max_relation_count.value();

	if ((variables_ptr == nullptr) || (variable_info_ptr == nullptr) || (_new_variable_count == 0)
		|| (observer_info_ptr == nullptr) || (relation_info_ptr == nullptr))
	{
		this->m_init = init_result_type::bad_parameters;
	}
	else
	{
		this->m_variables_ptr = variables_ptr;
		this->m_variable_info_ptr = variable_info_ptr;
		this->m_observer_info_ptr = observer_info_ptr;

		this->m_default_max_depth = new_default_max_depth.value();
		this->m_init = init_result_type::init_ongoing;

		this->m_variable_count = _new_variable_count;

		this->m_relation_info_ptr = relation_info_ptr;
		this->m_relation_count = 0;
		this->m_max_relation_count = _new_max_relation_count;

		for (std::size_t n = 0; n < _new_variable_count; n++)
		{
			*(this->m_variables_ptr + n) = refresh_result_Ty{};
			*(this->m_variable_info_ptr + n) = variable_info_type(default_cmp);
		}

		for (std::size_t n = 0; n < _new_max_relation_count; n++)
		{
			*(this->m_observer_info_ptr + n) = observer_info_type(static_cast<index_Ty>(0), default_cmp);
			*(this->m_relation_info_ptr + n) = relation_info_type(default_cmp);
		}
	}

	return init_result_type(this->m_init);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, std::initializer_list<index_Ty> observed_variables, refresh_func_type refresh_func) noexcept
{
	return init_add_relations(observer_variable_index, observed_variables.begin(), observed_variables.size(), refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, const index_Ty* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func) noexcept
{
	assert(refresh_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	index_Ty return_index = observer_variable_index;

	if (this->m_init == init_result_type::init_ongoing)
	{
		if ((refresh_func == nullptr) || ((observed_variables_ptr == nullptr) && (observed_variable_count != 0)))
		{
			this->m_init = init_result_type::bad_parameters;
		}
		else if (!(static_cast<std::size_t>(observer_variable_index) < this->m_variable_count))
		{
			this->m_init = init_result_type::bad_observer_variable_index;
		}
		else if ((this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func != nullptr)
		{
			this->m_init = init_result_type::bad_refresh_func_assignment;
		}
		else
		{
			(this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func = refresh_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				index_Ty observed_variable = *(observed_variables_ptr + n);

				if (!(static_cast<std::size_t>(observed_variable) < this->m_variable_count))
				{
					this->m_init = init_result_type::bad_observed_variable_index;
					break;
				}
				else if (!(this->m_relation_count < this->m_max_relation_count))
				{
					this->m_init = init_result_type::bad_relation_count;
					break;
				}
				else if (observed_variable != observer_variable_index)
				{
					(this->m_relation_info_ptr + this->m_relation_count)->observed_index = observed_variable;
					(this->m_relation_info_ptr + this->m_relation_count)->observer_index = observer_variable_index;
					this->m_relation_count++;
					return_index = static_cast<index_Ty>(0);
				}
			}
		}
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	return init_result_type(this->m_init, return_index);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, std::initializer_list<observed_info_type> observed_variables, refresh_func_type refresh_func) noexcept(std::is_nothrow_copy_assignable<cmp_Ty>::value)
{
	return init_add_relations(observer_variable_index, observed_variables.begin(), observed_variables.size(), refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, const observed_info_type* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func) noexcept(std::is_nothrow_copy_assignable<cmp_Ty>::value)
{
	assert(refresh_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	index_Ty return_index = observer_variable_index;

	if (this->m_init == init_result_type::init_ongoing)
	{
		if ((refresh_func == nullptr) || ((observed_variables_ptr == nullptr) && (observed_variable_count != 0)))
		{
			this->m_init = init_result_type::bad_parameters;
		}
		else if (!(static_cast<std::size_t>(observer_variable_index) < this->m_variable_count))
		{
			this->m_init = init_result_type::bad_observer_variable_index;
		}
		else if ((this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func != nullptr)
		{
			this->m_init = init_result_type::bad_refresh_func_assignment;
		}
		else
		{
			(this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func = refresh_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				const observed_info_type& observed_variable_ref = *(observed_variables_ptr + n);

				if (!(static_cast<std::size_t>(observed_variable_ref.observed_index) < this->m_variable_count))
				{
					this->m_init = init_result_type::bad_observed_variable_index;
					break;
				}
				else if (!(this->m_relation_count < this->m_max_relation_count))
				{
					this->m_init = init_result_type::bad_relation_count;
					break;
				}
				else if (observed_variable_ref.observed_index != observer_variable_index)
				{
					(this->m_relation_info_ptr + this->m_relation_count)->observed_index = observed_variable_ref.observed_index;
					(this->m_relation_info_ptr + this->m_relation_count)->observer_index = observer_variable_index;
					(this->m_relation_info_ptr + this->m_relation_count)->cmp = observed_variable_ref.cmp;
					this->m_relation_count++;
					return_index = static_cast<index_Ty>(0);
				}
			}
		}
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	return init_result_type(this->m_init, return_index);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, refresh_func_type refresh_func) noexcept
{
	return init_add_relations(observer_variable_index, static_cast<const index_Ty*>(nullptr), 0, refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_logic_ctrl_end()
{
	if (this->m_init == init_result_type::init_ongoing)
	{
		this->m_init = init_result_type::success;

		std::sort(this->m_relation_info_ptr, this->m_relation_info_ptr + this->m_relation_count,
			[](const relation_info_type& lhs, const relation_info_type& rhs) {
				std::size_t lhs_observed_index = static_cast<std::size_t>(lhs.observed_index);
				std::size_t rhs_observed_index = static_cast<std::size_t>(rhs.observed_index);
				std::size_t lhs_observer_index = static_cast<std::size_t>(lhs.observer_index);
				std::size_t rhs_observer_index = static_cast<std::size_t>(rhs.observer_index);
				return (lhs_observed_index < rhs_observed_index) || ((lhs_observed_index == rhs_observed_index) && (lhs_observer_index < rhs_observer_index));
			}
		);

		std::size_t current_rel = 0;
		std::size_t observer_count = 0;
		std::size_t last_observer_index;

		for (std::size_t observed_variable_index = 0; observed_variable_index < this->m_variable_count; observed_variable_index++)
		{
			(this->m_variable_info_ptr + observed_variable_index)->observer_index_begin = observer_count;
			last_observer_index = static_cast<std::size_t>(-1);

			while (current_rel < this->m_relation_count)
			{
				relation_info_type& current_rel_ref = *(this->m_relation_info_ptr + current_rel);

				if (static_cast<std::size_t>(current_rel_ref.observed_index) == observed_variable_index)
				{
					if (last_observer_index != static_cast<std::size_t>(current_rel_ref.observer_index))
					{
						last_observer_index = static_cast<std::size_t>(current_rel_ref.observer_index);
						(this->m_observer_info_ptr + observer_count)->observer_index = current_rel_ref.observer_index;
						(this->m_observer_info_ptr + observer_count)->cmp = std::move(current_rel_ref.cmp);

						observer_count++;
					}

					current_rel++;
				}
				else
				{
					break;
				}
			}

			(this->m_variable_info_ptr + observed_variable_index)->observer_index_end = observer_count;
		}

		this->m_relation_count = observer_count;
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}

	return init_result_type(this->m_init);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::logic_ctrl<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::delete_logic_ctrl() noexcept
{
	this->_delete_logic_ctrl_sub();
}

#ifdef COOL_LOGIC_CTRL_VECTOR
template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::logic_ctrl_vec(cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept
	: cool::_logic_ctrl_base<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>(rhs),
	m_observer_info_vec(std::move(rhs.m_observer_info_vec)),
	m_relation_info_vec(std::move(rhs.m_relation_info_vec))
{
	rhs._delete_logic_ctrl_sub();
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>& cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::operator=(
	cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>&& rhs) noexcept
{
	this->m_variables_ptr = rhs.m_variables_ptr;
	this->m_variable_info_ptr = rhs.m_variable_info_ptr;
	this->m_observer_info_ptr = rhs.m_observer_info_ptr;

	this->m_default_max_depth = rhs.m_default_max_depth;
	this->m_init = rhs.m_init;

	this->m_variable_count = rhs.m_variable_count;

	this->m_relation_info_ptr = rhs.m_relation_info_ptr;
	this->m_relation_count = rhs.m_relation_count;
	this->m_max_relation_count = rhs.m_max_relation_count;

	m_observer_info_vec = std::move(rhs.m_observer_info_vec);
	m_relation_info_vec = std::move(rhs.m_relation_info_vec);

	rhs._delete_logic_ctrl_sub();

	return *this;
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::logic_ctrl_vec::~logic_ctrl_vec()
{
	delete_logic_ctrl();
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_new_logic_ctrl_begin(
	cool::variable_count new_variable_count,
	cool::max_depth new_default_max_depth,
	cmp_Ty default_cmp)
{
	std::size_t _new_variable_count = new_variable_count.value();
	if (_new_variable_count == 0)
	{
		this->m_init = init_result_type::bad_parameters;
	}
	else
	{
		delete_logic_ctrl();

		this->m_variables_ptr = static_cast<Ty*>(::operator new(_new_variable_count * sizeof(Ty), std::nothrow));

		if (this->m_variables_ptr != nullptr)
		{
			for (std::size_t n = 0; n < _new_variable_count; n++)
			{
				new (this->m_variables_ptr + n) Ty{};
			}
		}
		else
		{
			this->m_init = init_result_type::bad_alloc;
			return init_result_type(this->m_init);
		}

		this->m_variable_info_ptr = static_cast<variable_info_type*>(::operator new(_new_variable_count * sizeof(variable_info_type), std::nothrow));

		if (this->m_variable_info_ptr != nullptr)
		{
			for (std::size_t n = 0; n < _new_variable_count; n++)
			{
				new (this->m_variable_info_ptr + n) variable_info_type{ default_cmp };
			}
		}
		else
		{
			delete_logic_ctrl();
			this->m_init = init_result_type::bad_alloc;
			return init_result_type(this->m_init);
		}

		this->m_default_max_depth = new_default_max_depth.value();
		this->m_init = init_result_type::init_ongoing;

		this->m_variable_count = _new_variable_count;

		m_relation_info_vec.reserve(_new_variable_count);
		this->m_max_relation_count = static_cast<std::size_t>(-1);
	}

	return init_result_type(this->m_init);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, std::initializer_list<index_Ty> observed_variables, refresh_func_type refresh_func)
{
	return init_add_relations(observer_variable_index, observed_variables.begin(), observed_variables.size(), refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, const index_Ty* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func)
{
	assert(refresh_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	index_Ty return_index = observer_variable_index;

	if (this->m_init == init_result_type::init_ongoing)
	{
		if ((refresh_func == nullptr) || ((observed_variables_ptr == nullptr) && (observed_variable_count != 0)))
		{
			this->m_init = init_result_type::bad_parameters;
		}
		else if (!(static_cast<std::size_t>(observer_variable_index) < this->m_variable_count))
		{
			this->m_init = init_result_type::bad_observer_variable_index;
		}
		else if ((this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func != nullptr)
		{
			this->m_init = init_result_type::bad_refresh_func_assignment;
		}
		else
		{
			(this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func = refresh_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				index_Ty observed_variable = *(observed_variables_ptr + n);

				if (!(static_cast<std::size_t>(observed_variable) < this->m_variable_count))
				{
					this->m_init = init_result_type::bad_observed_variable_index;
					break;
				}
				else if (observed_variable != observer_variable_index)
				{
					relation_info_type _relation_info;
					_relation_info.observed_index = observed_variable;
					_relation_info.observer_index = observer_variable_index;
					m_relation_info_vec.push_back(_relation_info);
					return_index = static_cast<index_Ty>(0);
				}
			}
		}
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	this->m_relation_count = m_relation_info_vec.size();
	this->m_relation_info_ptr = m_relation_info_vec.data();
	return init_result_type(this->m_init, return_index);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, std::initializer_list<observed_info_type> observed_variables, refresh_func_type refresh_func)
{
	return init_add_relations(observer_variable_index, observed_variables.begin(), observed_variables.size(), refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, const observed_info_type* observed_variables_ptr, std::size_t observed_variable_count, refresh_func_type refresh_func)
{
	assert(refresh_func != nullptr);
	assert((observed_variables_ptr != nullptr) || (observed_variable_count == 0));

	index_Ty return_index = observer_variable_index;

	if (this->m_init == init_result_type::init_ongoing)
	{
		if ((refresh_func == nullptr) || ((observed_variables_ptr == nullptr) && (observed_variable_count != 0)))
		{
			this->m_init = init_result_type::bad_parameters;
		}
		else if (!(static_cast<std::size_t>(observer_variable_index) < this->m_variable_count))
		{
			this->m_init = init_result_type::bad_observer_variable_index;
		}
		else if ((this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func != nullptr)
		{
			this->m_init = init_result_type::bad_refresh_func_assignment;
		}
		else
		{
			(this->m_variable_info_ptr + static_cast<std::size_t>(observer_variable_index))->refresh_func = refresh_func;

			for (std::size_t n = 0; n < observed_variable_count; n++)
			{
				const observed_info_type& observed_variable_ref = *(observed_variables_ptr + n);

				if (!(static_cast<std::size_t>(observed_variable_ref.observed_index) < this->m_variable_count))
				{
					this->m_init = init_result_type::bad_observed_variable_index;
					break;
				}
				else if (observed_variable_ref.observed_index != observer_variable_index)
				{
					relation_info_type _relation_info(observed_variable_ref.cmp);
					_relation_info.observed_index = observed_variable_ref.observed_index;
					_relation_info.observer_index = observer_variable_index;
					m_relation_info_vec.push_back(_relation_info);
					return_index = static_cast<index_Ty>(0);
				}
			}
		}
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}
	else
	{
		return_index = static_cast<index_Ty>(0);
	}

	this->m_relation_count = m_relation_info_vec.size();
	this->m_relation_info_ptr = m_relation_info_vec.data();
	return init_result_type(this->m_init, return_index);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_add_relations(
	index_Ty observer_variable_index, refresh_func_type refresh_func) noexcept
{
	return init_add_relations(observer_variable_index, static_cast<const index_Ty*>(nullptr), 0, refresh_func);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
typename cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_result_type cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::init_logic_ctrl_end()
{
	if (this->m_init == init_result_type::init_ongoing)
	{
		this->m_init = init_result_type::success;

		std::sort(this->m_relation_info_ptr, this->m_relation_info_ptr + this->m_relation_count,
			[](const relation_info_type& lhs, const relation_info_type& rhs) {
				std::size_t lhs_observed_index = static_cast<std::size_t>(lhs.observed_index);
				std::size_t rhs_observed_index = static_cast<std::size_t>(rhs.observed_index);
				std::size_t lhs_observer_index = static_cast<std::size_t>(lhs.observer_index);
				std::size_t rhs_observer_index = static_cast<std::size_t>(rhs.observer_index);
				return (lhs_observed_index < rhs_observed_index) || ((lhs_observed_index == rhs_observed_index) && (lhs_observer_index < rhs_observer_index));
			}
		);

		std::size_t current_rel = 0;
		std::size_t last_observer_index;

		for (std::size_t observed_variable_index = 0; observed_variable_index < this->m_variable_count; observed_variable_index++)
		{
			(this->m_variable_info_ptr + observed_variable_index)->observer_index_begin = m_observer_info_vec.size();
			last_observer_index = static_cast<std::size_t>(-1);

			while (current_rel < this->m_relation_count)
			{
				relation_info_type& current_rel_ref = *(this->m_relation_info_ptr + current_rel);

				if (static_cast<std::size_t>(current_rel_ref.observed_index) == observed_variable_index)
				{
					if (last_observer_index != static_cast<std::size_t>(current_rel_ref.observer_index))
					{
						last_observer_index = static_cast<std::size_t>(current_rel_ref.observer_index);
						m_observer_info_vec.push_back(observer_info_type(current_rel_ref.observer_index, std::move(current_rel_ref.cmp)));
					}

					current_rel++;
				}
				else
				{
					break;
				}
			}

			(this->m_variable_info_ptr + observed_variable_index)->observer_index_end = m_observer_info_vec.size();
		}

		this->m_relation_count = m_observer_info_vec.size();
		m_observer_info_vec.shrink_to_fit();
		this->m_observer_info_ptr = m_observer_info_vec.data();
	}
	else if (this->m_init == init_result_type::success)
	{
		this->m_init = init_result_type::modification_after_init_end;
	}

	m_relation_info_vec.clear();
	this->m_relation_info_ptr = nullptr;
	return init_result_type(this->m_init);
}

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
void cool::logic_ctrl_vec<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::delete_logic_ctrl() noexcept
{
	m_relation_info_vec.clear();
	m_observer_info_vec.clear();

	if (this->m_variable_info_ptr != nullptr)
	{
		for (std::size_t n = this->m_variable_count; n > 0; )
		{
			n--;
			(this->m_variable_info_ptr + n)->~variable_info_type();
		}

		::operator delete(this->m_variable_info_ptr);
	}

	if (this->m_variables_ptr != nullptr)
	{
		for (std::size_t n = this->m_variable_count; n > 0; )
		{
			n--;
			(this->m_variables_ptr + n)->~Ty();
		}

		::operator delete(this->m_variables_ptr);
	}

	this->_delete_logic_ctrl_sub();
}
#endif // COOL_LOGIC_CTRL_VECTOR

template <class Ty, class cmp_Ty, class index_Ty, class refresh_result_Ty, bool small_Ty>
inline cool::logic_ctrl_variable_info<Ty, cmp_Ty, index_Ty, refresh_result_Ty, small_Ty>::logic_ctrl_variable_info(cmp_Ty _cmp) noexcept : cmp(std::move(_cmp)) {}

template <class cmp_Ty, class index_Ty>
inline cool::logic_ctrl_observer_info<cmp_Ty, index_Ty>::logic_ctrl_observer_info(index_Ty _observer_index, cmp_Ty _cmp) noexcept
	: observer_index(_observer_index), cmp(std::move(_cmp)) {}

template <class cmp_Ty, class index_Ty>
inline cool::logic_ctrl_relation_info<cmp_Ty, index_Ty>::logic_ctrl_relation_info(cmp_Ty _cmp) noexcept : cmp(std::move(_cmp)) {}

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
