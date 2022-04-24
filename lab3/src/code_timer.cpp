#include "code_timer.h"

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

double CodeTimer::StopAndPrint(
	const double reference,
	const std::string& reference_name,
	const TimeUnit unit, 
	const std::streamsize fixedPrecision,
	const std::streamsize width)
{
	if (!is_started_)
	{
		std::cout << BRACKETED_LINE("Cannot end timer: The timer is not started.");

		return 0.0;
	}

	auto elapse = Stop(unit);

	this->PrintElapse(elapse, name_, reference, reference_name, unit, fixedPrecision, width);

	return elapse;
}

double CodeTimer::Stop(const TimeUnit unit)
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

void CodeTimer::PrintElapse(
	const double elapse, 
	const std::string& name, 
	const double reference, 
	const std::string& reference_name, 
	const TimeUnit unit, 
	const std::streamsize fixedPrecision, 
	const std::streamsize width) const
{
	std::cout << std::fixed << std::setprecision(fixedPrecision);

	if (name.length() > 0)
	{
		// setw is one-time
		std::cout << std::left << std::setw(width) << ('[' + name + "] ") << std::right;
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

	
	if (reference >= 0)
	{
		bool is_faster = elapse <= reference;

		if (is_faster)
		{
			SET_PRINT_COLOR_GREEN;
		}
		else
		{
			SET_PRINT_COLOR_RED;
		}

		std::cout << " (";

		if (elapse > 0)
		{
			double diff_percent = (reference / elapse - 1.0) * 100.0;
			std::cout
				// fixed is already set
				<< std::setprecision(2)
				<< abs(diff_percent)
				<< "% ";
		}
		else
		{
			std::cout << "¡Þ ";
		}

		std::cout
			<< (is_faster ? "faster" : "slower")
			<< ((reference_name.length() > 0) ? " than " : "")
			<< reference_name
			<< ')';
	}

	std::cout << std::endl;
	std::cout.unsetf(std::ios_base::fixed);
	std::cout.precision(6);

	SET_PRINT_COLOR_DEFAULT;
}
