#include "cpuminer-config.h"
#include "miner.h"

#include <string.h>
#include <stdint.h>

#include "sph_keccak.h"

static void keccakhash(void *state, const void *input)
{
    sph_keccak256_context ctx_keccak;
    uint32_t hash[32];

    sph_keccak256_init(&ctx_keccak);
    sph_keccak256 (&ctx_keccak, input, 100);
    sph_keccak256_close(&ctx_keccak, hash);

    memcpy(state, hash, 32);
}

int scanhash_keccak(int thr_id, uint32_t *pdata, const uint32_t *ptarget, uint32_t max_nonce, unsigned long *hashes_done)
{
	uint32_t n = pdata[24] - 1;
	const uint32_t first_nonce = pdata[24];
	const uint32_t Htarg = ptarget[0];

	uint32_t hash64[8] __attribute__((aligned(32)));
	uint32_t endiandata[32];

	int kk=0;
	for (; kk < 32; kk++)
	{
		be32enc(&endiandata[kk], ((uint32_t*)pdata)[kk]);
	};

	do {

		pdata[24] = ++n;
		keccakhash(hash64, pdata);

        if ( swab32(hash64[0]) <= swab32(Htarg) && fulltest(hash64, ptarget) ) {
            *hashes_done = n - first_nonce + 1;
            return true;
        }
	} while (n < max_nonce && !work_restart[thr_id].restart);

	*hashes_done = n - first_nonce + 1;
	pdata[24] = n;
	return 0;
}
