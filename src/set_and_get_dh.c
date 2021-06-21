/* How to use this sample code to check the behavior of DH_set0_key(), DH_get0_*() and so on.
 
 How to compile:
 $ gcc -Wall -fstack-protector-strong -o set_and_get_dh set_and_get_dh.c -lcrypto 

 or with downloaded openssl under ../../openssl/:
 $ gcc -Wall -fstack-protector-strong -o set_and_get_dh set_and_get_dh.c -I../../openssl/include -L../../openssl -lcrypto -Wl,-rpath=../../openssl

 Run:
 $ ./set_and_get_dh

*/
/* https://github.com/KazKobara/openssl-migration */

#include <stdio.h>
#include <string.h>
#include <openssl/engine.h>

/* 1024-bit p, 160-bit q are too short to use securely. 
 * Use larger ones in practice!! */
#define DL1_P	"ffffffffffffffffffffffffffffffffffffffd100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005b7dffffffffffffffffffffffffffffffffffef33df"
#define DL1_Q	"ffffffffffffffffffffffffffffffffffffffd1"
#define DL1_G	"25535ca11286379d13455dfd677ffd6684873b1dc4b40bec6cc1b21033e88ade3ae559fa7ab4170589a3a8904359fb32b74c59fbfea78b3204b0c126179900469543e503fc1218f6a78967a10432f56392497ce34de38dab5ec4090e20b39b93b1fcc2403457f831047bf4fcb70d0e536a48ebfbbdffb93e15f9d348"

#if OPENSSL_VERSION_NUMBER > 0x10100000L
int32_t DH_key_set_and_get(void){
    int32_t ret = 1;
    DH * dh_p = NULL;

    /* For `*_get*()` */
    /* Choose one to check their behaviors */
    const BIGNUM * cbn_q_p = NULL;      // OK recommended
    //const BIGNUM * cbn_q_p;             // OK but not recommended in error
    //const BIGNUM * cbn_q_p = BN_new();  // OK but freed by `*_get*()` and not used.

    /* For `*_put*()` and BN_bn2hex() */
    BIGNUM * bn_p_p = NULL, * bn_g_p = NULL;
    /* Choose one to check their behaviors */
    //BIGNUM * bn_q_p;             // segfault
    BIGNUM * bn_q_p = NULL;      // OK
    //BIGNUM * bn_q_p = BN_new();  // OK

    if (!(dh_p = DH_new())) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }

    printf("After init:  &bn_q_p=%p,  bn_q_p=%p\n",  &bn_q_p,  bn_q_p);
    printf("After init: &cbn_q_p=%p, cbn_q_p=%p\n", &cbn_q_p, cbn_q_p);

    // segfault for non-initialized pointer since non-null pointers seem to be freed at first.
    if (!BN_hex2bn(&bn_q_p, DL1_Q)) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }
    if (!BN_hex2bn(&bn_p_p, DL1_P)) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }
    if (!BN_hex2bn(&bn_g_p, DL1_G)) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }

    printf("Before set:  &bn_q_p=%p,  bn_q_p=%p, BN_bn2hex( bn_q_p)=%s\n",
        &bn_q_p, bn_q_p, BN_bn2hex(bn_q_p));
    if (!DH_set0_pqg(dh_p, bn_p_p, bn_q_p, bn_g_p)) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }
    bn_p_p = NULL;
    bn_q_p = NULL;
    bn_g_p = NULL;

    if (!DH_check_ex(dh_p)) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }

    cbn_q_p = DH_get0_q(dh_p);
    if (!cbn_q_p) {
        ERR_print_errors_fp(stderr);
		goto Done;
    }
    printf("After  get: &cbn_q_p=%p, cbn_q_p=%p, BN_bn2hex(cbn_q_p)=%s\n", &cbn_q_p, cbn_q_p, BN_bn2hex(cbn_q_p));
 
    ret = 0;
 Done:
    DH_free(dh_p);
    return ret;
}
#endif

int32_t main(void){
    int32_t ret = 0;
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    if (!(ret=DH_key_set_and_get())) goto Done;
#else
    fprintf(stderr, "Error: use libssl > 1.1!! (OPENSSL_VERSION_NUMBER=%lx)\n", OPENSSL_VERSION_NUMBER);
    ret = 1;
#endif

 Done:
    return ret;
}
