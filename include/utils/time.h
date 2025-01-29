#pragma once

#include <gctypes.h>

/**
 * @brief normalize a time as hh:mm:ss to the [0, 1[ range
 * @param h the hour [0, 23]
 * @param m the minute [0, 59]
 * @param s the second [0, 59]
 * @return the normalized time in [0, 1[
 */
consteval f32 ntime(const u8 h, const u8 m, const u8 s) {
	return static_cast<f32>(h) / 24.f + static_cast<f32>(m) / 1440.f + static_cast<f32>(s) / 86400.f;
}


/**
 *
 * @param[in] time the current time in [0, 1]
 * @param[in] is the fade-in start time in [0, 1] (is < ie)
 * @param[in] ie the fade-in end time in [0, 1] (is < ie)
 * @param[in] os the fade-out start time in [0, 1] (os < oe)
 * @param[in] oe the fade-out end time in [0, 1] (os < oe)
 * @note the fade-in and fade-out are exclusive
 * @note the fade-out can be before the fade-in
 * @return the activation value in [0, 1] corresponding to the current time
 */
inline f32 timeFrame(const f32 time, const f32 is, const f32 ie, const f32 os, const f32 oe) {
	const f32 fadeInDuration  = ie - is;
	const f32 fadeOutDuration = oe - os;

	const bool isFadedIn  = is <= time;
	const bool isFadedOut = os <= time;
	const bool reversed   = os < is;

	if (isFadedIn && time <= ie) return (time - is) / fadeInDuration;
	if (isFadedOut && time <= oe) return 1.f - (time - os) / fadeOutDuration;

	return static_cast<f32>(isFadedIn ^ isFadedOut ^ reversed);
}
