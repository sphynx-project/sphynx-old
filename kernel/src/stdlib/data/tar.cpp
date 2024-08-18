/*
Sphynx Operating System

File: tar.cpp
Author: Kevin Alavik
Year: 2024

License: MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Description: TAR Parsing library
*/

#include <data/tar.hpp>
#include <dev/tty.hpp>
#include <string.hpp>

static inline uint32_t octal_to_decimal(const char* octal_str) {
    uint32_t result = 0;
    while (*octal_str) {
        result = (result << 3) + (*octal_str++ - '0');
    }
    return result;
}

void list_dir_tar(const void* buffer, size_t size) {
    Logger logger("TarParser");
    const char* ptr = static_cast<const char*>(buffer);
    const char* end = ptr + size;

    logger.log(Logger::Level::OK, "---------------\n");
    logger.log(Logger::Level::OK, "Listing contents:\n");

    while (ptr < end) {
        const struct tar_header* header = reinterpret_cast<const struct tar_header*>(ptr);

        if (header->name[0] == '\0') {
            break;
        }

        uint32_t file_size = octal_to_decimal(header->size);
        char typeflag = header->typeflag;

        if (typeflag == '5') {
            logger.log(Logger::Level::OK, "- Directory: %s\n", header->name);
        } else {
            logger.log(Logger::Level::OK, "- File: %s (%u bytes)\n", header->name, file_size);
        }

        ptr += TAR_BLOCK_SIZE + ((file_size + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE) * TAR_BLOCK_SIZE;
    }
    logger.log(Logger::Level::OK, "---------------\n");
}

File get_file_tar(const void* buffer, size_t size, const char* path) {
    Logger logger("TarParser");
    const char* ptr = static_cast<const char*>(buffer);
    const char* end = ptr + size;

    while (ptr < end) {
        const struct tar_header* header = reinterpret_cast<const struct tar_header*>(ptr);

        if (header->name[0] == '\0') {
            break;
        }

        uint32_t file_size = octal_to_decimal(header->size);
        char typeflag = header->typeflag;

        if (strcmp(header->name, path) == 0) {
            File file;
            file.name = header->name;
            file.size = file_size;
            file.data = (void*)(ptr + TAR_BLOCK_SIZE);
            file.is_directory = (typeflag == '5');
            return file;
        }

        ptr += TAR_BLOCK_SIZE + ((file_size + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE) * TAR_BLOCK_SIZE;
    }

    logger.log(Logger::Level::ERROR, "Failed to find \"%s\"\n", path);

    return {};
}
