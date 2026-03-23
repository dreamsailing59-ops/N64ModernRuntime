#ifndef __ULTRAMODERN_TLB_HPP__
#define __ULTRAMODERN_TLB_HPP__

#include <stdint.h>
#include <stdbool.h>

// Forward declaration for the context
struct recomp_context;

#define N64_TLB_ENTRIES 32

// Define the struct once in a way both C and C++ like
typedef struct {
    uint32_t entry_hi;
    uint32_t entry_lo0;
    uint32_t entry_lo1;
    uint32_t page_mask;
    bool valid;
} TLBEntry;

#ifdef __cplusplus
namespace ultramodern {
    namespace mem {
        extern TLBEntry g_tlb_table[N64_TLB_ENTRIES];
    }
}

extern "C" {
#endif

// The core translation function called by the recompiled C code
uint32_t translate_vaddr(uint32_t vaddr);

// Hooks for the game to update the TLB
void recomp_map_tlb(int32_t index, uint32_t mask, uint32_t hi, uint32_t lo0, uint32_t lo1);
void recomp_unmap_tlb(int32_t index);

#ifdef __cplusplus
}
#endif

#endif // __ULTRAMODERN_TLB_HPP__