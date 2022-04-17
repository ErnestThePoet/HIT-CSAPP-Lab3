#include "code_timer.h"

std::string CodeTimer::name_;
std::chrono::steady_clock::time_point CodeTimer::start_time_;
bool CodeTimer::is_started_ = false;

void CodeTimer::Start()
{
	Start("");
}

void CodeTimer::Start(const std::string& name)
{
	if (is_started_)
	{
		std::cout << BRACKETED_LINE("Cannot start timer: The timer is already started.");

		return;
	}

	start_time_ = std::chrono::steady_clock::now();
	name_ = name;
	is_started_ = true;
}

double CodeTimer::EndAndPrint(TimeUnit unit, std::streamsize fixedPrecision)
{
	if (!is_started_)
	{
		std::cout << BRACKETED_LINE("Cannot end timer: The timer is not started.");

		return 0.0;
	}

	auto elapse = End();

	std::cout << std::fixed << std::setprecision(fixedPrecision);

	if (name_.length() > 0)
	{
		std::cout << '[' << name_ << "] ";
	}

	std::cout << "Time Cost: ";

	switch (unit)
	{
		case TimeUnit::SECOND:
			std::cout << elapse << "s";
			break;

		case TimeUnit::MILLISECOND:
		default:
			std::cout << elapse << "ms";
			break;
	}

	std::cout << std::endl;
	std::cout.unsetf(std::ios_base::fixed);
	std::cout.precision(6);

	return elapse;
}

double CodeTimer::End(TimeUnit unit)
{
	if (!is_started_)
	{
		std::cout << BRACKETED_LINE("Cannot end timer: The timer is not started.");

		return 0.0;
	}

	auto end_time = std::chrono::steady_clock::now();

	auto elapse_raw = end_time - start_time_;

	double elapse = 0.0;

	switch (unit)
	{
		case TimeUnit::SECOND:
			elapse = std::chrono::duration_cast<
				std::chrono::duration<double>>(elapse_raw).count();
			break;

		case TimeUnit::MILLISECOND:
		default:
			elapse = std::chrono::duration_cast<
				std::chrono::duration<double, std::milli>>(elapse_raw).count();
			break;
	}

	is_started_ = false;

	return elapse;
}
