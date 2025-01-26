#include "xcl2.hpp"
#include "event_timer.hpp"
#include <vector>

// DATA_SIZE should be multiple of 16 as Kernel Code is using int16 vector
// datatype
// to read the operands from Global Memory. So every read/write to global memory
// will read 16 integers value.
#define DATA_SIZE 16384

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    EventTimer et;

    std::string binaryFile = argv[1];

    et.add("Allocate Memory in Host Memory");
    // Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    std::vector<unsigned int, aligned_allocator<unsigned int> > source_in1(DATA_SIZE);
    std::vector<unsigned int, aligned_allocator<unsigned int> > source_in2(DATA_SIZE);
    std::vector<unsigned int, aligned_allocator<unsigned int> > source_hw_results(DATA_SIZE);
    std::vector<unsigned int, aligned_allocator<unsigned int> > source_sw_results(DATA_SIZE);
    et.finish();

    et.add("Fill the buffers");
    // Create the test data
    for (int i = 0; i < DATA_SIZE; i++) {
        source_in1[i] = i;
        source_in2[i] = i * i;
        source_sw_results[i] = 0;
        source_hw_results[i] = 0;
    }
    et.finish();

//    et.add("Software VADD run");
//    // Software Result
//    for (int i = 0; i < DATA_SIZE; i++) {
//        source_sw_results[i] = source_in1[i] + source_in2[i];
//    }
//    et.finish();

    et.add("Software Mult run");
    // Software Result
    for (int i = 0; i < DATA_SIZE; i++) {
        source_sw_results[i] = source_in1[i] * source_in2[i];
    }
    et.finish();

    et.add("OpenCL host code");
    // OPENCL HOST CODE AREA START
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_vector_add;
    auto devices = xcl::get_xil_devices();
    et.finish();

    et.add("Load Binary File to Alveo U200");
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_vector_add = cl::Kernel(program, "vadd", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }
    et.finish();

    et.add("Allocate Buffer in Global Memory");
    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_in1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                         source_in1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_in2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                         source_in2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vector_size_bytes,
                                            source_hw_results.data(), &err));
    et.finish();

    et.add("Set the Kernel Arguments");
    int size = DATA_SIZE;
    // Set the Kernel Arguments
    int nargs = 0;
    OCL_CHECK(err, err = krnl_vector_add.setArg(nargs++, buffer_in1));
    OCL_CHECK(err, err = krnl_vector_add.setArg(nargs++, buffer_in2));
    OCL_CHECK(err, err = krnl_vector_add.setArg(nargs++, buffer_output));
    OCL_CHECK(err, err = krnl_vector_add.setArg(nargs++, size));
    et.finish();

    et.add("Copy input data to device global memory");
    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2}, 0 /* 0 means from host*/));
    et.finish();

    et.add("Launch the Kernel");
    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vector_add));
    et.finish();

    et.add("Copy Result from Device Global Memory to Host Local Memory");
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    et.finish();
    // OPENCL HOST CODE AREA END



    et.add("Compare the results of the Device to the simulation");
    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }
    et.finish();

    std::cout <<"----------------- Key execution times -----------------" << std::endl;
    et.print();

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
