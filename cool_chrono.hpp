// cool_chrono.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_CHRONO_HPP
#define xCOOL_CHRONO_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <type_traits>


namespace cool
{
#ifndef xCOOL_DURATION_UNIT_ENUM
#define xCOOL_DURATION_UNIT_ENUM
	enum class duration_unit : std::size_t
	{
		s = 0,
		ms = 1,
		us = 2,
		ns = 3,

		min = 4,
		hour = 5
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

	template <class clock_Ty> class time_point;
	template <class clock_Ty> class duration;

	template <class clock_Ty> class time_point
	{

	public:

		using clock_type = clock_Ty;
		using rep = typename std::chrono::time_point<clock_Ty>::rep;
		using period = typename std::chrono::time_point<clock_Ty>::period;

		time_point() noexcept = default; // does not set time_point to now
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

		template <class clock_Ty2> friend class duration;
		std::chrono::time_point<clock_Ty> m_time;
	};


	template <class clock_Ty, std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1, class num_Ty>
	inline constexpr cool::duration<clock_Ty> make_duration(num_Ty time_duration, cool::duration_unit unit = cool::duration_unit::s) noexcept;

	template <class int_Ty, class clock_Ty>
	inline constexpr cool::duration<clock_Ty> operator*(int_Ty lhs, cool::duration<clock_Ty> rhs) noexcept;

	template <class clock_Ty> inline constexpr cool::duration<clock_Ty> operator-(cool::duration<clock_Ty> arg) noexcept;

	template <class clock_Ty> class duration
	{

	public:

		using clock_type = clock_Ty;
		using rep = typename std::chrono::time_point<clock_Ty>::rep;
		using period = typename std::chrono::time_point<clock_Ty>::period;

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
		inline constexpr duration(typename std::chrono::time_point<clock_Ty>::rep ticks) noexcept : m_ticks(ticks) {}
		constexpr duration(const cool::duration<clock_Ty>& rhs) noexcept = default;
		cool::duration<clock_Ty>& operator=(const cool::duration<clock_Ty>& rhs) noexcept = default;
		~duration() = default;


		inline constexpr typename std::chrono::time_point<clock_Ty>::rep get_ticks() const noexcept;

		template <class num_Ty, std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		inline constexpr num_Ty get(cool::duration_unit unit = cool::duration_unit::s) const noexcept;


		inline cool::duration<clock_Ty>& operator-=(cool::duration<clock_Ty> rhs) noexcept;
		inline cool::duration<clock_Ty>& operator+=(cool::duration<clock_Ty> rhs) noexcept;
		template <class int_Ty> inline cool::duration<clock_Ty>& operator/=(int_Ty rhs) noexcept;
		inline cool::duration<clock_Ty>& operator%=(cool::duration<clock_Ty> rhs) noexcept;
		inline constexpr cool::duration<clock_Ty> operator-(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr cool::duration<clock_Ty> operator+(cool::duration<clock_Ty> rhs) const noexcept;
		template <class int_Ty> inline constexpr cool::duration<clock_Ty> operator/(int_Ty rhs) const noexcept;
		inline constexpr cool::duration<clock_Ty> operator%(cool::duration<clock_Ty> rhs) const noexcept;

		inline constexpr bool operator==(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator!=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator<=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator>=(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator<(cool::duration<clock_Ty> rhs) const noexcept;
		inline constexpr bool operator>(cool::duration<clock_Ty> rhs) const noexcept;


		template <class num_Ty, std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline num_Ty duration_per_tick(cool::duration_unit unit) noexcept;

		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline typename cool::duration<clock_Ty>::ratio duration_per_tick_ratio(cool::duration_unit unit) noexcept;

		template <class num_Ty, std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline num_Ty tick_per_duration(cool::duration_unit unit) noexcept;

		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline typename cool::duration<clock_Ty>::ratio tick_per_duration_ratio(cool::duration_unit unit) noexcept;

	private:

		template <class clock_Ty2> friend class time;
		typename std::chrono::time_point<clock_Ty>::rep m_ticks;
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

	m_time -= std::chrono::duration<typename std::chrono::time_point<clock_Ty>::rep, _clock_period>(rhs.get_ticks());
	return *this;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty>& cool::time_point<clock_Ty>::operator+=(cool::duration<clock_Ty> rhs) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	m_time += std::chrono::duration<typename std::chrono::time_point<clock_Ty>::rep, _clock_period>(rhs.get_ticks());
	return *this;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty> cool::time_point<clock_Ty>::operator-(cool::duration<clock_Ty> rhs) const noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	cool::time_point<clock_Ty> ret(cool::no_init);
	ret.m_time = m_time - std::chrono::duration<typename std::chrono::time_point<clock_Ty>::rep, _clock_period>(rhs.get_ticks());
	return ret;
}

template <class clock_Ty>
inline cool::time_point<clock_Ty> cool::time_point<clock_Ty>::operator+(cool::duration<clock_Ty> rhs) const noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	cool::time_point<clock_Ty> ret(cool::no_init);
	ret.m_time = m_time + std::chrono::duration<typename std::chrono::time_point<clock_Ty>::rep, _clock_period>(rhs.get_ticks());
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
	if (std::is_integral<num_Ty>::value)
	{
		typename cool::duration<clock_Ty>::ratio _ratio = cool::duration<clock_Ty>::template tick_per_duration_ratio<unit_mod_num, unit_mod_den>(unit);
		return cool::duration<clock_Ty>(static_cast<typename std::chrono::time_point<clock_Ty>::rep>(
			(_ratio.num * static_cast<std::intmax_t>(time_duration)) / _ratio.den));
	}
	else
	{
		return cool::duration<clock_Ty>(static_cast<typename std::chrono::time_point<clock_Ty>::rep>(
			static_cast<num_Ty>(cool::duration<clock_Ty>::template tick_per_duration<num_Ty, unit_mod_num, unit_mod_den>(unit))
			* time_duration));
	}
}

template <class int_Ty, class clock_Ty>
inline constexpr cool::duration<clock_Ty> cool::operator*(int_Ty lhs, cool::duration<clock_Ty> rhs) noexcept
{
	static_assert(std::is_integral<int_Ty>::value, "operator/(lhs, cool::duration<...> rhs) : left hand side must be integral type");

	return cool::duration<clock_Ty>(lhs * rhs.get_ticks());
}

template <class clock_Ty> inline constexpr cool::duration<clock_Ty> cool::operator-(cool::duration<clock_Ty> arg) noexcept
{
	return cool::duration<clock_Ty>(-arg.get_ticks());
}

template <class clock_Ty>
inline constexpr typename std::chrono::time_point<clock_Ty>::rep cool::duration<clock_Ty>::get_ticks() const noexcept
{
	return m_ticks;
}

template <class clock_Ty> template <class num_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline constexpr num_Ty cool::duration<clock_Ty>::get(cool::duration_unit unit) const noexcept
{
	if (std::is_integral<num_Ty>::value)
	{
		typename cool::duration<clock_Ty>::ratio _ratio = duration_per_tick_ratio<unit_mod_num, unit_mod_den>(unit);
		return static_cast<num_Ty>((_ratio.num * m_ticks) / _ratio.den);
	}
	else
	{
		return duration_per_tick<num_Ty, unit_mod_num, unit_mod_den>(unit)
			* static_cast<num_Ty>(m_ticks);
	}
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

template <class clock_Ty> template <class int_Ty>
inline cool::duration<clock_Ty>& cool::duration<clock_Ty>::operator/=(int_Ty rhs) noexcept
{
	static_assert(std::is_integral<int_Ty>::value, "cool::duration<...>::operator/= : right hand side must be integral type");

	m_ticks /= rhs;
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

template <class clock_Ty> template <class int_Ty>
inline constexpr cool::duration<clock_Ty> cool::duration<clock_Ty>::operator/(int_Ty rhs) const noexcept
{
	static_assert(std::is_integral<int_Ty>::value, "cool::duration<...>::operator/ : right hand side must be integral type");

	return cool::duration<clock_Ty>(m_ticks / rhs);
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

template <class clock_Ty> template <class num_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline num_Ty cool::duration<clock_Ty>::duration_per_tick(cool::duration_unit unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration_unit::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<num_Ty>(_ratio_modded::num) / static_cast<num_Ty>(_ratio_modded::den);
		break;
	}

	case cool::duration_unit::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return static_cast<num_Ty>(_ratio_ms_modded::num) / static_cast<num_Ty>(_ratio_ms_modded::den);
		break;
	}

	case cool::duration_unit::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return static_cast<num_Ty>(_ratio_us_modded::num) / static_cast<num_Ty>(_ratio_us_modded::den);
		break;
	}

	case cool::duration_unit::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return static_cast<num_Ty>(_ratio_ns_modded::num) / static_cast<num_Ty>(_ratio_ns_modded::den);
		break;
	}

	case cool::duration_unit::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return static_cast<num_Ty>(_ratio_min_modded::num) / static_cast<num_Ty>(_ratio_min_modded::den);
		break;
	}

	case cool::duration_unit::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return static_cast<num_Ty>(_ratio_hour_modded::num) / static_cast<num_Ty>(_ratio_hour_modded::den);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<num_Ty>(_ratio_modded::num) / static_cast<num_Ty>(_ratio_modded::den);
		break;
	}

	}
}

template <class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::duration_per_tick_ratio(cool::duration_unit unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration_unit::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_modded::num, _ratio_modded::den);
		break;
	}

	case cool::duration_unit::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_ms_modded::num, _ratio_ms_modded::den);
		break;
	}

	case cool::duration_unit::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_us_modded::num, _ratio_us_modded::den);
		break;
	}

	case cool::duration_unit::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_ns_modded::num, _ratio_ns_modded::den);
		break;
	}

	case cool::duration_unit::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_min_modded::num, _ratio_min_modded::den);
		break;
	}

	case cool::duration_unit::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_hour_modded::num, _ratio_hour_modded::den);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_modded::num, _ratio_modded::den);
		break;
	}

	}
}

template <class clock_Ty> template <class num_Ty, std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline num_Ty cool::duration<clock_Ty>::tick_per_duration(cool::duration_unit unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration_unit::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<num_Ty>(_ratio_modded::den) / static_cast<num_Ty>(_ratio_modded::num);
		break;
	}

	case cool::duration_unit::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return static_cast<num_Ty>(_ratio_ms_modded::den) / static_cast<num_Ty>(_ratio_ms_modded::num);
		break;
	}

	case cool::duration_unit::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return static_cast<num_Ty>(_ratio_us_modded::den) / static_cast<num_Ty>(_ratio_us_modded::num);
		break;
	}

	case cool::duration_unit::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return static_cast<num_Ty>(_ratio_ns_modded::den) / static_cast<num_Ty>(_ratio_ns_modded::num);
		break;
	}

	case cool::duration_unit::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return static_cast<num_Ty>(_ratio_min_modded::den) / static_cast<num_Ty>(_ratio_min_modded::num);
		break;
	}

	case cool::duration_unit::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return static_cast<num_Ty>(_ratio_hour_modded::den) / static_cast<num_Ty>(_ratio_hour_modded::num);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<num_Ty>(_ratio_modded::den) / static_cast<num_Ty>(_ratio_modded::num);
		break;
	}

	}
}

template <class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline typename cool::duration<clock_Ty>::ratio cool::duration<clock_Ty>::tick_per_duration_ratio(cool::duration_unit unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration_unit::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_modded::den, _ratio_modded::num);
		break;
	}

	case cool::duration_unit::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_ms_modded::den, _ratio_ms_modded::num);
		break;
	}

	case cool::duration_unit::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_us_modded::den, _ratio_us_modded::num);
		break;
	}

	case cool::duration_unit::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_ns_modded::den, _ratio_ns_modded::num);
		break;
	}

	case cool::duration_unit::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_min_modded::den, _ratio_min_modded::num);
		break;
	}

	case cool::duration_unit::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_hour_modded::den, _ratio_hour_modded::num);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return typename cool::duration<clock_Ty>::ratio(_ratio_modded::den, _ratio_modded::num);
		break;
	}

	}
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