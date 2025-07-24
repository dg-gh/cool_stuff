// cool_rotations.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_ROTATIONS_HPP
#define xCOOL_ROTATIONS_HPP

#include <cstddef>
#include <cstdint>
#include <cmath>


// custom rotation traits class prototype

//class traits_example
//{
//public:
//
//	using value_type = TYPE;
//
//	static constexpr TYPE pi = PI_VALUE;
//	static constexpr TYPE half_turn = HALF_TURN_VALUE;
//
//	static TYPE cos(TYPE x) noexcept;
//	static TYPE sin(TYPE x) noexcept;
//	static TYPE acos(TYPE x) noexcept;
//	static TYPE asin(TYPE x) noexcept;
//	static TYPE atan2(TYPE y, TYPE x) noexcept;
//	static TYPE sqrt(TYPE x) noexcept;
//	static TYPE inv_sqrt(TYPE x) noexcept;
//};

// TYPE must allow constexpr + - * / operations
// TYPE must allow < comparison

// TYPE must allow static_cast<TYPE>(0), static_cast<TYPE>(1), static_cast<TYPE>(-1),
// static_cast<TYPE>(2), static_cast<TYPE>(3)
// static_cast<TYPE>(0.5), static_cast<TYPE>(-0.5),
// static_cast<TYPE>(0.25), static_cast<TYPE>(-0.25) constexpr expressions


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
	template <class Ty> class radians
	{

	public:

		using value_type = Ty;

		static constexpr Ty pi = static_cast<Ty>(3.141592653589793);
		static constexpr Ty half_turn = static_cast<Ty>(3.141592653589793);

		static inline Ty cos(Ty x) noexcept;
		static inline Ty sin(Ty x) noexcept;
		static inline Ty acos(Ty x) noexcept;
		static inline Ty asin(Ty x) noexcept;
		static inline Ty atan2(Ty y, Ty x) noexcept;
		static inline Ty sqrt(Ty x) noexcept;
		static inline Ty inv_sqrt(Ty x) noexcept;
	};

	template <class Ty> class degrees
	{

	public:

		using value_type = Ty;

		static constexpr Ty pi = static_cast<Ty>(3.141592653589793);
		static constexpr Ty half_turn = static_cast<Ty>(180);

		static inline Ty cos(Ty x) noexcept;
		static inline Ty sin(Ty x) noexcept;
		static inline Ty acos(Ty x) noexcept;
		static inline Ty asin(Ty x) noexcept;
		static inline Ty atan2(Ty y, Ty x) noexcept;
		static inline Ty sqrt(Ty x) noexcept;
		static inline Ty inv_sqrt(Ty x) noexcept;
	};
	
	template <class Ty> class turns
	{

	public:

		using value_type = Ty;

		static constexpr Ty pi = static_cast<Ty>(3.141592653589793);
		static constexpr Ty half_turn = static_cast<Ty>(0.5);

		static inline Ty cos(Ty x) noexcept;
		static inline Ty sin(Ty x) noexcept;
		static inline Ty acos(Ty x) noexcept;
		static inline Ty asin(Ty x) noexcept;
		static inline Ty atan2(Ty y, Ty x) noexcept;
		static inline Ty sqrt(Ty x) noexcept;
		static inline Ty inv_sqrt(Ty x) noexcept;
	};


	enum class rotation_status
	{
		regular = 0,
		singular = 1,
	};

#ifndef xCOOL_AXIS_CONSTANTS
#define xCOOL_AXIS_CONSTANTS
	enum class axis : std::size_t
	{
		X = 0,
		Y = 1,
		Z = 2,
		none = 3
	};

	namespace axis_index
	{
		static constexpr std::size_t none = 3;
	}
#endif // xCOOL_AXIS_CONSTANTS


	// WARNING : pointer arguments of rotation functions must not be nullptr

	// 2D rotations

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotation2d;
	template <class Ty> class polymorphic_rotation2d;

	template <class traits_Ty> class direction2d;
	template <class Ty> class polymorphic_direction2d;


	// 3D rotations

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotation3d;
	template <class Ty> class polymorphic_rotation3d;

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotation_axis_angle;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotation_quaternion;

	template <class traits_Ty> class direction3d;
	template <class Ty> class polymorphic_direction3d;


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

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationX;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationY;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZ;

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXY;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXZ;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYZ;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYX;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZX;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZY;

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXYZ;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXZY;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYZX;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYXZ;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZXY;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZYX;

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXYX2;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationXZX2;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYZY2;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationYXY2;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZXZ2;
	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout = COOL_ROTATIONS_DEFAULT_MATRIX_LAYOUT> class rotationZYZ2;


	// class member details

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotation2d
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static inline void get_matrix(value_type* m2x2_rotation_ptr, value_type angle) noexcept;
		static inline void get_matrix_inv(value_type* m2x2_rotation_ptr, value_type angle) noexcept;
		static inline void get_angle(value_type* angle_ptr, const value_type* m2x2_rotation_ptr) noexcept;
	};


	template <class Ty> class polymorphic_rotation2d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		polymorphic_rotation2d() = delete;
		template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
		polymorphic_rotation2d(cool::rotation2d<traits_Ty, _dim_padded, _layout>) noexcept;
		template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
		cool::polymorphic_rotation2d<Ty>& operator=(cool::rotation2d<traits_Ty, _dim_padded, _layout>) noexcept;

		inline void get_matrix(Ty* m2x2_rotation_ptr, Ty angle) const noexcept;
		inline void get_matrix_inv(Ty* m2x2_rotation_ptr, Ty angle) const noexcept;
		inline void get_angle(Ty* angle_ptr, const Ty* m2x2_rotation_ptr) const noexcept;

		inline Ty half_turn() const noexcept;

	private:

		void(*m_rotation_functions)(Ty*, const Ty*, int param);
	};


	template <class traits_Ty> class direction2d
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		static constexpr value_type half_turn = traits_type::half_turn;

		// v2_front_dir_ptr and v2_lateral_dir_ptr must be orthonormal

		static inline void get_direction(
			value_type* v2_direction_ptr,
			value_type angle,
			const value_type* v2_front_dir_ptr,
			const value_type* v2_lateral_dir_ptr) noexcept;

		static inline void get_angle(
			value_type* angle_ptr,
			const value_type* v2_direction_ptr,
			const value_type* v2_front_dir_ptr,
			const value_type* v2_lateral_dir_ptr) noexcept;
	};


	template <class Ty> class polymorphic_direction2d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		polymorphic_direction2d() = delete;
		template <class traits_Ty> polymorphic_direction2d(cool::direction2d<traits_Ty>) noexcept;
		template <class traits_Ty> cool::polymorphic_direction2d<Ty>& operator=(cool::direction2d<traits_Ty>) noexcept;

		// v2_front_dir_ptr and v2_lateral_dir_ptr must be orthonormal

		inline void get_direction(
			Ty* v2_direction_ptr,
			Ty angle,
			const Ty* v2_front_dir_ptr,
			const Ty* v2_lateral_dir_ptr) const noexcept;

		inline void get_angle(
			Ty* angle_ptr,
			const Ty* v2_direction_ptr,
			const Ty* v2_front_dir_ptr,
			const Ty* v2_lateral_dir_ptr) const noexcept;

		inline Ty half_turn() const noexcept;

	private:

		void(*m_direction_functions)(Ty*, const Ty*, const Ty*, const Ty*, int param);
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotation3d
		: public cool::polymorphic_rotation3d<typename traits_Ty::value_type>
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;

		inline rotation3d() noexcept;
		inline rotation3d(cool::rotation_type new_rotation_type) noexcept;
		rotation3d(const cool::rotation3d<traits_Ty, _dim_padded, _layout>&) noexcept = default;
		cool::rotation3d<traits_Ty, _dim_padded, _layout>& operator=(const cool::rotation3d<traits_Ty, _dim_padded, _layout>&) noexcept = default;
		rotation3d(cool::rotation3d<traits_Ty, _dim_padded, _layout>&&) noexcept = default;
		cool::rotation3d<traits_Ty, _dim_padded, _layout>& operator=(cool::rotation3d<traits_Ty, _dim_padded, _layout>&&) noexcept = default;
		~rotation3d() = default;

		inline cool::rotation3d<traits_Ty, _dim_padded, _layout>& operator=(cool::rotation_type new_rotation_type) noexcept;
		inline cool::rotation3d<traits_Ty, _dim_padded, _layout>& set_rotation_type(cool::rotation_type new_rotation_type) noexcept;

		inline value_type singular_angle(std::size_t pole) const noexcept;

		inline value_type required_epsilon(value_type angle_tol) const noexcept;

	private:

		static inline cool::rotation_status m_id(value_type* dest_ptr, const value_type*, value_type, value_type, int param) noexcept;
	};


	template <class Ty> class polymorphic_rotation3d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		inline operator cool::rotation_type() const noexcept;
		inline cool::rotation_type rotation_type() const noexcept;

		inline std::size_t axis_count() const noexcept;
		inline cool::axis axis(std::size_t _index) const noexcept;

		inline cool::axis singular_axis() const noexcept;

		inline std::size_t index(cool::axis _axis) const noexcept;
		inline std::size_t iX() const noexcept;
		inline std::size_t iY() const noexcept;
		inline std::size_t iZ() const noexcept;
		inline std::size_t iSg() const noexcept;

		inline void get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept;
		inline void get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept;
		inline cool::rotation_status get_coord(Ty* v4_coord_ptr, const Ty* m3x3_rotation_ptr,
			Ty angle_tol = static_cast<Ty>(0), Ty angle_choice_if_singular = static_cast<Ty>(0)) const noexcept;

		inline Ty half_turn() const noexcept;

	protected:

		polymorphic_rotation3d() noexcept {}

		cool::rotation_type m_rotation_type;
		cool::rotation_status(*m_rotation_functions)(Ty*, const Ty*, Ty, Ty, int param);
	};


#ifndef xCOOL_NO_AXIS_NORM_ENUM
#define xCOOL_NO_AXIS_NORM_ENUM
	enum no_axis_norm_t { no_axis_norm };
#endif // xCOOL_NO_AXIS_NORM_ENUM

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotation_axis_angle
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::AA;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, value_type norm_tol = static_cast<value_type>(0)) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, value_type norm_tol = static_cast<value_type>(0)) noexcept;
		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, cool::no_axis_norm_t) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(value_type* v3_axis_ptr, value_type* angle_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol = static_cast<value_type>(0)) noexcept;
		static inline cool::rotation_status get_axis_angle(value_type* v3_axis_ptr, value_type* angle_ptr, const value_type* m3x3_rotation_ptr, const value_type* v3_axis_way_ptr, value_type angle_tol = static_cast<value_type>(0)) noexcept;
		static inline void get_angle(value_type* angle_ptr, const value_type* m3x3_rotation_ptr) noexcept;
	};


#ifndef xCOOL_NO_QUATERNION_NORM_ENUM
#define xCOOL_NO_QUATERNION_NORM_ENUM
	enum no_quaternion_norm_t { no_quaternion_norm };
#endif // xCOOL_NO_QUATERNION_NORM_ENUM

	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotation_quaternion
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::Q;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr) noexcept;
		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept;
		static inline void get_quaternion_from_matrix(value_type* v4_quaternion_ptr, const value_type* m3x3_rotation_ptr) noexcept;
		static inline void get_quaternion_from_axis_angle(value_type* v4_quaternion_ptr, const value_type* v3_axis_ptr, value_type angle,
			value_type norm_tol = static_cast<value_type>(0)) noexcept;
		static inline void get_quaternion_from_axis_angle(value_type* v4_quaternion_ptr, const value_type* v3_axis_ptr, value_type angle,
			cool::no_axis_norm_t) noexcept;
		static inline cool::rotation_status get_axis_angle(value_type* v3_axis_ptr, value_type* angle, value_type* v4_quaternion_ptr, value_type angle_tol) noexcept;
		static inline cool::rotation_status get_axis_angle(value_type* v3_axis_ptr, value_type* angle, value_type* v4_quaternion_ptr,
			const value_type* v3_axis_way_ptr, value_type angle_tol) noexcept;
	};


#ifndef xCOOL_NO_DIRECTION_NORM_ENUM
#define xCOOL_NO_DIRECTION_NORM_ENUM
	enum no_direction_norm_t { no_direction_norm };
#endif // xCOOL_NO_DIRECTION_NORM_ENUM

	template <class traits_Ty> class direction3d
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr value_type singular_angle[2] = {
			static_cast<value_type>(-0.5) * traits_Ty::half_turn,
			static_cast<value_type>(0.5) * traits_Ty::half_turn
		};

		static constexpr std::size_t iSg = 1;

		// v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr must be orthonormal

		static inline void get_direction(
			value_type* v3_direction_ptr,
			const value_type* v2_azimuth_altitude_angles_ptr,
			const value_type* v3_front_dir_ptr,
			const value_type* v3_lateral_dir_ptr,
			const value_type* v3_up_dir_ptr) noexcept;

		static inline cool::rotation_status get_angles(
			value_type* v2_azimuth_altitude_angles_ptr,
			const value_type* v3_direction_ptr,
			const value_type* v3_front_dir_ptr,
			const value_type* v3_lateral_dir_ptr,
			const value_type* v3_up_dir_ptr,
			value_type altitude_angle_tol = static_cast<value_type>(0),
			value_type azimuth_angle_if_singular = static_cast<value_type>(0)) noexcept;

		static inline cool::rotation_status get_angles(
			value_type* v2_azimuth_altitude_angles_ptr,
			const value_type* v3_direction_ptr,
			const value_type* v3_front_dir_ptr,
			const value_type* v3_lateral_dir_ptr,
			const value_type* v3_up_dir_ptr,
			cool::no_direction_norm_t,
			value_type altitude_angle_tol = static_cast<value_type>(0),
			value_type azimuth_angle_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr value_type required_epsilon(value_type altitude_angle_tol) noexcept;
	};


	template <class Ty> class polymorphic_direction3d
	{

	public:

		using value_type = Ty;
		using pointer = Ty*;
		using const_pointer = const Ty*;

		polymorphic_direction3d() = delete;
		template <class traits_Ty> polymorphic_direction3d(cool::direction3d<traits_Ty>) noexcept;
		template <class traits_Ty> cool::polymorphic_direction3d<Ty>& operator=(cool::direction3d<traits_Ty>) noexcept;

		static constexpr std::size_t iSg() noexcept;

		// v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr must be orthonormal

		inline void get_direction(
			Ty* v3_direction_ptr,
			const Ty* angle,
			const Ty* v3_front_dir_ptr,
			const Ty* v3_lateral_dir_ptr,
			const Ty* v3_up_dir_ptr) const noexcept;

		inline cool::rotation_status get_angles(
			Ty* angle_ptr,
			const Ty* v3_direction_ptr,
			const Ty* v3_front_dir_ptr,
			const Ty* v3_lateral_dir_ptr,
			const Ty* v3_up_dir_ptr,
			Ty altitude_angle_tol = static_cast<Ty>(0),
			Ty azimuth_angle_if_singular = static_cast<Ty>(0)) const noexcept;

		inline Ty half_turn() const noexcept;

	private:

		cool::rotation_status(*m_direction_functions)(Ty*, const Ty*, const Ty*, const Ty*, const Ty*, Ty, Ty, int);
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationX
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::X;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 1;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::none, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* angle_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationY
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::Y;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 1;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::none, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* angle_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZ
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::Z;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 1;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::none, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* angle_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXY
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 3;
		static constexpr std::size_t iSg = 3;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rXY_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rXY_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXZ
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rXZ_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rXZ_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYZ
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rYZ_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rYZ_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYX
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = cool::axis_index::none;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rYX_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rYX_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZX
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = cool::axis_index::none;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rZX_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rZX_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZY
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_type::half_turn;

		static constexpr std::size_t axis_count = 2;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::none };

		static constexpr cool::axis singular_axis = cool::axis::none;
		static constexpr value_type singular_angle[2] = {};

		static constexpr std::size_t iX = cool::axis_index::none;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = cool::axis_index::none;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v2_rZY_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v2_rZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v2_rZY_ptr, const value_type* m3x3_rotation_ptr,
			value_type = static_cast<value_type>(0), value_type = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXYZ
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XYZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::Z };

		static constexpr cool::axis singular_axis = cool::axis::Y;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rXYZ_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rXYZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rXYZ_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rX_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXZY
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::Y };

		static constexpr cool::axis singular_axis = cool::axis::Z;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rXZY_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rXZY_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rXZY_ptr, const value_type* m3x3_rotation,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rX_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYZX
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::X };

		static constexpr cool::axis singular_axis = cool::axis::Z;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rYZX_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rYZX_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rYZX_ptr, const value_type* m3x3_rotation,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rY_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYXZ
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YXZ;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Z };

		static constexpr cool::axis singular_axis = cool::axis::X;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rYXZ_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rYXZ_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rYXZ_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rY_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZXY
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZXY;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Y };

		static constexpr cool::axis singular_axis = cool::axis::X;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rZXY_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rZXY_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rZXY_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rZ_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZYX
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZYX;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::X };

		static constexpr cool::axis singular_axis = cool::axis::Y;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(-0.5) * traits_Ty::half_turn, static_cast<value_type>(0.5) * traits_Ty::half_turn };

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rZYX_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rZYX_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rZYX_ptr, const value_type* m3x3_rotation,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rZ_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXYX2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XYX2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Y, cool::axis::X };

		static constexpr cool::axis singular_axis = cool::axis::Y;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rXYX2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rXYX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rXYX2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rX_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationXZX2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::XZX2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::X, cool::axis::Z, cool::axis::X };

		static constexpr cool::axis singular_axis = cool::axis::Z;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 0;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rXZX2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rXZX2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rXZX2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rX_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYZY2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YZY2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::Z, cool::axis::Y };

		static constexpr cool::axis singular_axis = cool::axis::Z;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 1;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rYZY2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rYZY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rYZY2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rY_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationYXY2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::YXY2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Y, cool::axis::X, cool::axis::Y };

		static constexpr cool::axis singular_axis = cool::axis::X;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 0;
		static constexpr std::size_t iZ = 2;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rYXY2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rYXY2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rYXY2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rY_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZXZ2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZXZ2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::X, cool::axis::Z };

		static constexpr cool::axis singular_axis = cool::axis::X;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 1;
		static constexpr std::size_t iY = 2;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rZXZ2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rZXZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rZXZ2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rZ_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
	};


	template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout> class rotationZYZ2
	{

	public:

		using traits_type = traits_Ty;
		using value_type = typename traits_Ty::value_type;
		using pointer = typename traits_Ty::value_type*;
		using const_pointer = const typename traits_Ty::value_type*;

		static constexpr cool::rotation_type rotation_type = cool::rotation_type::ZYZ2;

		static constexpr std::size_t dim_padded = _dim_padded;
		static constexpr cool::matrix_layout layout = _layout;
		static constexpr value_type half_turn = traits_Ty::half_turn;

		static constexpr std::size_t axis_count = 3;
		static constexpr cool::axis axis[3] = { cool::axis::Z, cool::axis::Y, cool::axis::Z };

		static constexpr cool::axis singular_axis = cool::axis::Y;
		static constexpr value_type singular_angle[2] = { static_cast<value_type>(0), traits_Ty::half_turn };

		static constexpr std::size_t iX = 2;
		static constexpr std::size_t iY = 1;
		static constexpr std::size_t iZ = 0;
		static constexpr std::size_t iSg = 1;

		static inline void get_matrix(value_type* m3x3_rotation_ptr, const value_type* v3_rZYZ2_ptr) noexcept;
		static inline void get_matrix_inv(value_type* m3x3_rotation_ptr, const value_type* v3_rZYZ2_ptr) noexcept;
		static inline cool::rotation_status get_angles(value_type* v3_rZYZ2_ptr, const value_type* m3x3_rotation_ptr,
			value_type angle_tol = static_cast<value_type>(0),
			value_type rZ_choice_if_singular = static_cast<value_type>(0)) noexcept;

		static constexpr inline value_type required_epsilon(value_type angle_tol) noexcept;
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

template <class Ty>
inline Ty cool::radians<Ty>::cos(Ty x) noexcept
{
	return std::cos(x);
}

template <class Ty>
inline Ty cool::radians<Ty>::sin(Ty x) noexcept
{
	return std::sin(x);
}

template <class Ty>
inline Ty cool::radians<Ty>::acos(Ty x) noexcept
{
	return std::acos(x);
}

template <class Ty>
inline Ty cool::radians<Ty>::asin(Ty x) noexcept
{
	return std::asin(x);
}

template <class Ty>
inline Ty cool::radians<Ty>::atan2(Ty y, Ty x) noexcept
{
	return std::atan2(x, y);
}

template <class Ty>
inline Ty cool::radians<Ty>::sqrt(Ty x) noexcept
{
	return std::sqrt(x);
}

template <class Ty>
inline Ty cool::radians<Ty>::inv_sqrt(Ty x) noexcept
{
	return static_cast<Ty>(1) / std::sqrt(x);
}


template <class Ty>
inline Ty cool::degrees<Ty>::cos(Ty x) noexcept
{
	constexpr float deg_to_rad = pi / static_cast<Ty>(180);
	return std::cos(deg_to_rad * x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::sin(Ty x) noexcept
{
	constexpr float deg_to_rad = pi / static_cast<Ty>(180);
	return std::sin(deg_to_rad * x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::acos(Ty x) noexcept
{
	constexpr float rad_to_deg = static_cast<Ty>(180) / pi;
	return rad_to_deg * std::acos(x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::asin(Ty x) noexcept
{
	constexpr float rad_to_deg = static_cast<Ty>(180) / pi;
	return rad_to_deg * std::asin(x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::atan2(Ty y, Ty x) noexcept
{
	constexpr float rad_to_deg = static_cast<Ty>(180) / pi;
	return rad_to_deg * std::atan2(y, x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::sqrt(Ty x) noexcept
{
	return std::sqrt(x);
}

template <class Ty>
inline Ty cool::degrees<Ty>::inv_sqrt(Ty x) noexcept
{
	return static_cast<Ty>(1) / std::sqrt(x);
}


template <class Ty>
inline Ty cool::turns<Ty>::cos(Ty x) noexcept
{
	constexpr float turn_to_rad = pi / static_cast<Ty>(0.5);
	return std::cos(turn_to_rad * x);
}

template <class Ty>
inline Ty cool::turns<Ty>::sin(Ty x) noexcept
{
	constexpr float turn_to_rad = pi / static_cast<Ty>(0.5);
	return std::sin(turn_to_rad * x);
}

template <class Ty>
inline Ty cool::turns<Ty>::acos(Ty x) noexcept
{
	constexpr float rad_to_turn = static_cast<Ty>(0.5) / pi;
	return rad_to_turn * std::acos(x);
}

template <class Ty>
inline Ty cool::turns<Ty>::asin(Ty x) noexcept
{
	constexpr float rad_to_turn = static_cast<Ty>(0.5) / pi;
	return rad_to_turn * std::asin(x);
}

template <class Ty>
inline Ty cool::turns<Ty>::atan2(Ty y, Ty x) noexcept
{
	constexpr float rad_to_turn = static_cast<Ty>(0.5) / pi;
	return rad_to_turn * std::atan2(y, x);
}

template <class Ty>
inline Ty cool::turns<Ty>::sqrt(Ty x) noexcept
{
	return std::sqrt(x);
}

template <class Ty>
inline Ty cool::turns<Ty>::inv_sqrt(Ty x) noexcept
{
	return static_cast<Ty>(1) / std::sqrt(x);
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


namespace cool
{
	template <std::size_t _dim_padded, std::size_t _dim_padded_min, cool::matrix_layout _layout> class _rotation_matrix_index_data
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


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m2x2_rotation_ptr, value_type angle) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 2, _layout>;

	value_type cosA = traits_Ty::cos(angle);
	value_type sinA = traits_Ty::sin(angle);

	*(m2x2_rotation_ptr + _index_data::i00) = cosA;
	*(m2x2_rotation_ptr + _index_data::i10) = sinA;
	*(m2x2_rotation_ptr + _index_data::i01) = -sinA;
	*(m2x2_rotation_ptr + _index_data::i11) = cosA;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m2x2_rotation_ptr, value_type angle) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation2d<traits_Ty, _dim_padded, _other_layout>::get_matrix(m2x2_rotation_ptr, angle);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation2d<traits_Ty, _dim_padded, _layout >::get_angle(
	value_type* angle_ptr, const value_type* m2x2_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 2, _layout>;

	*angle_ptr = traits_Ty::atan2(*(m2x2_rotation_ptr + _index_data::i10), *(m2x2_rotation_ptr + _index_data::i00));
}


template <class Ty> template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
cool::polymorphic_rotation2d<Ty>::polymorphic_rotation2d(cool::rotation2d<traits_Ty, _dim_padded, _layout>) noexcept
{
	m_rotation_functions = [](Ty* dest_ptr, const Ty* orig_ptr, int param)
	{
		if (param == 0)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix(dest_ptr, *orig_ptr);
		}
		else if (param == 1)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix_inv(dest_ptr, *orig_ptr);
		}
		else if (param == 2)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_angle(dest_ptr, orig_ptr);
		}
		else // param == 3)
		{
			*dest_ptr = traits_Ty::half_turn();
		}
	};
}

template <class Ty> template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
cool::polymorphic_rotation2d<Ty>& cool::polymorphic_rotation2d<Ty>::operator=(cool::rotation2d<traits_Ty, _dim_padded, _layout>) noexcept
{
	m_rotation_functions = [](Ty* dest_ptr, const Ty* orig_ptr, int param)
	{
		if (param == 0)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix(dest_ptr, *orig_ptr);
		}
		else if (param == 1)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_matrix_inv(dest_ptr, *orig_ptr);
		}
		else if (param == 2)
		{
			cool::rotation2d<traits_Ty, _dim_padded, _layout>::get_angle(dest_ptr, orig_ptr);
		}
		else // param == 3 
		{
			*dest_ptr = traits_Ty::half_turn();
		}
	};

	return *this;
}

template <class Ty>
inline void cool::polymorphic_rotation2d<Ty>::get_matrix(Ty* m2x2_rotation_ptr, Ty angle) const noexcept
{
	m_rotation_functions(m2x2_rotation_ptr, &angle, 0);
}

template <class Ty>
inline void cool::polymorphic_rotation2d<Ty>::get_matrix_inv(Ty* m2x2_rotation_ptr, Ty angle) const noexcept
{
	m_rotation_functions(m2x2_rotation_ptr, &angle, 1);
}

template <class Ty>
inline void cool::polymorphic_rotation2d<Ty>::get_angle(Ty* angle_ptr, const Ty* m2x2_rotation_ptr) const noexcept
{
	m_rotation_functions(angle_ptr, m2x2_rotation_ptr, 2);
}

template <class Ty>
inline Ty cool::polymorphic_rotation2d<Ty>::half_turn() const noexcept
{
	Ty ret;
	m_rotation_functions(&ret, nullptr, 3);
	return ret;
}


template <class traits_Ty>
inline void cool::direction2d<traits_Ty>::get_direction(
	value_type* v2_direction_ptr,
	value_type angle,
	const value_type* v2_front_dir_ptr,
	const value_type* v2_lateral_dir_ptr) noexcept
{
	value_type cosA = traits_Ty::cos(angle);
	value_type sinA = traits_Ty::sin(angle);

	value_type front_dir[2] = { *v2_front_dir_ptr, *(v2_front_dir_ptr + 1) };
	value_type lateral_dir[2] = { *v2_lateral_dir_ptr, *(v2_lateral_dir_ptr + 1) };

	*v2_direction_ptr = cosA * front_dir[0] + sinA * lateral_dir[0];
	*(v2_direction_ptr + 1) = cosA * front_dir[1] + sinA * lateral_dir[1];
}

template <class traits_Ty>
inline void cool::direction2d<traits_Ty>::get_angle(
	value_type* angle_ptr,
	const value_type* v2_direction_ptr,
	const value_type* v2_front_dir_ptr,
	const value_type* v2_lateral_dir_ptr) noexcept
{
	value_type cosA = *v2_direction_ptr * *v2_front_dir_ptr + *(v2_direction_ptr + 1) * *(v2_front_dir_ptr + 1);
	value_type sinA = *v2_direction_ptr * *v2_lateral_dir_ptr + *(v2_direction_ptr + 1) * *(v2_lateral_dir_ptr + 1);

	*angle_ptr = traits_Ty::atan2(sinA, cosA);
}


template <class Ty> template <class traits_Ty>
cool::polymorphic_direction2d<Ty>::polymorphic_direction2d(cool::direction2d<traits_Ty>) noexcept
{
	m_direction_functions = [](Ty* dest_ptr, const Ty* orig_ptr, const Ty* v2_front_dir_ptr, const Ty* v2_lateral_dir_ptr, int param)
	{
		if (param == 0)
		{
			cool::direction2d<traits_Ty>::get_direction(dest_ptr, *orig_ptr, v2_front_dir_ptr, v2_lateral_dir_ptr);
		}
		else if (param == 1)
		{
			cool::direction2d<traits_Ty>::get_angle(dest_ptr, orig_ptr, v2_front_dir_ptr, v2_lateral_dir_ptr);
		}
		else // param == 2
		{
			*dest_ptr = traits_Ty::half_turn;
		}
	};
}

template <class Ty> template <class traits_Ty>
cool::polymorphic_direction2d<Ty>& cool::polymorphic_direction2d<Ty>::operator=(cool::direction2d<traits_Ty>) noexcept
{
	m_direction_functions = [](Ty* dest_ptr, const Ty* orig_ptr, const Ty* v2_front_dir_ptr, const Ty* v2_lateral_dir_ptr, int param)
	{
		if (param == 0)
		{
			cool::direction2d<traits_Ty>::get_direction(dest_ptr, *orig_ptr, v2_front_dir_ptr, v2_lateral_dir_ptr);
		}
		else if (param == 1)
		{
			cool::direction2d<traits_Ty>::get_angle(dest_ptr, orig_ptr, v2_front_dir_ptr, v2_lateral_dir_ptr);
		}
		else // param == 2
		{
			*dest_ptr = traits_Ty::half_turn;
		}
	};

	return *this;
}

template <class Ty>
inline void cool::polymorphic_direction2d<Ty>::get_direction(
	Ty* v2_direction_ptr,
	Ty angle,
	const Ty* v2_front_dir_ptr,
	const Ty* v2_lateral_dir_ptr) const noexcept
{
	m_direction_functions(v2_direction_ptr, &angle, v2_front_dir_ptr, v2_lateral_dir_ptr, 0);
}

template <class Ty>
inline void cool::polymorphic_direction2d<Ty>::get_angle(
	Ty* angle_ptr,
	const Ty* v2_direction_ptr,
	const Ty* v2_front_dir_ptr,
	const Ty* v2_lateral_dir_ptr) const noexcept
{
	m_direction_functions(angle_ptr, v2_direction_ptr, v2_front_dir_ptr, v2_lateral_dir_ptr, 1);
}

template <class Ty>
inline Ty cool::polymorphic_direction2d<Ty>::half_turn() const noexcept
{
	Ty ret;
	m_direction_functions(&ret, nullptr, nullptr, nullptr, 2);
	return ret;
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation3d<traits_Ty, _dim_padded, _layout>::rotation3d() noexcept
{
	this->m_rotation_type = cool::rotation_type::id;
	this->m_rotation_functions = m_id;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation3d<traits_Ty, _dim_padded, _layout>::rotation3d(cool::rotation_type new_rotation_type) noexcept
{
	set_rotation_type(new_rotation_type);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation3d<traits_Ty, _dim_padded, _layout>& cool::rotation3d<traits_Ty, _dim_padded, _layout>::operator=(cool::rotation_type new_rotation_type) noexcept
{
	return set_rotation_type(new_rotation_type);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation3d<traits_Ty, _dim_padded, _layout>& cool::rotation3d<traits_Ty, _dim_padded, _layout>::set_rotation_type(cool::rotation_type new_rotation_type) noexcept
{
	this->m_rotation_type = new_rotation_type;

	switch (cool::rotation_type_as_index(new_rotation_type))
	{

	case cool::rotation_type_as_index(cool::rotation_type::id):
		this->m_rotation_functions = m_id; break;

	case cool::rotation_type_as_index(cool::rotation_type::X):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationX<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::Y):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationY<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::Z):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZ<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXY<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZ<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZ<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYX<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZX<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZY<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XYZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXYZ<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZY<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZX<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YXZ):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYXZ<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZXY):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZXY<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZYX):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZYX<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::XYX2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXYX2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::XZX2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationXZX2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YZY2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYZY2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::YXY2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationYXY2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZXZ2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZXZ2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;
	case cool::rotation_type_as_index(cool::rotation_type::ZYZ2):
		this->m_rotation_functions = cool::_rotation3d_functions<cool::rotationZYZ2<traits_Ty, _dim_padded, _layout>>::rotation_functions; break;

	case cool::rotation_type_as_index(cool::rotation_type::AA):
	{
		this->m_rotation_functions = [](value_type* dest_ptr, const value_type* orig_ptr, value_type angle_tol, value_type angle_choice_if_singular, int param) -> cool::rotation_status
		{
			if (param == 0)
			{
				cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix(dest_ptr, orig_ptr, *(orig_ptr + 3), static_cast<value_type>(0));
				return cool::rotation_status::regular;
			}
			else if (param == 1)
			{
				cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix_inv(dest_ptr, orig_ptr, *(orig_ptr + 3), static_cast<value_type>(0));
				return cool::rotation_status::regular;
			}
			else if (param == 2)
			{
				return cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_axis_angle(dest_ptr, dest_ptr + 3, orig_ptr, angle_tol);
			}
			else // param == 3
			{
				*dest_ptr = traits_Ty::half_turn;
				return cool::rotation_status::regular;
			}
		};
	}
	break;

	case cool::rotation_type_as_index(cool::rotation_type::Q):
	{
		this->m_rotation_functions = [](value_type* dest_ptr, const value_type* orig_ptr, value_type angle_tol, value_type angle_choice_if_singular, int param) -> cool::rotation_status
		{
			if (param == 0)
			{
				cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
			else if (param == 1)
			{
				cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix_inv(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
			else if (param == 2)
			{
				cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_quaternion_from_matrix(dest_ptr, orig_ptr);
				return cool::rotation_status::regular;
			}
			else // param == 3
			{
				*dest_ptr = traits_Ty::half_turn;
				return cool::rotation_status::regular;
			}
		};
	}
	break;

	}

	return *this;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline typename traits_Ty::value_type cool::rotation3d<traits_Ty, _dim_padded, _layout>::singular_angle(std::size_t pole) const noexcept
{
	constexpr std::uint32_t limit3 = static_cast<std::uint32_t>(cool::rotation_type::XYZ);
	constexpr std::uint32_t limit4 = static_cast<std::uint32_t>(cool::rotation_type::XYX2);
	constexpr std::uint32_t limit5 = static_cast<std::uint32_t>(cool::rotation_type::AA);

	std::uint32_t index = static_cast<std::uint32_t>(this->m_rotation_type);

	if (limit3 <= index && index < limit5)
	{
		if (index < limit4)
		{
			return (pole == 0) ? cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::singular_angle[0]
				: cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::singular_angle[1];
		}
		else
		{
			return (pole == 0) ? cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::singular_angle[0]
				: cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::singular_angle[1];
		}
	}
	else
	{
		return value_type{};
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline typename traits_Ty::value_type cool::rotation3d<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) const noexcept
{
	constexpr std::uint32_t limit3 = static_cast<std::uint32_t>(cool::rotation_type::XYZ);
	constexpr std::uint32_t limit5 = static_cast<std::uint32_t>(cool::rotation_type::AA);

	std::uint32_t index = static_cast<std::uint32_t>(this->m_rotation_type);

	if (limit3 <= index && index < limit5)
	{
		constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
		constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
		return angle_tol_coeff * (angle_tol * angle_tol);
	}
	else
	{
		return static_cast<value_type>(1);
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotation3d<traits_Ty, _dim_padded, _layout>::m_id(value_type* dest_ptr, const value_type*, value_type, value_type, int param) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	if (param < 2)
	{
		*(dest_ptr + _index_data::i00) = static_cast<value_type>(1);
		*(dest_ptr + _index_data::i10) = static_cast<value_type>(0);
		*(dest_ptr + _index_data::i20) = static_cast<value_type>(0);

		*(dest_ptr + _index_data::i01) = static_cast<value_type>(0);
		*(dest_ptr + _index_data::i11) = static_cast<value_type>(1);
		*(dest_ptr + _index_data::i21) = static_cast<value_type>(0);

		*(dest_ptr + _index_data::i02) = static_cast<value_type>(0);
		*(dest_ptr + _index_data::i12) = static_cast<value_type>(0);
		*(dest_ptr + _index_data::i22) = static_cast<value_type>(1);
	}
	else if (param == 3)
	{
		*dest_ptr = traits_Ty::half_turn;
	}

	return cool::rotation_status::regular;
}


template <class Ty>
inline cool::polymorphic_rotation3d<Ty>::operator cool::rotation_type() const noexcept
{
	return m_rotation_type;
}

template <class Ty>
inline cool::rotation_type cool::polymorphic_rotation3d<Ty>::rotation_type() const noexcept
{
	return m_rotation_type;
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::axis_count() const noexcept
{
	constexpr std::uint32_t limit1 = static_cast<std::uint32_t>(cool::rotation_type::X);
	constexpr std::uint32_t limit2 = static_cast<std::uint32_t>(cool::rotation_type::XY);
	constexpr std::uint32_t limit3 = static_cast<std::uint32_t>(cool::rotation_type::XYZ);
	constexpr std::uint32_t limit5 = static_cast<std::uint32_t>(cool::rotation_type::AA);

	std::uint32_t _index = static_cast<std::uint32_t>(m_rotation_type);

	return (_index >= limit5) ? 1
		: static_cast<std::size_t>(_index >= limit1)
		+ static_cast<std::size_t>(_index >= limit2)
		+ static_cast<std::size_t>(_index >= limit3);
}

template <class Ty>
inline cool::axis cool::polymorphic_rotation3d<Ty>::axis(std::size_t _index) const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<cool::axis>((static_cast<std::uint32_t>(m_rotation_type) >> (_index * 2 + 8)) & mask);
}

template <class Ty>
inline cool::axis cool::polymorphic_rotation3d<Ty>::singular_axis() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<cool::axis>((static_cast<std::uint32_t>(m_rotation_type) >> 14) & mask);
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::index(cool::axis _axis) const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> (static_cast<std::size_t>(_axis) * 2)) & mask);
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::iX() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>(static_cast<std::uint32_t>(m_rotation_type) & mask);
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::iY() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 2) & mask);
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::iZ() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 4) & mask);
}

template <class Ty>
inline std::size_t cool::polymorphic_rotation3d<Ty>::iSg() const noexcept
{
	constexpr std::uint32_t mask = static_cast<std::uint32_t>(1) << 0 | static_cast<std::uint32_t>(1) << 1;
	return static_cast<std::size_t>((static_cast<std::uint32_t>(m_rotation_type) >> 6) & mask);
}

template <class Ty>
inline void cool::polymorphic_rotation3d<Ty>::get_matrix(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept
{
	this->m_rotation_functions(m3x3_rotation_ptr, v4_coord_ptr, static_cast<Ty>(0), static_cast<Ty>(0), 0);
}

template <class Ty>
inline void cool::polymorphic_rotation3d<Ty>::get_matrix_inv(Ty* m3x3_rotation_ptr, const Ty* v4_coord_ptr) const noexcept
{
	this->m_rotation_functions(m3x3_rotation_ptr, v4_coord_ptr, static_cast<Ty>(0), static_cast<Ty>(0), 1);
}

template <class Ty>
inline cool::rotation_status cool::polymorphic_rotation3d<Ty>::get_coord(Ty* v4_coord_ptr, const Ty* m3x3_rotation_ptr,
	Ty angle_tol, Ty angle_choice_if_singular) const noexcept
{
	return this->m_rotation_functions(v4_coord_ptr, m3x3_rotation_ptr, angle_tol, angle_choice_if_singular, 2);
}

template <class Ty>
inline Ty cool::polymorphic_rotation3d<Ty>::half_turn() const noexcept
{
	Ty ret;
	this->m_rotation_functions(&ret, nullptr, static_cast<Ty>(0), static_cast<Ty>(0), 3);
	return ret;
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, value_type norm_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type s = traits_Ty::sin(angle);
	value_type c = traits_Ty::cos(angle);
	value_type cc = static_cast<value_type>(1) - c;

	value_type axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (norm_tol * norm_tol < axis_norm_sq)
	{
		value_type axis_norm_inv = traits_Ty::inv_sqrt(axis_norm_sq);

		value_type axis[3] = {
			axis_norm_inv * *v3_axis_ptr,
			axis_norm_inv * *(v3_axis_ptr + 1),
			axis_norm_inv * *(v3_axis_ptr + 2)
		};

		value_type temp01 = cc * axis[0];
		value_type temp12 = cc * axis[1];
		value_type temp20 = cc * axis[2];

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
		*(m3x3_rotation_ptr + _index_data::i00) = static_cast<value_type>(1);
		*(m3x3_rotation_ptr + _index_data::i10) = static_cast<value_type>(0);
		*(m3x3_rotation_ptr + _index_data::i20) = static_cast<value_type>(0);

		*(m3x3_rotation_ptr + _index_data::i01) = static_cast<value_type>(0);
		*(m3x3_rotation_ptr + _index_data::i11) = static_cast<value_type>(1);
		*(m3x3_rotation_ptr + _index_data::i21) = static_cast<value_type>(0);

		*(m3x3_rotation_ptr + _index_data::i02) = static_cast<value_type>(0);
		*(m3x3_rotation_ptr + _index_data::i12) = static_cast<value_type>(0);
		*(m3x3_rotation_ptr + _index_data::i22) = static_cast<value_type>(1);
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, value_type norm_tol) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_axis_angle<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_axis_ptr, angle, norm_tol);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, cool::no_axis_norm_t) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type s = traits_Ty::sin(angle);
	value_type c = traits_Ty::cos(angle);
	value_type cc = static_cast<value_type>(1) - c;

	value_type axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	value_type temp01 = cc * axis[0];
	value_type temp12 = cc * axis[1];
	value_type temp20 = cc * axis[2];

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_axis_ptr, value_type angle, cool::no_axis_norm_t) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_axis_angle<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_axis_ptr, angle, angle, cool::no_axis_norm);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_axis_angle(
	value_type* v3_axis_ptr, value_type* angle_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type quat_comp_sq[4];
	value_type quat_comp_sgn[4];

	{
		value_type a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
		value_type b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
		value_type a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
		value_type b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
		value_type a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
		value_type b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

		value_type c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = static_cast<value_type>(0.25);
		quat_comp_sgn[1] = (a0 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
		quat_comp_sgn[2] = (a1 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
		quat_comp_sgn[3] = (a2 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
	}

	value_type quat[4] = {
		quat_comp_sgn[0] * traits_Ty::sqrt(quat_comp_sq[0]),
		quat_comp_sgn[1] * traits_Ty::sqrt(quat_comp_sq[1]),
		quat_comp_sgn[2] * traits_Ty::sqrt(quat_comp_sq[2]),
		quat_comp_sgn[3] * traits_Ty::sqrt(quat_comp_sq[3])
	};

	value_type xyz_norm = static_cast<value_type>(0.25) * traits_Ty::sqrt(quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * traits_Ty::pi / traits_Ty::half_turn;

	if (angle_tol_coeff * angle_tol < xyz_norm)
	{
		value_type xyz_norm_inv = static_cast<value_type>(1) / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = static_cast<value_type>(2) * traits_Ty::atan2(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<value_type>(0);
		*(v3_axis_ptr + 1) = static_cast<value_type>(0);
		*(v3_axis_ptr + 2) = static_cast<value_type>(1);
		*angle_ptr = static_cast<value_type>(0);

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_axis_angle(
	value_type* v3_axis_ptr, value_type* angle_ptr, const value_type* m3x3_rotation_ptr, const value_type* v3_axis_way_ptr, value_type angle_tol) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type quat_comp_sq[4];
	value_type quat_comp_sgn[4];

	{
		value_type a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
		value_type b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
		value_type a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
		value_type b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
		value_type a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
		value_type b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

		value_type c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

		c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

		c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

		c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
		quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

		quat_comp_sgn[0] = static_cast<value_type>(0.25);
		quat_comp_sgn[1] = (a0 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
		quat_comp_sgn[2] = (a1 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
		quat_comp_sgn[3] = (a2 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25);
	}

	value_type quat[4] = {
		quat_comp_sgn[0] * traits_Ty::sqrt(quat_comp_sq[0]),
		quat_comp_sgn[1] * traits_Ty::sqrt(quat_comp_sq[1]),
		quat_comp_sgn[2] * traits_Ty::sqrt(quat_comp_sq[2]),
		quat_comp_sgn[3] * traits_Ty::sqrt(quat_comp_sq[3])
	};

	value_type xyz_norm = static_cast<value_type>(0.25) * traits_Ty::sqrt(quat_comp_sq[1] + quat_comp_sq[2] + quat_comp_sq[3]);

	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * traits_Ty::pi / traits_Ty::half_turn;

	if (angle_tol_coeff * angle_tol < xyz_norm)
	{
		value_type param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		value_type flip = (param < static_cast<value_type>(0)) ? static_cast<value_type>(-1) : static_cast<value_type>(0);

		value_type xyz_norm_inv = flip / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * static_cast<value_type>(2) * traits_Ty::atan2(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		value_type axis_way[3] = { *v3_axis_way_ptr, *(v3_axis_way_ptr + 1), *(v3_axis_way_ptr + 2) };

		value_type axis_way_norm_inv = traits_Ty::inv_sqrt(axis_way[0] * axis_way[0] + axis_way[1] * axis_way[1] + axis_way[2] * axis_way[2]);

		*v3_axis_ptr = axis_way_norm_inv * axis_way[0];
		*(v3_axis_ptr + 1) = axis_way_norm_inv * axis_way[1];
		*(v3_axis_ptr + 2) = axis_way_norm_inv * axis_way[2];
		*angle_ptr = static_cast<value_type>(0);

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_axis_angle<traits_Ty, _dim_padded, _layout>::get_angle(value_type* angle_ptr, const value_type* m3x3_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type temp = static_cast<value_type>(3) - (*(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22));
	temp = (static_cast<value_type>(0) < temp) ? temp : static_cast<value_type>(0);

	value_type sin_angle_d2 = static_cast<value_type>(0.5) * traits_Ty::sqrt(temp);

	if (sin_angle_d2 < static_cast<value_type>(1))
	{
		*angle_ptr = static_cast<value_type>(2) * traits_Ty::asin(sin_angle_d2);
	}
	else
	{
		*angle_ptr = traits_Ty::half_turn;
	}
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type q00 = *v4_quaternion_ptr * *v4_quaternion_ptr;
	value_type q11 = *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	value_type q22 = *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	value_type q33 = *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);
	value_type s = static_cast<value_type>(2) / (q00 + q11 + q22 + q33);
	q00 *= s; q11 *= s; q22 *= s; q33 *= s;
	value_type q01 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	value_type q02 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	value_type q03 = s * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	value_type q12 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	value_type q13 = s * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	value_type q23 = s * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);


	*(m3x3_rotation_ptr + _index_data::i00) = static_cast<value_type>(1) - q22 - q33;
	*(m3x3_rotation_ptr + _index_data::i10) = q12 + q03;
	*(m3x3_rotation_ptr + _index_data::i20) = q13 - q02;

	*(m3x3_rotation_ptr + _index_data::i01) = q12 - q03;
	*(m3x3_rotation_ptr + _index_data::i11) = static_cast<value_type>(1) - q11 - q33;
	*(m3x3_rotation_ptr + _index_data::i21) = q23 + q01;

	*(m3x3_rotation_ptr + _index_data::i02) = q13 + q02;
	*(m3x3_rotation_ptr + _index_data::i12) = q23 - q01;
	*(m3x3_rotation_ptr + _index_data::i22) = static_cast<value_type>(1) - q11 - q22;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_quaternion<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v4_quaternion_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type q00 = static_cast<value_type>(2) * *v4_quaternion_ptr * *v4_quaternion_ptr;
	value_type q01 = static_cast<value_type>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 1);
	value_type q02 = static_cast<value_type>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 2);
	value_type q03 = static_cast<value_type>(2) * *v4_quaternion_ptr * *(v4_quaternion_ptr + 3);
	value_type q11 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 1);
	value_type q12 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 2);
	value_type q13 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 1) * *(v4_quaternion_ptr + 3);
	value_type q22 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 2);
	value_type q23 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 2) * *(v4_quaternion_ptr + 3);
	value_type q33 = static_cast<value_type>(2) * *(v4_quaternion_ptr + 3) * *(v4_quaternion_ptr + 3);

	*(m3x3_rotation_ptr + _index_data::i00) = static_cast<value_type>(1) - q22 - q33;
	*(m3x3_rotation_ptr + _index_data::i10) = q12 + q03;
	*(m3x3_rotation_ptr + _index_data::i20) = q13 - q02;

	*(m3x3_rotation_ptr + _index_data::i01) = q12 - q03;
	*(m3x3_rotation_ptr + _index_data::i11) = static_cast<value_type>(1) - q11 - q33;
	*(m3x3_rotation_ptr + _index_data::i21) = q23 + q01;

	*(m3x3_rotation_ptr + _index_data::i02) = q13 + q02;
	*(m3x3_rotation_ptr + _index_data::i12) = q23 - q01;
	*(m3x3_rotation_ptr + _index_data::i22) = static_cast<value_type>(1) - q11 - q22;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v4_quaternion_ptr, cool::no_quaternion_norm_t) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotation_quaternion<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v4_quaternion_ptr, cool::no_quaternion_norm);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_quaternion_from_matrix(
	value_type* v4_quaternion_ptr, const value_type* m3x3_rotation_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type quat_comp_sq[4];

	value_type a0 = *(m3x3_rotation_ptr + _index_data::i21) - *(m3x3_rotation_ptr + _index_data::i12);
	value_type b0 = *(m3x3_rotation_ptr + _index_data::i21) + *(m3x3_rotation_ptr + _index_data::i12);
	value_type a1 = *(m3x3_rotation_ptr + _index_data::i02) - *(m3x3_rotation_ptr + _index_data::i20);
	value_type b1 = *(m3x3_rotation_ptr + _index_data::i02) + *(m3x3_rotation_ptr + _index_data::i20);
	value_type a2 = *(m3x3_rotation_ptr + _index_data::i10) - *(m3x3_rotation_ptr + _index_data::i01);
	value_type b2 = *(m3x3_rotation_ptr + _index_data::i10) + *(m3x3_rotation_ptr + _index_data::i01);

	value_type c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[0] = c * c + a0 * a0 + a1 * a1 + a2 * a2;

	c = static_cast<value_type>(1) + *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[1] = a0 * a0 + c * c + b1 * b1 + b2 * b2;

	c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) + *(m3x3_rotation_ptr + _index_data::i11) - *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[2] = a1 * a1 + b0 * b0 + c * c + b2 * b2;

	c = static_cast<value_type>(1) - *(m3x3_rotation_ptr + _index_data::i00) - *(m3x3_rotation_ptr + _index_data::i11) + *(m3x3_rotation_ptr + _index_data::i22);
	quat_comp_sq[3] = a2 * a2 + b0 * b0 + b1 * b1 + c * c;

	value_type quat_comp_sgn[4] = {
		static_cast<value_type>(0.25),
		(a0 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25),
		(a1 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25),
		(a2 < static_cast<value_type>(0)) ? static_cast<value_type>(-0.25) : static_cast<value_type>(0.25)
	};

	*v4_quaternion_ptr = quat_comp_sgn[0] * traits_Ty::sqrt(quat_comp_sq[0]);
	*(v4_quaternion_ptr + 1) = quat_comp_sgn[1] * traits_Ty::sqrt(quat_comp_sq[1]);
	*(v4_quaternion_ptr + 2) = quat_comp_sgn[2] * traits_Ty::sqrt(quat_comp_sq[2]);
	*(v4_quaternion_ptr + 3) = quat_comp_sgn[3] * traits_Ty::sqrt(quat_comp_sq[3]);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_quaternion_from_axis_angle(
	value_type* v4_quaternion_ptr, const value_type* v3_axis_ptr, value_type angle, value_type norm_tol) noexcept
{
	value_type c = traits_Ty::cos(static_cast<value_type>(0.5) * angle);
	value_type s = traits_Ty::sin(static_cast<value_type>(0.5) * angle);

	value_type axis_norm_sq = *v3_axis_ptr * *v3_axis_ptr
		+ *(v3_axis_ptr + 1) * *(v3_axis_ptr + 1)
		+ *(v3_axis_ptr + 2) * *(v3_axis_ptr + 2);

	if (norm_tol * norm_tol < axis_norm_sq)
	{
		value_type axis_norm_inv = traits_Ty::inv_sqrt(axis_norm_sq);

		value_type axis[3] = {
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
		*v4_quaternion_ptr = static_cast<value_type>(1);
		*(v4_quaternion_ptr + 1) = static_cast<value_type>(0);
		*(v4_quaternion_ptr + 2) = static_cast<value_type>(0);
		*(v4_quaternion_ptr + 3) = static_cast<value_type>(0);
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_quaternion_from_axis_angle(
	value_type* v4_quaternion_ptr, const value_type* v3_axis_ptr, value_type angle, cool::no_axis_norm_t) noexcept
{
	value_type c = traits_Ty::cos(static_cast<value_type>(0.5) * angle);
	value_type s = traits_Ty::sin(static_cast<value_type>(0.5) * angle);

	value_type axis[3] = { *v3_axis_ptr, *(v3_axis_ptr + 1), *(v3_axis_ptr + 2) };

	*v4_quaternion_ptr = c;
	*(v4_quaternion_ptr + 1) = s * axis[0];
	*(v4_quaternion_ptr + 2) = s * axis[1];
	*(v4_quaternion_ptr + 3) = s * axis[2];
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_axis_angle(
	value_type* v3_axis_ptr, value_type* angle_ptr, value_type* v4_quaternion_ptr, value_type angle_tol) noexcept
{
	value_type quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	value_type xyz_norm = traits_Ty::sqrt(quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (traits_Ty::pi / traits_Ty::half_turn);

	if (angle_tol_coeff * angle_tol < xyz_norm)
	{
		value_type xyz_norm_inv = static_cast<value_type>(1) / xyz_norm;
		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];

		*angle_ptr = static_cast<value_type>(2) * traits_Ty::atan2(xyz_norm, quat[0]);

		return cool::rotation_status::regular;
	}
	else
	{
		*v3_axis_ptr = static_cast<value_type>(0);
		*(v3_axis_ptr + 1) = static_cast<value_type>(0);
		*(v3_axis_ptr + 2) = static_cast<value_type>(1);
		*angle_ptr = static_cast<value_type>(0);

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotation_quaternion<traits_Ty, _dim_padded, _layout>::get_axis_angle(
	value_type* v3_axis_ptr, value_type* angle_ptr, value_type* v4_quaternion_ptr, const value_type* v3_axis_way_ptr, value_type angle_tol) noexcept
{
	value_type quat[4] = {
		*v4_quaternion_ptr,
		*(v4_quaternion_ptr + 1),
		*(v4_quaternion_ptr + 2),
		*(v4_quaternion_ptr + 3)
	};

	value_type xyz_norm = traits_Ty::sqrt(quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);

	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (traits_Ty::pi / traits_Ty::half_turn);

	if (angle_tol_coeff * angle_tol < xyz_norm)
	{
		value_type angle = static_cast<value_type>(2) * traits_Ty::atan2(xyz_norm, quat[0]);

		value_type param = quat[1] * *v3_axis_way_ptr + quat[2] * *(v3_axis_way_ptr + 1) + quat[3] * *(v3_axis_way_ptr + 2);
		param = (param != static_cast<value_type>(0)) ? param : angle;

		value_type flip = (param < static_cast<value_type>(0)) ? static_cast<value_type>(-1) : static_cast<value_type>(1);

		value_type xyz_norm_inv = flip / xyz_norm;

		*v3_axis_ptr = xyz_norm_inv * quat[1];
		*(v3_axis_ptr + 1) = xyz_norm_inv * quat[2];
		*(v3_axis_ptr + 2) = xyz_norm_inv * quat[3];
		*angle_ptr = flip * angle;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type axis_way[3] = { *v3_axis_way_ptr, *(v3_axis_way_ptr + 1), *(v3_axis_way_ptr + 2) };

		value_type axis_way_norm_inv = traits_Ty::inv_sqrt(axis_way[0] * axis_way[0] + axis_way[1] * axis_way[1] + axis_way[2] * axis_way[2]);

		*v3_axis_ptr = axis_way_norm_inv * axis_way[0];
		*(v3_axis_ptr + 1) = axis_way_norm_inv * axis_way[1]
			* (v3_axis_ptr + 2) = axis_way_norm_inv * axis_way[2];
		*angle_ptr = static_cast<value_type>(0);

		return cool::rotation_status::singular;
	}
}


template <class traits_Ty>
inline void cool::direction3d<traits_Ty>::get_direction(
	value_type* v3_direction_ptr,
	const value_type* v2_azimuth_altitude_angles_ptr,
	const value_type* v3_front_dir_ptr,
	const value_type* v3_lateral_dir_ptr,
	const value_type* v3_up_dir_ptr) noexcept
{
	value_type cosAZ = traits_Ty::cos(*v2_azimuth_altitude_angles_ptr);
	value_type cosAL = traits_Ty::cos(*(v2_azimuth_altitude_angles_ptr + 1));
	value_type sinAZ = traits_Ty::sin(*v2_azimuth_altitude_angles_ptr);
	value_type sinAL = traits_Ty::sin(*(v2_azimuth_altitude_angles_ptr + 1));

	value_type coord[3] = { cosAZ * cosAL, sinAZ * cosAL, sinAL };

	value_type front_dir[3] = { *v3_front_dir_ptr, *(v3_front_dir_ptr + 1), *(v3_front_dir_ptr + 2) };
	value_type lateral_dir[3] = { *v3_lateral_dir_ptr, *(v3_lateral_dir_ptr + 1), *(v3_lateral_dir_ptr + 2) };
	value_type up_dir[3] = { *v3_up_dir_ptr, *(v3_up_dir_ptr + 1), *(v3_up_dir_ptr + 2) };

	*v3_direction_ptr = coord[0] * front_dir[0] + coord[1] * lateral_dir[0] + coord[2] * up_dir[0];
	*(v3_direction_ptr + 1) = coord[0] * front_dir[1] + coord[1] * lateral_dir[1] + coord[2] * up_dir[1];
	*(v3_direction_ptr + 2) = coord[0] * front_dir[2] + coord[1] * lateral_dir[2] + coord[2] * up_dir[2];
}

template <class traits_Ty>
inline cool::rotation_status cool::direction3d<traits_Ty>::get_angles(
	value_type* v2_azimuth_altitude_angles_ptr,
	const value_type* v3_direction_ptr,
	const value_type* v3_front_dir_ptr,
	const value_type* v3_lateral_dir_ptr,
	const value_type* v3_up_dir_ptr,
	value_type altitude_angle_tol,
	value_type azimuth_angle_if_singular) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type direction[3] = { *v3_direction_ptr, *(v3_direction_ptr + 1), *(v3_direction_ptr + 2) };

	{
		value_type s = traits_Ty::inv_sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
		direction[0] *= s; direction[1] *= s; direction[2] *= s;
	}

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (altitude_angle_tol * altitude_angle_tol);

	value_type sinAL = direction[0] * *v3_up_dir_ptr + direction[1] * *(v3_up_dir_ptr + 1) + direction[2] * *(v3_up_dir_ptr + 2);

	if (-bound < sinAL && sinAL < bound)
	{
		*v2_azimuth_altitude_angles_ptr = traits_Ty::atan2( // rAZ
			direction[0] * *v3_lateral_dir_ptr + direction[1] * *(v3_lateral_dir_ptr + 1) + direction[2] * *(v3_lateral_dir_ptr + 2),
			direction[0] * *v3_front_dir_ptr + direction[1] * *(v3_front_dir_ptr + 1) + direction[2] * *(v3_front_dir_ptr + 2)
		);
		*(v2_azimuth_altitude_angles_ptr + 1) = traits_Ty::asin(sinAL); // rAL

		return cool::rotation_status::regular;
	}
	else
	{
		*v2_azimuth_altitude_angles_ptr = azimuth_angle_if_singular; // rAZ
		*(v2_azimuth_altitude_angles_ptr + 1) = (sinAL < static_cast<value_type>(0)) ? singular_angle[0] : singular_angle[1]; // rAL

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty>
inline cool::rotation_status cool::direction3d<traits_Ty>::get_angles(
	value_type* v2_azimuth_altitude_angles_ptr,
	const value_type* v3_direction_ptr,
	const value_type* v3_front_dir_ptr,
	const value_type* v3_lateral_dir_ptr,
	const value_type* v3_up_dir_ptr,
	cool::no_direction_norm_t,
	value_type altitude_angle_tol,
	value_type azimuth_angle_if_singular) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi	/ traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (altitude_angle_tol * altitude_angle_tol);

	value_type sinAL = *v3_direction_ptr * *v3_up_dir_ptr + *(v3_direction_ptr + 1) * *(v3_up_dir_ptr + 1)
		+ *(v3_direction_ptr + 2) * *(v3_up_dir_ptr + 2);

	if (-bound < sinAL && sinAL < bound)
	{
		*v2_azimuth_altitude_angles_ptr = traits_Ty::atan2( // rAZ
			*v3_direction_ptr * *v3_lateral_dir_ptr + *(v3_direction_ptr + 1) * *(v3_lateral_dir_ptr + 1) + *(v3_direction_ptr + 2) * *(v3_lateral_dir_ptr + 2),
			*v3_direction_ptr * *v3_front_dir_ptr + *(v3_direction_ptr + 1) * *(v3_front_dir_ptr + 1) + *(v3_direction_ptr + 2) * *(v3_front_dir_ptr + 2)
		);
		*(v2_azimuth_altitude_angles_ptr + 1) = traits_Ty::asin(sinAL); // rAL

		return cool::rotation_status::regular;
	}
	else
	{
		*v2_azimuth_altitude_angles_ptr = azimuth_angle_if_singular; // rAZ
		*(v2_azimuth_altitude_angles_ptr + 1) = (sinAL < static_cast<value_type>(0)) ? singular_angle[0] : singular_angle[1]; // rAL

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty>
constexpr inline typename traits_Ty::value_type cool::direction3d<traits_Ty>::required_epsilon(value_type altitude_angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (altitude_angle_tol * altitude_angle_tol);
}


template <class Ty> template <class traits_Ty>
cool::polymorphic_direction3d<Ty>::polymorphic_direction3d(cool::direction3d<traits_Ty>) noexcept
{
	m_direction_functions = [](Ty* dest_ptr, const Ty* orig_ptr, const Ty* v3_front_dir_ptr, const Ty* v3_lateral_dir_ptr, const Ty* v3_up_dir_ptr,
		Ty altitude_angle_tol, Ty azimuth_angle_if_singular, int param) -> cool::rotation_status
	{
		if (param == 0)
		{
			cool::direction3d<traits_Ty>::get_direction(dest_ptr, orig_ptr, v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr);
			return cool::rotation_status::regular;
		}
		else if (param == 1)
		{
			return cool::direction3d<traits_Ty>::get_angles(dest_ptr, orig_ptr, v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr,
				altitude_angle_tol, azimuth_angle_if_singular);
		}
		else // param == 2
		{
			*dest_ptr = traits_Ty::half_turn;
			return cool::rotation_status::regular;
		}
	};
}

template <class Ty> template <class traits_Ty>
cool::polymorphic_direction3d<Ty>& cool::polymorphic_direction3d<Ty>::operator=(cool::direction3d<traits_Ty>) noexcept
{
	m_direction_functions = [](Ty* dest_ptr, const Ty* orig_ptr, const Ty* v3_front_dir_ptr, const Ty* v3_lateral_dir_ptr, const Ty* v3_up_dir_ptr,
		Ty altitude_angle_tol, Ty azimuth_angle_if_singular, int param) -> cool::rotation_status
	{
		if (param == 0)
		{
			cool::direction3d<traits_Ty>::get_direction(dest_ptr, orig_ptr, v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr);
			return cool::rotation_status::regular;
		}
		else if (param == 1)
		{
			return cool::direction3d<traits_Ty>::get_angles(dest_ptr, orig_ptr, v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr,
				altitude_angle_tol, azimuth_angle_if_singular);
		}
		else // param == 2
		{
			*dest_ptr = traits_Ty::half_turn;
			return cool::rotation_status::regular;
		}
	};

	return *this;
}

template <class Ty>
constexpr std::size_t cool::polymorphic_direction3d<Ty>::iSg() noexcept
{
	return 1;
}

template <class Ty>
inline void cool::polymorphic_direction3d<Ty>::get_direction(
	Ty* v3_direction_ptr,
	const Ty* v2_azimuth_altitude_angles_ptr,
	const Ty* v3_front_dir_ptr,
	const Ty* v3_lateral_dir_ptr,
	const Ty* v3_up_dir_ptr) const noexcept
{
	m_direction_functions(v3_direction_ptr, v2_azimuth_altitude_angles_ptr,	v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr,
		static_cast<Ty>(0), static_cast<Ty>(0), 0);
}

template <class Ty>
inline cool::rotation_status cool::polymorphic_direction3d<Ty>::get_angles(
	Ty* v2_azimuth_altitude_angles_ptr,
	const Ty* v3_direction_ptr,
	const Ty* v3_front_dir_ptr,
	const Ty* v3_lateral_dir_ptr,
	const Ty* v3_up_dir_ptr,
	Ty altitude_angle_tol,
	Ty azimuth_angle_if_singular) const noexcept
{
	return m_direction_functions(v2_azimuth_altitude_angles_ptr, v3_direction_ptr, v3_front_dir_ptr, v3_lateral_dir_ptr, v3_up_dir_ptr,
		altitude_angle_tol, azimuth_angle_if_singular, 1);
}

template <class Ty>
inline Ty cool::polymorphic_direction3d<Ty>::half_turn() const noexcept
{
	Ty ret;
	m_direction_functions(&ret, nullptr, nullptr, nullptr, nullptr, static_cast<Ty>(0), static_cast<Ty>(0), 2);
	return ret;
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationX<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type angle = *angle_ptr;

	value_type cosX = traits_type::cos(angle);
	value_type sinX = traits_type::sin(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = static_cast<value_type>(1);
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationX<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationX<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationX<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* angle_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	*angle_ptr = traits_type::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i11));

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename cool::rotationX<traits_Ty, _dim_padded, _layout>::value_type cool::rotationX<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationY<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type angle = *angle_ptr;

	value_type cosY = traits_type::cos(angle);
	value_type sinY = traits_type::sin(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = static_cast<value_type>(1);
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationY<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationY<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationY<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* angle_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	*angle_ptr = traits_type::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i00));

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename cool::rotationY<traits_Ty, _dim_padded, _layout>::value_type cool::rotationY<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZ<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type angle = *angle_ptr;

	value_type cosZ = traits_type::cos(angle);
	value_type sinZ = traits_type::sin(angle);

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = static_cast<value_type>(1);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZ<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* angle_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZ<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, angle_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZ<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* angle_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	*angle_ptr = traits_type::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i00));

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename cool::rotationZ<traits_Ty, _dim_padded, _layout>::value_type cool::rotationZ<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXY<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rXY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v2_rXY_ptr);
	value_type cosY = traits_Ty::cos(*(v2_rXY_ptr + 1));
	value_type sinX = traits_Ty::sin(*v2_rXY_ptr);
	value_type sinY = traits_Ty::sin(*(v2_rXY_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = sinX * sinY;
	*(m3x3_rotation_ptr + _index_data::i20) = -cosX * sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosY;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXY<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rXY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXY<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rXY_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXY<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rXY_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rY = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i00));
	*v2_rXY_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i11)); // rX
	*(v2_rXY_ptr + 1) = rY;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXY<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZ<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rXZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v2_rXZ_ptr);
	value_type cosZ = traits_Ty::cos(*(v2_rXZ_ptr + 1));
	value_type sinX = traits_Ty::sin(*v2_rXZ_ptr);
	value_type sinZ = traits_Ty::sin(*(v2_rXZ_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = sinX * sinZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosZ;

	*(m3x3_rotation_ptr + _index_data::i02) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZ<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rXZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZ<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rXZ_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXZ<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rXZ_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rZ = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i00));
	*v2_rXZ_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i22)); // rX
	*(v2_rXZ_ptr + 1) = rZ;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXZ<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZ<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rYZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v2_rYZ_ptr);
	value_type cosZ = traits_Ty::cos(*(v2_rYZ_ptr + 1));
	value_type sinY = traits_Ty::sin(*v2_rYZ_ptr);
	value_type sinZ = traits_Ty::sin(*(v2_rYZ_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY * cosZ;

	*(m3x3_rotation_ptr + _index_data::i01) = -cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinY * sinZ;

	*(m3x3_rotation_ptr + _index_data::i02) = sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZ<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rYZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZ<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rYZ_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYZ<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rYZ_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rZ = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i11));
	*v2_rYZ_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i22)); // rY
	*(v2_rYZ_ptr + 1) = rZ;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationYZ<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYX<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rYX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v2_rYX_ptr);
	value_type cosX = traits_Ty::cos(*(v2_rYX_ptr + 1));
	value_type sinY = traits_Ty::sin(*v2_rYX_ptr);
	value_type sinX = traits_Ty::sin(*(v2_rYX_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY;
	*(m3x3_rotation_ptr + _index_data::i10) = static_cast<value_type>(0);
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = sinX * sinY;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX * cosY;

	*(m3x3_rotation_ptr + _index_data::i02) = cosX * sinY;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX * cosY;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYX<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rYX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYX<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rYX_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYX<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rYX_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rX = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i11));
	*v2_rYX_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i00)); // rY
	*(v2_rYX_ptr + 1) = rX;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationYX<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZX<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rZX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v2_rZX_ptr);
	value_type cosX = traits_Ty::cos(*(v2_rZX_ptr + 1));
	value_type sinZ = traits_Ty::sin(*v2_rZX_ptr);
	value_type sinX = traits_Ty::sin(*(v2_rZX_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i01) = -cosX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = sinX;

	*(m3x3_rotation_ptr + _index_data::i02) = sinX * sinZ;
	*(m3x3_rotation_ptr + _index_data::i12) = -sinX * cosZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosX;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZX<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rZX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZX<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rZX_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZX<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rZX_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rX = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i22));
	*v2_rZX_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i00)); // rZ
	*(v2_rZX_ptr + 1) = rX;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZX<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZY<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v2_rZY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v2_rZY_ptr);
	value_type cosY = traits_Ty::cos(*(v2_rZY_ptr + 1));
	value_type sinZ = traits_Ty::sin(*v2_rZY_ptr);
	value_type sinY = traits_Ty::sin(*(v2_rZY_ptr + 1));

	*(m3x3_rotation_ptr + _index_data::i00) = cosY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i10) = cosY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i20) = -sinY;

	*(m3x3_rotation_ptr + _index_data::i01) = -sinZ;
	*(m3x3_rotation_ptr + _index_data::i11) = cosZ;
	*(m3x3_rotation_ptr + _index_data::i21) = static_cast<value_type>(0);

	*(m3x3_rotation_ptr + _index_data::i02) = sinY * cosZ;
	*(m3x3_rotation_ptr + _index_data::i12) = sinY * sinZ;
	*(m3x3_rotation_ptr + _index_data::i22) = cosY;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZY<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v2_rZY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZY<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v2_rZY_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZY<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v2_rZY_ptr, const value_type* m3x3_rotation_ptr, value_type, value_type) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type rY = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i22));
	*v2_rZY_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i11)); // rZ
	*(v2_rZY_ptr + 1) = rY;

	return cool::rotation_status::regular;
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZY<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type) noexcept
{
	return static_cast<value_type>(1);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXYZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v3_rXYZ_ptr);
	value_type cosY = traits_Ty::cos(*(v3_rXYZ_ptr + 1));
	value_type cosZ = traits_Ty::cos(*(v3_rXYZ_ptr + 2));
	value_type sinX = traits_Ty::sin(*v3_rXYZ_ptr);
	value_type sinY = traits_Ty::sin(*(v3_rXYZ_ptr + 1));
	value_type sinZ = traits_Ty::sin(*(v3_rXYZ_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXYZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXYZ<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rXYZ_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rXYZ_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i02) && *(m3x3_rotation_ptr + _index_data::i02) < bound)
	{
		value_type rY = traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i02));
		value_type rZ = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i00));
		*v3_rXYZ_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i22)); // rX
		*(v3_rXYZ_ptr + 1) = rY;
		*(v3_rXYZ_ptr + 2) = rZ;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rZ0 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i11));
		bool Yneg = *(m3x3_rotation_ptr + _index_data::i02) < static_cast<value_type>(0);
		*v3_rXYZ_ptr = rX_choice_if_singular; // rX
		*(v3_rXYZ_ptr + 1) = Yneg ? singular_angle[0] : singular_angle[1]; // rY
		*(v3_rXYZ_ptr + 2) = Yneg ? rZ0 + rX_choice_if_singular : rZ0 - rX_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXYZ<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZY<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXZY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v3_rXZY_ptr);
	value_type cosZ = traits_Ty::cos(*(v3_rXZY_ptr + 1));
	value_type cosY = traits_Ty::cos(*(v3_rXZY_ptr + 2));
	value_type sinX = traits_Ty::sin(*v3_rXZY_ptr);
	value_type sinZ = traits_Ty::sin(*(v3_rXZY_ptr + 1));
	value_type sinY = traits_Ty::sin(*(v3_rXZY_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZY<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXZY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZY<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rXZY_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXZY<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rXZY_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i01) && *(m3x3_rotation_ptr + _index_data::i01) < bound)
	{
		value_type rZ = -traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i01));
		value_type rY = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i00));
		*v3_rXZY_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i11)); // rX
		*(v3_rXZY_ptr + 1) = rZ;
		*(v3_rXZY_ptr + 2) = rY;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rY0 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i22));
		bool Zneg = !(*(m3x3_rotation_ptr + _index_data::i01) < static_cast<value_type>(0));
		*v3_rXZY_ptr = rX_choice_if_singular; // rX
		*(v3_rXZY_ptr + 1) = Zneg ? singular_angle[0] : singular_angle[1]; // rZ
		*(v3_rXZY_ptr + 2) = Zneg ? rY0 - rX_choice_if_singular : rY0 + rX_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXZY<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZX<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYZX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v3_rYZX_ptr);
	value_type cosZ = traits_Ty::cos(*(v3_rYZX_ptr + 1));
	value_type cosX = traits_Ty::cos(*(v3_rYZX_ptr + 2));
	value_type sinY = traits_Ty::sin(*v3_rYZX_ptr);
	value_type sinZ = traits_Ty::sin(*(v3_rYZX_ptr + 1));
	value_type sinX = traits_Ty::sin(*(v3_rYZX_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZX<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYZX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZX<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rYZX_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYZX<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rYZX_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i10) && *(m3x3_rotation_ptr + _index_data::i10) < bound)
	{
		value_type rZ = traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i10));
		value_type rX = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i11));
		*v3_rYZX_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i00)); // rY
		*(v3_rYZX_ptr + 1) = rZ;
		*(v3_rYZX_ptr + 2) = rX;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rX0 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i22));
		bool Zneg = *(m3x3_rotation_ptr + _index_data::i10) < static_cast<value_type>(0);
		*v3_rYZX_ptr = rY_choice_if_singular; // rY
		*(v3_rYZX_ptr + 1) = Zneg ? singular_angle[0] : singular_angle[1]; // rZ
		*(v3_rYZX_ptr + 2) = Zneg ? rX0 + rY_choice_if_singular : rX0 - rY_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr typename traits_Ty::value_type cool::rotationYZX<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYXZ<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYXZ_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v3_rYXZ_ptr);
	value_type cosX = traits_Ty::cos(*(v3_rYXZ_ptr + 1));
	value_type cosZ = traits_Ty::cos(*(v3_rYXZ_ptr + 2));
	value_type sinY = traits_Ty::sin(*v3_rYXZ_ptr);
	value_type sinX = traits_Ty::sin(*(v3_rYXZ_ptr + 1));
	value_type sinZ = traits_Ty::sin(*(v3_rYXZ_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYXZ<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYXZ_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYXZ<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rYXZ_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYXZ<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rYXZ_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i12) && *(m3x3_rotation_ptr + _index_data::i12) < bound)
	{
		value_type rX = -traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i12));
		value_type rZ = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i11));
		*v3_rYXZ_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i22)); // rY
		*(v3_rYXZ_ptr + 1) = rX;
		*(v3_rYXZ_ptr + 2) = rZ;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rZ0 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i00));
		bool Xneg = !(*(m3x3_rotation_ptr + _index_data::i12) < static_cast<value_type>(0));
		*v3_rYXZ_ptr = rY_choice_if_singular; // rY
		*(v3_rYXZ_ptr + 1) = Xneg ? singular_angle[0] : singular_angle[1]; // rX
		*(v3_rYXZ_ptr + 2) = Xneg ? rZ0 - rY_choice_if_singular : rZ0 + rY_choice_if_singular; // rZ

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationYXZ<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZXY<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZXY_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v3_rZXY_ptr);
	value_type cosX = traits_Ty::cos(*(v3_rZXY_ptr + 1));
	value_type cosY = traits_Ty::cos(*(v3_rZXY_ptr + 2));
	value_type sinZ = traits_Ty::sin(*v3_rZXY_ptr);
	value_type sinX = traits_Ty::sin(*(v3_rZXY_ptr + 1));
	value_type sinY = traits_Ty::sin(*(v3_rZXY_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZXY<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZXY_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZXY<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rZXY_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZXY<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rZXY_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i21) && *(m3x3_rotation_ptr + _index_data::i21) < bound)
	{
		value_type rX = traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i21));
		value_type rY = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i22));
		*v3_rZXY_ptr = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i11)); // rZ
		*(v3_rZXY_ptr + 1) = rX;
		*(v3_rZXY_ptr + 2) = rY;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rY0 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i00));
		bool Xneg = *(m3x3_rotation_ptr + _index_data::i21) < static_cast<value_type>(0);
		*v3_rZXY_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZXY_ptr + 1) = Xneg ? singular_angle[0] : singular_angle[1]; // rX
		*(v3_rZXY_ptr + 2) = Xneg ? rY0 + rZ_choice_if_singular : rY0 - rZ_choice_if_singular; // rY

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZXY<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZYX<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZYX_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v3_rZYX_ptr);
	value_type cosY = traits_Ty::cos(*(v3_rZYX_ptr + 1));
	value_type cosX = traits_Ty::cos(*(v3_rZYX_ptr + 2));
	value_type sinZ = traits_Ty::sin(*v3_rZYX_ptr);
	value_type sinY = traits_Ty::sin(*(v3_rZYX_ptr + 1));
	value_type sinX = traits_Ty::sin(*(v3_rZYX_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZYX<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZYX_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZYX<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rZYX_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZYX<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rZYX_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i20) && *(m3x3_rotation_ptr + _index_data::i20) < bound)
	{
		value_type rY = -traits_Ty::asin(*(m3x3_rotation_ptr + _index_data::i20));
		value_type rX = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i22));
		*v3_rZYX_ptr = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i00)); // rZ
		*(v3_rZYX_ptr + 1) = rY;
		*(v3_rZYX_ptr + 2) = rX;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rX0 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i11));
		bool Yneg = !(*(m3x3_rotation_ptr + _index_data::i20) < static_cast<value_type>(0));
		*v3_rZYX_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZYX_ptr + 1) = Yneg ? singular_angle[0] : singular_angle[1]; // rY
		*(v3_rZYX_ptr + 2) = Yneg ? rX0 - rZ_choice_if_singular : rX0 + rZ_choice_if_singular; // rX

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZYX<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXYX2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v3_rXYX2_ptr);
	value_type cosY = traits_Ty::cos(*(v3_rXYX2_ptr + 1));
	value_type cosX2 = traits_Ty::cos(*(v3_rXYX2_ptr + 2));
	value_type sinX = traits_Ty::sin(*v3_rXYX2_ptr);
	value_type sinY = traits_Ty::sin(*(v3_rXYX2_ptr + 1));
	value_type sinX2 = traits_Ty::sin(*(v3_rXYX2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXYX2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXYX2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rXYX2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rXYX2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i00) && *(m3x3_rotation_ptr + _index_data::i00) < bound)
	{
		value_type rYabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i00));
		value_type rYsgn = (*(m3x3_rotation_ptr + _index_data::i02) < static_cast<value_type>(0)) ? static_cast<value_type>(-1) : static_cast<value_type>(1);
		value_type rX2 = traits_Ty::atan2(rYsgn * *(m3x3_rotation_ptr + _index_data::i01), rYsgn * *(m3x3_rotation_ptr + _index_data::i02));
		*v3_rXYX2_ptr = traits_Ty::atan2(rYsgn * *(m3x3_rotation_ptr + _index_data::i10), -rYsgn * *(m3x3_rotation_ptr + _index_data::i20)); // rX
		*(v3_rXYX2_ptr + 1) = rYsgn * rYabs;
		*(v3_rXYX2_ptr + 2) = rX2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rX20 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i12), *(m3x3_rotation_ptr + _index_data::i11));
		bool Yflipped = *(m3x3_rotation_ptr + _index_data::i00) < static_cast<value_type>(0);
		*v3_rXYX2_ptr = rX_choice_if_singular; // rX
		*(v3_rXYX2_ptr + 1) = Yflipped ? singular_angle[1] : singular_angle[0]; // rY
		*(v3_rXYX2_ptr + 2) = Yflipped ? rX20 + rX_choice_if_singular : rX20 - rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXYX2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZX2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXZX2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosX = traits_Ty::cos(*v3_rXZX2_ptr);
	value_type cosZ = traits_Ty::cos(*(v3_rXZX2_ptr + 1));
	value_type cosX2 = traits_Ty::cos(*(v3_rXZX2_ptr + 2));
	value_type sinX = traits_Ty::sin(*v3_rXZX2_ptr);
	value_type sinZ = traits_Ty::sin(*(v3_rXZX2_ptr + 1));
	value_type sinX2 = traits_Ty::sin(*(v3_rXZX2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationXZX2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rXZX2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationXZX2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rXZX2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationXZX2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rXZX2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rX_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i00) && *(m3x3_rotation_ptr + _index_data::i00) < bound)
	{
		value_type rZabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i00));
		value_type rZsgn = (*(m3x3_rotation_ptr + _index_data::i01) < static_cast<value_type>(0)) ? static_cast<value_type>(1) : static_cast<value_type>(-1);
		value_type rX2 = traits_Ty::atan2(rZsgn * *(m3x3_rotation_ptr + _index_data::i02), -rZsgn * *(m3x3_rotation_ptr + _index_data::i01));
		*v3_rXZX2_ptr = traits_Ty::atan2(rZsgn * *(m3x3_rotation_ptr + _index_data::i20), rZsgn * *(m3x3_rotation_ptr + _index_data::i10)); // rX
		*(v3_rXZX2_ptr + 1) = rZsgn * rZabs;
		*(v3_rXZX2_ptr + 2) = rX2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rX20 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i21), *(m3x3_rotation_ptr + _index_data::i22));
		bool Zflipped = *(m3x3_rotation_ptr + _index_data::i00) < static_cast<value_type>(0);
		*v3_rXZX2_ptr = rX_choice_if_singular; // rX
		*(v3_rXZX2_ptr + 1) = Zflipped ? singular_angle[1] : singular_angle[0]; // rZ2
		*(v3_rXZX2_ptr + 2) = Zflipped ? rX20 + rX_choice_if_singular : rX20 - rX_choice_if_singular; // rX2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationXZX2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZY2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYZY2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v3_rYZY2_ptr);
	value_type cosZ = traits_Ty::cos(*(v3_rYZY2_ptr + 1));
	value_type cosY2 = traits_Ty::cos(*(v3_rYZY2_ptr + 2));
	value_type sinY = traits_Ty::sin(*v3_rYZY2_ptr);
	value_type sinZ = traits_Ty::sin(*(v3_rYZY2_ptr + 1));
	value_type sinY2 = traits_Ty::sin(*(v3_rYZY2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYZY2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYZY2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYZY2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rYZY2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYZY2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rYZY2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i11) && *(m3x3_rotation_ptr + _index_data::i11) < bound)
	{
		value_type rZabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i11));
		value_type rZsgn = (*(m3x3_rotation_ptr + _index_data::i10) < static_cast<value_type>(0)) ? static_cast<value_type>(-1) : static_cast<value_type>(1);
		value_type rY2 = traits_Ty::atan2(rZsgn * *(m3x3_rotation_ptr + _index_data::i12), rZsgn * *(m3x3_rotation_ptr + _index_data::i10));
		*v3_rYZY2_ptr = traits_Ty::atan2(rZsgn * *(m3x3_rotation_ptr + _index_data::i21), -rZsgn * *(m3x3_rotation_ptr + _index_data::i01)); // rY
		*(v3_rYZY2_ptr + 1) = rZsgn * rZabs;
		*(v3_rYZY2_ptr + 2) = rY2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rY20 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i20), *(m3x3_rotation_ptr + _index_data::i22));
		bool Zflipped = *(m3x3_rotation_ptr + _index_data::i11) < static_cast<value_type>(0);
		*v3_rYZY2_ptr = rY_choice_if_singular; // rY
		*(v3_rYZY2_ptr + 1) = Zflipped ? singular_angle[1] : singular_angle[0]; // rZ
		*(v3_rYZY2_ptr + 2) = Zflipped ? rY20 + rY_choice_if_singular : rY20 - rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationYZY2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYXY2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYXY2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosY = traits_Ty::cos(*v3_rYXY2_ptr);
	value_type cosX = traits_Ty::cos(*(v3_rYXY2_ptr + 1));
	value_type cosY2 = traits_Ty::cos(*(v3_rYXY2_ptr + 2));
	value_type sinY = traits_Ty::sin(*v3_rYXY2_ptr);
	value_type sinX = traits_Ty::sin(*(v3_rYXY2_ptr + 1));
	value_type sinY2 = traits_Ty::sin(*(v3_rYXY2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationYXY2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rYXY2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationYXY2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rYXY2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationYXY2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rYXY2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rY_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i11) && *(m3x3_rotation_ptr + _index_data::i11) < bound)
	{
		value_type rXabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i11));
		value_type rXsgn = (*(m3x3_rotation_ptr + _index_data::i12) < static_cast<value_type>(0)) ? static_cast<value_type>(1) : static_cast<value_type>(-1);
		value_type rY2 = traits_Ty::atan2(rXsgn * *(m3x3_rotation_ptr + _index_data::i10), -rXsgn * *(m3x3_rotation_ptr + _index_data::i12));
		*v3_rYXY2_ptr = traits_Ty::atan2(rXsgn * *(m3x3_rotation_ptr + _index_data::i01), rXsgn * *(m3x3_rotation_ptr + _index_data::i21)); // rY
		*(v3_rYXY2_ptr + 1) = rXsgn * rXabs;
		*(v3_rYXY2_ptr + 2) = rY2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rY20 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i02), *(m3x3_rotation_ptr + _index_data::i00));
		bool Xflipped = *(m3x3_rotation_ptr + _index_data::i11) < static_cast<value_type>(0);
		*v3_rYXY2_ptr = rY_choice_if_singular; // rY
		*(v3_rYXY2_ptr + 1) = Xflipped ? singular_angle[1] : singular_angle[0]; // rX
		*(v3_rYXY2_ptr + 2) = Xflipped ? rY20 + rY_choice_if_singular : rY20 - rY_choice_if_singular; // rY2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationYXY2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZXZ2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZXZ2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v3_rZXZ2_ptr);
	value_type cosX = traits_Ty::cos(*(v3_rZXZ2_ptr + 1));
	value_type cosZ2 = traits_Ty::cos(*(v3_rZXZ2_ptr + 2));
	value_type sinZ = traits_Ty::sin(*v3_rZXZ2_ptr);
	value_type sinX = traits_Ty::sin(*(v3_rZXZ2_ptr + 1));
	value_type sinZ2 = traits_Ty::sin(*(v3_rZXZ2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZXZ2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZXZ2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZXZ2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rZXZ2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZXZ2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rZXZ2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i22) && *(m3x3_rotation_ptr + _index_data::i22) < bound)
	{
		value_type rXabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i22));
		value_type rXsgn = (*(m3x3_rotation_ptr + _index_data::i21) < static_cast<value_type>(0)) ? static_cast<value_type>(-1) : static_cast<value_type>(1);
		value_type rZ2 = traits_Ty::atan2(rXsgn * *(m3x3_rotation_ptr + _index_data::i20), rXsgn * *(m3x3_rotation_ptr + _index_data::i21));
		*v3_rZXZ2_ptr = traits_Ty::atan2(rXsgn * *(m3x3_rotation_ptr + _index_data::i02), -rXsgn * *(m3x3_rotation_ptr + _index_data::i12)); // rZ
		*(v3_rZXZ2_ptr + 1) = rXsgn * rXabs;
		*(v3_rZXZ2_ptr + 2) = rZ2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rZ20 = traits_Ty::atan2(-*(m3x3_rotation_ptr + _index_data::i01), *(m3x3_rotation_ptr + _index_data::i00));
		bool Xflipped = *(m3x3_rotation_ptr + _index_data::i22) < static_cast<value_type>(0);
		*v3_rZXZ2_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZXZ2_ptr + 1) = Xflipped ? singular_angle[1] : singular_angle[0]; // rX
		*(v3_rZXZ2_ptr + 2) = Xflipped ? rZ20 + rZ_choice_if_singular : rZ20 - rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZXZ2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
}


template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZYZ2<traits_Ty, _dim_padded, _layout>::get_matrix(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZYZ2_ptr) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	value_type cosZ = traits_Ty::cos(*v3_rZYZ2_ptr);
	value_type cosY = traits_Ty::cos(*(v3_rZYZ2_ptr + 1));
	value_type cosZ2 = traits_Ty::cos(*(v3_rZYZ2_ptr + 2));
	value_type sinZ = traits_Ty::sin(*v3_rZYZ2_ptr);
	value_type sinY = traits_Ty::sin(*(v3_rZYZ2_ptr + 1));
	value_type sinZ2 = traits_Ty::sin(*(v3_rZYZ2_ptr + 2));

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

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline void cool::rotationZYZ2<traits_Ty, _dim_padded, _layout>::get_matrix_inv(
	value_type* m3x3_rotation_ptr, const value_type* v3_rZYZ2_ptr) noexcept
{
	constexpr cool::matrix_layout _other_layout = (_layout == cool::matrix_layout::col) ? cool::matrix_layout::row : cool::matrix_layout::col;
	cool::rotationZYZ2<traits_Ty, _dim_padded, _other_layout>::get_matrix(m3x3_rotation_ptr, v3_rZYZ2_ptr);
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
inline cool::rotation_status cool::rotationZYZ2<traits_Ty, _dim_padded, _layout>::get_angles(
	value_type* v3_rZYZ2_ptr, const value_type* m3x3_rotation_ptr, value_type angle_tol, value_type rZ_choice_if_singular) noexcept
{
	using _index_data = cool::_rotation_matrix_index_data<_dim_padded, 3, _layout>;

	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);

	value_type bound = static_cast<value_type>(1) - angle_tol_coeff * (angle_tol * angle_tol);

	if (-bound < *(m3x3_rotation_ptr + _index_data::i22) && *(m3x3_rotation_ptr + _index_data::i22) < bound)
	{
		value_type rYabs = traits_Ty::acos(*(m3x3_rotation_ptr + _index_data::i22));
		value_type rYsgn = (*(m3x3_rotation_ptr + _index_data::i20) < static_cast<value_type>(0)) ? static_cast<value_type>(1) : static_cast<value_type>(-1);
		value_type rZ2 = traits_Ty::atan2(rYsgn * *(m3x3_rotation_ptr + _index_data::i21), -rYsgn * *(m3x3_rotation_ptr + _index_data::i20));
		*v3_rZYZ2_ptr = traits_Ty::atan2(rYsgn * *(m3x3_rotation_ptr + _index_data::i12), rYsgn * *(m3x3_rotation_ptr + _index_data::i02)); // rZ
		*(v3_rZYZ2_ptr + 1) = rYsgn * rYabs;
		*(v3_rZYZ2_ptr + 2) = rZ2;

		return cool::rotation_status::regular;
	}
	else
	{
		value_type rZ20 = traits_Ty::atan2(*(m3x3_rotation_ptr + _index_data::i10), *(m3x3_rotation_ptr + _index_data::i11));
		bool Yflipped = *(m3x3_rotation_ptr + _index_data::i22) < static_cast<value_type>(0);
		*v3_rZYZ2_ptr = rZ_choice_if_singular; // rZ
		*(v3_rZYZ2_ptr + 1) = Yflipped ? singular_angle[1] : singular_angle[0]; // rY
		*(v3_rZYZ2_ptr + 2) = Yflipped ? rZ20 + rZ_choice_if_singular : rZ20 - rZ_choice_if_singular; // rZ2

		return cool::rotation_status::singular;
	}
}

template <class traits_Ty, std::size_t _dim_padded, cool::matrix_layout _layout>
constexpr inline typename traits_Ty::value_type cool::rotationZYZ2<traits_Ty, _dim_padded, _layout>::required_epsilon(value_type angle_tol) noexcept
{
	constexpr value_type coeff_temp = traits_Ty::pi / traits_Ty::half_turn;
	constexpr value_type angle_tol_coeff = static_cast<value_type>(0.5) * (coeff_temp * coeff_temp);
	return angle_tol_coeff * (angle_tol * angle_tol);
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
	else if (param == 2)
	{
		return rotation3d_Ty::get_angles(dest_ptr, orig_ptr, angle_tol, angle_choice_if_singular);
	}
	else // param == 3
	{
		*dest_ptr = rotation3d_Ty::half_turn;
		return cool::rotation_status::regular;
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