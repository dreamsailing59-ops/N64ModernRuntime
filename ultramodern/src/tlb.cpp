#include "ultramodern/tlb.hpp"

namespace ultramodern {
    namespace mem {
        TLBEntry g_tlb_table[N64_TLB_ENTRIES];
    }
}

extern "C" uint32_t translate_vaddr(uint32_t vaddr) {
    // 1. Direct Mapped (KSEG0/KSEG1)
    if (vaddr >= 0x80000000 && vaddr <= 0xBFFFFFFF) {
        return vaddr & 0x1FFFFFFF;
    }

    // 2. TLB Lookup
    for (int i = 0; i < N64_TLB_ENTRIES; i++) {
        if (!ultramodern::mem::g_tlb_table[i].valid) continue;

        uint32_t mask = ~(ultramodern::mem::g_tlb_table[i].page_mask | 0x1FFF);
        if ((vaddr & mask) == (ultramodern::mem::g_tlb_table[i].entry_hi & mask)) {
            bool is_odd = vaddr & (ultramodern::mem::g_tlb_table[i].page_mask + 0x1000);
            uint32_t entry_lo = is_odd ? ultramodern::mem::g_tlb_table[i].entry_lo1 : ultramodern::mem::g_tlb_table[i].entry_lo0;
            return ((entry_lo >> 6) << 12) | (vaddr & (ultramodern::mem::g_tlb_table[i].page_mask | 0xFFF));
        }
    }
    return 0xFFFFFFFF;
}

extern "C" void recomp_map_tlb(int32_t index, uint32_t mask, uint32_t hi, uint32_t lo0, uint32_t lo1) {
    if (index < 0 || index >= N64_TLB_ENTRIES) return;
    ultramodern::mem::g_tlb_table[index].entry_hi = hi;
    ultramodern::mem::g_tlb_table[index].entry_lo0 = lo0;
    ultramodern::mem::g_tlb_table[index].entry_lo1 = lo1;
    ultramodern::mem::g_tlb_table[index].page_mask = mask;
    ultramodern::mem::g_tlb_table[index].valid = true;
}

extern "C" void recomp_unmap_tlb(int32_t index) {
    if (index < 0 || index >= N64_TLB_ENTRIES) return;
    ultramodern::mem::g_tlb_table[index].valid = false;
}