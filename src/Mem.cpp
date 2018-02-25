#include <memory.h>


#include "crypto/CryptoNight.h"
#include "Mem.h"


int Mem::m_algo          = 0;
int Mem::m_flags         = 0;
size_t Mem::m_hashFactor = 1;
size_t Mem::m_threads    = 0;
size_t Mem::m_memorySize = 0;
uint8_t *Mem::m_memory   = nullptr;
Mem::ThreadBitSet Mem::m_multiHashThreadMask = Mem::ThreadBitSet(-1L);

cryptonight_ctx *Mem::create(int threadId)
{
    size_t scratchPadSize = m_algo == Options::ALGO_CRYPTONIGHT ? MEMORY : MEMORY_LITE;

    size_t offset = 0;
    for (int i=0; i < threadId; i++) {
        offset += sizeof(cryptonight_ctx);
        offset += scratchPadSize * getThreadHashFactor(i);
    }

    auto* ctx = reinterpret_cast<cryptonight_ctx *>(&m_memory[offset]);

    size_t memOffset = offset+sizeof(cryptonight_ctx);

    ctx->memory = &m_memory[memOffset];

    return ctx;
}
