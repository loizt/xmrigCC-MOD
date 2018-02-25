#include "crypto/CryptoNight.h"

#if defined(XMRIG_ARM)
#   include "crypto/CryptoNight_arm.h"
#else
#   include "crypto/CryptoNight_x86.h"
#endif

#include "crypto/CryptoNight_test.h"

template <size_t NUM_HASH_BLOCKS>
static void cryptonight_aesni(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
#   if !defined(XMRIG_ARMv7)
    CryptoNightMultiHash<0x80000, MEMORY, 0x1FFFF0, false, NUM_HASH_BLOCKS>::hash(input, size, output, ctx);
#   endif
}

template <size_t NUM_HASH_BLOCKS>
static void cryptonight_softaes(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    CryptoNightMultiHash<0x80000, MEMORY, 0x1FFFF0, true, NUM_HASH_BLOCKS>::hash(input, size, output, ctx);
}

template <size_t NUM_HASH_BLOCKS>
static void cryptonight_lite_aesni(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
#   if !defined(XMRIG_ARMv7)
    CryptoNightMultiHash<0x40000, MEMORY_LITE, 0xFFFF0, false, NUM_HASH_BLOCKS>::hash(input, size, output, ctx);
#   endif
}

template <size_t NUM_HASH_BLOCKS>
static void cryptonight_lite_softaes(const void *input, size_t size, void *output, cryptonight_ctx *ctx) {
    CryptoNightMultiHash<0x40000, MEMORY_LITE, 0xFFFF0, true, NUM_HASH_BLOCKS>::hash(input, size, output, ctx);
}

void (*cryptonight_hash_ctx[MAX_NUM_HASH_BLOCKS])(const void *input, size_t size, void *output, cryptonight_ctx *ctx);

template <size_t HASH_FACTOR>
void setCryptoNightHashMethods(Options::Algo algo, bool aesni)
{

    switch (algo) {
        case Options::ALGO_CRYPTONIGHT:
            if (aesni) {
                cryptonight_hash_ctx[HASH_FACTOR - 1] = cryptonight_aesni<HASH_FACTOR>;
            } else {
                cryptonight_hash_ctx[HASH_FACTOR - 1] = cryptonight_softaes<HASH_FACTOR>;
            }
            break;

        case Options::ALGO_CRYPTONIGHT_LITE:
            if (aesni) {
                cryptonight_hash_ctx[HASH_FACTOR - 1] = cryptonight_lite_aesni<HASH_FACTOR>;
            } else {
                cryptonight_hash_ctx[HASH_FACTOR - 1] = cryptonight_lite_softaes<HASH_FACTOR>;
            }
            break;
    }
    // next iteration
    setCryptoNightHashMethods<HASH_FACTOR-1>(algo, aesni);
}

template <>
void setCryptoNightHashMethods<0>(Options::Algo algo, bool aesni)
{
    // template recursion abort
};

bool CryptoNight::init(int algo, bool aesni)
{
    setCryptoNightHashMethods<MAX_NUM_HASH_BLOCKS>(static_cast<Options::Algo>(algo), aesni);
    return selfTest(algo);
}

void CryptoNight::hash(size_t factor, const uint8_t* input, size_t size, uint8_t* output, cryptonight_ctx* ctx)
{
    cryptonight_hash_ctx[factor-1](input, size, output, ctx);
}

bool CryptoNight::selfTest(int algo)
{
    if (cryptonight_hash_ctx[0] == nullptr || cryptonight_hash_ctx[2] == nullptr ||
        cryptonight_hash_ctx[2] == nullptr || cryptonight_hash_ctx[3] == nullptr ||
		cryptonight_hash_ctx[4] == nullptr) {
        return false;
    }

    char output[160];

    auto ctx = (struct cryptonight_ctx*) _mm_malloc(sizeof(struct cryptonight_ctx), 16);
    ctx->memory = (uint8_t *) _mm_malloc(MEMORY * 6, 16);

    cryptonight_hash_ctx[0](test_input, 76, output, ctx);
    bool resultSingle = memcmp(output, algo == Options::ALGO_CRYPTONIGHT_LITE ? test_output1 : test_output0, 32) == 0;

    cryptonight_hash_ctx[1](test_input, 76, output, ctx);
    bool resultDouble = memcmp(output, algo == Options::ALGO_CRYPTONIGHT_LITE ? test_output1 : test_output0, 64) == 0;

    cryptonight_hash_ctx[2](test_input, 76, output, ctx);
    bool resultTriple = memcmp(output, algo == Options::ALGO_CRYPTONIGHT_LITE ? test_output1 : test_output0, 96) == 0;

    cryptonight_hash_ctx[3](test_input, 76, output, ctx);
    bool resultQuadruple = memcmp(output, algo == Options::ALGO_CRYPTONIGHT_LITE ? test_output1 : test_output0, 128) == 0;

    cryptonight_hash_ctx[4](test_input, 76, output, ctx);
    bool resultQuintuple = memcmp(output, algo == Options::ALGO_CRYPTONIGHT_LITE ? test_output1 : test_output0, 160) == 0;

    _mm_free(ctx->memory);
    _mm_free(ctx);

    return resultSingle && resultDouble && resultTriple && resultQuadruple && resultQuintuple;
}