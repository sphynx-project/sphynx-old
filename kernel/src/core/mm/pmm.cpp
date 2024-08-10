/*
Sphynx Operating System

File: pmm.cpp
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

Description: Sphynx physical memory manager
*/

#include <core/mm/pmm.hpp>
#include <math_utils.hpp>
#include <dev/tty.hpp>
#include <sys/cpu.hpp>
#include <string.hpp>

namespace PMM {
    uint8_t *bitmap;
    uint64_t bitmapPages;
    uint64_t bitmapSize;

    static constexpr uint64_t PAGE_SIZE = 4096;

    void init(memory_map_t *memmap) {
        if (memmap == nullptr) {
            kpanic(nullptr, "NULL Memory map passed to PMM init");
        }

        uint64_t highAddr = 0;
        uint64_t topAddr;

        for (int i = 0; i < memmap->region_count; i++) {
            memory_region_t *region = memmap->regions[i];
            if (region->type == MEMMAP_USABLE) {
                printf("Usable entry at 0x%.16llx\n", region->base_address);
                topAddr = region->base_address + region->length;
                if (topAddr > highAddr)
                    highAddr = topAddr;
            }
        }

        bitmapPages = (highAddr + PAGE_SIZE - 1) / PAGE_SIZE;
        bitmapSize = ALIGN_UP(bitmapPages / 8, PAGE_SIZE);

        for (int i = 0; i < memmap->region_count; i++) {
            memory_region_t *region = memmap->regions[i];
            if (region->type == MEMMAP_USABLE) {
                if (region->length >= bitmapSize) {
                    bitmap = (uint8_t*)(region->base_address);
                    memset(bitmap, 0xFF, bitmapSize);
                    region->base_address += bitmapSize;
                    region->length -= bitmapSize;
                    break;
                }
            }
        }

        for (int i = 0; i < memmap->region_count; i++) {
            memory_region_t *region = memmap->regions[i];
            if (region->type == MEMMAP_USABLE) {
                for (uint64_t addr = region->base_address; addr < region->base_address + region->length; addr += PAGE_SIZE) {
                    BITMAP_UNSET_BIT(bitmap, (addr - region->base_address) / PAGE_SIZE);
                }
            }
        }
    }

    uint64_t get_free() {
        uint64_t free = 0;

        for (uint64_t i = 0; i < bitmapPages; i++) {
            if (!BITMAP_READ_BIT(bitmap, i)) {
                free += PAGE_SIZE;
            }
        }

        return free;
    }

    void* request_pages(uint64_t pageCount) {
        if (pageCount == 0) {
            return nullptr;
        }

        uint64_t startIndex = 0;
        uint64_t contiguousCount = 0;

        for (uint64_t i = 0; i < bitmapPages; i++) {
            if (!BITMAP_READ_BIT(bitmap, i)) {
                if (contiguousCount == 0) {
                    startIndex = i;
                }
                contiguousCount++;
                if (contiguousCount == pageCount) {
                    for (uint64_t j = startIndex; j < startIndex + pageCount; j++) {
                        BITMAP_SET_BIT(bitmap, j);
                    }
                    return (void *)(startIndex * PAGE_SIZE);
                }
            } else {
                contiguousCount = 0;
            }
        }

        return nullptr;
    }

    void free_pages(void* ptr) {
        uint64_t startAddr = (uint64_t)ptr;
        uint64_t startIndex = startAddr / PAGE_SIZE;

        uint64_t pageCount = 0;
        while (startIndex + pageCount < bitmapPages && !BITMAP_READ_BIT(bitmap, startIndex + pageCount)) {
            pageCount++;
        }

        for (uint64_t i = startIndex; i < startIndex + pageCount; i++) {
            BITMAP_UNSET_BIT(bitmap, i);
        }
    }
}
