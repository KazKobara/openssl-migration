/*
How to compile:
$ gcc -Wall -fstack-protector-strong -o bn_runtime_error bn_runtime_error.c -lcrypto

Compile with downloaded openssl under ../../openssl/
$ gcc -Wall -fstack-protector-strong -o bn_runtime_error  bn_runtime_error.c -I../../openssl/include -L../../openssl -lcrypto -Wl,-rpath=../../openssl

Run:
$ ./bn_runtime_error
*/
/* https://github.com/KazKobara/openssl-migration */

#include <stdint.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/err.h>

#define RAND_BIT_LEN 160
/* 160-bit prime 0xffffffffffffffffffffffffffffffffffffffd1 */
/* Choose one to test */
#define RAND_RANGE_DEC "1461501637330902918203684832716283019655932542929"
//#define RAND_RANGE_DEC "-1461501637330902918203684832716283019655932542929"
//#define RAND_RANGE_DEC "0"

/**
 * @brief Test of BN pointer initialization for BN_priv_rand_range(). 
 * 
 * @param max_dec_str: decimal string of maximum random number > 0.  
 * @return int32_t: 0 on success, 1 on error. 
 */
static int32_t test_rand_range(const char * const max_dec_str){
    int32_t ret = 1, rc;

    //BIGNUM * bn_range_p;  // No warning, but causes `Segmentation fault`.
    BIGNUM * bn_range_p = NULL;  // No warning, no error, recommended!!
    /*BIGNUM * bn_range_p = BN_new();  // No warning, no runtime error.
    if (NULL==bn_range_p){
		(void)fprintf(stderr, "BN_new(): %s.\n",
			ERR_error_string(ERR_get_error(), NULL));
		goto Done;
    }
    */

    /* Errors and/or warnings occur depending the initialization of bn_rnd_p,
       on at least on the following environment:
       - gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
       - OpenSSL 1.1.1 
       */
    //BIGNUM * const bn_rnd_p = NULL;        // No warning, but causes the following error
        /* "error:0308A071:bignum routines:bnrand_range:too many iterations." */
    //BIGNUM * const bn_rnd_p;             // Compiler warns but no error. 
    //BIGNUM * const
    BIGNUM * bn_rnd_p = NULL;
    /*BIGNUM * bn_rnd_p = BN_secure_new();  // No warning, no error, recommended!!
    if (NULL==bn_rnd_p){
		(void)fprintf(stderr, "BN_secure_new(): %s.\n",
			ERR_error_string(ERR_get_error(), NULL));
		goto Done;
    }
    */

	(void)fprintf(stdout,"--- Example of BN_priv_rand_range() ---\n");
    printf("Before BN_dec2bn(&bn_range_p): &bn_range_p =%p, bn_range_p =%p\n", &bn_range_p, bn_range_p);
 	if (0==BN_dec2bn(&bn_range_p, max_dec_str)) { 
        ERR_print_errors_fp(stderr);
    	goto Done;
	}
    printf("After  BN_dec2bn(&bn_range_p): &bn_range_p =%p, bn_range_p =%p\n", &bn_range_p, bn_range_p);
    // 0 <= rnd < bn_range_p
	rc=BN_priv_rand_range(bn_rnd_p, bn_range_p);
    if (1!=rc){
        ERR_print_errors_fp(stderr);
    	goto Done;
    }

    (void)fprintf(stdout, "bn_range_p: %s\n", BN_bn2dec(bn_range_p));
    (void)fprintf(stdout, "bn_rnd_p  : %s\n", BN_bn2dec(bn_rnd_p));
    (void)fprintf(stdout, "\n");

    ret = 0;
Done:
    BN_free(bn_range_p);
    BN_clear_free(bn_rnd_p);
    //BN_clear_free(bn_rnd_p);  // To test double BN_*free()
    return ret;
}

/**
 * @brief Test of BN pointer initialization for BN_priv_rand_range().
 * 
 * @param test_case:  
 * @param rand_bit_len: Ransom bit length to generate. 
 * @return int32_t: 0 on success, 1 on error.
 */
#if defined(TEST_CASE)
static int32_t test_rand(uint8_t test_case, const int32_t rand_bit_len){
#else
static int32_t test_rand(const int32_t rand_bit_len){
#endif

    int32_t ret = 1, rc;

#if defined(TEST_CASE)
	(void)fprintf(stdout,"--- Test case %u for BN_priv_rand(,bits=%d,,) ---\n", test_case, rand_bit_len);
#else
	(void)fprintf(stdout,"--- Test for BN_priv_rand(,bits=%d,,) ---\n", rand_bit_len);
#endif

    // if (0 >= rand_bit_len) { return ret; }

    /* Choose One */
    //BIGNUM * const r = NULL;
    //BIGNUM * const r;
    BIGNUM * const r = BN_new();
    if (NULL==r){
        ERR_print_errors_fp(stderr);
		goto Done;
    }

    /* Choose One */
    //BIGNUM * const bn_one;
    //BIGNUM * const bn_one = NULL;
    BIGNUM * const bn_one = BN_new();
    if (NULL==bn_one){
        ERR_print_errors_fp(stderr);
		goto Done;
    }

    /* Choose One */
    //BIGNUM * bn_rnd_p;  // warned and causes segfault.
    BIGNUM * bn_rnd_p = NULL;  // warned and causes segfault.
    //bn_rnd_p = BN_new();
    bn_rnd_p = BN_secure_new();
    if (NULL==bn_rnd_p){
        ERR_print_errors_fp(stderr);
		goto Done;
    }

#if defined(TEST_CASE)
    switch(test_case){
        case 0:
            bn_rnd_p = BN_secure_new();  // OK 
            break;
        case 1:
            bn_rnd_p = NULL;  // not warned but causes segfault.
            break;
        default:
            // Uninitialized: warned and causes segfault.
            break;
    }
#endif

    rc=BN_priv_rand(bn_rnd_p, rand_bit_len, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY);
    if (1!=rc){
        ERR_print_errors_fp(stderr);
		goto Done;
    }
    /* r = (1<<rand_bit_len) - 1 */
    if(1!=BN_one(bn_one)) { goto Done; }
    if(1!=BN_lshift(r, bn_one, rand_bit_len)) { goto Done; }
    if(1!=BN_sub(r, r, bn_one)) { goto Done; }
    /* To test */
    //if(1!=BN_sub(r, bn_one, bn_one)) { goto Done; }
    //if(1!=BN_add(r, bn_one, bn_one)) { goto Done; }

    (void)fprintf(stdout, "max     : %s\n", BN_bn2dec(r));
    (void)fprintf(stdout, "bn_rnd_p: %s\n", BN_bn2dec(bn_rnd_p));
    (void)fprintf(stdout, "\n");

    ret = 0;
 Done:
    BN_free(bn_one);          //If NULL, nothing is done.
    BN_free(r);               //If NULL, nothing is done.
    BN_clear_free(bn_rnd_p);  //If NULL, nothing is done.

    return ret;
}

int32_t main(void){
    int32_t num_of_errors = 0, rc;

    (void)fprintf(stdout, "\n%s\n\n", OpenSSL_version(OPENSSL_VERSION));

    rc=test_rand_range(RAND_RANGE_DEC);
    if(0!=rc) { num_of_errors++; }
    (void)fprintf(stdout, "test_rand_range() ...... %s\n\n", 0==rc?"OK":"NG");

#if defined(TEST_CASE)
    for (int32_t i = 1; i >= -1 ; i--)
    {
        rc=test_rand(0, RAND_BIT_LEN*i);
        if(0!=rc) { num_of_errors++; }
        (void)fprintf(stdout, "test_rand()       ...... %s\n\n", 0==rc?"OK":"NG");
    }

    // test_case >= 1 causes segfault.
    uint8_t test_case_list[] = {0, 1, 2};
    for (uint8_t i = 0; i < sizeof(test_case_list); i++)
    {
        rc=test_rand(test_case_list[i], RAND_BIT_LEN);
        if(0!=rc) { num_of_errors++; }
        (void)fprintf(stdout, "test_rand()       ...... %s\n\n", 0==rc?"OK":"NG");
    }
#else
    rc=test_rand(RAND_BIT_LEN);
    if(0!=rc) { num_of_errors++; }
    (void)fprintf(stdout, "test_rand()       ...... %s\n\n", 0==rc?"OK":"NG");
#endif

    return num_of_errors;
}
