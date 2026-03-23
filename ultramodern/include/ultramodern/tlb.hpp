#ifndef __ULTRAMODERN_TLB_HPP__
#define __ULTRAMODERN_TLB_HPP__

#include <stdint.h>
#include <stdbool.h>
#include "ultra64.h" //

namespace ultramodern {
    namespace mem {
        #define N64_TLB_ENTRIES 32

        struct TLBEntry {
            uint32_t entry_hi;   // Virtual Page Number
            uint32_t entry_lo0;  // Physical Page 0
            uint32_t entry_lo1;  // Physical Page 1
            uint32_t page_mask;  // Page size
            bool valid;
        };

        extern TLBEntry g_tlb_table[N64_TLB_ENTRIES];
        
        static inline uint32_t translate_vaddr(uint32_t vaddr) {
            // KSEG0 and KSEG1 are direct-mapped (80000000 and A0000000)
            if (vaddr >= 0x80000000 && vaddr <= 0xBFFFFFFF) {
                return vaddr & 0x1FFFFFFF;
            }

            for (int i = 0; i < N64_TLB_ENTRIES; i++) {
                if (!g_tlb_table[i].valid) continue;

                uint32_t mask = ~(g_tlb_table[i].page_mask | 0x1FFF);
                if ((vaddr & mask) == (g_tlb_table[i].entry_hi & mask)) {
                    // Determine if we use the even or odd page
                    bool is_odd = vaddr & (g_tlb_table[i].page_mask + 0x1000);
                    uint32_t entry_lo = is_odd ? g_tlb_table[i].entry_lo1 : g_tlb_table[i].entry_lo0;
                    
                    // Return the physical RAM address
                    return ((entry_lo >> 6) << 12) | (vaddr & (g_tlb_table[i].page_mask | 0xFFF));
                }
            }

            return 0xFFFFFFFF;
        }
    }
}

#endif