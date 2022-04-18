#include "cl_device_helper.h"

// M: error message, S: status
#define CHECK_STATUS(M,S) do{if(S!=CL_SUCCESS){throw std::runtime_error(M);}} while(false)
// I: is_initialized
#define CHECK_INITIALIZATION(I) do{if(!I){throw std::runtime_error("CLDeviceHelper is not initialized.");}} while(false)

const char* CLDeviceHelper::device_type_tags_[] = {
    "GPU","CPU","Accelerator","Default","Custom","Unknown"
};

const char* CLDeviceHelper::vendor_name_keywords_[] = { "advanced micro devices","nvidia","intel" };

const VendorPreferenceList CLDeviceHelper::kPreferenceANI = {
    CLDeviceHelper::vendor_name_keywords_[0],
    CLDeviceHelper::vendor_name_keywords_[1], 
    CLDeviceHelper::vendor_name_keywords_[2]
};
const VendorPreferenceList CLDeviceHelper::kPreferenceNAI = {
    CLDeviceHelper::vendor_name_keywords_[1],
    CLDeviceHelper::vendor_name_keywords_[0], 
    CLDeviceHelper::vendor_name_keywords_[2]
};
const VendorPreferenceList CLDeviceHelper::kPreferenceAIN = {
    CLDeviceHelper::vendor_name_keywords_[0],
    CLDeviceHelper::vendor_name_keywords_[2],
    CLDeviceHelper::vendor_name_keywords_[1]
};
const VendorPreferenceList CLDeviceHelper::kPreferenceNIA = {
    CLDeviceHelper::vendor_name_keywords_[1],
    CLDeviceHelper::vendor_name_keywords_[2],
    CLDeviceHelper::vendor_name_keywords_[0]
};
const VendorPreferenceList CLDeviceHelper::kPreferenceIAN = {
    CLDeviceHelper::vendor_name_keywords_[2],
    CLDeviceHelper::vendor_name_keywords_[0], 
    CLDeviceHelper::vendor_name_keywords_[1]
};
const VendorPreferenceList CLDeviceHelper::kPreferenceINA = {
    CLDeviceHelper::vendor_name_keywords_[2],
    CLDeviceHelper::vendor_name_keywords_[1], 
    CLDeviceHelper::vendor_name_keywords_[0]
};

void CLDeviceHelper::PrintAllDevices() const
{
    CHECK_INITIALIZATION(this->is_initialized_);

    std::cout << "All platform(s) and device(s) found:" << std::endl;

    for (int i=0;i<this->platforms_.size();i++)
    {
        std::cout << "[Platform ID: " << i << "] " << this->platforms_[i].platform_name << std::endl;

        for (int j = 0; j < this->platforms_[i].devices.size(); j++)
        {
            std::cout << std::string(4, ' ');
            std::cout << "[Device ID: " << j << ", Type: "
                << GetDeviceTypeTag(this->platforms_[i].devices[j].device_type)
                <<"] "
                << this->platforms_[i].devices[j].device_name
                <<' '
                << std::endl;
        }

        std::cout << std::endl;
    }
}

cl_device_id CLDeviceHelper::GetDeviceIdFromInput() const
{
    CHECK_INITIALIZATION(this->is_initialized_);

    int chosen_platform_index = GetRangedIntFromInput(
        std::cin,
        0,
        this->platforms_.size() - 1,
        std::string("Enter your chosen platform ID: "));

    int chosen_device_index = GetRangedIntFromInput(
        std::cin,
        0,
        this->platforms_[chosen_platform_index].devices.size() - 1,
        std::string("Enter your chosen device ID on platform [")
        + this->platforms_[chosen_platform_index].platform_name
        + "]: ");

    return this->platforms_[chosen_platform_index].devices[chosen_device_index].device_id;
}

void CLDeviceHelper::Initialize()
{
    cl_int status = CL_SUCCESS;

    cl_uint platform_count = 0;
    status = clGetPlatformIDs(0, nullptr, &platform_count);

    CHECK_STATUS("Failed to get OpenCL platform count",status);

    if (platform_count == 0)
    {
        throw std::runtime_error("No OpenCL-capable platform found");
    }

    std::vector<cl_platform_id> platform_ids(platform_count);

    status = clGetPlatformIDs(platform_count, platform_ids.data(), nullptr);
    CHECK_STATUS("Failed to get OpenCL platform IDs", status);

    for (auto i : platform_ids)
    {
        // Platform name
        // length includes '\0'
        size_t platform_name_length = 0;

        status = clGetPlatformInfo(i, CL_PLATFORM_NAME, 0, nullptr, &platform_name_length);
        CHECK_STATUS("Failed to get OpenCL platform name length", status);

        std::vector<char> platform_name_buffer(platform_name_length);

        status = clGetPlatformInfo(
            i, 
            CL_PLATFORM_NAME, 
            platform_name_length, 
            platform_name_buffer.data(), 
            nullptr);
        CHECK_STATUS("Failed to get OpenCL platform name", status);

        // Platform vendor
        size_t platform_vendor_length = 0;

        status = clGetPlatformInfo(i, CL_PLATFORM_VENDOR, 0, nullptr, &platform_vendor_length);
        CHECK_STATUS("Failed to get OpenCL platform vendor length", status);

        std::vector<char> platform_vendor_buffer(platform_vendor_length);

        status = clGetPlatformInfo(
            i,
            CL_PLATFORM_VENDOR,
            platform_vendor_length,
            platform_vendor_buffer.data(),
            nullptr);
        CHECK_STATUS("Failed to get OpenCL platform vendor", status);

        for (int j = 0; j < platform_vendor_length; j++)
        {
            platform_vendor_buffer[j] = tolower(platform_vendor_buffer[j]);
        }

        this->platforms_.emplace_back(platform_name_buffer.data(), platform_vendor_buffer.data(), i);
    }

    for (auto& i : this->platforms_)
    {
        cl_uint device_count = 0;

        status = clGetDeviceIDs(i.platform_id, CL_DEVICE_TYPE_ALL, 0, nullptr, &device_count);
        CHECK_STATUS(
            std::string("Failed to get OpenCL device count on platform: ")+i.platform_name,
            status);

        std::vector<cl_device_id> device_ids(device_count);

        status = clGetDeviceIDs(
            i.platform_id, 
            CL_DEVICE_TYPE_ALL, 
            device_count, 
            device_ids.data(), 
            nullptr);
        CHECK_STATUS(
            std::string("Failed to get OpenCL device IDs on platform: ") + i.platform_name,
            status);

        for (auto j : device_ids)
        {
            // Device name
            size_t device_name_length = 0;
            status = clGetDeviceInfo(j, CL_DEVICE_NAME, 0, nullptr, &device_name_length);
            CHECK_STATUS(
                std::string("Failed to get OpenCL device name length on platform: ") + i.platform_name,
                status);

            std::vector<char> device_name_buffer(device_name_length);

            status = clGetDeviceInfo(
                j, 
                CL_DEVICE_NAME, 
                device_name_length, 
                device_name_buffer.data(), 
                nullptr);
            CHECK_STATUS(
                std::string("Failed to get OpenCL device name on platform: ") + i.platform_name,
                status);

            // Device type
            cl_device_type device_type;

            status = clGetDeviceInfo(
                j,
                CL_DEVICE_TYPE,
                sizeof(cl_device_type),
                &device_type,
                nullptr);
            CHECK_STATUS(
                std::string("Failed to get OpenCL device type on platform: ") + i.platform_name,
                status);

            i.devices.emplace_back(device_name_buffer.data(), device_type, j);
        }
    }

    this->is_initialized_ = true;
}

int CLDeviceHelper::GetRangedIntFromInput(
    std::istream& stream, const int min, const int max,const std::string& prompt) const
{
    bool is_read_normal= false;
    int ret = 0;

    do
    {
        std::cout << prompt;

        // Situations:
        // 1. beginning with an integer: clear remaining chars in input buffer if any.
        // 2. not beginning with an integer: clear flags and clear remaining chars in input buffer.
        // 3. single EOF: clear flags.

        is_read_normal = static_cast<bool>(stream >> ret);

        if (is_read_normal)
        {
            ClearInputBuffer(stream);
        }
        else
        {
            bool was_eof = stream.eof();
            stream.clear();
            if (!was_eof)
            {
                ClearInputBuffer(stream);
            }
        }
    } while (ret < min || ret > max || !is_read_normal);

    return ret;
}

void CLDeviceHelper::ClearInputBuffer(std::istream& stream) const
{
    int c = 0;
    // if ctrl+z is pressed after other characters, it will be read as 26 and all chars
    // that follow will be discarded. Next data will be read from new inputs.
    while (!stream.eof() && (c = stream.get()) != '\n' && c != 26);
}

const char* CLDeviceHelper::GetDeviceTypeTag(const cl_device_type device_type) const
{
    if (device_type & CL_DEVICE_TYPE_GPU)
    {
        return CLDeviceHelper::device_type_tags_[0];
    }
    else if (device_type & CL_DEVICE_TYPE_CPU)
    {
        return CLDeviceHelper::device_type_tags_[1];
    }
    else if (device_type & CL_DEVICE_TYPE_ACCELERATOR)
    {
        return CLDeviceHelper::device_type_tags_[2];
    }
    else if (device_type & CL_DEVICE_TYPE_DEFAULT)
    {
        return CLDeviceHelper::device_type_tags_[3];
    }
    else if (device_type == CL_DEVICE_TYPE_CUSTOM)
    {
        return CLDeviceHelper::device_type_tags_[4];
    }
    else
    {
        return CLDeviceHelper::device_type_tags_[5];
    }
}

cl_device_id CLDeviceHelper::GetDeviceIdWithPreference(
    const VendorPreferenceList& preference_lower) const
{
    CHECK_INITIALIZATION(this->is_initialized_);

    // find best preferred GPU device
    for (auto& i : preference_lower)
    {
        for (auto& j : this->platforms_)
        {
            if (j.platform_vendor_lower.find(i) != std::string::npos)
            {
                for (auto& k : j.devices)
                {
                    if (k.device_type == CL_DEVICE_TYPE_GPU)
                    {
                        return k.device_id;
                    }
                }
            }
        }
    }

    // else find best preferred device of other types
    for (auto& i : preference_lower)
    {
        for (auto& j : this->platforms_)
        {
            if (j.platform_vendor_lower.find(i) != std::string::npos
                && j.devices.size() > 0)
            {
                return j.devices[0].device_id;
            }
        }
    }

    // no match vendors, then return first GPU
    for (auto& i : this->platforms_)
    {
        for (auto& j : i.devices)
        {
            if (j.device_type == CL_DEVICE_TYPE_GPU)
            {
                return j.device_id;
            }
        }
    }

    // otherwise return first device
    for (auto& i : this->platforms_)
    {
        if (i.devices.size() > 0)
        {
            return i.devices[0].device_id;
        }
    }

    // guaranteed not reachable
    return nullptr;
}

void CLDeviceHelper::GetSuitableGlobalLocalSize(
    const cl_device_id device_id,
    const cl_uint work_dim,
    const size_t* work_item_size,
    size_t* global_size_ret,
    size_t* local_size_ret) const
{
    cl_int status = CL_SUCCESS;

    // max work dims
    cl_uint max_dims = 0;

    status = clGetDeviceInfo(
        device_id,
        CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        sizeof(cl_uint),
        &max_dims,
        nullptr
    );

    CHECK_STATUS("Failed to query max work item dimensions.", status);

    // max work items
    std::vector<size_t> max_work_items(max_dims);

    status = clGetDeviceInfo(
        device_id,
        CL_DEVICE_MAX_WORK_ITEM_SIZES,
        sizeof(size_t) * max_dims,
        max_work_items.data(),
        nullptr
    );

    CHECK_STATUS("Failed to query max work item sizes.", status);

    // max work group size
    size_t max_work_group_size = 0;

    status = clGetDeviceInfo(
        device_id,
        CL_DEVICE_MAX_WORK_GROUP_SIZE,
        sizeof(size_t),
        &max_work_group_size,
        nullptr);

    CHECK_STATUS("Failed to query max work group size.", status);

    // computation
    switch (work_dim)
    {
    case 1:
        // brackets added to avoid error C2360, C2361
    {
        size_t measure = std::min(max_work_group_size, max_work_items[0]);
        local_size_ret[0] = measure;
        break;
    }
    case 2:
        switch (max_work_group_size)
        {
        case 64:
            local_size_ret[0] = 8;
            local_size_ret[1] = 8;
            break;

        case 128:
            local_size_ret[0] = 16;
            local_size_ret[1] = 8;
            break;

        case 256:
            local_size_ret[0] = 16;
            local_size_ret[1] = 16;
            break;

        case 512:
            local_size_ret[0] = 32;
            local_size_ret[1] = 16;
            break;

        case 1024:
            local_size_ret[0] = 32;
            local_size_ret[1] = 32;
            break;

        case 2048:
            local_size_ret[0] = 64;
            local_size_ret[1] = 32;
            break;

        case 4096:
            local_size_ret[0] = 64;
            local_size_ret[1] = 64;
            break;

        case 8192:
            local_size_ret[0] = 128;
            local_size_ret[1] = 64;
            break;

        case 16384:
            local_size_ret[0] = 128;
            local_size_ret[1] = 128;
            break;

        case 32768:
            local_size_ret[0] = 256;
            local_size_ret[1] = 128;
            break;

        default:
            local_size_ret[0] = local_size_ret[1] = (size_t)sqrt(max_work_group_size);
        }

        break;

    case 3:
        switch (max_work_group_size)
        {
        case 64:
            local_size_ret[0] = 4;
            local_size_ret[1] = 4;
            local_size_ret[2] = 4;
            break;

        case 128:
            local_size_ret[0] = 8;
            local_size_ret[1] = 4;
            local_size_ret[2] = 4;
            break;

        case 256:
            local_size_ret[0] = 8;
            local_size_ret[1] = 8;
            local_size_ret[2] = 4;
            break;

        case 512:
            local_size_ret[0] = 8;
            local_size_ret[1] = 8;
            local_size_ret[2] = 8;
            break;

        case 1024:
            local_size_ret[0] = 16;
            local_size_ret[1] = 8;
            local_size_ret[2] = 8;
            break;

        case 2048:
            local_size_ret[0] = 16;
            local_size_ret[1] = 16;
            local_size_ret[2] = 8;
            break;

        case 4096:
            local_size_ret[0] = 16;
            local_size_ret[1] = 16;
            local_size_ret[2] = 16;
            break;

        case 8192:
            local_size_ret[0] = 32;
            local_size_ret[1] = 16;
            local_size_ret[2] = 16;
            break;

        case 16384:
            local_size_ret[0] = 32;
            local_size_ret[1] = 32;
            local_size_ret[2] = 16;
            break;

        case 32768:
            local_size_ret[0] = 32;
            local_size_ret[1] = 32;
            local_size_ret[2] = 32;
            break;

        default:
            local_size_ret[0] = local_size_ret[1] = local_size_ret[1] = 
                (size_t)cbrt(max_work_group_size);
        }

        break;

    default:
        throw std::runtime_error("Provided work dim is currently not supported by CLDeviceHelper.");
    }

    // so far we make sure local_size_ret[0]*...*local_size_ret[work_dim-1]<=max_work_group_size.
    // next check if local_size_ret[i] exceeds max_work_item_size[i]
    // then compute global_size_ret

    for (int i = 0; i < work_dim; i++)
    {
        if (local_size_ret[i] > max_work_items[i])
        {
            local_size_ret[i] = max_work_items[i];
        }

        global_size_ret[i] = ((work_item_size[i] - 1) / local_size_ret[i] + 1) * local_size_ret[i];
    }
}