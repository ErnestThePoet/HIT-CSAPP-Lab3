#pragma once

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>

#include "utilities.h"

class CodeTimer
{
public:
	enum class TimeUnit
	{
		MILLISECOND,
		SECOND
	};

	static void Start();
	static void Start(const std::string& name);
	// pass a negative reference to disable reference
	static double EndAndPrint(
		const double reference = -1.0,
		const std::string& reference_name = "",
		const TimeUnit unit = TimeUnit::MILLISECOND,
		const std::streamsize fixedPrecision = 2);
	static double End(const TimeUnit unit = TimeUnit::MILLISECOND);

private:
	static std::string name_;
	static std::chrono::steady_clock::time_point start_time_;
	static bool is_started_;
};

