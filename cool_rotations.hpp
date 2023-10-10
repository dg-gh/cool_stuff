// cool_rotations.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef _COOL_ROTATIONS_HPP
#define _COOL_ROTATIONS_HPP

#include <cstddef>
#include <cmath>


namespace cool
{
	// UV rotation matrix is :
	// rotation matrix around axis U * ...
	// rotation matrix around axis V

	// UVW rotation matrix is :
	// rotation matrix around axis U * ...
	// rotation matrix around axis V * ...
	// rotation matrix around axis W

	// U, V, W being X = (1, 0, 0), Y = (0, 1, 0), Z = (0, 0, 1)

	namespace rotation_subroutine
	{
		// template specializable functions, default to <cmath> functions
		template <class Ty, int _func_impl_number> static inline Ty cos(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty sin(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty sqrt(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty inv_sqrt(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty acos(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty asin(Ty x) noexcept;
		template <class Ty, int _func_impl_number> static inline Ty atan2(Ty y, Ty x) noexcept;
	}

#ifndef _COOL_MATRIX_LAYOUT_ENUM
#define _COOL_MATRIX_LAYOUT_ENUM
	enum class matrix_layout
	{
		col = 0, // indicates usage of column-major matrices
		row = 1 // indicates usage of row-major matrices
	};
#endif // _COOL_MATRIX_LAYOUT_ENUM

	enum class rotation_status
	{
		regular = 0,
		singular = 1,
	};

	namespace axis
	{
		static constexpr std::size_t X = 0;
		static constexpr std::size_t Y = 1;
		static constexpr std::size_t Z = 2;
		static constexpr std::size_t none = 3;
	}

	template <class Ty, std::size_t _dim_padded = 2, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotation2d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static inline void get_matrix(Ty* m2x2_rotation_ptr, Ty angle) noexcept;
		static inline void get_angle(Ty* angle_ptr, const Ty* m2x2_rotation_ptr) noexcept;

	private:

		static_assert(_dim_padded >= 2, "dimension too small");

		static constexpr std::size_t i00 = (_layout == cool::matrix_layout::col) ? 0 + _dim_padded * 0 : _dim_padded * 0 + 0;
		static constexpr std::size_t i10 = (_layout == cool::matrix_layout::col) ? 1 + _dim_padded * 0 : _dim_padded * 1 + 0;

		static constexpr std::size_t i01 = (_layout == cool::matrix_layout::col) ? 0 + _dim_padded * 1 : _dim_padded * 0 + 1;
		static constexpr std::size_t i11 = (_layout == cool::matrix_layout::col) ? 1 + _dim_padded * 1 : _dim_padded * 1 + 1;
	};


	template <std::size_t _dim_padded, cool::matrix_layout _layout> class _rotation3d_data
	{

	public:

		static_assert(_dim_padded >= 3, "dimension too small");

		static constexpr std::size_t i00 = (_layout == cool::matrix_layout::col) ? 0 + _dim_padded * 0 : _dim_padded * 0 + 0;
		static constexpr std::size_t i10 = (_layout == cool::matrix_layout::col) ? 1 + _dim_padded * 0 : _dim_padded * 1 + 0;
		static constexpr std::size_t i20 = (_layout == cool::matrix_layout::col) ? 2 + _dim_padded * 0 : _dim_padded * 2 + 0;

		static constexpr std::size_t i01 = (_layout == cool::matrix_layout::col) ? 0 + _dim_padded * 1 : _dim_padded * 0 + 1;
		static constexpr std::size_t i11 = (_layout == cool::matrix_layout::col) ? 1 + _dim_padded * 1 : _dim_padded * 1 + 1;
		static constexpr std::size_t i21 = (_layout == cool::matrix_layout::col) ? 2 + _dim_padded * 1 : _dim_padded * 2 + 1;

		static constexpr std::size_t i02 = (_layout == cool::matrix_layout::col) ? 0 + _dim_padded * 2 : _dim_padded * 0 + 2;
		static constexpr std::size_t i12 = (_layout == cool::matrix_layout::col) ? 1 + _dim_padded * 2 : _dim_padded * 1 + 2;
		static constexpr std::size_t i22 = (_layout == cool::matrix_layout::col) ? 2 + _dim_padded * 2 : _dim_padded * 2 + 2;
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXYZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXYZ_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXZY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXZY_ptr, const Ty* m3x3_rotation, Ty tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYZX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYZX_ptr, const Ty* m3x3_rotation, Ty tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYXZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYXZ_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZXY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZXY_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZYX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(-3.14159265358979 * 0.5),
			static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZYX_ptr, const Ty* m3x3_rotation, Ty tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXYX2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXYX2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationXZX2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXZX2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYZY2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYZY2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationYXY2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYXY2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZXZ2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 3;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZXZ2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotationZYZ2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0), static_cast<Ty>(3.14159265358979 * 0.5)
		};

		static constexpr std::size_t iX = 3;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t i_singular_axis = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZYZ2_ptr, const Ty* m3x3_rotation_ptr, Ty tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	enum no_axis_norm_t { no_axis_norm };

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotation_axis
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty tol = static_cast<Ty>(0)) noexcept;
		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty tol) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr,
			const Ty* v3_axis_way_ptr, Ty tol) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};


	enum no_quaternion_norm_t { no_quaternion_norm };

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = cool::matrix_layout::col,
		int _func_impl_number = 0> class rotation_quaternion
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept;
		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept;
		static inline void get_quaternion_from_matrix(Ty* v4_quaternion_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline void get_quaternion_from_axis_angle(Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle,
			Ty tol = static_cast<Ty>(0)) noexcept;
		static inline void get_quaternion_from_axis_angle(Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle,
			cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle, Ty* v4_quaternion_ptr, Ty tol) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle, Ty* v4_quaternion_ptr,
			const Ty* v3_axis_way_ptr, Ty tol) noexcept;

	private:

		static constexpr cool::_rotation3d_data<_dim_padded, _layout> dat
			= cool::_rotation3d_data<_dim_padded, _layout>();
	};
}


// detail

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::cos(Ty x) noexcept
{
	return std::cos(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::sin(Ty x) noexcept
{
	return std::sin(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::sqrt(Ty x) noexcept
{
	return std::sqrt(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::inv_sqrt(Ty x) noexcept
{
	return static_cast<Ty>(1) / std::sqrt(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::acos(Ty x) noexcept
{
	return std::acos(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::asin(Ty x) noexcept
{
	return std::asin(x);
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::atan2(Ty y, Ty x) noexcept
{
	return std::atan2(y, x);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation2d<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m2x2_rotation_ptr, Ty angle) noexcept
{
	Ty cosA = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinA = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m2x2_rotation_ptr + i00) = cosA;
	*(m2x2_rotation_ptr + i10) = sinA;
	*(m2x2_rotation_ptr + i01) = -sinA;
	*(m2x2_rotation_ptr + i11) = cosA;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation2d<Ty, _dim_padded, _layout, _func_impl_number>::get_angle(
	Ty* angle_ptr, const Ty* m2x2_rotation_ptr) noexcept
{
	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m2x2_rotation_ptr + i10),
		*(m2x2_rotation_ptr + i00));
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	Ty angle = *angle_ptr;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + dat.i00) = static_cast<Ty>(1);
	*(m3x3_rotation_ptr + dat.i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i11) = cosX;
	*(m3x3_rotation_ptr + dat.i21) = sinX;

	*(m3x3_rotation_ptr + dat.i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i12) = -sinX;
	*(m3x3_rotation_ptr + dat.i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m3x3_rotation_ptr + dat.i21),
		*(m3x3_rotation_ptr + dat.i11));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(angle_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	Ty angle = *angle_ptr;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + dat.i00) = cosY;
	*(m3x3_rotation_ptr + dat.i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i20) = -sinY;

	*(m3x3_rotation_ptr + dat.i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i11) = static_cast<Ty>(1);
	*(m3x3_rotation_ptr + dat.i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i02) = sinY;
	*(m3x3_rotation_ptr + dat.i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		-*(m3x3_rotation_ptr + dat.i20),
		*(m3x3_rotation_ptr + dat.i00));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(angle_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	Ty angle = *angle_ptr;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + dat.i00) = cosZ;
	*(m3x3_rotation_ptr + dat.i10) = sinZ;
	*(m3x3_rotation_ptr + dat.i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i01) = -sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosZ;
	*(m3x3_rotation_ptr + dat.i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i22) = static_cast<Ty>(1);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m3x3_rotation_ptr + dat.i10),
		*(m3x3_rotation_ptr + dat.i00));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(angle_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rXY_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rXY_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rXY_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rXY_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosY;
	*(m3x3_rotation_ptr + dat.i10) = sinX * sinY;
	*(m3x3_rotation_ptr + dat.i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + dat.i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i11) = cosX;
	*(m3x3_rotation_ptr + dat.i21) = sinX;

	*(m3x3_rotation_ptr + dat.i02) = sinY;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosY;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rXY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		*(m3x3_rotation_ptr + dat.i21),
		*(m3x3_rotation_ptr + dat.i11));
	*(v2_rXY_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		*(m3x3_rotation_ptr + dat.i02),
		*(m3x3_rotation_ptr + dat.i00));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rXY_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rXZ_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rXZ_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rXZ_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rXZ_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosZ;
	*(m3x3_rotation_ptr + dat.i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = sinX * sinZ;

	*(m3x3_rotation_ptr + dat.i01) = -sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosZ;

	*(m3x3_rotation_ptr + dat.i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i12) = -sinX;
	*(m3x3_rotation_ptr + dat.i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rXZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		-*(m3x3_rotation_ptr + dat.i12),
		*(m3x3_rotation_ptr + dat.i22));
	*(v2_rXZ_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		-*(m3x3_rotation_ptr + dat.i01),
		*(m3x3_rotation_ptr + dat.i00));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rXZ_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rYZ_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rYZ_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rYZ_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rYZ_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -sinY * cosZ;

	*(m3x3_rotation_ptr + dat.i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinY * sinZ;

	*(m3x3_rotation_ptr + dat.i02) = sinY;
	*(m3x3_rotation_ptr + dat.i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rYZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		*(m3x3_rotation_ptr + dat.i02),
		*(m3x3_rotation_ptr + dat.i22));
	*(v2_rYZ_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		*(m3x3_rotation_ptr + dat.i10),
		*(m3x3_rotation_ptr + dat.i11));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rYZ_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rYX_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rYX_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rYX_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rYX_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosY;
	*(m3x3_rotation_ptr + dat.i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + dat.i20) = -sinY;

	*(m3x3_rotation_ptr + dat.i01) = sinX * sinY;
	*(m3x3_rotation_ptr + dat.i11) = cosX;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosY;

	*(m3x3_rotation_ptr + dat.i02) = cosX * sinY;
	*(m3x3_rotation_ptr + dat.i12) = -sinX;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rYX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		-*(m3x3_rotation_ptr + dat.i20),
		*(m3x3_rotation_ptr + dat.i00));
	*(v2_rYX_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX;
		-*(m3x3_rotation_ptr + dat.i12),
		*(m3x3_rotation_ptr + dat.i11));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rYX_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rZX_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rZX_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rZX_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rZX_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosZ;
	*(m3x3_rotation_ptr + dat.i10) = sinZ;
	*(m3x3_rotation_ptr + dat.i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i01) = -cosX * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX;

	*(m3x3_rotation_ptr + dat.i02) = sinX * sinZ;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosZ;
	*(m3x3_rotation_ptr + dat.i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rZX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		*(m3x3_rotation_ptr + dat.i10),
		*(m3x3_rotation_ptr + dat.i00));
	*(v2_rZX_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		*(m3x3_rotation_ptr + dat.i21),
		*(m3x3_rotation_ptr + dat.i22));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rZX_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rZY_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rZY_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rZY_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rZY_ptr + 1));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -sinY;

	*(m3x3_rotation_ptr + dat.i01) = -sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosZ;
	*(m3x3_rotation_ptr + dat.i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + dat.i02) = sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i12) = sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	*v2_rZY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		-*(m3x3_rotation_ptr + dat.i01),
		*(m3x3_rotation_ptr + dat.i11));
	*(v2_rZY_ptr + 1) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		-*(m3x3_rotation_ptr + dat.i20),
		*(m3x3_rotation_ptr + dat.i22));

	return cool::rotation_status::regular;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr, Ty) noexcept
{
	return cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(v2_rZY_ptr, m3x3_rotation_ptr);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXYZ_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXYZ_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 1));
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = cosX * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i20) = sinX * sinZ - cosX * sinY * cosZ;

	*(m3x3_rotation_ptr + dat.i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ - sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosZ + cosX * sinY * sinZ;

	*(m3x3_rotation_ptr + dat.i02) = sinY;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosY;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXYZ_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rX_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i02) && *(m3x3_rotation_ptr + dat.i02) < bound)
	{
		Ty rY = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i02));
		*(v3_rXYZ_ptr + 1) = rY; // rY
		Ty cosYinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rY);
		*v3_rXYZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			-cosYinv * *(m3x3_rotation_ptr + dat.i12),
			cosYinv * *(m3x3_rotation_ptr + dat.i22));
		*(v3_rXYZ_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			-cosYinv * *(m3x3_rotation_ptr + dat.i01),
			cosYinv * *(m3x3_rotation_ptr + dat.i00));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rXYZ_ptr = rX_choice_if_singular; // rX
		Ty rZ0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i10),
			*(m3x3_rotation_ptr + dat.i11));
		bool Yneg = *(m3x3_rotation_ptr + dat.i02) < static_cast<Ty>(0);
		*(v3_rXYZ_ptr + 1) = Yneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rY
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Yneg ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rXYZ_ptr + 2) = rZ0 - flip * rX_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXZY_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXZY_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZY_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZY_ptr + 1));
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZY_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZY_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = sinX * sinY + cosX * cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -cosX * sinY + sinX * cosY * sinZ;

	*(m3x3_rotation_ptr + dat.i01) = -sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosZ;

	*(m3x3_rotation_ptr + dat.i02) = sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosY + cosX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY + sinX * sinY * sinZ;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXZY_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rX_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i01) && *(m3x3_rotation_ptr + dat.i01) < bound)
	{
		Ty rZ = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i01));
		*(v3_rXZY_ptr + 1) = rZ; // rZ
		Ty cosZinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rZ);
		*v3_rXZY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			cosZinv * *(m3x3_rotation_ptr + dat.i21),
			cosZinv * *(m3x3_rotation_ptr + dat.i11));
		*(v3_rXZY_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			cosZinv * *(m3x3_rotation_ptr + dat.i02),
			cosZinv * *(m3x3_rotation_ptr + dat.i00));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rXZY_ptr = rX_choice_if_singular; // rX
		Ty rY0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i20),
			*(m3x3_rotation_ptr + dat.i22));
		bool Zneg = !(*(m3x3_rotation_ptr + dat.i01) < static_cast<Ty>(0));
		*(v3_rXZY_ptr + 1) = Zneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rZ
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Zneg ? static_cast<Ty>(1) : static_cast<Ty>(-1);
		*(v3_rXZY_ptr + 2) = rY0 - flip * rX_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYZX_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYZX_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZX_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZX_ptr + 1));
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZX_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZX_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -sinY * cosZ;

	*(m3x3_rotation_ptr + dat.i01) = sinX * sinY - cosX * cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosY + cosX * sinY * sinZ;

	*(m3x3_rotation_ptr + dat.i02) = cosX * sinY + sinX * cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosZ;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY - sinX * sinY * sinZ;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYZX_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rY_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i10) && *(m3x3_rotation_ptr + dat.i10) < bound)
	{
		Ty rZ = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i10));
		*(v3_rYZX_ptr + 1) = rZ; // rZ
		Ty cosZinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rZ);
		*v3_rYZX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			-cosZinv * *(m3x3_rotation_ptr + dat.i20),
			cosZinv * *(m3x3_rotation_ptr + dat.i00));
		*(v3_rYZX_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			-cosZinv * *(m3x3_rotation_ptr + dat.i12),
			cosZinv * *(m3x3_rotation_ptr + dat.i11));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rYZX_ptr = rY_choice_if_singular; // rY
		Ty rX0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i21),
			*(m3x3_rotation_ptr + dat.i22));
		bool Zneg = *(m3x3_rotation_ptr + dat.i10) < static_cast<Ty>(0);
		*(v3_rYZX_ptr + 1) = Zneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rZ
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Zneg ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rYZX_ptr + 2) = rX0 - flip * rY_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYXZ_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYXZ_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 1));
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ + sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -sinY * cosZ + sinX * cosY * sinZ;

	*(m3x3_rotation_ptr + dat.i01) = -cosY * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinY * sinZ + sinX * cosY * cosZ;

	*(m3x3_rotation_ptr + dat.i02) = cosX * sinY;
	*(m3x3_rotation_ptr + dat.i12) = -sinX;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYXZ_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rY_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i12) && *(m3x3_rotation_ptr + dat.i12) < bound)
	{
		Ty rX = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i12));
		*(v3_rYXZ_ptr + 1) = rX; // rX
		Ty cosXinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rX);
		*v3_rYXZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			cosXinv * *(m3x3_rotation_ptr + dat.i02),
			cosXinv * *(m3x3_rotation_ptr + dat.i22));
		*(v3_rYXZ_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			cosXinv * *(m3x3_rotation_ptr + dat.i10),
			cosXinv * *(m3x3_rotation_ptr + dat.i11));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rYXZ_ptr = rY_choice_if_singular; // rY
		Ty rZ0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i01),
			*(m3x3_rotation_ptr + dat.i00));
		bool Xneg = !(*(m3x3_rotation_ptr + dat.i12) < static_cast<Ty>(0));
		*(v3_rYXZ_ptr + 1) = Xneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rX
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Xneg ? static_cast<Ty>(1) : static_cast<Ty>(-1);
		*(v3_rYXZ_ptr + 2) = rZ0 - flip * rY_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZXY_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZXY_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXY_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXY_ptr + 1));
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXY_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXY_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ - sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i10) = cosY * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + dat.i01) = -cosX * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX;

	*(m3x3_rotation_ptr + dat.i02) = sinY * cosZ + sinX * cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i12) = sinY * sinZ - sinX * cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZXY_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rZ_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i21) && *(m3x3_rotation_ptr + dat.i21) < bound)
	{
		Ty rX = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i21));
		*(v3_rZXY_ptr + 1) = rX; // rX
		Ty cosXinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rX);
		*v3_rZXY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			-cosXinv * *(m3x3_rotation_ptr + dat.i01),
			cosXinv * *(m3x3_rotation_ptr + dat.i11));
		*(v3_rZXY_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			-cosXinv * *(m3x3_rotation_ptr + dat.i20),
			cosXinv * *(m3x3_rotation_ptr + dat.i22));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rZXY_ptr = rZ_choice_if_singular; // rZ
		Ty rY0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i02),
			*(m3x3_rotation_ptr + dat.i00));
		bool Xneg = *(m3x3_rotation_ptr + dat.i21) < static_cast<Ty>(0);
		*(v3_rZXY_ptr + 1) = Xneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rX
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Xneg ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rZXY_ptr + 2) = rY0 - flip * rZ_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZYX_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZYX_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYX_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYX_ptr + 1));
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYX_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYX_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + dat.i10) = cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = -sinY;

	*(m3x3_rotation_ptr + dat.i01) = -cosX * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosZ + sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosY;

	*(m3x3_rotation_ptr + dat.i02) = sinX * sinZ + cosX * sinY * cosZ;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosZ + cosX * sinY * sinZ;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZYX_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rZ_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i20) && *(m3x3_rotation_ptr + dat.i20) < bound)
	{
		Ty rY = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i20));
		*(v3_rZYX_ptr + 1) = rY; // rY
		Ty cosYinv = static_cast<Ty>(1) / cool::rotation_subroutine::cos<Ty, _func_impl_number>(rY);
		*v3_rZYX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			cosYinv * *(m3x3_rotation_ptr + dat.i10),
			cosYinv * *(m3x3_rotation_ptr + dat.i00));
		*(v3_rZYX_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			cosYinv * *(m3x3_rotation_ptr + dat.i21),
			cosYinv * *(m3x3_rotation_ptr + dat.i22));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rZYX_ptr = rZ_choice_if_singular; // rZ
		Ty rX0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i12),
			*(m3x3_rotation_ptr + dat.i11));
		bool Yneg = !(*(m3x3_rotation_ptr + dat.i20) < static_cast<Ty>(0));
		*(v3_rZYX_ptr + 1) = Yneg ? static_cast<Ty>(-3.14159265358979 * 0.5) // rY
			: static_cast<Ty>(3.14159265358979 * 0.5);
		Ty flip = Yneg ? static_cast<Ty>(1) : static_cast<Ty>(-1);
		*(v3_rZYX_ptr + 2) = rX0 - flip * rZ_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXYX2_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXYX2_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 1));
	Ty cosX2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 2));
	Ty sinX2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY;
	*(m3x3_rotation_ptr + dat.i10) = sinX * sinY;
	*(m3x3_rotation_ptr + dat.i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + dat.i01) = sinY * sinX2;
	*(m3x3_rotation_ptr + dat.i11) = cosX * cosX2 - sinX * cosY * sinX2;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosX2 + cosX * cosY * sinX2;

	*(m3x3_rotation_ptr + dat.i02) = sinY * cosX2;
	*(m3x3_rotation_ptr + dat.i12) = -cosX * sinX2 - sinX * cosY * cosX2;
	*(m3x3_rotation_ptr + dat.i22) = -sinX * sinX2 + cosX * cosY * cosX2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXYX2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rX_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i00) && *(m3x3_rotation_ptr + dat.i00) < bound)
	{
		Ty rY = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i00));
		rY = (*(m3x3_rotation_ptr + dat.i02) < static_cast<Ty>(0)) ? -rY : rY;
		*(v3_rXYX2_ptr + 1) = rY; // rY
		Ty sinYinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rY);
		*v3_rXYX2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			sinYinv * *(m3x3_rotation_ptr + dat.i10),
			-sinYinv * *(m3x3_rotation_ptr + dat.i20));
		*(v3_rXYX2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX2
			sinYinv * *(m3x3_rotation_ptr + dat.i01),
			sinYinv * *(m3x3_rotation_ptr + dat.i02));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rXYX2_ptr = rX_choice_if_singular; // rX
		Ty rX20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i12),
			*(m3x3_rotation_ptr + dat.i11));
		bool Yflipped = *(m3x3_rotation_ptr + dat.i00) < static_cast<Ty>(0);
		*(v3_rXYX2_ptr + 1) = Yflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rY
		Ty flip = Yflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rXYX2_ptr + 2) = rX20 - flip * rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept
{
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXZX2_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXZX2_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 1));
	Ty cosX2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 2));
	Ty sinX2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosZ;
	*(m3x3_rotation_ptr + dat.i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + dat.i20) = sinX * sinZ;

	*(m3x3_rotation_ptr + dat.i01) = -sinZ * cosX2;
	*(m3x3_rotation_ptr + dat.i11) = -sinX * sinX2 + cosX * cosZ * cosX2;
	*(m3x3_rotation_ptr + dat.i21) = cosX * sinX2 + sinX * cosZ * cosX2;

	*(m3x3_rotation_ptr + dat.i02) = sinZ * sinX2;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosX2 - cosX * cosZ * sinX2;
	*(m3x3_rotation_ptr + dat.i22) = cosX * cosX2 - sinX * cosZ * sinX2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXZX2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rX_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i00) && *(m3x3_rotation_ptr + dat.i00) < bound)
	{
		Ty rZ = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i00));
		rZ = (*(m3x3_rotation_ptr + dat.i01) < static_cast<Ty>(0)) ? rZ : -rZ;
		*(v3_rXZX2_ptr + 1) = rZ; // rZ
		Ty sinZinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rZ);
		*v3_rXZX2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			sinZinv * *(m3x3_rotation_ptr + dat.i20),
			sinZinv * *(m3x3_rotation_ptr + dat.i10));
		*(v3_rXZX2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX2
			sinZinv * *(m3x3_rotation_ptr + dat.i02),
			-sinZinv * *(m3x3_rotation_ptr + dat.i01));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rXZX2_ptr = rX_choice_if_singular; // rX
		Ty rX20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i21),
			*(m3x3_rotation_ptr + dat.i22));
		bool Zflipped = *(m3x3_rotation_ptr + dat.i00) < static_cast<Ty>(0);
		*(v3_rXZX2_ptr + 1) = Zflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rZ2
		Ty flip = Zflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rXZX2_ptr + 2) = rX20 - flip * rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYZY2_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYZY2_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 1));
	Ty cosY2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 2));
	Ty sinY2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = -sinY * sinY2 + cosY * cosZ * cosY2;
	*(m3x3_rotation_ptr + dat.i10) = sinZ * cosY2;
	*(m3x3_rotation_ptr + dat.i20) = -cosY * sinY2 - sinY * cosZ * cosY2;

	*(m3x3_rotation_ptr + dat.i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + dat.i11) = cosZ;
	*(m3x3_rotation_ptr + dat.i21) = sinY * sinZ;

	*(m3x3_rotation_ptr + dat.i02) = sinY * cosY2 + cosY * cosZ * sinY2;
	*(m3x3_rotation_ptr + dat.i12) = sinZ * sinY2;
	*(m3x3_rotation_ptr + dat.i22) = cosY * cosY2 - sinY * cosZ * sinY2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYZY2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rY_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i11) && *(m3x3_rotation_ptr + dat.i11) < bound)
	{
		Ty rZ = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i11));
		rZ = (*(m3x3_rotation_ptr + dat.i10) < static_cast<Ty>(0)) ? -rZ : rZ;
		*(v3_rYZY2_ptr + 1) = rZ; // rZ
		Ty sinZinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rZ);
		*v3_rYZY2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			sinZinv * *(m3x3_rotation_ptr + dat.i21),
			-sinZinv * *(m3x3_rotation_ptr + dat.i01));
		*(v3_rYZY2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY2
			sinZinv * *(m3x3_rotation_ptr + dat.i12),
			sinZinv * *(m3x3_rotation_ptr + dat.i10));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rYZY2_ptr = rY_choice_if_singular; // rY
		Ty rY20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i20),
			*(m3x3_rotation_ptr + dat.i22));
		bool Zflipped = *(m3x3_rotation_ptr + dat.i11) < static_cast<Ty>(0);
		*(v3_rYZY2_ptr + 1) = Zflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rZ
		Ty flip = Zflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rYZY2_ptr + 2) = rY20 - flip * rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept
{
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYXY2_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYXY2_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 1));
	Ty cosY2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 2));
	Ty sinY2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosY * cosY2 - sinY * cosX * sinY2;
	*(m3x3_rotation_ptr + dat.i10) = sinX * sinY2;
	*(m3x3_rotation_ptr + dat.i20) = -sinY * cosY2 - cosY * cosX * sinY2;

	*(m3x3_rotation_ptr + dat.i01) = sinY * sinX;
	*(m3x3_rotation_ptr + dat.i11) = cosX;
	*(m3x3_rotation_ptr + dat.i21) = cosY * sinX;

	*(m3x3_rotation_ptr + dat.i02) = cosY * sinY2 + sinY * cosX * cosY2;
	*(m3x3_rotation_ptr + dat.i12) = -sinX * cosY2;
	*(m3x3_rotation_ptr + dat.i22) = -sinY * sinY2 + cosY * cosX * cosY2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYXY2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rY_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i11) && *(m3x3_rotation_ptr + dat.i11) < bound)
	{
		Ty rX = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i11));
		rX = (*(m3x3_rotation_ptr + dat.i12) < static_cast<Ty>(0)) ? rX : -rX;
		*(v3_rYXY2_ptr + 1) = rX; // rX
		Ty sinXinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rX);
		*v3_rYXY2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			sinXinv * *(m3x3_rotation_ptr + dat.i01),
			sinXinv * *(m3x3_rotation_ptr + dat.i21));
		*(v3_rYXY2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY2
			sinXinv * *(m3x3_rotation_ptr + dat.i10),
			-sinXinv * *(m3x3_rotation_ptr + dat.i12));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rYXY2_ptr = rY_choice_if_singular; // rY
		Ty rY20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i02),
			*(m3x3_rotation_ptr + dat.i00));
		bool Xflipped = *(m3x3_rotation_ptr + dat.i11) < static_cast<Ty>(0);
		*(v3_rYXY2_ptr + 1) = Xflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rX
		Ty flip = Xflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rYXY2_ptr + 2) = rY20 - flip * rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZXZ2_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZXZ2_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 1));
	Ty cosZ2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 2));
	Ty sinZ2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = cosZ * cosZ2 - sinZ * cosX * sinZ2;
	*(m3x3_rotation_ptr + dat.i10) = sinZ * cosZ2 + cosZ * cosX * sinZ2;
	*(m3x3_rotation_ptr + dat.i20) = sinX * sinZ2;

	*(m3x3_rotation_ptr + dat.i01) = -cosZ * sinZ2 - sinZ * cosX * cosZ2;
	*(m3x3_rotation_ptr + dat.i11) = -sinZ * sinZ2 + cosZ * cosX * cosZ2;
	*(m3x3_rotation_ptr + dat.i21) = sinX * cosZ2;

	*(m3x3_rotation_ptr + dat.i02) = sinZ * sinX;
	*(m3x3_rotation_ptr + dat.i12) = -cosZ * sinX;
	*(m3x3_rotation_ptr + dat.i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZXZ2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rZ_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i22) && *(m3x3_rotation_ptr + dat.i22) < bound)
	{
		Ty rX = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i22));
		rX = (*(m3x3_rotation_ptr + dat.i21) < static_cast<Ty>(0)) ? -rX : rX;
		*(v3_rZXZ2_ptr + 1) = rX; // rX
		Ty sinXinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rX);
		*v3_rZXZ2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			sinXinv * *(m3x3_rotation_ptr + dat.i02),
			-sinXinv * *(m3x3_rotation_ptr + dat.i12));
		*(v3_rZXZ2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ2
			sinXinv * *(m3x3_rotation_ptr + dat.i20),
			sinXinv * *(m3x3_rotation_ptr + dat.i21));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rZXZ2_ptr = rZ_choice_if_singular; // rZ
		Ty rZ20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + dat.i01),
			*(m3x3_rotation_ptr + dat.i00));
		bool Xflipped = *(m3x3_rotation_ptr + dat.i22) < static_cast<Ty>(0);
		*(v3_rZXZ2_ptr + 1) = Xflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rX
		Ty flip = Xflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rZXZ2_ptr + 2) = rZ20 - flip * rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept
{
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZYZ2_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZYZ2_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 1));
	Ty cosZ2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 2));
	Ty sinZ2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 2));

	*(m3x3_rotation_ptr + dat.i00) = -sinZ * sinZ2 + cosZ * cosY * cosZ2;
	*(m3x3_rotation_ptr + dat.i10) = cosZ * sinZ2 + sinZ * cosY * cosZ2;
	*(m3x3_rotation_ptr + dat.i20) = -sinY * cosZ2;

	*(m3x3_rotation_ptr + dat.i01) = -sinZ * cosZ2 - cosZ * cosY * sinZ2;
	*(m3x3_rotation_ptr + dat.i11) = cosZ * cosZ2 - sinZ * cosY * sinZ2;
	*(m3x3_rotation_ptr + dat.i21) = sinY * sinZ2;

	*(m3x3_rotation_ptr + dat.i02) = cosZ * sinY;
	*(m3x3_rotation_ptr + dat.i12) = sinZ * sinY;
	*(m3x3_rotation_ptr + dat.i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZYZ2_ptr, const Ty* m3x3_rotation_ptr, Ty tol, Ty rZ_choice_if_singular) noexcept
{
	Ty bound = static_cast<Ty>(1) - static_cast<Ty>(0.5) * (tol * tol);

	if (-bound < *(m3x3_rotation_ptr + dat.i22) && *(m3x3_rotation_ptr + dat.i22) < bound)
	{
		Ty rY = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + dat.i22));
		rY = (*(m3x3_rotation_ptr + dat.i20) < static_cast<Ty>(0)) ? rY : -rY;
		*(v3_rZYZ2_ptr + 1) = rY; // rY
		Ty sinYinv = static_cast<Ty>(1) / cool::rotation_subroutine::sin<Ty, _func_impl_number>(rY);
		*v3_rZYZ2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			sinYinv * *(m3x3_rotation_ptr + dat.i12),
			sinYinv * *(m3x3_rotation_ptr + dat.i02));
		*(v3_rZYZ2_ptr + 2) = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ2
			sinYinv * *(m3x3_rotation_ptr + dat.i21),
			-sinYinv * *(m3x3_rotation_ptr + dat.i20));

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rZYZ2_ptr = rZ_choice_if_singular; // rZ
		Ty rZ20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + dat.i10),
			*(m3x3_rotation_ptr + dat.i11));
		bool Yflipped = *(m3x3_rotation_ptr + dat.i22) < static_cast<Ty>(0);
		*(v3_rZYZ2_ptr + 1) = Yflipped ? static_cast<Ty>(3.14159265358979) : static_cast<Ty>(0); // rY
		Ty flip = Yflipped ? static_cast<Ty>(-1) : static_cast<Ty>(1);
		*(v3_rZYZ2_ptr + 2) = rZ20 - flip * rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty tol) noexcept
{
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty cc = static_cast<Ty>(1) - c;

	Ty axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (axis_norm_sq > tol * tol)
	{
		Ty axis_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(axis_norm_sq);

		Ty axis[3] = {
			axis_norm_inv * *v3_axis_ptr,
			axis_norm_inv * *(v3_axis_ptr + 1),
			axis_norm_inv * *(v3_axis_ptr + 2)
		};

		Ty temp01 = cc * axis[0];
		Ty temp12 = cc * axis[1];
		Ty temp20 = cc * axis[2];

		*(m3x3_rotation_ptr + dat.i00) = temp01 * axis[0] + c;
		*(m3x3_rotation_ptr + dat.i11) = temp12 * axis[1] + c;
		*(m3x3_rotation_ptr + dat.i22) = temp20 * axis[2] + c;

		temp20 *= axis[0];
		temp01 *= axis[1];
		temp12 *= axis[2];

		axis[0] *= s;
		axis[1] *= s;
		axis[2] *= s;

		*(m3x3_rotation_ptr + dat.i21) = temp12 + axis[0];
		*(m3x3_rotation_ptr + dat.i12) = temp12 - axis[0];
		*(m3x3_rotation_ptr + dat.i20) = temp20 - axis[1];
		*(m3x3_rotation_ptr + dat.i02) = temp20 + axis[1];
		*(m3x3_rotation_ptr + dat.i10) = temp01 + axis[2];
		*(m3x3_rotation_ptr + dat.i01) = temp01 - axis[2];
	}
	else
	{
		*(m3x3_rotation_ptr + dat.i00) = static_cast<Ty>(1);
		*(m3x3_rotation_ptr + dat.i10) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + dat.i20) = static_cast<Ty>(0);

		*(m3x3_rotation_ptr + dat.i01) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + dat.i11) = static_cast<Ty>(1);
		*(m3x3_rotation_ptr + dat.i21) = static_cast<Ty>(0);

		*(m3x3_rotation_ptr + dat.i02) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + dat.i12) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + dat.i22) = static_cast<Ty>(1);
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept
{
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty cc = static_cast<Ty>(1) - c;

	Ty axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	Ty temp01 = cc * axis[0];
	Ty temp12 = cc * axis[1];
	Ty temp20 = cc * axis[2];

	*(m3x3_rotation_ptr + dat.i00) = temp01 * axis[0] + c;
	*(m3x3_rotation_ptr + dat.i11) = temp12 * axis[1] + c;
	*(m3x3_rotation_ptr + dat.i22) = temp20 * axis[2] + c;

	temp20 *= axis[0];
	temp01 *= axis[1];
	temp12 *= axis[2];

	axis[0] *= s;
	axis[1] *= s;
	axis[2] *= s;

	*(m3x3_rotation_ptr + dat.i21) = temp12 + axis[0];
	*(m3x3_rotation_ptr + dat.i12) = temp12 - axis[0];
	*(m3x3_rotation_ptr + dat.i20) = temp20 - axis[1];
	*(m3x3_rotation_ptr + dat.i02) = temp20 + axis[1];
	*(m3x3_rotation_ptr + dat.i10) = temp01 + axis[2];
	*(m3x3_rotation_ptr + dat.i01) = temp01 - axis[2];
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_axis<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty tol) noexcept
{
	Ty quat_comp_sq[4];
	Ty quat_comp_sgn[4];

	{
		Ty a0 = *(m3x3_rotation_ptr + dat.i21) - *(m3x3_rotation_ptr + dat.i12);
		Ty b0 = *(m3x3_rotation_ptr + dat.i21) + *(m3x3_rotation_ptr + dat.i12);
		Ty a1 = *(m3x3_rotation_ptr + dat.i02) - *(m3x3_rotation_ptr + dat.i20);
		Ty b1 = *(m3x3_rotation_ptr + dat.i02) + *(m3x3_rotation_ptr + dat.i20);
		Ty a2 = *(m3x3_rotation_ptr + dat.i10) - *(m3x3_rotation_ptr + dat.i01);
		Ty b2 = *(m3x3_rotation_ptr + dat.i10) + *(m3x3_rotation_ptr + dat.i01);

		Ty c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
			+ *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
			- *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
			+ *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
			- *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = static_cast<Ty>(0.25);
		quat_comp_sgn[1] = (a0 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
		quat_comp_sgn[2] = (a1 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
		quat_comp_sgn[3] = (a2 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
	}

	Ty quat[4] = {
		quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]),
		quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]),
		quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]),
		quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3])
	};

	Ty xyz_norm = static_cast<Ty>(0.25) * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	if (xyz_norm > static_cast<Ty>(0.5) * tol)
	{
		Ty xyz_norm_inv = static_cast<Ty>(1) / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = static_cast<Ty>(2) * cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<Ty>(0);
		*(v3_axis_ptr + 1) = static_cast<Ty>(0);
		*(v3_axis_ptr + 2) = static_cast<Ty>(1);
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_axis<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, const Ty* v3_axis_way_ptr, Ty tol) noexcept
{
	Ty quat_comp_sq[4];
	Ty quat_comp_sgn[4];

	{
		Ty a0 = *(m3x3_rotation_ptr + dat.i21) - *(m3x3_rotation_ptr + dat.i12);
		Ty b0 = *(m3x3_rotation_ptr + dat.i21) + *(m3x3_rotation_ptr + dat.i12);
		Ty a1 = *(m3x3_rotation_ptr + dat.i02) - *(m3x3_rotation_ptr + dat.i20);
		Ty b1 = *(m3x3_rotation_ptr + dat.i02) + *(m3x3_rotation_ptr + dat.i20);
		Ty a2 = *(m3x3_rotation_ptr + dat.i10) - *(m3x3_rotation_ptr + dat.i01);
		Ty b2 = *(m3x3_rotation_ptr + dat.i10) + *(m3x3_rotation_ptr + dat.i01);

		Ty c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
			+ *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
			- *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
			+ *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
			- *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = static_cast<Ty>(0.25);
		quat_comp_sgn[1] = (a0 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
		quat_comp_sgn[2] = (a1 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
		quat_comp_sgn[3] = (a2 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25);
	}

	Ty quat[4] = {
		quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]),
		quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]),
		quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]),
		quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3])
	};

	Ty xyz_norm = static_cast<Ty>(0.25) * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	if (xyz_norm > static_cast<Ty>(0.5) * tol)
	{
		Ty param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		Ty flip = (param < static_cast<Ty>(0)) ? static_cast<Ty>(-1) : static_cast<Ty>(1);

		Ty xyz_norm_inv = flip / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * static_cast<Ty>(2) * cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		Ty axis_way_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(
			*v3_axis_way_ptr * *v3_axis_way_ptr
			+ *(v3_axis_way_ptr + 1) * *(v3_axis_way_ptr + 1)
			+ *(v3_axis_way_ptr + 2) * *(v3_axis_way_ptr + 2));

		*v3_axis_ptr = axis_way_norm_inv * *v3_axis_way_ptr;
		*(v3_axis_ptr + 1) = axis_way_norm_inv * *(v3_axis_way_ptr + 1);
		*(v3_axis_ptr + 2) = axis_way_norm_inv * *(v3_axis_way_ptr + 2);
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept
{
	Ty q00 = *v4_quaternion_ptr * *v4_quaternion_ptr;
	Ty q11 = *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	Ty q22 = *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	Ty q33 = *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);
	Ty s = static_cast<Ty>(2) / (q00 + q11 + q22 + q33);
	q00 *= s; q11 *= s; q22 *= s; q33 *= s;
	Ty q01 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	Ty q02 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	Ty q03 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	Ty q12 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	Ty q13 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	Ty q23 = s * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);


	*(m3x3_rotation_ptr + dat.i00) = static_cast<Ty>(1) - q22 - q33;
	*(m3x3_rotation_ptr + dat.i10) = q12 + q03;
	*(m3x3_rotation_ptr + dat.i20) = q13 - q02;

	*(m3x3_rotation_ptr + dat.i01) = q12 - q03;
	*(m3x3_rotation_ptr + dat.i11) = static_cast<Ty>(1) - q11 - q33;
	*(m3x3_rotation_ptr + dat.i21) = q23 + q01;

	*(m3x3_rotation_ptr + dat.i02) = q13 + q02;
	*(m3x3_rotation_ptr + dat.i12) = q23 - q01;
	*(m3x3_rotation_ptr + dat.i22) = static_cast<Ty>(1) - q11 - q22;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept
{
	Ty q00 = static_cast<Ty>(2) * *v4_quaternion_ptr * *v4_quaternion_ptr;
	Ty q01 = static_cast<Ty>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	Ty q02 = static_cast<Ty>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	Ty q03 = static_cast<Ty>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	Ty q11 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	Ty q12 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	Ty q13 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	Ty q22 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	Ty q23 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);
	Ty q33 = static_cast<Ty>(2) * *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);

	*(m3x3_rotation_ptr + dat.i00) = static_cast<Ty>(1) - q22 - q33;
	*(m3x3_rotation_ptr + dat.i10) = q12 + q03;
	*(m3x3_rotation_ptr + dat.i20) = q13 - q02;

	*(m3x3_rotation_ptr + dat.i01) = q12 - q03;
	*(m3x3_rotation_ptr + dat.i11) = static_cast<Ty>(1) - q11 - q33;
	*(m3x3_rotation_ptr + dat.i21) = q23 + q01;

	*(m3x3_rotation_ptr + dat.i02) = q13 + q02;
	*(m3x3_rotation_ptr + dat.i12) = q23 - q01;
	*(m3x3_rotation_ptr + dat.i22) = static_cast<Ty>(1) - q11 - q22;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_matrix(
	Ty* v4_quaternion_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	Ty quat_comp_sq[4];

	Ty a0 = *(m3x3_rotation_ptr + dat.i21) - *(m3x3_rotation_ptr + dat.i12);
	Ty b0 = *(m3x3_rotation_ptr + dat.i21) + *(m3x3_rotation_ptr + dat.i12);
	Ty a1 = *(m3x3_rotation_ptr + dat.i02) - *(m3x3_rotation_ptr + dat.i20);
	Ty b1 = *(m3x3_rotation_ptr + dat.i02) + *(m3x3_rotation_ptr + dat.i20);
	Ty a2 = *(m3x3_rotation_ptr + dat.i10) - *(m3x3_rotation_ptr + dat.i01);
	Ty b2 = *(m3x3_rotation_ptr + dat.i10) + *(m3x3_rotation_ptr + dat.i01);

	Ty c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
		+ *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
	quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

	c = static_cast<Ty>(1) + *(m3x3_rotation_ptr + dat.i00)
		- *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
	quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

	c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
		+ *(m3x3_rotation_ptr + dat.i11) - *(m3x3_rotation_ptr + dat.i22);
	quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

	c = static_cast<Ty>(1) - *(m3x3_rotation_ptr + dat.i00)
		- *(m3x3_rotation_ptr + dat.i11) + *(m3x3_rotation_ptr + dat.i22);
	quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

	Ty quat_comp_sgn[4] = {
		static_cast<Ty>(0.25),
		(a0 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25),
		(a1 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25),
		(a2 < static_cast<Ty>(0)) ? static_cast<Ty>(-0.25) : static_cast<Ty>(0.25)
	};

	*v4_quaternion_ptr = quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]);
	*(v4_quaternion_ptr + 1) = quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]);
	*(v4_quaternion_ptr + 2) = quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]);
	*(v4_quaternion_ptr + 3) = quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3]);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_axis_angle(
	Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle, Ty tol) noexcept
{
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(static_cast<Ty>(0.5) * angle);
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(static_cast<Ty>(0.5) * angle);

	Ty axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (axis_norm_sq > tol * tol)
	{
		Ty axis_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(axis_norm_sq);

		Ty axis[3] = {
			axis_norm_inv * *v3_axis_ptr,
			axis_norm_inv * *(v3_axis_ptr + 1),
			axis_norm_inv * *(v3_axis_ptr + 2)
		};

		*v4_quaternion_ptr = c;
		*(v4_quaternion_ptr + 1) = s * axis[0];
		*(v4_quaternion_ptr + 2) = s * axis[1];
		*(v4_quaternion_ptr + 3) = s * axis[2];
	}
	else
	{
		*v4_quaternion_ptr = static_cast<Ty>(1);
		*(v4_quaternion_ptr + 1) = static_cast<Ty>(0);
		*(v4_quaternion_ptr + 2) = static_cast<Ty>(0);
		*(v4_quaternion_ptr + 3) = static_cast<Ty>(0);
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_axis_angle(
	Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept
{
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(static_cast<Ty>(0.5) * angle);
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(static_cast<Ty>(0.5) * angle);

	Ty axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	*v4_quaternion_ptr = c;
	*(v4_quaternion_ptr + 1) = s * axis[0];
	*(v4_quaternion_ptr + 2) = s * axis[1];
	*(v4_quaternion_ptr + 3) = s * axis[2];
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, Ty* v4_quaternion_ptr, Ty tol) noexcept
{
	Ty quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	Ty xyz_norm = cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	if (xyz_norm > static_cast<Ty>(0.5) * tol)
	{
		Ty xyz_norm_inv = static_cast<Ty>(1) / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];

		*angle_ptr = static_cast<Ty>(2) * cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<Ty>(0);
		*(v3_axis_ptr + 1) = static_cast<Ty>(0);
		*(v3_axis_ptr + 2) = static_cast<Ty>(1);
		*angle_ptr = static_cast<Ty>(0);

		cool::rotation_status::singular;
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, Ty* v4_quaternion_ptr, const Ty* v3_axis_way_ptr, Ty tol) noexcept
{
	Ty quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	Ty xyz_norm = cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	if (xyz_norm > static_cast<Ty>(0.5) * tol)
	{
		Ty angle = static_cast<Ty>(2) * cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			xyz_norm, quat[0]);

		Ty param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		param = (param != static_cast<Ty>(0)) ? param : angle;

		Ty flip = (param < static_cast<Ty>(0)) ? static_cast<Ty>(-1) : static_cast<Ty>(1);

		Ty xyz_norm_inv = flip / xyz_norm;

		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * angle;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty axis_way_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(
			*v3_axis_way_ptr * *v3_axis_way_ptr
			+ *(v3_axis_way_ptr + 1) * *(v3_axis_way_ptr + 1)
			+ *(v3_axis_way_ptr + 2) * *(v3_axis_way_ptr + 2));

		*v3_axis_ptr = axis_way_norm_inv * *v3_axis_way_ptr;
		*(v3_axis_ptr + 1) = axis_way_norm_inv * *(v3_axis_way_ptr + 1);
		*(v3_axis_ptr + 2) = axis_way_norm_inv * *(v3_axis_way_ptr + 2);
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}

#endif // _COOL_ROTATIONS_HPP


// cool_rotations.hpp
//
// This is free software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.
