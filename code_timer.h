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

	CodeTimer():is_started_(false){}
	~CodeTimer() = default;
	CodeTimer(const CodeTimer& timer) = delete;
	CodeTimer& operator=(const CodeTimer& timer) = delete;

	void Start();
	void Start(const std::string& name);
	// pass a negative reference to disable reference
	double StopAndPrint(
		const double reference = -1.0,
		const std::string& reference_name = "",
		const TimeUnit unit = TimeUnit::MILLISECOND,
		const std::streamsize fixedPrecision = 2,
		const std::streamsize width = 35);
	double Stop(const TimeUnit unit = TimeUnit::MILLISECOND);

private:
	std::string name_;
	std::chrono::steady_clock::time_point start_time_;
	bool is_started_ = false;
};

