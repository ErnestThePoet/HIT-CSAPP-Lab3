#pragma once

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
	static double EndAndPrint(
		TimeUnit unit = TimeUnit::MILLISECOND,
		std::streamsize fixedPrecision = 2);
	static double End(TimeUnit unit = TimeUnit::MILLISECOND);

private:
	static std::string name_;
	static std::chrono::steady_clock::time_point start_time_;
	static bool is_started_;
};

