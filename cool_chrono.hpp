// cool_chrono.hpp
// License <http://unlicense.org/> (statement below at the end of the file)
// Feel free to Ctrl+F/replace the silly namespace and rename the file if convenient

#ifndef xCOOL_CHRONO_HPP
#define xCOOL_CHRONO_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>


namespace cool
{
#ifndef xCOOL_DURATION_ENUM
#define xCOOL_DURATION_ENUM
	enum class duration : std::size_t
	{
		s = 0,
		ms = 1,
		us = 2,
		ns = 3,

		min = 4,
		hour = 5
	};
#endif // xCOOL_DURATION_ENUM

#ifndef xCOOL_NO_INIT_ENUM
#define xCOOL_NO_INIT_ENUM
	enum no_init_t { no_init };
#endif // xCOOL_NO_INIT_ENUM

	template <class float_Ty, class int_Ty = unsigned int, class clock_Ty = std::chrono::steady_clock> class chrono
	{

	public:

		using duration_type = float_Ty;
		using count_type = int_Ty;
		using clock_type = clock_Ty;
		using tick_type = typename std::chrono::time_point<clock_Ty>::rep;

		inline chrono() noexcept : m_start(clock_Ty::now()), m_stop(m_start), m_total_ticks(0), m_count(0) {}
		inline chrono(cool::no_init_t) noexcept {}
		chrono(const cool::chrono<float_Ty, int_Ty, clock_Ty>& rhs) noexcept = default;
		cool::chrono<float_Ty, int_Ty, clock_Ty>& operator=(const cool::chrono<float_Ty, int_Ty, clock_Ty>&) noexcept = default;
		chrono(cool::chrono<float_Ty, int_Ty, clock_Ty>&&) noexcept = default;
		cool::chrono<float_Ty, int_Ty, clock_Ty>& operator=(cool::chrono<float_Ty, int_Ty, clock_Ty>&&) noexcept = default;
		~chrono() = default;

		// units
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline float_Ty duration_per_tick(cool::duration unit) noexcept;
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static constexpr inline float_Ty tick_per_duration(cool::duration unit) noexcept;

		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		static inline typename std::chrono::time_point<clock_Ty>::rep duration_to_ticks(
			float_Ty duration_value, cool::duration unit = cool::duration::s) noexcept;

		// now and future time point
		static inline std::chrono::time_point<clock_Ty> now() noexcept;
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		// caution if clock counter overflows, "now() < future_time_point(...)" might
		// return wrong results around that time.
		static inline std::chrono::time_point<clock_Ty> future_time_point(
			float_Ty delay, cool::duration unit = cool::duration::s) noexcept;

		// duration capture
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& start() noexcept;
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& stop() noexcept;
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		inline float_Ty get_duration(cool::duration unit = cool::duration::s) const noexcept;
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		inline float_Ty get_duration_until_now(cool::duration unit = cool::duration::s) const noexcept;
		inline typename std::chrono::time_point<clock_Ty>::rep get_ticks() const noexcept;
		inline typename std::chrono::time_point<clock_Ty>::rep get_ticks_until_now() const noexcept;

		// total duration
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& add_to_total_duration() noexcept;
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		inline float_Ty get_total_duration(cool::duration unit = cool::duration::s) const noexcept;
		inline typename std::chrono::time_point<clock_Ty>::rep get_total_ticks() const noexcept;
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& reset_total_duration() noexcept;

		// count
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& add_to_count() noexcept;
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& add_to_count(int_Ty n) noexcept;
		inline int_Ty get_count() const noexcept;
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& reset_count() noexcept;

		// average time
		template <std::intmax_t unit_mod_num = 1, std::intmax_t unit_mod_den = 1>
		inline float_Ty get_average_duration(cool::duration unit = cool::duration::s) const noexcept;

		// global reset
		inline cool::chrono<float_Ty, int_Ty, clock_Ty>& reset_all() noexcept;

	private:

		std::chrono::time_point<clock_Ty> m_start;
		std::chrono::time_point<clock_Ty> m_stop;
		typename std::chrono::time_point<clock_Ty>::rep m_total_ticks;
		int_Ty m_count;
	};
}


// detail

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::duration_per_tick(cool::duration unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<float_Ty>(_ratio_modded::num) / static_cast<float_Ty>(_ratio_modded::den);
		break;
	}

	case cool::duration::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return static_cast<float_Ty>(_ratio_ms_modded::num) / static_cast<float_Ty>(_ratio_ms_modded::den);
		break;
	}

	case cool::duration::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return static_cast<float_Ty>(_ratio_us_modded::num) / static_cast<float_Ty>(_ratio_us_modded::den);
		break;
	}

	case cool::duration::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return static_cast<float_Ty>(_ratio_ns_modded::num) / static_cast<float_Ty>(_ratio_ns_modded::den);
		break;
	}

	case cool::duration::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return static_cast<float_Ty>(_ratio_min_modded::num) / static_cast<float_Ty>(_ratio_min_modded::den);
		break;
	}

	case cool::duration::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return static_cast<float_Ty>(_ratio_hour_modded::num) / static_cast<float_Ty>(_ratio_hour_modded::den);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<float_Ty>(_ratio_modded::num) / static_cast<float_Ty>(_ratio_modded::den);
		break;
	}

	}
}

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
constexpr inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::tick_per_duration(cool::duration unit) noexcept
{
	using _clock_period = typename clock_Ty::duration::period;

	switch (unit)
	{

	case cool::duration::s:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<float_Ty>(_ratio_modded::den) / static_cast<float_Ty>(_ratio_modded::num);
		break;
	}

	case cool::duration::ms:
	{
		using _ratio_ms = std::ratio_multiply<std::ratio<1000, 1>, _clock_period>;
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ms>;
		return static_cast<float_Ty>(_ratio_ms_modded::den) / static_cast<float_Ty>(_ratio_ms_modded::num);
		break;
	}

	case cool::duration::us:
	{
		using _ratio_us = std::ratio_multiply<std::ratio<1000000, 1>, _clock_period>;
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_us>;
		return static_cast<float_Ty>(_ratio_us_modded::den) / static_cast<float_Ty>(_ratio_us_modded::num);
		break;
	}

	case cool::duration::ns:
	{
		using _ratio_ns = std::ratio_multiply<std::ratio<1000000000, 1>, _clock_period>;
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_ns>;
		return static_cast<float_Ty>(_ratio_ns_modded::den) / static_cast<float_Ty>(_ratio_ns_modded::num);
		break;
	}

	case cool::duration::min:
	{
		using _ratio_min = std::ratio_multiply<std::ratio<1, 60>, _clock_period>;
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_min>;
		return static_cast<float_Ty>(_ratio_min_modded::den) / static_cast<float_Ty>(_ratio_min_modded::num);
		break;
	}

	case cool::duration::hour:
	{
		using _ratio_hour = std::ratio_multiply<std::ratio<1, 3600>, _clock_period>;
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _ratio_hour>;
		return static_cast<float_Ty>(_ratio_hour_modded::den) / static_cast<float_Ty>(_ratio_hour_modded::num);
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio_multiply<std::ratio<unit_mod_den, unit_mod_num>, _clock_period>;
		return static_cast<float_Ty>(_ratio_modded::den) / static_cast<float_Ty>(_ratio_modded::num);
		break;
	}

	}
}


template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline typename std::chrono::time_point<clock_Ty>::rep cool::chrono<float_Ty, int_Ty, clock_Ty>::duration_to_ticks(
	float_Ty duration_value, cool::duration unit) noexcept
{
	switch (unit)
	{

	case cool::duration::s:
	{
		using _ratio_modded = std::ratio<unit_mod_num, unit_mod_den>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_modded>(duration_value)).count();
		break;
	}

	case cool::duration::ms:
	{
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000>>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_ms_modded>(duration_value)).count();
		break;
	}

	case cool::duration::us:
	{
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000000>>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_us_modded>(duration_value)).count();
		break;
	}

	case cool::duration::ns:
	{
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000000000>>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_ns_modded>(duration_value)).count();
		break;
	}

	case cool::duration::min:
	{
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<60, 1>>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_min_modded>(duration_value)).count();
		break;
	}

	case cool::duration::hour:
	{
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<3600, 1>>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_hour_modded>(duration_value)).count();
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio<unit_mod_num, unit_mod_num>;
		return std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_modded>(duration_value)).count();
		break;
	}

	}
}


template <class float_Ty, class int_Ty, class clock_Ty>
inline std::chrono::time_point<clock_Ty> cool::chrono<float_Ty, int_Ty, clock_Ty>::now() noexcept
{
	return clock_Ty::now();
}

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline std::chrono::time_point<clock_Ty> cool::chrono<float_Ty, int_Ty, clock_Ty>::future_time_point(float_Ty delay, cool::duration unit) noexcept
{
	std::chrono::time_point<clock_Ty> _time_point = clock_Ty::now();

	switch (unit)
	{

	case cool::duration::s:
	{
		using _ratio_modded = std::ratio<unit_mod_num, unit_mod_den>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_modded>(delay));
		break;
	}

	case cool::duration::ms:
	{
		using _ratio_ms_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000>>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_ms_modded>(delay));
		break;
	}

	case cool::duration::us:
	{
		using _ratio_us_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000000>>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_us_modded>(delay));
		break;
	}

	case cool::duration::ns:
	{
		using _ratio_ns_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<1, 1000000000>>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_ns_modded>(delay));
		break;
	}

	case cool::duration::min:
	{
		using _ratio_min_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<60, 1>>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_min_modded>(delay));
		break;
	}

	case cool::duration::hour:
	{
		using _ratio_hour_modded = std::ratio_multiply<std::ratio<unit_mod_num, unit_mod_den>, std::ratio<3600, 1>>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_hour_modded>(delay));
		break;
	}

	default:
	{
		using _ratio_modded = std::ratio<unit_mod_num, unit_mod_num>;
		return _time_point + std::chrono::duration_cast<clock_Ty::duration>(
			std::chrono::duration<float_Ty, _ratio_modded>(delay));
		break;
	}

	}
}


template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::start() noexcept
{
	m_start = clock_Ty::now();
	return *this;
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::stop() noexcept
{
	m_stop = clock_Ty::now();
	return *this;
}

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::get_duration(cool::duration unit) const noexcept
{
	return duration_per_tick<unit_mod_num, unit_mod_den>(unit) * static_cast<float_Ty>((m_stop - m_start).count());
}

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::get_duration_until_now(cool::duration unit) const noexcept
{
	return duration_per_tick<unit_mod_num, unit_mod_den>(unit) * static_cast<float_Ty>((
		clock_Ty::now() - m_start).count());
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline typename std::chrono::time_point<clock_Ty>::rep cool::chrono<float_Ty, int_Ty, clock_Ty>::get_ticks() const noexcept
{
	return (m_stop - m_start).count();
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline typename std::chrono::time_point<clock_Ty>::rep cool::chrono<float_Ty, int_Ty, clock_Ty>::get_ticks_until_now() const noexcept
{
	return (clock_Ty::now() - m_start).count();
}


template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::add_to_total_duration() noexcept
{
	m_total_ticks += (m_stop - m_start).count();
	return *this;
}

template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::get_total_duration(cool::duration unit) const noexcept
{
	return duration_per_tick<unit_mod_num, unit_mod_den>(unit) * static_cast<float_Ty>(m_total_ticks);
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline typename std::chrono::time_point<clock_Ty>::rep cool::chrono<float_Ty, int_Ty, clock_Ty>::get_total_ticks() const noexcept
{
	return m_total_ticks;
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::reset_total_duration() noexcept
{
	m_total_ticks = static_cast<float_Ty>(0); return *this;
}


template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::add_to_count() noexcept
{
	m_count++;
	return *this;
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::add_to_count(int_Ty n) noexcept
{
	m_count += n;
	return *this;
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline int_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::get_count() const noexcept
{
	return m_count;
}

template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::reset_count() noexcept
{
	m_count = 0;
	return *this;
}


template <class float_Ty, class int_Ty, class clock_Ty> template <std::intmax_t unit_mod_num, std::intmax_t unit_mod_den>
inline float_Ty cool::chrono<float_Ty, int_Ty, clock_Ty>::get_average_duration(cool::duration unit) const noexcept
{
	return (duration_per_tick<unit_mod_num, unit_mod_den>(unit) * static_cast<float_Ty>(m_total_ticks)) / static_cast<float_Ty>(m_count);
}


template <class float_Ty, class int_Ty, class clock_Ty>
inline cool::chrono<float_Ty, int_Ty, clock_Ty>& cool::chrono<float_Ty, int_Ty, clock_Ty>::reset_all() noexcept
{
	m_total_ticks = 0;
	m_count = 0;
	return *this;
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
