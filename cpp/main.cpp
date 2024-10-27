#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <algorithm>

#include <random>
#include <chrono>
#include <thread>

class NumpyMemmap {
public:
    NumpyMemmap(const char* filename) {
        fd = open(filename, O_RDWR);
        if (fd == -1) {
            std::runtime_error("Failed to open file");
        }

        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Failed to get file size");
        }
        file_size = sb.st_size;

        mapped_data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mapped_data == MAP_FAILED) {
            throw std::runtime_error("Error mapping memory");
        }
    }

    float* get_data() {
        return static_cast<float*>(mapped_data);
    }

    void reset_mem() {
        float* data = get_data();
        size_t num_elements = get_size() / sizeof(float);
        for (size_t i = 0; i < num_elements; i++) {
            data[i] = -1.0;
        }
        msync(mapped_data, file_size, MS_SYNC);
    }

    size_t get_size() const {
        return file_size;
    }

    bool check_new_value() {
        float* ptr = get_data();
        return std::all_of(ptr, ptr+8, [](float val) {
            return val == -1.0f;
        });
    }

    ~NumpyMemmap() {
        if (mapped_data != MAP_FAILED) {
            munmap(mapped_data, file_size);
        }
        if (fd != -1) {
            close(fd);
        }
    }
private:
    void* mapped_data;
    size_t file_size;
    int fd;
};

void random_delay(int min_ms, int max_ms) {
    // Setup random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(min_ms, max_ms);
    
    // Generate random delay and sleep
    int delay_ms = dist(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
}

int main() {
    try {
        NumpyMemmap mmap("/home/swin/shmtest/uavdata.bin");

        while (true) {
            if (!mmap.check_new_value()) {
                float* data = mmap.get_data();

                size_t num_elements = mmap.get_size() / sizeof(float);

                for (size_t i = 0; i < num_elements; i++) {
                    std::cout << data[i] << " ";
                }
                std::cout << std::endl;
                
                mmap.reset_mem();
                random_delay(16, 30);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}