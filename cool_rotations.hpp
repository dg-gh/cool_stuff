// cool_rotations.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_ROTATIONS_HPP
#define xCOOL_ROTATIONS_HPP

#include <cstddef>
#include <cstdint>
#include <cmath>


namespace cool
{
#ifndef xCOOL_MATRIX_LAYOUT_ENUM
#define xCOOL_MATRIX_LAYOUT_ENUM
	enum class matrix_layout
	{
		col = 0, // indicates usage of column-major matrices
		row = 1 // indicates usage of row-major matrices
	};
#endif // xCOOL_MATRIX_LAYOUT_ENUM
}

#ifndef COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT
#define COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT cool::matrix_layout::col
#endif // COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT


namespace cool
{
	namespace rotation_subroutine
	{
		// template specializable functions, default to <cmath> functions

		// defaults to std::cos(x)
		// can be specialized to std::cos(pi / 180 * x) if angles are in degrees
		template <class Ty, int _func_impl_number> inline Ty cos(Ty x) noexcept;
		// defaults to std::sin(x)
		// can be specialized to std::sin(pi / 180 * x) if angles are in degrees
		template <class Ty, int _func_impl_number> inline Ty sin(Ty x) noexcept;
		// defaults to std::acos(x)
		// can be specialized to 180 / pi * std::acos(x) if angles are in degrees
		template <class Ty, int _func_impl_number> inline Ty acos(Ty x) noexcept;
		// defaults to std::asin(x)
		// can be specialized to 180 / pi * std::asin(x) if angles are in degrees
		template <class Ty, int _func_impl_number> inline Ty asin(Ty x) noexcept;
		// defaults to std::atan(y, x)
		// can be specialized to 180 / pi * std::atan2(y, x) if angles are in degrees
		template <class Ty, int _func_impl_number> inline Ty atan2(Ty y, Ty x) noexcept;
		// defaults to std::sqrt(x)
		template <class Ty, int _func_impl_number> inline Ty sqrt(Ty x) noexcept;


		// template specializable constexpr functions

		// defaults to static_cast<Ty>(3.141592653589793)
		template <class Ty, int _func_impl_number> inline constexpr Ty pi() noexcept;
		// defaults to static_cast<Ty>(3.141592653589793)
		// should be specialized to return 180 if degrees are used to get correct angles of singular positions
		template <class Ty, int _func_impl_number> inline constexpr Ty half_turn() noexcept;
		// defaults to static_cast<Ty>(1)
		template <class Ty, int _func_impl_number> inline constexpr Ty one() noexcept;
		// defaults to static_cast<Ty>(0.5)
		template <class Ty, int _func_impl_number> inline constexpr Ty half() noexcept;

		// expression static_cast<Ty>(0) must yield value considered to be zero for type Ty


		// template specializable constexpr functions, deduced from previous functions
		// if no implementation is specified

		// default deduced from one + one
		template <class Ty, int _func_impl_number> inline constexpr Ty two() noexcept;
		// default deduced from half * half_turn
		template <class Ty, int _func_impl_number> inline constexpr Ty quarter_turn() noexcept;
		// default deduced from half * half
		template <class Ty, int _func_impl_number> inline constexpr Ty quarter() noexcept;
		// default deduced from to -half_turn
		template <class Ty, int _func_impl_number> inline constexpr Ty minus_half_turn() noexcept;
		// default deduced from -minus_quarter_turn
		template <class Ty, int _func_impl_number> inline constexpr Ty minus_quarter_turn() noexcept;
		// default deduced from -one
		template <class Ty, int _func_impl_number> inline constexpr Ty minus_one() noexcept;
		// default deduced from -quarter
		template <class Ty, int _func_impl_number> inline constexpr Ty minus_quarter() noexcept;
		// default deduced from one / sqrt(x)
		template <class Ty, int _func_impl_number> inline Ty inv_sqrt(Ty x) noexcept;
	}

	enum class rotation_status
	{
		regular = 0,
		singular = 1,
	};

#ifndef xCOOL_AXIS_CONSTANTS
#define xCOOL_AXIS_CONSTANTS
	namespace axis
	{
		static constexpr std::size_t X = 0;
		static constexpr std::size_t Y = 1;
		static constexpr std::size_t Z = 2;
		static constexpr std::size_t none = 3;
	}

	namespace axis_index
	{
		static constexpr std::size_t none = 3;
	}
#endif // xCOOL_AXIS_CONSTANTS


	// 2D rotations

	template <class Ty, std::size_t _dim_padded = 2, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotation2d;

	template <class Ty, int _func_impl_number = 0> class direction2d;


	// 3D rotations

	template <class Ty> class template_rotation3d;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotation3d;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotation_axis_angle;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotation_quaternion;

	template <class Ty, int _func_impl_number = 0> class direction3d;


	// class rotation3d with enum class rotation_type

	// cool::rotation_type::U :
	// rotation matrix around axis U

	// cool::rotation_type::UV :
	// rotation matrix around axis U * rotation matrix around axis V

	// cool::rotation_type::UVW :
	// rotation matrix around axis U * rotation matrix around axis V * rotation matrix around axis W

	// U, V, W being X = (1, 0, 0) or Y = (0, 1, 0) or Z = (0, 0, 1)

	// cool::rotation_type::AA :
	// rotation with axis angle representation
	// coordinate data as (axis_x, axis_y, axis_z, angle)

	// cool::rotation_type::Q :
	// rotation with quaternion representation
	// with A = angle coordinate data as (cos(A/2), sin(A/2) * axis_x, sin(A/2) * axis_y, sin(A/2) * axis_z)

	// cool::rotation_type::id :
	// no rotation

	inline constexpr std::uint32_t _rotation_type_val(std::size_t rotation_type_index,
		std::size_t iX, std::size_t iY, std::size_t iZ, std::size_t iSg,
		std::size_t axis0, std::size_t axis1, std::size_t axis2, std::size_t singular_axis) noexcept
	{
		return static_cast<std::uint32_t>(rotation_type_index) << 16
			| static_cast<std::uint32_t>(iX) << 0
			| static_cast<std::uint32_t>(iY) << 2
			| static_cast<std::uint32_t>(iZ) << 4
			| static_cast<std::uint32_t>(iSg) << 6
			| static_cast<std::uint32_t>(axis0) << 8
			| static_cast<std::uint32_t>(axis1) << 10
			| static_cast<std::uint32_t>(axis2) << 12
			| static_cast<std::uint32_t>(singular_axis) << 14;
	}

	enum class rotation_type : std::uint32_t
	{
		id = cool::_rotation_type_val(0, 3, 3, 3, 3, 3, 3, 3, 3),

		X = cool::_rotation_type_val(1, 0, 3, 3, 3, 0, 3, 3, 3),
		Y = cool::_rotation_type_val(2, 3, 0, 3, 3, 1, 3, 3, 3),
		Z = cool::_rotation_type_val(3, 3, 3, 0, 3, 2, 3, 3, 3),

		XY = cool::_rotation_type_val(4, 0, 1, 3, 3, 0, 1, 3, 3),
		XZ = cool::_rotation_type_val(5, 0, 3, 1, 3, 0, 2, 3, 3),
		YZ = cool::_rotation_type_val(6, 3, 0, 1, 3, 1, 2, 3, 3),
		YX = cool::_rotation_type_val(7, 1, 0, 3, 3, 1, 0, 3, 3),
		ZX = cool::_rotation_type_val(8, 1, 3, 0, 3, 2, 0, 3, 3),
		ZY = cool::_rotation_type_val(9, 3, 1, 0, 3, 2, 1, 3, 3),

		XYZ = cool::_rotation_type_val(10, 0, 1, 2, 1, 0, 1, 2, 1),
		XZY = cool::_rotation_type_val(11, 0, 2, 1, 1, 0, 2, 1, 2),
		YZX = cool::_rotation_type_val(12, 2, 0, 1, 1, 1, 2, 0, 2),
		YXZ = cool::_rotation_type_val(13, 1, 0, 2, 1, 1, 0, 2, 0),
		ZXY = cool::_rotation_type_val(14, 1, 2, 0, 1, 2, 0, 1, 0),
		ZYX = cool::_rotation_type_val(15, 2, 1, 0, 1, 2, 1, 0, 1),

		XYX2 = cool::_rotation_type_val(16, 0, 1, 2, 1, 0, 1, 2, 1),
		XZX2 = cool::_rotation_type_val(17, 0, 2, 1, 1, 0, 2, 1, 2),
		YZY2 = cool::_rotation_type_val(18, 2, 0, 1, 1, 1, 2, 0, 2),
		YXY2 = cool::_rotation_type_val(19, 1, 0, 2, 1, 1, 0, 2, 0),
		ZXZ2 = cool::_rotation_type_val(20, 1, 2, 0, 1, 2, 0, 1, 0),
		ZYZ2 = cool::_rotation_type_val(21, 2, 1, 0, 1, 2, 1, 0, 1),

		AA = cool::_rotation_type_val(22, 3, 3, 3, 3, 3, 3, 3, 3),
		Q = cool::_rotation_type_val(23, 3, 3, 3, 3, 3, 3, 3, 3),
	};

	inline constexpr std::size_t rotation_type_as_index(cool::rotation_type _rotation_type) noexcept;
	inline constexpr cool::rotation_type index_as_rotation_type(std::size_t _index) noexcept;


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationX;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationY;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZ;


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXY;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXZ;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYZ;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYX;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZX;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZY;


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXYZ;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXZY;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYZX;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYXZ;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZXY;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZYX;


	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXYX2;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationXZX2;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYZY2;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationYXY2;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZXZ2;

	template <class Ty, std::size_t _dim_padded = 3, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT,
		int _func_impl_number = 0> class rotationZYZ2;


	// class member details

	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotation2d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static inline void get_matrix(Ty* m2x2_rotation_ptr, Ty angle) noexcept;
		static inline void get_matrix_inv(Ty* m2x2_rotation_ptr, Ty angle) noexcept;
		static inline void get_angle(Ty* angle_ptr, const Ty* m2x2_rotation_ptr) noexcept;
	};


#ifndef xCOOL_NO_DIRECTION_NORM_ENUM
#define xCOOL_NO_DIRECTION_NORM_ENUM
	enum no_direction_norm_t { no_direction_norm };
#endif // xCOOL_NO_DIRECTION_NORM_ENUM

	template <class Ty, int _func_impl_number> class direction2d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		// v2_front_dir_ptr and v2_lateral_dir_ptr must be orthonormal

		static inline void get_direction(
			Ty* v2_direction_ptr,
			Ty angle,
			const Ty* v2_front_dir_ptr,
			const Ty* v2_lateral_dir_ptr) noexcept;

		static inline void get_angle(
			Ty* angle_ptr,
			const Ty* v2_direction_ptr,
			const Ty* v2_front_dir_ptr,
			const Ty* v2_lateral_dir_ptr) noexcept;
	};


#ifndef xCOOL_NO_AXIS_NORM_ENUM
#define xCOOL_NO_AXIS_NORM_ENUM
	enum no_axis_norm_t { no_axis_norm };
#endif // xCOOL_NO_AXIS_NORM_ENUM

	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotation_axis_angle
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty norm_tol = static_cast<Ty>(0)) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty norm_tol = static_cast<Ty>(0)) noexcept;
		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr,
			const Ty* v3_axis_way_ptr, Ty angle_tol) noexcept;
		static inline void get_angle(Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept;
	};


#ifndef xCOOL_NO_QUATERNION_NORM_ENUM
#define xCOOL_NO_QUATERNION_NORM_ENUM
	enum no_quaternion_norm_t { no_quaternion_norm };
#endif // xCOOL_NO_QUATERNION_NORM_ENUM

	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotation_quaternion
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept;
		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept;
		static inline void get_quaternion_from_matrix(Ty* v4_quaternion_ptr, const Ty* m3x3_rotation_ptr) noexcept;
		static inline void get_quaternion_from_axis_angle(Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle,
			Ty norm_tol = static_cast<Ty>(0)) noexcept;
		static inline void get_quaternion_from_axis_angle(Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle,
			cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle, Ty* v4_quaternion_ptr, Ty angle_tol) noexcept;
		static inline cool::rotation_status get_axis_angle(Ty* v3_axis_ptr, Ty* angle, Ty* v4_quaternion_ptr,
			const Ty* v3_axis_way_ptr, Ty angle_tol) noexcept;
	};


	template <class Ty, int _func_impl_number> class direction3d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iSg = 1;

		// v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr must be orthonormal

		static inline void get_direction(
			Ty* v3_direction_ptr,
			const Ty* v2_azimuth_altitude_angles_ptr,
			const Ty* v3_front_dir_ptr,
			const Ty* v3_lateral_dir_ptr,
			const Ty* v3_up_dir_ptr) noexcept;

		static inline cool::rotation_status get_angles(
			Ty* v2_azimuth_altitude_angles_ptr,
			const Ty* v3_direction_ptr,
			const Ty* v3_front_dir_ptr,
			const Ty* v3_lateral_dir_ptr,
			const Ty* v3_up_dir_ptr,
			Ty altitude_angle_tol = static_cast<Ty>(0),
			Ty azimuth_angle_if_singular = static_cast<Ty>(0)) noexcept;

		static inline cool::rotation_status get_angles(
			Ty* v2_azimuth_altitude_angles_ptr,
			const Ty* v3_direction_ptr,
			const Ty* v3_front_dir_ptr,
			const Ty* v3_lateral_dir_ptr,
			const Ty* v3_up_dir_ptr,
			cool::no_direction_norm_t,
			Ty altitude_angle_tol = static_cast<Ty>(0),
			Ty azimuth_angle_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty> class template_rotation3d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		inline operator cool::rotation_type() const noexcept;
		inline cool::rotation_type rotation_type() const noexcept;

		inline std::size_t axis_count() const noexcept;
		inline std::size_t axis(std::size_t _index) const noexcept;

		inline std::size_t singular_axis() const noexcept;

		inline std::size_t index(std::size_t _axis) const noexcept;
		inline std::size_t iX() const noexcept;
		inline std::size_t iY() const noexcept;
		inline std::size_t iZ() const noexcept;
		inline std::size_t iSg() const noexcept;

		inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept;
		inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept;
		inline cool::rotation_status get_coord(Ty* v4_coord_ptr, const Ty* m3x3_rotation_ptr,
			Ty angle_tol = static_cast<Ty>(0), Ty angle_choice_if_singular = static_cast<Ty>(0)) const noexcept;

	protected:

		template_rotation3d() noexcept {}

		cool::rotation_type m_rotation_type;
		cool::rotation_status(*m_rotation_functions)(Ty*, const Ty*, Ty, Ty, int param);
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotation3d
		: public cool::template_rotation3d<Ty>
	{

	public:

		using typename cool::template_rotation3d<Ty>::value_type;
		using typename cool::template_rotation3d<Ty>::pointer;
		using typename cool::template_rotation3d<Ty>::const_pointer;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		inline rotation3d() noexcept;
		inline rotation3d(cool::rotation_type new_rotation_type) noexcept;
		rotation3d(const cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&) noexcept = default;
		cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>& operator=(const cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&) noexcept = default;
		rotation3d(cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&&) noexcept = default;
		cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>& operator=(cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&&) noexcept = default;
		~rotation3d() = default;

		inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>& operator=(cool::rotation_type new_rotation_type) noexcept;
		inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>& set_rotation_type(cool::rotation_type new_rotation_type) noexcept;

		inline Ty singular_angle(std::size_t _pole) const noexcept;

	private:

		static inline cool::rotation_status m_id(Ty* dest_ptr, const Ty*, Ty, Ty, int param) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::X;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::Y;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::Z;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 1;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::none, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* angle_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationXY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t iSg = 3;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationXZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout,
		int _func_impl_number> class rotationZY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 2;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::none };

		static constexpr std::size_t singular_axis = cool::axis::none;
		static constexpr Ty singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr,
			Ty = static_cast<Ty>(0), Ty = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout,
		int _func_impl_number> class rotationXYZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XYZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXYZ_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationXZY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXZY_ptr, const Ty* m3x3_rotation, Ty angle_tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYZX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYZX_ptr, const Ty* m3x3_rotation, Ty angle_tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYXZ
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YXZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYXZ_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZXY
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZXY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZXY_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZYX
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZYX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>(),
			cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZYX_ptr, const Ty* m3x3_rotation, Ty angle_tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationXYX2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XYX2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXYX2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationXZX2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZX2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::X };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rXZX2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rX_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYZY2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZY2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::Z;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYZY2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationYXY2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YXY2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Y };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rYXY2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rY_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZXZ2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZXZ2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::X;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZXZ2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};


	template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number> class rotationZYZ2
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZYZ2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		static constexpr std::size_t axis_count = 3;
		static constexpr std::size_t axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::Z };

		static constexpr std::size_t singular_axis = cool::axis::Y;
		static constexpr Ty singular_angle[2] = {
			static_cast<Ty>(0),
			cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
		};

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept;
		static inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(Ty* v3_rZYZ2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol,
			Ty rZ_choice_if_singular = static_cast<Ty>(0)) noexcept;
	};

	template <class rotation3d_Ty> class _rotation3d_functions
	{

	public:

		using value_type = typename rotation3d_Ty::value_type;
		using pointer = typename rotation3d_Ty::pointer;
		using const_pointer = typename rotation3d_Ty::const_pointer;

		static inline cool::rotation_status rotation_functions(
			pointer dest_ptr, const_pointer orig_ptr, value_type tol, value_type angle_choice_if_singular, int param) noexcept;
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

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::sqrt(Ty x) noexcept
{
	return std::sqrt(x);
}


template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::pi() noexcept
{
	return static_cast<Ty>(3.141592653589793);
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::half_turn() noexcept
{
	return static_cast<Ty>(3.141592653589793);
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::one() noexcept
{
	return static_cast<Ty>(1);
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::half() noexcept
{
	return static_cast<Ty>(0.5);
}


template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::two() noexcept
{
	constexpr Ty ret = cool::rotation_subroutine::one<Ty, _func_impl_number>()
		+ cool::rotation_subroutine::one<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::quarter_turn() noexcept
{
	constexpr Ty ret = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::quarter() noexcept
{
	constexpr Ty ret = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* cool::rotation_subroutine::half<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::minus_half_turn() noexcept
{
	constexpr Ty ret = -cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::minus_quarter_turn() noexcept
{
	constexpr Ty ret = -cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::minus_one() noexcept
{
	constexpr Ty ret = -cool::rotation_subroutine::one<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline constexpr Ty cool::rotation_subroutine::minus_quarter() noexcept
{
	constexpr Ty ret = -cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
	return ret;
}

template <class Ty, int _func_impl_number>
inline Ty cool::rotation_subroutine::inv_sqrt(Ty x) noexcept
{
	return cool::rotation_subroutine::one<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(x);
}


namespace cool
{
	template <std::size_t _dim_padded, cool::matrix_layout _layout, std::size_t _dim_padded_min> class _rotation_matrix_index_data
	{

	public:

		static_assert(_dim_padded >= _dim_padded_min, "dimension too small");

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
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation2d<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m2x2_rotation_ptr, Ty angle) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 2>;

	Ty cosA = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinA = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m2x2_rotation_ptr + _index_data::i00) = cosA;
	*(m2x2_rotation_ptr + _index_data::i10) = sinA;
	*(m2x2_rotation_ptr + _index_data::i01) = -sinA;
	*(m2x2_rotation_ptr + _index_data::i11) = cosA;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation2d<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m2x2_rotation_ptr, Ty angle) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation2d<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m2x2_rotation_ptr, angle);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation2d<Ty, _dim_padded, _layout, _func_impl_number>::get_angle(
	Ty* angle_ptr, const Ty* m2x2_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 2>;

	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m2x2_rotation_ptr + _index_data::i10),
		*(m2x2_rotation_ptr + _index_data::i00));
}


template <class Ty, int _func_impl_number>
inline void cool::direction2d<Ty, _func_impl_number>::get_direction(
	Ty* v2_direction_ptr,
	Ty angle,
	const Ty* v2_front_dir_ptr,
	const Ty* v2_lateral_dir_ptr) noexcept
{
	Ty cosA = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinA = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	Ty front_dir[2] = { *v2_front_dir_ptr, *(v2_front_dir_ptr + 1) };
	Ty lateral_dir[2] = { *v2_lateral_dir_ptr, *(v2_lateral_dir_ptr + 1) };

	*v2_direction_ptr = cosA * front_dir[0] + sinA * lateral_dir[0];
	*(v2_direction_ptr + 1) = cosA * front_dir[1] + sinA * lateral_dir[1];
}

template <class Ty, int _func_impl_number>
inline void cool::direction2d<Ty, _func_impl_number>::get_angle(
	Ty* angle_ptr,
	const Ty* v2_direction_ptr,
	const Ty* v2_front_dir_ptr,
	const Ty* v2_lateral_dir_ptr) noexcept
{
	Ty cosA = *v2_direction_ptr * *v2_front_dir_ptr + *(v2_direction_ptr + 1) * *(v2_front_dir_ptr + 1);
	Ty sinA = *v2_direction_ptr * *v2_lateral_dir_ptr + *(v2_direction_ptr + 1) * *(v2_lateral_dir_ptr + 1);

	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(sinA, cosA);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty norm_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty cc = cool::rotation_subroutine::one<Ty, _func_impl_number>() - c;

	Ty axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (axis_norm_sq > norm_tol * norm_tol)
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

		*(m3x3_rotation_ptr + _index_data::i00) = temp01 * axis[0] + c;
		*(m3x3_rotation_ptr + _index_data::i11) = temp12 * axis[1] + c;
		*(m3x3_rotation_ptr + _index_data::i22) = temp20 * axis[2] + c;

		temp20 *= axis[0];
		temp01 *= axis[1];
		temp12 *= axis[2];

		axis[0] *= s;
		axis[1] *= s;
		axis[2] *= s;

		*(m3x3_rotation_ptr + _index_data::i21) = temp12 + axis[0];
		*(m3x3_rotation_ptr + _index_data::i12) = temp12 - axis[0];
		*(m3x3_rotation_ptr + _index_data::i20) = temp20 - axis[1];
		*(m3x3_rotation_ptr + _index_data::i02) = temp20 + axis[1];
		*(m3x3_rotation_ptr + _index_data::i10) = temp01 + axis[2];
		*(m3x3_rotation_ptr + _index_data::i01) = temp01 - axis[2];
	}
	else
	{
		*(m3x3_rotation_ptr + _index_data::i00) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*(m3x3_rotation_ptr + _index_data::i10) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + _index_data::i20) = static_cast<Ty>(0);

		*(m3x3_rotation_ptr + _index_data::i01) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + _index_data::i11) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*(m3x3_rotation_ptr + _index_data::i21) = static_cast<Ty>(0);

		*(m3x3_rotation_ptr + _index_data::i02) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + _index_data::i12) = static_cast<Ty>(0);
		*(m3x3_rotation_ptr + _index_data::i22) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, Ty norm_tol) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_axis_angle<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_axis_ptr, angle, norm_tol);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty cc = cool::rotation_subroutine::one<Ty, _func_impl_number>() - c;

	Ty axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	Ty temp01 = cc * axis[0];
	Ty temp12 = cc * axis[1];
	Ty temp20 = cc * axis[2];

	*(m3x3_rotation_ptr + _index_data::i00) = temp01 * axis[0] + c;
	*(m3x3_rotation_ptr + _index_data::i11) = temp12 * axis[1] + c;
	*(m3x3_rotation_ptr + _index_data::i22) = temp20 * axis[2] + c;

	temp20 *= axis[0];
	temp01 *= axis[1];
	temp12 *= axis[2];

	axis[0] *= s;
	axis[1] *= s;
	axis[2] *= s;

	*(m3x3_rotation_ptr + _index_data::i21) = temp12 + axis[0];
	*(m3x3_rotation_ptr + _index_data::i12) = temp12 - axis[0];
	*(m3x3_rotation_ptr + _index_data::i20) = temp20 - axis[1];
	*(m3x3_rotation_ptr + _index_data::i02) = temp20 + axis[1];
	*(m3x3_rotation_ptr + _index_data::i10) = temp01 + axis[2];
	*(m3x3_rotation_ptr + _index_data::i01) = temp01 - axis[2];
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_axis_angle<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_axis_ptr, angle, angle, cool::no_axis_norm);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty quat_comp_sq[4];
	Ty quat_comp_sgn[4];

	{
		Ty a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
		Ty b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
		Ty a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
		Ty b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
		Ty a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
		Ty b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

		Ty c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
			+ *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
			- *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
			+ *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
			- *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[1] = (a0 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[2] = (a1 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[3] = (a2 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
	}

	Ty quat[4] = {
		quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]),
		quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]),
		quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]),
		quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3])
	};

	Ty xyz_norm = cool::rotation_subroutine::quarter<Ty, _func_impl_number>()
		* cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* (cool::rotation_subroutine::pi<Ty, _func_impl_number>()
			/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>());

	if (xyz_norm > angle_tol_coeff * angle_tol)
	{
		Ty xyz_norm_inv = cool::rotation_subroutine::one<Ty, _func_impl_number>() / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = cool::rotation_subroutine::two<Ty, _func_impl_number>()
			* cool::rotation_subroutine::atan2<Ty, _func_impl_number>(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<Ty>(0);
		*(v3_axis_ptr + 1) = static_cast<Ty>(0);
		*(v3_axis_ptr + 2) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, const Ty* m3x3_rotation_ptr, const Ty* v3_axis_way_ptr, Ty angle_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty quat_comp_sq[4];
	Ty quat_comp_sgn[4];

	{
		Ty a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
		Ty b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
		Ty a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
		Ty b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
		Ty a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
		Ty b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

		Ty c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
			+ *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
			- *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
			+ *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
			- *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[1] = (a0 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[2] = (a1 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
		quat_comp_sgn[3] = (a2 < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>();
	}

	Ty quat[4] = {
		quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]),
		quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]),
		quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]),
		quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3])
	};

	Ty xyz_norm = cool::rotation_subroutine::quarter<Ty, _func_impl_number>()
		* cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* (cool::rotation_subroutine::pi<Ty, _func_impl_number>()
			/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>());

	if (xyz_norm > angle_tol_coeff * angle_tol)
	{
		Ty param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		Ty flip = (param < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::one<Ty, _func_impl_number>();

		Ty xyz_norm_inv = flip / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * cool::rotation_subroutine::two<Ty, _func_impl_number>()
			* cool::rotation_subroutine::atan2<Ty, _func_impl_number>(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		Ty axis_way[3] = { *v3_axis_way_ptr, *(v3_axis_way_ptr + 1), *(v3_axis_way_ptr + 2) };

		Ty axis_way_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(
			axis_way[0] * axis_way[0] + axis_way[1] * axis_way[1] + axis_way[2] * axis_way[2]);

		*v3_axis_ptr = axis_way_norm_inv * axis_way[0];
		*(v3_axis_ptr + 1) = axis_way_norm_inv * axis_way[1];
		*(v3_axis_ptr + 2) = axis_way_norm_inv * axis_way[2];
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_angle(Ty* angle_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty three = cool::rotation_subroutine::two<Ty, _func_impl_number>()
		+ cool::rotation_subroutine::one<Ty, _func_impl_number>();

	Ty temp = three - (*(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22));
	temp = (temp >= static_cast<Ty>(0)) ? temp : static_cast<Ty>(0);

	Ty sin_angle_d2 = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(temp);

	if (sin_angle_d2 < cool::rotation_subroutine::one<Ty, _func_impl_number>())
	{
		*angle_ptr = cool::rotation_subroutine::two<Ty, _func_impl_number>()
			* cool::rotation_subroutine::asin<Ty, _func_impl_number>(sin_angle_d2);
	}
	else
	{
		*angle_ptr = cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty q00 = *v4_quaternion_ptr * *v4_quaternion_ptr;
	Ty q11 = *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	Ty q22 = *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	Ty q33 = *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);
	Ty s = cool::rotation_subroutine::two<Ty, _func_impl_number>() / (q00 + q11 + q22 + q33);
	q00 *= s; q11 *= s; q22 *= s; q33 *= s;
	Ty q01 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	Ty q02 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	Ty q03 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	Ty q12 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	Ty q13 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	Ty q23 = s * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);


	*(m3x3_rotation_ptr + _index_data::i00) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q22 - q33;
	*(m3x3_rotation_ptr + _index_data::i10) = q12 + q03;
	*(m3x3_rotation_ptr + _index_data::i20) = q13 - q02;

	*(m3x3_rotation_ptr + _index_data::i01) = q12 - q03;
	*(m3x3_rotation_ptr + _index_data::i11) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q11 - q33;
	*(m3x3_rotation_ptr + _index_data::i21) = q23 + q01;

	*(m3x3_rotation_ptr + _index_data::i02) = q13 + q02;
	*(m3x3_rotation_ptr + _index_data::i12) = q23 - q01;
	*(m3x3_rotation_ptr + _index_data::i22) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q11 - q22;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_quaternion<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v4_quaternion_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty q00 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *v4_quaternion_ptr * *v4_quaternion_ptr;
	Ty q01 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	Ty q02 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	Ty q03 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	Ty q11 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	Ty q12 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	Ty q13 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	Ty q22 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	Ty q23 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);
	Ty q33 = cool::rotation_subroutine::two<Ty, _func_impl_number>() * *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);

	*(m3x3_rotation_ptr + _index_data::i00) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q22 - q33;
	*(m3x3_rotation_ptr + _index_data::i10) = q12 + q03;
	*(m3x3_rotation_ptr + _index_data::i20) = q13 - q02;

	*(m3x3_rotation_ptr + _index_data::i01) = q12 - q03;
	*(m3x3_rotation_ptr + _index_data::i11) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q11 - q33;
	*(m3x3_rotation_ptr + _index_data::i21) = q23 + q01;

	*(m3x3_rotation_ptr + _index_data::i02) = q13 + q02;
	*(m3x3_rotation_ptr + _index_data::i12) = q23 - q01;
	*(m3x3_rotation_ptr + _index_data::i22) = cool::rotation_subroutine::one<Ty, _func_impl_number>() - q11 - q22;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_quaternion<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v4_quaternion_ptr, cool::no_quaternion_norm);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_matrix(
	Ty* v4_quaternion_ptr, const Ty* m3x3_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty quat_comp_sq[4];

	Ty a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
	Ty b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
	Ty a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
	Ty b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
	Ty a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
	Ty b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

	Ty c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
		+ *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

	c = cool::rotation_subroutine::one<Ty, _func_impl_number>() + *(m3x3_rotation_ptr + _index_data::i00)
		- *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

	c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
		+ *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

	c = cool::rotation_subroutine::one<Ty, _func_impl_number>() - *(m3x3_rotation_ptr + _index_data::i00)
		- *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

	Ty quat_comp_sgn[4] = {
		cool::rotation_subroutine::quarter<Ty, _func_impl_number>(),
		(a0 < static_cast<Ty>(0)) ?
		cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>(),
		(a1 < static_cast<Ty>(0)) ?
		cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>(),
		(a2 < static_cast<Ty>(0)) ?
		cool::rotation_subroutine::minus_quarter<Ty, _func_impl_number>() : cool::rotation_subroutine::quarter<Ty, _func_impl_number>()
	};

	*v4_quaternion_ptr = quat_comp_sgn[0] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[0]);
	*(v4_quaternion_ptr + 1) = quat_comp_sgn[1] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[1]);
	*(v4_quaternion_ptr + 2) = quat_comp_sgn[2] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[2]);
	*(v4_quaternion_ptr + 3) = quat_comp_sgn[3] * cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(quat_comp_sq[3]);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_axis_angle(
	Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle, Ty norm_tol) noexcept
{
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(
		cool::rotation_subroutine::half<Ty, _func_impl_number>() * angle);
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(
		cool::rotation_subroutine::half<Ty, _func_impl_number>() * angle);

	Ty axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (axis_norm_sq > norm_tol * norm_tol)
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
		*v4_quaternion_ptr = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*(v4_quaternion_ptr + 1) = static_cast<Ty>(0);
		*(v4_quaternion_ptr + 2) = static_cast<Ty>(0);
		*(v4_quaternion_ptr + 3) = static_cast<Ty>(0);
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_axis_angle(
	Ty* v4_quaternion_ptr, const Ty* v3_axis_ptr, Ty angle, cool::no_axis_norm_t) noexcept
{
	Ty c = cool::rotation_subroutine::cos<Ty, _func_impl_number>(
		cool::rotation_subroutine::half<Ty, _func_impl_number>() * angle);
	Ty s = cool::rotation_subroutine::sin<Ty, _func_impl_number>(
		cool::rotation_subroutine::half<Ty, _func_impl_number>() * angle);

	Ty axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	*v4_quaternion_ptr = c;
	*(v4_quaternion_ptr + 1) = s * axis[0];
	*(v4_quaternion_ptr + 2) = s * axis[1];
	*(v4_quaternion_ptr + 3) = s * axis[2];
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, Ty* v4_quaternion_ptr, Ty angle_tol) noexcept
{
	Ty quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	Ty xyz_norm = cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* (cool::rotation_subroutine::pi<Ty, _func_impl_number>()
			/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>());

	if (xyz_norm > angle_tol_coeff * angle_tol)
	{
		Ty xyz_norm_inv = cool::rotation_subroutine::one<Ty, _func_impl_number>() / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];

		*angle_ptr = cool::rotation_subroutine::two<Ty, _func_impl_number>()
			* cool::rotation_subroutine::atan2<Ty, _func_impl_number>(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<Ty>(0);
		*(v3_axis_ptr + 1) = static_cast<Ty>(0);
		*(v3_axis_ptr + 2) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(
	Ty* v3_axis_ptr, Ty* angle_ptr, Ty* v4_quaternion_ptr, const Ty* v3_axis_way_ptr, Ty angle_tol) noexcept
{
	Ty quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	Ty xyz_norm = cool::rotation_subroutine::sqrt<Ty, _func_impl_number>(
		quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>()
		* (cool::rotation_subroutine::pi<Ty, _func_impl_number>()
			/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>());

	if (xyz_norm > angle_tol_coeff * angle_tol)
	{
		Ty angle = cool::rotation_subroutine::two<Ty, _func_impl_number>()
			* cool::rotation_subroutine::atan2<Ty, _func_impl_number>(xyz_norm, quat[0]);

		Ty param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		param = (param != static_cast<Ty>(0)) ? param : angle;

		Ty flip = (param < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::one<Ty, _func_impl_number>();

		Ty xyz_norm_inv = flip / xyz_norm;

		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * angle;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty axis_way[3] = { *v3_axis_way_ptr, *(v3_axis_way_ptr + 1), *(v3_axis_way_ptr + 2) };

		Ty axis_way_norm_inv = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(
			axis_way[0] * axis_way[0] + axis_way[1] * axis_way[1] + axis_way[2] * axis_way[2]);

		*v3_axis_ptr = axis_way_norm_inv * axis_way[0];
		*(v3_axis_ptr + 1) = axis_way_norm_inv * axis_way[1]
		*(v3_axis_ptr + 2) = axis_way_norm_inv * axis_way[2];
		*angle_ptr = static_cast<Ty>(0);

		return cool::rotation_status::singular;
	}
}


template <class Ty, int _func_impl_number>
inline void cool::direction3d<Ty, _func_impl_number>::get_direction(
	Ty* v3_direction_ptr,
	const Ty* v2_azimuth_altitude_angles_ptr,
	const Ty* v3_front_dir_ptr,
	const Ty* v3_lateral_dir_ptr,
	const Ty* v3_up_dir_ptr) noexcept
{
	Ty cosAZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_azimuth_altitude_angles_ptr);
	Ty cosAL = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_azimuth_altitude_angles_ptr + 1));
	Ty sinAZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_azimuth_altitude_angles_ptr);
	Ty sinAL = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_azimuth_altitude_angles_ptr + 1));

	Ty coord[3] = { cosAZ * cosAL, sinAZ * cosAL, sinAL };

	Ty front_dir[3] = { *v3_front_dir_ptr, *(v3_front_dir_ptr + 1), *(v3_front_dir_ptr + 2) };
	Ty lateral_dir[3] = { *v3_lateral_dir_ptr, *(v3_lateral_dir_ptr + 1), *(v3_lateral_dir_ptr + 2) };
	Ty up_dir[3] = { *v3_up_dir_ptr, *(v3_up_dir_ptr + 1), *(v3_up_dir_ptr + 2) };

	*v3_direction_ptr = coord[0] * front_dir[0] + coord[1] * lateral_dir[0] + coord[2] * up_dir[0];
	*(v3_direction_ptr + 1) = coord[0] * front_dir[1] + coord[1] * lateral_dir[1] + coord[2] * up_dir[1];
	*(v3_direction_ptr + 2) = coord[0] * front_dir[2] + coord[1] * lateral_dir[2] + coord[2] * up_dir[2];
}

template <class Ty, int _func_impl_number>
inline cool::rotation_status cool::direction3d<Ty, _func_impl_number>::get_angles(
	Ty* v2_azimuth_altitude_angles_ptr,
	const Ty* v3_direction_ptr,
	const Ty* v3_front_dir_ptr,
	const Ty* v3_lateral_dir_ptr,
	const Ty* v3_up_dir_ptr,
	Ty altitude_angle_tol,
	Ty azimuth_angle_if_singular) noexcept
{
	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty direction[3] = { *v3_direction_ptr, *(v3_direction_ptr + 1), *(v3_direction_ptr + 2) };

	{
		Ty s = cool::rotation_subroutine::inv_sqrt<Ty, _func_impl_number>(
			direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
		direction[0] *= s; direction[1] *= s; direction[2] *= s;
	}

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (altitude_angle_tol * altitude_angle_tol);

	Ty sinAL = direction[0] * *v3_up_dir_ptr + direction[1] * *(v3_up_dir_ptr + 1) + direction[2] * *(v3_up_dir_ptr + 2);

	if (-bound < sinAL && sinAL < bound)
	{
		*v2_azimuth_altitude_angles_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rAZ
			direction[0] * *v3_lateral_dir_ptr + direction[1] * *(v3_lateral_dir_ptr + 1) + direction[2] * *(v3_lateral_dir_ptr + 2),
			direction[0] * *v3_front_dir_ptr + direction[1] * *(v3_front_dir_ptr + 1) + direction[2] * *(v3_front_dir_ptr + 2)
		);
		*(v2_azimuth_altitude_angles_ptr + 1) = cool::rotation_subroutine::asin<Ty, _func_impl_number>(sinAL); // rAL

		return cool::rotation_status::regular;
	}
	else
	{
		*v2_azimuth_altitude_angles_ptr = azimuth_angle_if_singular; // rAZ
		*(v2_azimuth_altitude_angles_ptr + 1) = (sinAL < static_cast<Ty>(0)) ? // rAL
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>()
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();

		return cool::rotation_status::singular;
	}
}

template <class Ty, int _func_impl_number>
inline cool::rotation_status cool::direction3d<Ty, _func_impl_number>::get_angles(
	Ty* v2_azimuth_altitude_angles_ptr,
	const Ty* v3_direction_ptr,
	const Ty* v3_front_dir_ptr,
	const Ty* v3_lateral_dir_ptr,
	const Ty* v3_up_dir_ptr,
	cool::no_direction_norm_t,
	Ty altitude_angle_tol,
	Ty azimuth_angle_if_singular) noexcept
{
	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (altitude_angle_tol * altitude_angle_tol);

	Ty sinAL = *v3_direction_ptr * *v3_up_dir_ptr + *(v3_direction_ptr + 1) * *(v3_up_dir_ptr + 1)
		+ *(v3_direction_ptr + 2) * *(v3_up_dir_ptr + 2);

	if (-bound < sinAL && sinAL < bound)
	{
		*v2_azimuth_altitude_angles_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rAZ
			*v3_direction_ptr * *v3_lateral_dir_ptr + *(v3_direction_ptr + 1) * *(v3_lateral_dir_ptr + 1)
			+ *(v3_direction_ptr + 2) * *(v3_lateral_dir_ptr + 2),
			*v3_direction_ptr * *v3_front_dir_ptr + *(v3_direction_ptr + 1) * *(v3_front_dir_ptr + 1)
			+ *(v3_direction_ptr + 2) * *(v3_front_dir_ptr + 2)
		);
		*(v2_azimuth_altitude_angles_ptr + 1) = cool::rotation_subroutine::asin<Ty, _func_impl_number>(sinAL); // rAL

		return cool::rotation_status::regular;
	}
	else
	{
		*v2_azimuth_altitude_angles_ptr = azimuth_angle_if_singular; // rAZ
		*(v2_azimuth_altitude_angles_ptr + 1) = (sinAL < static_cast<Ty>(0)) ? // rAL
			cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>()
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();

		return cool::rotation_status::singular;
	}
}


inline constexpr std::size_t cool::rotation_type_as_index(cool::rotation_type _rotation_type) noexcept
{
	return static_cast<std::size_t>(static_cast<std::uint32_t>(_rotation_type) >> 16);
}

inline constexpr cool::rotation_type cool::index_as_rotation_type(std::size_t _index) noexcept
{
	switch (_index)
	{

	case 0: return cool::rotation_type::id; break;

	case 1: return cool::rotation_type::X; break;
	case 2: return cool::rotation_type::Y; break;
	case 3: return cool::rotation_type::Z; break;

	case 4: return cool::rotation_type::XY; break;
	case 5: return cool::rotation_type::XZ; break;
	case 6: return cool::rotation_type::YZ; break;
	case 7: return cool::rotation_type::YX; break;
	case 8: return cool::rotation_type::ZX; break;
	case 9: return cool::rotation_type::ZY; break;

	case 10: return cool::rotation_type::XYZ; break;
	case 11: return cool::rotation_type::XZY; break;
	case 12: return cool::rotation_type::YZX; break;
	case 13: return cool::rotation_type::YXZ; break;
	case 14: return cool::rotation_type::ZXY; break;
	case 15: return cool::rotation_type::ZYX; break;

	case 16: return cool::rotation_type::XYX2; break;
	case 17: return cool::rotation_type::XZX2; break;
	case 18: return cool::rotation_type::YZY2; break;
	case 19: return cool::rotation_type::YXY2; break;
	case 20: return cool::rotation_type::ZXZ2; break;
	case 21: return cool::rotation_type::ZYZ2; break;

	case 22: return cool::rotation_type::AA; break;
	case 23: return cool::rotation_type::Q; break;

	default: return cool::rotation_type::id; break;
	}
}


template <class Ty>
inline cool::template_rotation3d<Ty>::operator cool::rotation_type() const noexcept
{
	return m_rotation_type;
}

template <class Ty>
inline cool::rotation_type cool::template_rotation3d<Ty>::rotation_type() const noexcept
{
	return m_rotation_type;
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::axis_count() const noexcept
{
	constexpr std::size_t limit1 = cool::rotation_type_as_index(cool::rotation_type::X);
	constexpr std::size_t limit2 = cool::rotation_type_as_index(cool::rotation_type::XY);
	constexpr std::size_t limit3 = cool::rotation_type_as_index(cool::rotation_type::XYZ);
	constexpr std::size_t limit5 = cool::rotation_type_as_index(cool::rotation_type::AA);

	std::size_t _index = cool::rotation_type_as_index(m_rotation_type);

	return (_index >= limit5) ? 1
		: static_cast<std::size_t>(_index >= limit1)
		+ static_cast<std::size_t>(_index >= limit2)
		+ static_cast<std::size_t>(_index >= limit3);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::axis(std::size_t _index) const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> (_index * 2 + 8)) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::singular_axis() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 14) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::index(std::size_t _axis) const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> (_axis * 2)) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::iX() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>(static_cast<std::uint32_t>(m_rotation_type) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::iY() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 2) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::iZ() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 4) & mask);
}

template <class Ty>
inline std::size_t cool::template_rotation3d<Ty>::iSg() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 6) & mask);
}

template <class Ty>
inline void cool::template_rotation3d<Ty>::get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept
{
	this->m_rotation_functions(m3x3_rotation_ptr, v4_coord_ptr, static_cast<Ty>(0), static_cast<Ty>(0), 0);
}

template <class Ty>
inline void cool::template_rotation3d<Ty>::get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept
{
	this->m_rotation_functions(m3x3_rotation_ptr, v4_coord_ptr, static_cast<Ty>(0), static_cast<Ty>(0), 1);
}

template <class Ty>
inline cool::rotation_status cool::template_rotation3d<Ty>::get_coord(Ty* v4_coord_ptr, const Ty* m3x3_rotation_ptr,
	Ty angle_tol, Ty angle_choice_if_singular) const noexcept
{
	return this->m_rotation_functions(v4_coord_ptr, m3x3_rotation_ptr, angle_tol, angle_choice_if_singular, 2);
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::rotation3d() noexcept
{
	this->m_rotation_type = cool::rotation_type::id;
	this->m_rotation_functions = m_id;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::rotation3d(cool::rotation_type new_rotation_type) noexcept
{
	set_rotation_type(new_rotation_type);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&
cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::operator=(cool::rotation_type new_rotation_type) noexcept
{
	return set_rotation_type(new_rotation_type);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>&
cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::set_rotation_type(cool::rotation_type new_rotation_type) noexcept
{
	this->m_rotation_type = new_rotation_type;

	switch (cool::rotation_type_as_index(new_rotation_type))
	{

	case cool::rotation_type_as_index(cool::rotation_type::id):
		this->m_rotation_functions = m_id; break;

	case cool::rotation_type_as_index(cool::rotation_type::X):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::Y):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::Z):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XYZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YXZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZXY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZYX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XYX2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZX2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZY2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YXY2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZXZ2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZYZ2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::AA):
	{
		this->m_rotation_functions = [](Ty* dest_ptr, const Ty* orig_ptr, Ty angle_tol, Ty angle_choice_if_singular, int param)
		{
			if (param == 0)
			{
				cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(dest_ptr, orig_ptr, *(orig_ptr + 3), static_cast<Ty>(0));
				return cool::rotation_status::regular;
			}
			else if (param == 1)
			{
				cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(dest_ptr, orig_ptr, *(orig_ptr + 3), static_cast<Ty>(0));
				return cool::rotation_status::regular;
			}
			else
			{
				return cool::rotation_axis_angle<Ty, _dim_padded, _layout, _func_impl_number>::get_axis_angle(dest_ptr, dest_ptr + 3, orig_ptr, angle_tol);
			}
		};
	}
	break;

	case cool::rotation_type_as_index(cool::rotation_type::Q):
	{
		this->m_rotation_functions = [](Ty* dest_ptr, const Ty* orig_ptr, Ty angle_tol, Ty angle_choice_if_singular, int param)
		{
			if (param == 0)
			{
				cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
			else if (param == 1)
			{
				cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
			else
			{
				cool::rotation_quaternion<Ty, _dim_padded, _layout, _func_impl_number>::get_quaternion_from_matrix(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
		};
	}
	break;

	}

	return *this;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline Ty cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::singular_angle(std::size_t _pole) const noexcept
{
	constexpr std::uint32_t limit3 = cool::rotation_type_as_index(cool::rotation_type::XYZ);
	constexpr std::uint32_t limit4 = cool::rotation_type_as_index(cool::rotation_type::XYX2);
	constexpr std::uint32_t limit5 = cool::rotation_type_as_index(cool::rotation_type::AA);

	std::uint32_t index = cool::rotation_type_as_index(this->m_rotation_type);

	if (limit3 <= index && index < limit5)
	{
		if (index < limit4)
		{
			return (_pole == 0) ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>()
				: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		}
		else
		{
			return (_pole == 0) ? static_cast<Ty>(0) : cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
		}
	}
	else
	{
		return Ty();
	}
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotation3d<Ty, _dim_padded, _layout, _func_impl_number>::m_id(Ty* dest_ptr, const Ty*, Ty, Ty, int param) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	if (param != 2)
	{
		*(dest_ptr + _index_data::i00) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*(dest_ptr + _index_data::i10) = static_cast<Ty>(0);
		*(dest_ptr + _index_data::i20) = static_cast<Ty>(0);

		*(dest_ptr + _index_data::i01) = static_cast<Ty>(0);
		*(dest_ptr + _index_data::i11) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
		*(dest_ptr + _index_data::i21) = static_cast<Ty>(0);

		*(dest_ptr + _index_data::i02) = static_cast<Ty>(0);
		*(dest_ptr + _index_data::i12) = static_cast<Ty>(0);
		*(dest_ptr + _index_data::i22) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
	}

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty angle = *angle_ptr;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationX<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m3x3_rotation_ptr + _index_data::i21),
		*(m3x3_rotation_ptr + _index_data::i11));

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty angle = *angle_ptr;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationY<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		-*(m3x3_rotation_ptr + _index_data::i20),
		*(m3x3_rotation_ptr + _index_data::i00));

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty angle = *angle_ptr;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(angle);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = cool::rotation_subroutine::one<Ty, _func_impl_number>();
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZ<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* angle_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	*angle_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
		*(m3x3_rotation_ptr + _index_data::i10),
		*(m3x3_rotation_ptr + _index_data::i00));

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rXY_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rXY_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rXY_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rXY_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = sinX * sinY;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosY;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXY<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rXY_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXY_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rY = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		*(m3x3_rotation_ptr + _index_data::i02),
		*(m3x3_rotation_ptr + _index_data::i00));
	*v2_rXY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		*(m3x3_rotation_ptr + _index_data::i21),
		*(m3x3_rotation_ptr + _index_data::i11));
	*(v2_rXY_ptr + 1) = rY;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rXZ_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rXZ_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rXZ_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rXZ_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = sinX * sinZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosZ;

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rXZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZ<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rXZ_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rXZ_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rZ = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		-*(m3x3_rotation_ptr + _index_data::i01),
		*(m3x3_rotation_ptr + _index_data::i00));
	*v2_rXZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		-*(m3x3_rotation_ptr + _index_data::i12),
		*(m3x3_rotation_ptr + _index_data::i22));
	*(v2_rXZ_ptr + 1) = rZ;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rYZ_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rYZ_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rYZ_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rYZ_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZ<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rYZ_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYZ_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rZ = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		*(m3x3_rotation_ptr + _index_data::i10),
		*(m3x3_rotation_ptr + _index_data::i11));
	*v2_rYZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		*(m3x3_rotation_ptr + _index_data::i02),
		*(m3x3_rotation_ptr + _index_data::i22));
	*(v2_rYZ_ptr + 1) = rZ;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rYX_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rYX_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rYX_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rYX_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<Ty>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = sinX * sinY;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosY;

	*(m3x3_rotation_ptr + _index_data::i02) = cosX * sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rYX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYX<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rYX_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rYX_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rX = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX;
		-*(m3x3_rotation_ptr + _index_data::i12),
		*(m3x3_rotation_ptr + _index_data::i11));
	*v2_rYX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		-*(m3x3_rotation_ptr + _index_data::i20),
		*(m3x3_rotation_ptr + _index_data::i00));
	*(v2_rYX_ptr + 1) = rX;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rZX_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rZX_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rZX_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rZX_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = -cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = sinX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZX<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rZX_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZX_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rX = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
		*(m3x3_rotation_ptr + _index_data::i21),
		*(m3x3_rotation_ptr + _index_data::i22));
	*v2_rZX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		*(m3x3_rotation_ptr + _index_data::i10),
		*(m3x3_rotation_ptr + _index_data::i00));
	*(v2_rZX_ptr + 1) = rX;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v2_rZY_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v2_rZY_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v2_rZY_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v2_rZY_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<Ty>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i12) = sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v2_rZY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZY<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v2_rZY_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v2_rZY_ptr, const Ty* m3x3_rotation_ptr, Ty, Ty) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty rY = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
		-*(m3x3_rotation_ptr + _index_data::i20),
		*(m3x3_rotation_ptr + _index_data::i22));
	*v2_rZY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
		-*(m3x3_rotation_ptr + _index_data::i01),
		*(m3x3_rotation_ptr + _index_data::i11));
	*(v2_rZY_ptr + 1) = rY;

	return cool::rotation_status::regular;
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXYZ_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 1));
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXYZ_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYZ_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosX * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i20) = sinX * sinZ - cosX * sinY * cosZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ - sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosZ + cosX * sinY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosY;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXYZ<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rXYZ_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXYZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXYZ_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i02) && *(m3x3_rotation_ptr + _index_data::i02) < bound)
	{
		Ty rY = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i02)); // rY
		Ty rZ = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			-*(m3x3_rotation_ptr + _index_data::i01),
			*(m3x3_rotation_ptr + _index_data::i00));
		*v3_rXYZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			-*(m3x3_rotation_ptr + _index_data::i12),
			*(m3x3_rotation_ptr + _index_data::i22));
		*(v3_rXYZ_ptr + 1) = rY;
		*(v3_rXYZ_ptr + 2) = rZ;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rZ0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i10),
			*(m3x3_rotation_ptr + _index_data::i11));
		bool Yneg = *(m3x3_rotation_ptr + _index_data::i02) < static_cast<Ty>(0);
		*v3_rXYZ_ptr = rX_choice_if_singular; // rX
		*(v3_rXYZ_ptr + 1) = Yneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rY
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rXYZ_ptr + 2) = Yneg ? rZ0 + rX_choice_if_singular : rZ0 - rX_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXZY_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZY_ptr + 1));
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZY_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXZY_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZY_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZY_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinX * sinY + cosX * cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosX * sinY + sinX * cosY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosZ;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosY + cosX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY + sinX * sinY * sinZ;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZY<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rXZY_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXZY_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i01) && *(m3x3_rotation_ptr + _index_data::i01) < bound)
	{
		Ty rZ = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i01)); // rZ
		Ty rY = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			*(m3x3_rotation_ptr + _index_data::i02),
			*(m3x3_rotation_ptr + _index_data::i00));
		*v3_rXZY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			*(m3x3_rotation_ptr + _index_data::i21),
			*(m3x3_rotation_ptr + _index_data::i11));
		*(v3_rXZY_ptr + 1) = rZ;
		*(v3_rXZY_ptr + 2) = rY;

		return cool::rotation_status::regular;
	}
	else
	{	
		Ty rY0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i20),
			*(m3x3_rotation_ptr + _index_data::i22));
		bool Zneg = !(*(m3x3_rotation_ptr + _index_data::i01) < static_cast<Ty>(0));
		*v3_rXZY_ptr = rX_choice_if_singular; // rX
		*(v3_rXZY_ptr + 1) = Zneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rZ
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rXZY_ptr + 2) = Zneg ? rY0 - rX_choice_if_singular : rY0 + rX_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYZX_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZX_ptr + 1));
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZX_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYZX_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZX_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZX_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosZ;

	*(m3x3_rotation_ptr + _index_data::i01) = sinX * sinY - cosX * cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosY + cosX * sinY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i02) = cosX * sinY + sinX * cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY - sinX * sinY * sinZ;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZX<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rYZX_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYZX_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i10) && *(m3x3_rotation_ptr + _index_data::i10) < bound)
	{
		Ty rZ = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i10)); // rZ
		Ty rX = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			-*(m3x3_rotation_ptr + _index_data::i12),
			*(m3x3_rotation_ptr + _index_data::i11));
		*v3_rYZX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			-*(m3x3_rotation_ptr + _index_data::i20),
			*(m3x3_rotation_ptr + _index_data::i00));
		*(v3_rYZX_ptr + 1) = rZ;
		*(v3_rYZX_ptr + 2) = rX;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rX0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i21),
			*(m3x3_rotation_ptr + _index_data::i22));
		bool Zneg = *(m3x3_rotation_ptr + _index_data::i10) < static_cast<Ty>(0);
		*v3_rYZX_ptr = rY_choice_if_singular; // rY
		*(v3_rYZX_ptr + 1) = Zneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rZ
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rYZX_ptr + 2) = Zneg ? rX0 + rY_choice_if_singular : rX0 - rY_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYXZ_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 1));
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYXZ_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 1));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXZ_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ + sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosZ + sinX * cosY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosY * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinY * sinZ + sinX * cosY * cosZ;

	*(m3x3_rotation_ptr + _index_data::i02) = cosX * sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYXZ<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rYXZ_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYXZ<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYXZ_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i12) && *(m3x3_rotation_ptr + _index_data::i12) < bound)
	{
		Ty rX = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i12)); // rX	
		Ty rZ = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			*(m3x3_rotation_ptr + _index_data::i10),
			*(m3x3_rotation_ptr + _index_data::i11));
		*v3_rYXZ_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			*(m3x3_rotation_ptr + _index_data::i02),
			*(m3x3_rotation_ptr + _index_data::i22));
		*(v3_rYXZ_ptr + 1) = rX;
		*(v3_rYXZ_ptr + 2) = rZ;

		return cool::rotation_status::regular;
	}
	else
	{	
		Ty rZ0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i01),
			*(m3x3_rotation_ptr + _index_data::i00));
		bool Xneg = !(*(m3x3_rotation_ptr + _index_data::i12) < static_cast<Ty>(0));
		*v3_rYXZ_ptr = rY_choice_if_singular; // rY
		*(v3_rYXZ_ptr + 1) = Xneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rX
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rYXZ_ptr + 2) = Xneg ? rZ0 - rY_choice_if_singular : rZ0 + rY_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZXY_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXY_ptr + 1));
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXY_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZXY_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXY_ptr + 1));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXY_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ - sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosY * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosZ + sinX * cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i12) = sinY * sinZ - sinX * cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZXY<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rZXY_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZXY<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZXY_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i21) && *(m3x3_rotation_ptr + _index_data::i21) < bound)
	{
		Ty rX = cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i21)); // rX
		Ty rY = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			-*(m3x3_rotation_ptr + _index_data::i20),
			*(m3x3_rotation_ptr + _index_data::i22));
		*v3_rZXY_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			-*(m3x3_rotation_ptr + _index_data::i01),
			*(m3x3_rotation_ptr + _index_data::i11));
		*(v3_rZXY_ptr + 1) = rX;
		*(v3_rZXY_ptr + 2) = rY;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rY0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i02),
			*(m3x3_rotation_ptr + _index_data::i00));
		bool Xneg = *(m3x3_rotation_ptr + _index_data::i21) < static_cast<Ty>(0);
		*v3_rZXY_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZXY_ptr + 1) = Xneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rX
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rZXY_ptr + 2) = Xneg ? rY0 + rZ_choice_if_singular : rY0 - rZ_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZYX_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYX_ptr + 1));
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYX_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZYX_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYX_ptr + 1));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYX_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosX * sinZ + sinX * sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ + sinX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosY;

	*(m3x3_rotation_ptr + _index_data::i02) = sinX * sinZ + cosX * sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosZ + cosX * sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZYX<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rZYX_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZYX<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZYX_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i20) && *(m3x3_rotation_ptr + _index_data::i20) < bound)
	{
		Ty rY = -cool::rotation_subroutine::asin<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i20)); // rY
		Ty rX = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			*(m3x3_rotation_ptr + _index_data::i21),
			*(m3x3_rotation_ptr + _index_data::i22));
		*v3_rZYX_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			*(m3x3_rotation_ptr + _index_data::i10),
			*(m3x3_rotation_ptr + _index_data::i00));
		*(v3_rZYX_ptr + 1) = rY;
		*(v3_rZYX_ptr + 2) = rX;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rX0 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i12),
			*(m3x3_rotation_ptr + _index_data::i11));
		bool Yneg = !(*(m3x3_rotation_ptr + _index_data::i20) < static_cast<Ty>(0));
		*v3_rZYX_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZYX_ptr + 1) = Yneg ? cool::rotation_subroutine::minus_quarter_turn<Ty, _func_impl_number>() // rY
			: cool::rotation_subroutine::quarter_turn<Ty, _func_impl_number>();
		*(v3_rZYX_ptr + 2) = Yneg ? rX0 - rZ_choice_if_singular : rX0 + rZ_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXYX2_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 1));
	Ty cosX2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXYX2_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 1));
	Ty sinX2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXYX2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = sinX * sinY;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = sinY * sinX2;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosX2 - sinX * cosY * sinX2;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosX2 + cosX * cosY * sinX2;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosX2;
	*(m3x3_rotation_ptr + _index_data::i12) = -cosX * sinX2 - sinX * cosY * cosX2;
	*(m3x3_rotation_ptr + _index_data::i22) = -sinX * sinX2 + cosX * cosY * cosX2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXYX2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXYX2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rXYX2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXYX2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXYX2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i00) && *(m3x3_rotation_ptr + _index_data::i00) < bound)
	{
		Ty rYabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i00)); // rY
		Ty rYsgn = (*(m3x3_rotation_ptr + _index_data::i02) < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::one<Ty, _func_impl_number>();
		Ty rX2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX2
			rYsgn * *(m3x3_rotation_ptr + _index_data::i01),
			rYsgn * *(m3x3_rotation_ptr + _index_data::i02));
		*v3_rXYX2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			rYsgn * *(m3x3_rotation_ptr + _index_data::i10),
			-rYsgn * *(m3x3_rotation_ptr + _index_data::i20));
		*(v3_rXYX2_ptr + 1) = rYsgn * rYabs;
		*(v3_rXYX2_ptr + 2) = rX2;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rX20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i12),
			*(m3x3_rotation_ptr + _index_data::i11));
		bool Yflipped = *(m3x3_rotation_ptr + _index_data::i00) < static_cast<Ty>(0);
		*v3_rXYX2_ptr = rX_choice_if_singular; // rX
		*(v3_rXYX2_ptr + 1) = Yflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rY
		*(v3_rXYX2_ptr + 2) = Yflipped ? rX20 + rX_choice_if_singular : rX20 - rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rXZX2_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 1));
	Ty cosX2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 2));
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rXZX2_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 1));
	Ty sinX2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rXZX2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = sinX * sinZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ * cosX2;
	*(m3x3_rotation_ptr + _index_data::i11) = -sinX * sinX2 + cosX * cosZ * cosX2;
	*(m3x3_rotation_ptr + _index_data::i21) = cosX * sinX2 + sinX * cosZ * cosX2;

	*(m3x3_rotation_ptr + _index_data::i02) = sinZ * sinX2;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosX2 - cosX * cosZ * sinX2;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosX2 - sinX * cosZ * sinX2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rXZX2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZX2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rXZX2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationXZX2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rXZX2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i00) && *(m3x3_rotation_ptr + _index_data::i00) < bound)
	{
		Ty rZabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i00)); // rZ
		Ty rZsgn = (*(m3x3_rotation_ptr + _index_data::i01) < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::minus_one<Ty, _func_impl_number>();
		Ty rX2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX2
			rZsgn * *(m3x3_rotation_ptr + _index_data::i02),
			-rZsgn * *(m3x3_rotation_ptr + _index_data::i01));
		*v3_rXZX2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rX
			rZsgn * *(m3x3_rotation_ptr + _index_data::i20),
			rZsgn * *(m3x3_rotation_ptr + _index_data::i10));
		*(v3_rXZX2_ptr + 1) = rZsgn * rZabs;
		*(v3_rXZX2_ptr + 2) = rX2;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rX20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i21),
			*(m3x3_rotation_ptr + _index_data::i22));
		bool Zflipped = *(m3x3_rotation_ptr + _index_data::i00) < static_cast<Ty>(0);
		*v3_rXZX2_ptr = rX_choice_if_singular; // rX
		*(v3_rXZX2_ptr + 1) = Zflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rZ2
		*(v3_rXZX2_ptr + 2) = Zflipped ? rX20 + rX_choice_if_singular : rX20 - rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYZY2_ptr);
	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 1));
	Ty cosY2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYZY2_ptr);
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 1));
	Ty sinY2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYZY2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = -sinY * sinY2 + cosY * cosZ * cosY2;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ * cosY2;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosY * sinY2 - sinY * cosZ * cosY2;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosY2 + cosY * cosZ * sinY2;
	*(m3x3_rotation_ptr + _index_data::i12) = sinZ * sinY2;
	*(m3x3_rotation_ptr + _index_data::i22) = cosY * cosY2 - sinY * cosZ * sinY2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYZY2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZY2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rYZY2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYZY2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYZY2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i11) && *(m3x3_rotation_ptr + _index_data::i11) < bound)
	{
		Ty rZabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i11)); // rZ
		Ty rZsgn = (*(m3x3_rotation_ptr + _index_data::i10) < static_cast<Ty>(0)) ? 
			cool::rotation_subroutine::minus_one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::one<Ty, _func_impl_number>();
		Ty rY2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY2
			rZsgn * *(m3x3_rotation_ptr + _index_data::i12),
			rZsgn * *(m3x3_rotation_ptr + _index_data::i10));
		*v3_rYZY2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			rZsgn * *(m3x3_rotation_ptr + _index_data::i21),
			-rZsgn * *(m3x3_rotation_ptr + _index_data::i01));
		*(v3_rYZY2_ptr + 1) = rZsgn * rZabs;
		*(v3_rYZY2_ptr + 2) = rY2;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rY20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i20),
			*(m3x3_rotation_ptr + _index_data::i22));
		bool Zflipped = *(m3x3_rotation_ptr + _index_data::i11) < static_cast<Ty>(0);
		*v3_rYZY2_ptr = rY_choice_if_singular; // rY
		*(v3_rYZY2_ptr + 1) = Zflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rZ
		*(v3_rYZY2_ptr + 2) = Zflipped ? rY20 + rY_choice_if_singular : rY20 - rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rYXY2_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 1));
	Ty cosY2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 2));
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rYXY2_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 1));
	Ty sinY2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rYXY2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosY2 - sinY * cosX * sinY2;
	*(m3x3_rotation_ptr + _index_data::i10) = sinX * sinY2;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosY2 - cosY * cosX * sinY2;

	*(m3x3_rotation_ptr + _index_data::i01) = sinY * sinX;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = cosY * sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = cosY * sinY2 + sinY * cosX * cosY2;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosY2;
	*(m3x3_rotation_ptr + _index_data::i22) = -sinY * sinY2 + cosY * cosX * cosY2;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rYXY2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYXY2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rYXY2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationYXY2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rYXY2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i11) && *(m3x3_rotation_ptr + _index_data::i11) < bound)
	{
		Ty rXabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i11)); // rX
		Ty rXsgn = (*(m3x3_rotation_ptr + _index_data::i12) < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::minus_one<Ty, _func_impl_number>();
		Ty rY2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY2
			rXsgn * *(m3x3_rotation_ptr + _index_data::i10),
			-rXsgn * *(m3x3_rotation_ptr + _index_data::i12));
		*v3_rYXY2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rY
			rXsgn * *(m3x3_rotation_ptr + _index_data::i01),
			rXsgn * *(m3x3_rotation_ptr + _index_data::i21));
		*(v3_rYXY2_ptr + 1) = rXsgn * rXabs;
		*(v3_rYXY2_ptr + 2) = rY2;

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_rYXY2_ptr = rY_choice_if_singular; // rY
		Ty rY20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i02),
			*(m3x3_rotation_ptr + _index_data::i00));
		bool Xflipped = *(m3x3_rotation_ptr + _index_data::i11) < static_cast<Ty>(0);
		*(v3_rYXY2_ptr + 1) = Xflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rX
		*(v3_rYXY2_ptr + 2) = Xflipped ? rY20 + rY_choice_if_singular : rY20 - rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZXZ2_ptr);
	Ty cosX = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 1));
	Ty cosZ2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZXZ2_ptr);
	Ty sinX = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 1));
	Ty sinZ2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZXZ2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ * cosZ2 - sinZ * cosX * sinZ2;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ * cosZ2 + cosZ * cosX * sinZ2;
	*(m3x3_rotation_ptr + _index_data::i20) = sinX * sinZ2;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosZ * sinZ2 - sinZ * cosX * cosZ2;
	*(m3x3_rotation_ptr + _index_data::i11) = -sinZ * sinZ2 + cosZ * cosX * cosZ2;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosZ2;

	*(m3x3_rotation_ptr + _index_data::i02) = sinZ * sinX;
	*(m3x3_rotation_ptr + _index_data::i12) = -cosZ * sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZXZ2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZXZ2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rZXZ2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZXZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZXZ2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i22) && *(m3x3_rotation_ptr + _index_data::i22) < bound)
	{
		Ty rXabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i22)); // rX
		Ty rXsgn = (*(m3x3_rotation_ptr + _index_data::i21) < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::minus_one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::one<Ty, _func_impl_number>();
		Ty rZ2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ2
			rXsgn * *(m3x3_rotation_ptr + _index_data::i20),
			rXsgn * *(m3x3_rotation_ptr + _index_data::i21));
		*v3_rZXZ2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			rXsgn * *(m3x3_rotation_ptr + _index_data::i02),
			-rXsgn * *(m3x3_rotation_ptr + _index_data::i12));
		*(v3_rZXZ2_ptr + 1) = rXsgn * rXabs;
		*(v3_rZXZ2_ptr + 2) = rZ2;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rZ20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			-*(m3x3_rotation_ptr + _index_data::i01),
			*(m3x3_rotation_ptr + _index_data::i00));
		bool Xflipped = *(m3x3_rotation_ptr + _index_data::i22) < static_cast<Ty>(0);
		*v3_rZXZ2_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZXZ2_ptr + 1) = Xflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rX
		*(v3_rZXZ2_ptr + 2) = Xflipped ? rZ20 + rZ_choice_if_singular : rZ20 - rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}


template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	Ty cosZ = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*v3_rZYZ2_ptr);
	Ty cosY = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 1));
	Ty cosZ2 = cool::rotation_subroutine::cos<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 2));
	Ty sinZ = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*v3_rZYZ2_ptr);
	Ty sinY = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 1));
	Ty sinZ2 = cool::rotation_subroutine::sin<Ty, _func_impl_number>(*(v3_rZYZ2_ptr + 2));

	*(m3x3_rotation_ptr + _index_data::i00) = -sinZ * sinZ2 + cosZ * cosY * cosZ2;
	*(m3x3_rotation_ptr + _index_data::i10) = cosZ * sinZ2 + sinZ * cosY * cosZ2;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosZ2;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ * cosZ2 - cosZ * cosY * sinZ2;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ * cosZ2 - sinZ * cosY * sinZ2;
	*(m3x3_rotation_ptr + _index_data::i21) = sinY * sinZ2;

	*(m3x3_rotation_ptr + _index_data::i02) = cosZ * sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = sinZ * sinY;
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline void cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_matrix_inv(
	Ty* m3x3_rotation_ptr, const Ty* v3_rZYZ2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZYZ2<Ty, _dim_padded, _other_layout, _func_impl_number>::get_matrix(m3x3_rotation_ptr, v3_rZYZ2_ptr);
}

template <class Ty, std::size_t _dim_padded, cool::matrix_layout _layout, int _func_impl_number>
inline cool::rotation_status cool::rotationZYZ2<Ty, _dim_padded, _layout, _func_impl_number>::get_angles(
	Ty* v3_rZYZ2_ptr, const Ty* m3x3_rotation_ptr, Ty angle_tol, Ty rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, _layout, 3>;

	constexpr Ty coeff_temp = cool::rotation_subroutine::pi<Ty, _func_impl_number>()
		/ cool::rotation_subroutine::half_turn<Ty, _func_impl_number>();
	constexpr Ty angle_tol_coeff = cool::rotation_subroutine::half<Ty, _func_impl_number>() * (coeff_temp * coeff_temp);

	Ty bound = cool::rotation_subroutine::one<Ty, _func_impl_number>() - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i22) && *(m3x3_rotation_ptr + _index_data::i22) < bound)
	{
		Ty rYabs = cool::rotation_subroutine::acos<Ty, _func_impl_number>(*(m3x3_rotation_ptr + _index_data::i22)); // rY
		Ty rYsgn = (*(m3x3_rotation_ptr + _index_data::i20) < static_cast<Ty>(0)) ?
			cool::rotation_subroutine::one<Ty, _func_impl_number>()
			: cool::rotation_subroutine::minus_one<Ty, _func_impl_number>();
		Ty rZ2 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ2
			rYsgn * *(m3x3_rotation_ptr + _index_data::i21),
			-rYsgn * *(m3x3_rotation_ptr + _index_data::i20));
		*v3_rZYZ2_ptr = cool::rotation_subroutine::atan2<Ty, _func_impl_number>( // rZ
			rYsgn * *(m3x3_rotation_ptr + _index_data::i12),
			rYsgn * *(m3x3_rotation_ptr + _index_data::i02));
		*(v3_rZYZ2_ptr + 1) = rYsgn * rYabs;
		*(v3_rZYZ2_ptr + 2) = rZ2;

		return cool::rotation_status::regular;
	}
	else
	{
		Ty rZ20 = cool::rotation_subroutine::atan2<Ty, _func_impl_number>(
			*(m3x3_rotation_ptr + _index_data::i10),
			*(m3x3_rotation_ptr + _index_data::i11));
		bool Yflipped = *(m3x3_rotation_ptr + _index_data::i22) < static_cast<Ty>(0);
		*v3_rZYZ2_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZYZ2_ptr + 1) = Yflipped ? cool::rotation_subroutine::half_turn<Ty, _func_impl_number>()
			: static_cast<Ty>(0); // rY
		*(v3_rZYZ2_ptr + 2) = Yflipped ? rZ20 + rZ_choice_if_singular : rZ20 - rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}


template <class rotation3d_Ty>
inline cool::rotation_status cool::_rotation3d_functions<rotation3d_Ty>::rotation_functions(
	pointer dest_ptr, const_pointer orig_ptr, value_type angle_tol, value_type angle_choice_if_singular, int param) noexcept
{
	if (param == 0)
	{
		rotation3d_Ty::get_matrix(dest_ptr, orig_ptr);
		return cool::rotation_status::regular;
	}
	else if (param == 1)
	{
		rotation3d_Ty::get_matrix_inv(dest_ptr, orig_ptr);
		return cool::rotation_status::regular;
	}
	else
	{
		return rotation3d_Ty::get_angles(dest_ptr, orig_ptr, angle_tol, angle_choice_if_singular);
	}
}

#endif // xCOOL_ROTATIONS_HPP


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