// cool_chrono.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_CHRONO_HPP
#define xCOOL_CHRONO_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <limits>

#ifdef COOL_DURATION_CUSTOM_UNIT0
#ifndef COOL_DURATION_CUSTOM_UNIT0_RATIO
#define COOL_DURATION_CUSTOM_UNIT0_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT0_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
#ifndef COOL_DURATION_CUSTOM_UNIT1_RATIO
#define COOL_DURATION_CUSTOM_UNIT1_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT1_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT1, 1

#ifdef COOL_DURATION_CUSTOM_UNIT2
#ifndef COOL_DURATION_CUSTOM_UNIT2_RATIO
#define COOL_DURATION_CUSTOM_UNIT2_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT2_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
#ifndef COOL_DURATION_CUSTOM_UNIT3_RATIO
#define COOL_DURATION_CUSTOM_UNIT3_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT3_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
#ifndef COOL_DURATION_CUSTOM_UNIT4_RATIO
#define COOL_DURATION_CUSTOM_UNIT4_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT4_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
#ifndef COOL_DURATION_CUSTOM_UNIT5_RATIO
#define COOL_DURATION_CUSTOM_UNIT5_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT5_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
#ifndef COOL_DURATION_CUSTOM_UNIT6_RATIO
#define COOL_DURATION_CUSTOM_UNIT6_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT6_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
#ifndef COOL_DURATION_CUSTOM_UNIT7_RATIO
#define COOL_DURATION_CUSTOM_UNIT7_RATIO 1, 1
#endif // COOL_DURATION_CUSTOM_UNIT7_RATIO
#endif // COOL_DURATION_CUSTOM_UNIT7


// custom clock code example

//class custom_clock_example
//{
//public:
//	using rep = INTEGRAL_TYPE;
//	using period = std::ratio<NUM, DEM>; // one period is NUM/DEM second
//	using duration = std::chrono::duration<rep, period>;
//	using time_point = std::chrono::time_point<custom_clock_example>;
//	static constexpr bool is_steady = true;
//	static time_point now() noexcept
//	{
//		return time_point(duration(INTEGRAL_COUNTER_VALUE));
//	}
//};


namespace cool
{
	template <class clock_Ty> class time_point;
	template <class clock_Ty> class duration;

#ifndef xCOOL_DURATION_UNIT_ENUM
#define xCOOL_DURATION_UNIT_ENUM
	enum class duration_unit : std::size_t
	{
		s = 0,
		ms = 1,
		us = 2,
		ns = 3,

		min = 4,
		hour = 5,
		day = 6,

		tick = 7,

#ifdef COOL_DURATION_CUSTOM_UNIT0
		COOL_DURATION_CUSTOM_UNIT0,
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
		COOL_DURATION_CUSTOM_UNIT1,
#endif // COOL_DURATION_CUSTOM_UNIT1

#ifdef COOL_DURATION_CUSTOM_UNIT2
		COOL_DURATION_CUSTOM_UNIT2,
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
		COOL_DURATION_CUSTOM_UNIT3,
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
		COOL_DURATION_CUSTOM_UNIT4,
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
		COOL_DURATION_CUSTOM_UNIT5,
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
		COOL_DURATION_CUSTOM_UNIT6,
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
		COOL_DURATION_CUSTOM_UNIT7,
#endif // COOL_DURATION_CUSTOM_UNIT7
	};
#endif // xCOOL_DURATION_UNIT_ENUM

#ifndef xCOOL_NOW_ENUM
#define xCOOL_NOW_ENUM
	enum now_t { now };
#endif // xCOOL_NOW_ENUM

#ifndef xCOOL_NO_INIT_ENUM
#define xCOOL_NO_INIT_ENUM
	enum no_init_t { no_init };
#endif // xCOOL_NO_INIT_ENUM

	template <class clock_Ty> class time_point
	{

	public:

		using clock_type = clock_Ty;
		using rep = typename clock_Ty::time_point::rep;
		using period = typename clock_Ty::time_point::period;

		time_point() = default; // does not set time_point to now
		explicit inline time_point(cool::now_t) noexcept : m_time(clock_Ty::now()) {}
		explicit inline time_point(cool::no_init_t) noexcept {}
		time_point(const cool::time_point<clock_Ty>& rhs) noexcept = default;
		cool::time_point<clock_Ty>& operator=(const cool::time_point<clock_Ty>& rhs) noexcept = default;
		~time_point() = default;

		inline cool::time_point<clock_Ty>& now() noexcept;

		inline cool::time_point<clock_Ty>& operator-=(cool::duration<clock_Ty> rhs) noexcept;
		inline cool::time_point<clock_Ty>& operator+=(cool::duration<clock_Ty> rhs) noexcept;
		inline cool::time_point<clock_Ty> operator-(cool::duration<clock_Ty> rhs) const noexcept;
		inline cool::time_point<clock_Ty> operator+(cool::duration<clock_Ty> rhs) const noexcept;
		inline cool::duration<clock_Ty> operator-(cool::time_point<clock_Ty> rhs) const noexcept;

		inline bool operator==(cool::time_point<clock_Ty> rhs) const noexcept;
		inline bool operator!=(cool::time_point<clock_Ty> rhs) const noexcept;
		inline bool operator<=(cool::time_point<clock_Ty> rhs) const noexcept;
		inline bool operator>=(cool::time_point<clock_Ty> rhs) const noexcept;
		inline bool operator<(cool::time_point<clock_Ty> rhs) const noexcept;
		inline bool operator>(cool::time_point<clock_Ty> rhs) const noexcept;

	private:

		std::chrono::time_point<clock_Ty> m_time;
	};


	template <class clock_Ty, std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1, class num_Ty>
	inline constexpr cool::duration<clock_Ty> make_duration(num_Ty time_duration, cool::duration_unit unit = cool::duration_unit::s) noexcept;

	template <class num_Ty, class clock_Ty>
	inline constexpr cool::duration<clock_Ty> operator*(num_Ty lhs, cool::duration<clock_Ty> rhs) noexcept;

	template <class clock_Ty> inline constexpr cool::duration<clock_Ty> operator-(cool::duration<clock_Ty> arg) noexcept;

	template <class clock_Ty> class duration
	{

	public:

		using clock_type = clock_Ty;
		using rep = typename clock_Ty::duration::rep;
		using period = typename clock_Ty::duration::period;

		class ratio
		{

		public:

			std::intmax_t num;
			std::intmax_t den;

			ratio() noexcept = default;
			inline constexpr ratio(std::intmax_t _num, std::intmax_t _den) noexcept : num(_num), den(_den) {}
			constexpr ratio(const typename cool::duration<clock_Ty>::ratio& rhs) noexcept = default;
			typename cool::duration<clock_Ty>::ratio& operator=(const typename cool::duration<clock_Ty>::ratio& rhs) noexcept = default;
			~ratio() = default;
		};

		duration() noexcept = default;
		inline constexpr duration(typename clock_Ty::duration::rep ticks) noexcept : m_ticks(ticks) {}
		constexpr duration(const cool::duration<clock_Ty>& rhs) noexcept = default;
		cool::duration<clock_Ty>& operator=(const cool::duration<clock_Ty>& rhs) noexcept = default;
		~duration() = default;


		template <class num_Ty, class unit_mod_ratio_Ty = std::ratio<1, 1>>
		inline constexpr num_Ty get(cool::duration_unit unit = cool::duration_unit::s) const noexcept;

		inline constexpr typename clock_Ty::duration::rep get_ticks() const noexcept;


		inline cool::duration<clock_Ty>& operator-=(cool::duration<clock_Ty> rhs) noexcept;
		inline cool::duration<clock_Ty>& operator+=(cool::duration<clock_Ty> rhs) noexcept;
		template <class num_Ty> inline cool::duration<clock_Ty>& operator/=(num_Ty rhs) noexcept;
		inline cool::duration<clock_Ty>& operator%=(cool::duration<clock_Ty> rhs) noexcept;
		inline constexpr cool::duration<clock_Ty> operator-(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr cool::duration<clock_Ty> operator+(cool::duration<clock_Ty> rhs) const noexcept;
		template <class num_Ty> inline constexpr cool::duration<clock_Ty> operator/(num_Ty rhs) const noexcept;
		inline constexpr cool::duration<clock_Ty> operator%(cool::duration<clock_Ty> rhs) const noexcept;

		inline constexpr bool operator==(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator!=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator<=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator>=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator<(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator>(cool::duration<clock_Ty> rhs) const noexcept;


		template <class num_Ty, class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr num_Ty duration_per_tick(cool::duration_unit unit) noexcept;

		template <class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr typename cool::duration<clock_Ty>::ratio duration_per_tick_ratio(cool::duration_unit unit) noexcept;

		template <class num_Ty, class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr num_Ty tick_per_duration(cool::duration_unit unit) noexcept;

		template <class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr typename cool::duration<clock_Ty>::ratio tick_per_duration_ratio(cool::duration_unit unit) noexcept;

		template <class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr bool valid_unit(cool::duration_unit unit) noexcept;

		template <class num_Ty, class unit_mod_ratio_Ty = std::ratio<1, 1>>
		static inline constexpr num_Ty max(cool::duration_unit unit = cool::duration_unit::s) noexcept;

		static inline constexpr typename clock_Ty::duration::rep max_ticks() noexcept;

	private:

		template <std::intmax_t lhs, std::intmax_t rhs> static constexpr inline bool _can_multiply() noexcept;

		template <class num_Ty, class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
		static inline constexpr num_Ty _duration_per_tick() noexcept;

		template <class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
		static inline constexpr typename cool::duration<clock_Ty>::ratio _duration_per_tick_ratio() noexcept;

		template <class num_Ty, class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
		static inline constexpr num_Ty _tick_per_duration() noexcept;

		template <class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
		static inline constexpr typename cool::duration<clock_Ty>::ratio _tick_per_duration_ratio() noexcept;

		typename clock_Ty::duration::rep m_ticks;
	};
}


// detail

template <class clock_Ty>
inline cool::time_point<clock_Ty>& cool::time_point<clock_Ty>::now() noexcept
{
	m_time = clock_Ty::now();
	return *this;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty>& cool::time_point<clock_Ty>::operator-=(cool::duration<clock_Ty> rhs) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	m_time -= std::chrono::duration<typename clock_Ty::duration::rep, _clock_period>(rhs.get_ticks());
	return *this;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty>& cool::time_point<clock_Ty>::operator+=(cool::duration<clock_Ty> rhs) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	m_time += std::chrono::duration<typename clock_Ty::duration::rep, _clock_period>(rhs.get_ticks());
	return *this;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty> cool::time_point<clock_Ty>::operator-(cool::duration<clock_Ty> rhs) const noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	cool::time_point<clock_Ty> ret(cool::no_init);
	ret.m_time = m_time - std::chrono::duration<typename clock_Ty::duration::rep, _clock_period>(rhs.get_ticks());
	return ret;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty> cool::time_point<clock_Ty>::operator+(cool::duration<clock_Ty> rhs) const noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	cool::time_point<clock_Ty> ret(cool::no_init);
	ret.m_time = m_time + std::chrono::duration<typename clock_Ty::duration::rep, _clock_period>(rhs.get_ticks());
	return ret;
}

template <class clock_Ty>
inline cool::duration<clock_Ty> cool::time_point<clock_Ty>::operator-(cool::time_point<clock_Ty> rhs) const noexcept
{
	return cool::duration<clock_Ty>((m_time - rhs.m_time).count());
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator==(cool::time_point<clock_Ty> rhs) const noexcept
{
	return m_time == rhs.m_time;
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator!=(cool::time_point<clock_Ty> rhs) const noexcept
{
	return !(m_time == rhs.m_time);
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator<=(cool::time_point<clock_Ty> rhs) const noexcept
{
	return m_time <= rhs.m_time;
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator>=(cool::time_point<clock_Ty> rhs) const noexcept
{
	return m_time >= rhs.m_time;
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator<(cool::time_point<clock_Ty> rhs) const noexcept
{
	return m_time < rhs.m_time;
}

template <class clock_Ty>
inline bool cool::time_point<clock_Ty>::operator>(cool::time_point<clock_Ty> rhs) const noexcept
{
	return m_time > rhs.m_time;
}


template <class clock_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den, class num_Ty>
inline constexpr cool::duration<clock_Ty> cool::make_duration(num_Ty time_duration, cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_num > 0 && unit_mod_den > 0, "cool::make_duration requirement : unit_mod_num > 0 and unit_mod_den > 0");

	if (std::is_integral<num_Ty>::value)
	{
		typename cool::duration<clock_Ty>::ratio _ratio = cool::duration<clock_Ty>::template tick_per_duration_ratio<unit_mod_num, unit_mod_den>(unit);
		return cool::duration<clock_Ty>(static_cast<typename clock_Ty::duration::rep>(
			(_ratio.num * static_cast<std::intmax_t>(time_duration)) / _ratio.den));
	}
	else
	{
		return cool::duration<clock_Ty>(static_cast<typename clock_Ty::duration::rep>(
			static_cast<num_Ty>(cool::duration<clock_Ty>::template tick_per_duration<num_Ty, unit_mod_num, unit_mod_den>(unit))
			* time_duration));
	}
}

template <class num_Ty, class clock_Ty>
inline constexpr cool::duration<clock_Ty> cool::operator*(num_Ty lhs, cool::duration<clock_Ty> rhs) noexcept
{
	if (std::is_integral<num_Ty>::value)
	{
		return cool::duration<clock_Ty>(lhs * rhs.get_ticks());
	}
	else
	{
		return cool::duration<clock_Ty>(static_cast<typename clock_Ty::duration::rep>(lhs * static_cast<num_Ty>(rhs.get_ticks())));
	}
}

template <class clock_Ty> inline constexpr cool::duration<clock_Ty> cool::operator-(cool::duration<clock_Ty> arg) noexcept
{
	return cool::duration<clock_Ty>(-arg.get_ticks());
}

template <class clock_Ty> template <class num_Ty, class unit_mod_ratio_Ty>
inline constexpr num_Ty cool::duration<clock_Ty>::get(cool::duration_unit unit) const noexcept
{
	if (std::is_integral<num_Ty>::value)
	{
		typename cool::duration<clock_Ty>::ratio _ratio = duration_per_tick_ratio<unit_mod_ratio_Ty>(unit);
		return static_cast<num_Ty>((_ratio.num * m_ticks) / _ratio.den);
	}
	else
	{
		return duration_per_tick<num_Ty, unit_mod_ratio_Ty>(unit) * static_cast<num_Ty>(m_ticks);
	}
}

template <class clock_Ty>
inline constexpr typename clock_Ty::duration::rep cool::duration<clock_Ty>::get_ticks() const noexcept
{
	return m_ticks;
}

template <class clock_Ty>
inline cool::duration<clock_Ty>& cool::duration<clock_Ty>::operator-=(cool::duration<clock_Ty> rhs) noexcept
{
	m_ticks -= rhs.m_ticks;
	return *this;
}

template <class clock_Ty>
inline cool::duration<clock_Ty>& cool::duration<clock_Ty>::operator+=(cool::duration<clock_Ty> rhs) noexcept
{
	m_ticks += rhs.m_ticks;
	return *this;
}

template <class clock_Ty> template <class num_Ty>
inline cool::duration<clock_Ty>& cool::duration<clock_Ty>::operator/=(num_Ty rhs) noexcept
{
	if (std::is_integral<num_Ty>::value)
	{
		m_ticks /= rhs;
	}
	else
	{
		m_ticks = static_cast<typename clock_Ty::duration::rep>(static_cast<num_Ty>(m_ticks) / rhs);
	}

	return *this;
}

template <class clock_Ty>
inline cool::duration<clock_Ty>& cool::duration<clock_Ty>::operator%=(cool::duration<clock_Ty> rhs) noexcept
{
	m_ticks %= rhs.m_ticks;
	return *this;
}

template <class clock_Ty>
inline constexpr cool::duration<clock_Ty> cool::duration<clock_Ty>::operator-(cool::duration<clock_Ty> rhs) const noexcept
{
	return cool::duration<clock_Ty>(m_ticks - rhs.m_ticks);
}

template <class clock_Ty>
inline constexpr cool::duration<clock_Ty> cool::duration<clock_Ty>::operator+(cool::duration<clock_Ty> rhs) const noexcept
{
	return cool::duration<clock_Ty>(m_ticks + rhs.m_ticks);
}

template <class clock_Ty> template <class num_Ty>
inline constexpr cool::duration<clock_Ty> cool::duration<clock_Ty>::operator/(num_Ty rhs) const noexcept
{
	if (std::is_integral<num_Ty>::value)
	{
		return cool::duration<clock_Ty>(m_ticks / rhs);
	}
	else
	{
		return cool::duration<clock_Ty>(static_cast<typename clock_Ty::duration::rep>(static_cast<num_Ty>(m_ticks) / rhs));
	}
}

template <class clock_Ty>
inline constexpr cool::duration<clock_Ty> cool::duration<clock_Ty>::operator%(cool::duration<clock_Ty> rhs) const noexcept
{
	return cool::duration<clock_Ty>(m_ticks % rhs.m_ticks);
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator==(cool::duration<clock_Ty> rhs) const noexcept
{
	return m_ticks == rhs.m_ticks;
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator!=(cool::duration<clock_Ty> rhs) const noexcept
{
	return !(m_ticks == rhs.m_ticks);
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator<=(cool::duration<clock_Ty> rhs) const noexcept
{
	return m_ticks <= rhs.m_ticks;
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator>=(cool::duration<clock_Ty> rhs) const noexcept
{
	return m_ticks >= rhs.m_ticks;
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator<(cool::duration<clock_Ty> rhs) const noexcept
{
	return m_ticks < rhs.m_ticks;
}

template <class clock_Ty>
inline constexpr bool cool::duration<clock_Ty>::operator>(cool::duration<clock_Ty> rhs) const noexcept
{
	return m_ticks > rhs.m_ticks;
}

template <class clock_Ty> template <class num_Ty, class unit_mod_ratio_Ty>
inline constexpr num_Ty cool::duration<clock_Ty>::duration_per_tick(cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_ratio_Ty::num > 0 && unit_mod_ratio_Ty::den > 0,
		"cool::duration<...>::duration_per_tick requirement : unit_mod_ratio::num > 0 and unit_mod_ratio::den > 0");

	switch (unit)
	{
	case cool::duration_unit::s: return _duration_per_tick<num_Ty, std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ms: return _duration_per_tick<num_Ty, std::ratio<1, 1000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::us: return _duration_per_tick<num_Ty, std::ratio<1, 1000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ns: return _duration_per_tick<num_Ty, std::ratio<1, 1000000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::min: return _duration_per_tick<num_Ty, std::ratio<60, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::hour: return _duration_per_tick<num_Ty, std::ratio<3600, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::day: return _duration_per_tick<num_Ty, std::ratio<86400, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::tick: return static_cast<num_Ty>(
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::den)
		/ static_cast<num_Ty>(std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::num);

#ifdef COOL_DURATION_CUSTOM_UNIT0
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT0:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT0_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT1:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT1_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT1

#ifdef COOL_DURATION_CUSTOM_UNIT2
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT2:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT2_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT3:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT3_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT4:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT4_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT5:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT5_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT6:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT6_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT7:
		return _duration_per_tick<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT7_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT7

	default: return _duration_per_tick<num_Ty, std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	}
}

template <class clock_Ty> template <class unit_mod_ratio_Ty>
inline constexpr typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::duration_per_tick_ratio(cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_ratio_Ty::num > 0 && unit_mod_ratio_Ty::den > 0,
		"cool::duration<...>::duration_per_tick requirement : unit_mod_ratio::num > 0 and unit_mod_ratio::den > 0");

	switch (unit)
	{
	case cool::duration_unit::s: return _duration_per_tick_ratio<std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ms: return _duration_per_tick_ratio<std::ratio<1, 1000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::us: return _duration_per_tick_ratio<std::ratio<1, 1000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ns: return _duration_per_tick_ratio<std::ratio<1, 1000000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::min: return _duration_per_tick_ratio<std::ratio<60, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::hour: return _duration_per_tick_ratio<std::ratio<3600, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::day: return _duration_per_tick_ratio<std::ratio<86400, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::tick: return typename cool::duration<clock_Ty>::ratio(
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::den,
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::num);

#ifdef COOL_DURATION_CUSTOM_UNIT0
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT0:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT0_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT1:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT1_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT1

#ifdef COOL_DURATION_CUSTOM_UNIT2
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT2:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT2_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT3:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT3_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT4:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT4_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT5:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT5_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT6:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT6_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT7:
		return _duration_per_tick_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT7_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT7

	default: return _duration_per_tick_ratio<std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	}
}

template <class clock_Ty> template <class num_Ty, class unit_mod_ratio_Ty>
inline constexpr num_Ty cool::duration<clock_Ty>::tick_per_duration(cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_ratio_Ty::num > 0 && unit_mod_ratio_Ty::den > 0,
		"cool::duration<...>::duration_per_tick requirement : unit_mod_ratio::num > 0 and unit_mod_ratio::den > 0");

	switch (unit)
	{
	case cool::duration_unit::s: return _tick_per_duration<num_Ty, std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ms: return _tick_per_duration<num_Ty, std::ratio<1, 1000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::us: return _tick_per_duration<num_Ty, std::ratio<1, 1000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ns: return _tick_per_duration<num_Ty, std::ratio<1, 1000000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::min: return _tick_per_duration<num_Ty, std::ratio<60, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::hour: return _tick_per_duration<num_Ty, std::ratio<3600, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::day: return _tick_per_duration<num_Ty, std::ratio<86400, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::tick: return static_cast<num_Ty>(
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::num)
		/ static_cast<num_Ty>(std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::den);

#ifdef COOL_DURATION_CUSTOM_UNIT0
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT0:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT0_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT1:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT1_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT1

#ifdef COOL_DURATION_CUSTOM_UNIT2
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT2:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT2_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT3:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT3_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT4:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT4_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT5:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT5_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT6:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT6_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT7:
		return _tick_per_duration<num_Ty, std::ratio<COOL_DURATION_CUSTOM_UNIT7_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT7

	default: return _tick_per_duration<num_Ty, std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	}
}

template <class clock_Ty> template <class unit_mod_ratio_Ty>
inline constexpr typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::tick_per_duration_ratio(cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_ratio_Ty::num > 0 && unit_mod_ratio_Ty::den > 0,
		"cool::duration<...>::duration_per_tick requirement : unit_mod_ratio::num > 0 and unit_mod_ratio::den > 0");

	switch (unit)
	{
	case cool::duration_unit::s: return _tick_per_duration_ratio<std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ms: return _tick_per_duration_ratio<std::ratio<1, 1000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::us: return _tick_per_duration_ratio<std::ratio<1, 1000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::ns: return _tick_per_duration_ratio<std::ratio<1, 1000000000>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::min: return _tick_per_duration_ratio<std::ratio<60, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::hour: return _tick_per_duration_ratio<std::ratio<3600, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::day: return _tick_per_duration_ratio<std::ratio<86400, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	case cool::duration_unit::tick: return typename cool::duration<clock_Ty>::ratio(
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::num,
		std::ratio<unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>::den);

#ifdef COOL_DURATION_CUSTOM_UNIT0
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT0:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT0_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT0

#ifdef COOL_DURATION_CUSTOM_UNIT1
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT1:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT1_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT1

#ifdef COOL_DURATION_CUSTOM_UNIT2
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT2:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT2_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT2

#ifdef COOL_DURATION_CUSTOM_UNIT3
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT3:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT3_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT3

#ifdef COOL_DURATION_CUSTOM_UNIT4
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT4:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT4_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT4

#ifdef COOL_DURATION_CUSTOM_UNIT5
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT5:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT5_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT5

#ifdef COOL_DURATION_CUSTOM_UNIT6
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT6:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT6_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT6

#ifdef COOL_DURATION_CUSTOM_UNIT7
	case cool::duration_unit::COOL_DURATION_CUSTOM_UNIT7:
		return _tick_per_duration_ratio<std::ratio<COOL_DURATION_CUSTOM_UNIT7_RATIO>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
#endif // COOL_DURATION_CUSTOM_UNIT7

	default: return _tick_per_duration_ratio<std::ratio<1, 1>, unit_mod_ratio_Ty::num, unit_mod_ratio_Ty::den>();
	}
}

template <class clock_Ty> template <class unit_mod_ratio_Ty>
inline constexpr bool cool::duration<clock_Ty>::valid_unit(cool::duration_unit unit) noexcept
{
	static_assert(unit_mod_ratio_Ty::num > 0 && unit_mod_ratio_Ty::den > 0,
		"cool::duration<...>::duration_per_tick requirement : unit_mod_ratio::num > 0 and unit_mod_ratio::den > 0");

	return tick_per_duration_ratio<unit_mod_ratio_Ty>(unit).num != 0;
}

template <class clock_Ty> template <class num_Ty, class unit_mod_ratio_Ty>
inline constexpr num_Ty cool::duration<clock_Ty>::max(cool::duration_unit unit) noexcept
{
	typename cool::duration<clock_Ty>::ratio _ratio = duration_per_tick_ratio<unit_mod_ratio_Ty>(unit);

	if (_ratio.num != 0)
	{
		if (std::is_integral<num_Ty>::value)
		{
			constexpr bool duration_rep_greater = std::numeric_limits<typename clock_Ty::duration::rep>::max() >= std::numeric_limits<num_Ty>::max();

			if (duration_rep_greater)
			{
				typename clock_Ty::duration::rep a = std::numeric_limits<typename clock_Ty::duration::rep>::max() / _ratio.den;
				typename clock_Ty::duration::rep b = static_cast<typename clock_Ty::duration::rep>(std::numeric_limits<num_Ty>::max());
				return (a < b) ? static_cast<num_Ty>(a) : static_cast<num_Ty>(b);
			}
			else
			{
				return static_cast<num_Ty>(std::numeric_limits<typename clock_Ty::duration::rep>::max() / _ratio.den);
			}
		}
		else
		{
			num_Ty a = duration_per_tick<num_Ty, unit_mod_ratio_Ty>(unit) * static_cast<num_Ty>(std::numeric_limits<typename clock_Ty::duration::rep>::max());
			num_Ty b = std::numeric_limits<num_Ty>::max();
			return (a < b) ? a : b;
		}
	}
	else
	{
		return static_cast<num_Ty>(0);
	}
}

template <class clock_Ty>
inline constexpr typename clock_Ty::duration::rep cool::duration<clock_Ty>::max_ticks() noexcept
{
	return std::numeric_limits<typename clock_Ty::duration::rep>::max();
}

template <class clock_Ty> template <std::intmax_t lhs, std::intmax_t rhs>
inline constexpr bool cool::duration<clock_Ty>::_can_multiply() noexcept
{
	return lhs <= std::numeric_limits<std::intmax_t>::max() / rhs;
}

template <class clock_Ty> template <class num_Ty, class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline constexpr num_Ty cool::duration<clock_Ty>::_duration_per_tick() noexcept
{
	constexpr cool::duration<clock_Ty>::ratio _ratio_inv = _tick_per_duration_ratio<duration_unit_ratio_Ty, unit_mod_num, unit_mod_den>();
	return (_ratio_inv.num != 0) ?
		static_cast<num_Ty>(_ratio_inv.den) / static_cast<num_Ty>(_ratio_inv.num)
		: static_cast<num_Ty>(0);
}

template <class clock_Ty> template <class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline constexpr typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::_duration_per_tick_ratio() noexcept
{
	constexpr cool::duration<clock_Ty>::ratio _ratio_inv = _tick_per_duration_ratio<duration_unit_ratio_Ty, unit_mod_num, unit_mod_den>();
	return (_ratio_inv.num != 0) ?
		typename cool::duration<clock_Ty>::ratio(_ratio_inv.den, _ratio_inv.num)
		: typename cool::duration<clock_Ty>::ratio(0, 1);
}

template <class clock_Ty> template <class num_Ty, class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline constexpr num_Ty cool::duration<clock_Ty>::_tick_per_duration() noexcept
{
	constexpr cool::duration<clock_Ty>::ratio _ratio = _tick_per_duration_ratio<duration_unit_ratio_Ty, unit_mod_num, unit_mod_den>();
	return (_ratio.num != 0) ?
		static_cast<num_Ty>(_ratio.num) / static_cast<num_Ty>(_ratio.den)
		: static_cast<num_Ty>(0);
}

template <class clock_Ty> template <class duration_unit_ratio_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline constexpr typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::_tick_per_duration_ratio() noexcept
{
	constexpr bool _no_u_overflow = _can_multiply<duration_unit_ratio_Ty::den, clock_Ty::duration::period::num>()
		&& _can_multiply<duration_unit_ratio_Ty::num, clock_Ty::duration::period::den>();
	constexpr std::intmax_t unit_num_checked = _no_u_overflow ? duration_unit_ratio_Ty::num : 1;
	constexpr std::intmax_t unit_den_checked = _no_u_overflow ? duration_unit_ratio_Ty::den : 1;
	using _ratio_u = std::ratio_multiply<std::ratio<unit_den_checked, unit_num_checked>, typename clock_Ty::duration::period>;

	using unit_mod_simpl = std::ratio<unit_mod_num, unit_mod_den>;
	constexpr bool _no_mod_overflow = _can_multiply<unit_mod_simpl::den, _ratio_u::num>()
		&& _can_multiply<unit_mod_simpl::num, _ratio_u::den>();
	constexpr std::intmax_t unit_mod_num_checked = _no_mod_overflow ? unit_mod_num : 1;
	constexpr std::intmax_t unit_mod_den_checked = _no_mod_overflow ? unit_mod_den : 1;
	using _ratio_u_modded = std::ratio_multiply<std::ratio<unit_mod_den_checked, unit_mod_num_checked>, _ratio_u>;

	return (_no_u_overflow & _no_mod_overflow) ?
		typename cool::duration<clock_Ty>::ratio(_ratio_u_modded::den, _ratio_u_modded::num) :
		typename cool::duration<clock_Ty>::ratio(0, 1);
}

#endif // xCOOL_CHRONO_HPP


// cool_chrono.hpp
// 
// This is free software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software for any purpose and by any means.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY IN CONNECTION WITH THE SOFTWARE.