#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <CL/cl.h>

using VendorPreferenceList = std::vector<const char*>;

class CLDeviceHelper
{
	struct CLDevice
	{
		std::string device_name;
		cl_device_id device_id;
		cl_device_type device_type;
		CLDevice(
			const char* device_name,
			cl_device_type device_type,
			cl_device_id device_id) :
			device_name(device_name),
			device_type(device_type),
			device_id(device_id)
		{}
	};

	struct CLPlatform
	{
		std::string platform_name;
		std::string platform_vendor_lower;
		cl_platform_id platform_id;

		std::vector<CLDevice> devices;

		CLPlatform(
			const char* platform_name,
			const char* platform_vendor_lower,
			cl_platform_id platform_id) :
			platform_name(platform_name),
			platform_vendor_lower(platform_vendor_lower),
			platform_id(platform_id)
		{}
	};

public:
	CLDeviceHelper() = default;
	~CLDeviceHelper() = default;

	void Initialize();
	void PrintAllDevices() const;
	cl_device_id GetDeviceIdFromInput() const;
	cl_device_id GetDeviceIdWithPreference(
		const VendorPreferenceList& preference_lower = CLDeviceHelper::kPreferenceANI) const;
	void GetSuitableGlobalLocalSize(
		const cl_device_id device_id,
		const cl_uint work_dim,
		const size_t* work_item_size,
		size_t* global_size_ret,
		size_t* local_size_ret) const;

	static const VendorPreferenceList kPreferenceANI;
	static const VendorPreferenceList kPreferenceNAI;
	static const VendorPreferenceList kPreferenceAIN;
	static const VendorPreferenceList kPreferenceNIA;
	static const VendorPreferenceList kPreferenceIAN;
	static const VendorPreferenceList kPreferenceINA;

private:
	const char* GetDeviceTypeTag(const cl_device_type device_type) const;
	int GetRangedIntFromInput(
		std::istream& stream, 
		const int min, 
		const int max,
		const std::string& prompt) const;
	void ClearInputBuffer(std::istream& stream) const;

	std::vector<CLPlatform> platforms_;

	bool is_initialized_ = false;

	static const char* device_type_tags_[];
	static const char* vendor_name_keywords_[];
};

