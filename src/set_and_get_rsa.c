/* How to use this sample code to check the behavior of RSA_get0_key(), RSA_set0_key() and so on.

 How to compile:
 $ gcc -Wall -fstack-protector-strong -o set_and_get_rsa set_and_get_rsa.c -lcrypto 

 or with downloaded openssl under ../../openssl/:
 $ gcc -Wall -fstack-protector-strong -o set_and_get_rsa set_and_get_rsa.c -I../../openssl/include -L../../openssl -lcrypto -Wl,-rpath=../../openssl

 Run:
 $ ./set_and_get_rsa

*/
/* https://github.com/KazKobara/openssl-migration */

#include <stdio.h>
#include <string.h>
#include <openssl/engine.h>

#if OPENSSL_VERSION_NUMBER > 0x10100000L
int RSA_key_set_and_get(int keyLength, unsigned long exponent){
    int ret = 1;
    RSA *rsa_p = NULL, *rsa1_p = NULL;
    /* BIGNUM should be set to NULL after `*_put*()` since they are freed by {RSA,DSA,DH}_free() etc. */
    BIGNUM *tmp_bn1_p = NULL, *tmp_bn2_p = NULL, *bn_expo_p = NULL;
    /* const BIGNUM for `*_get*()` should not be applied by BN_free() since they are freed by {RSA,DSA,DH}_free() etc. */
    const BIGNUM *tmp_cbn2_p = NULL, *tmp_cbn3_p = NULL;
    const BIGNUM *tmp_cbn1_p = NULL;
    //const BIGNUM *tmp_cbn1_p = BN_new(); // freed by `*_get*()` and not used
    
    bn_expo_p = BN_new();
    BN_set_word(bn_expo_p, exponent);

    printf("After init:            &tmp_cbn1_p =%p, tmp_cbn1_p =%p\n", &tmp_cbn1_p, tmp_cbn1_p);
    /* key gen */
    rsa_p = RSA_new();
    if (!RSA_generate_key_ex(rsa_p, keyLength, bn_expo_p, 0)) goto Done;

    /* get */
    /* NOTE: const BIGNUMs received by get functions are freed by *_free() */
    RSA_get0_key(rsa_p, &tmp_cbn1_p, NULL, NULL);
    printf("After get:             &tmp_cbn1_p =%p, tmp_cbn1_p =%p\n", &tmp_cbn1_p, tmp_cbn1_p);
    RSA_get0_key(rsa_p, &tmp_cbn1_p, NULL, NULL);
    printf("After get again:       &tmp_cbn1_p =%p, tmp_cbn1_p =%p, *tmp_cbn1_p=%s\n", &tmp_cbn1_p, tmp_cbn1_p, BN_bn2dec(tmp_cbn1_p));
    RSA_get0_key(rsa_p, &tmp_cbn2_p, NULL, NULL);
    printf("Get to another BN:     &tmp_cbn2_p =%p, tmp_cbn2_p =%p\n", &tmp_cbn2_p, tmp_cbn2_p);

    /* set */
    tmp_bn1_p = BN_dup(tmp_cbn1_p);
    printf("After BN_dup():        &tmp_bn1_p  =%p, tmp_bn1_p  =%p\n", &tmp_bn1_p, tmp_bn1_p);
    if (!RSA_set0_key(rsa_p, tmp_bn1_p, NULL, NULL)) goto Done;
    tmp_bn1_p = NULL; /* give NULL after *_set*() */
    /* or use */
    /*
    if (!RSA_set0_key(rsa_p, BN_dup(tmp_cbn1_p), NULL, NULL)) goto Done;
    */

    /* get */
    RSA_get0_key(rsa_p, &tmp_cbn1_p, NULL, NULL);
    printf("After set the new BN:  &tmp_cbn1_p =%p, tmp_cbn1_p =%p\n", &tmp_cbn1_p, tmp_cbn1_p);

   /* key gen */
    rsa1_p = RSA_new();
    if (!RSA_generate_key_ex(rsa1_p, keyLength, bn_expo_p, 0)) goto Done;

    /* get from rsa1_p */
    RSA_get0_key(rsa1_p, &tmp_cbn3_p, NULL, NULL);
    printf("Get from new key:      &tmp_cbn3_p =%p, tmp_cbn3_p =%p\n", &tmp_cbn3_p, tmp_cbn3_p);

    /* set rsa1_p's n */
    tmp_bn2_p = BN_dup(tmp_cbn2_p);
    printf("After BN_dup():        &tmp_bn2_p  =%p, tmp_bn2_p  =%p\n", &tmp_bn2_p, tmp_bn2_p);
    if (!RSA_set0_key(rsa_p, tmp_bn2_p, NULL, NULL)) goto Done;
    tmp_bn2_p = NULL; /* give NULL after *_set*() */

    /* get */
    RSA_get0_key(rsa_p, &tmp_cbn1_p, NULL, NULL);
    printf("After set the new key: &tmp_cbn1_p =%p, tmp_cbn1_p =%p\n", &tmp_cbn1_p, tmp_cbn1_p);

    ret = 0;
 Done:
    RSA_free(rsa_p);
    RSA_free(rsa1_p);
    BN_free(tmp_bn1_p);
    BN_free(tmp_bn2_p);
    BN_free(bn_expo_p);
    /* const BIGNUM should not be freed manually */
    printf("After free           : &tmp_cbn1_p =%p, tmp_cbn1_p =%p\n", &tmp_cbn1_p, tmp_cbn1_p);
    printf("After free           : &tmp_bn1_p  =%p, tmp_bn1_p  =%p\n", &tmp_bn1_p, tmp_bn1_p);
    //BN_free(tmp_cbn1_p); // Already freed by RSA_free. Freeing 'const BIGNUM *' is warned by compiler and aborted with the message 'double free or corruption (out)'
    //BN_free(tmp_cbn3_p); // Already freed by set. Freeing 'const BIGNUM *' is warned by compiler and aborted with the message 'double free or corruption (out)'
    return ret;
}
#endif

int main(int argc, char *argv[]){
    int ret = 0;
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    int keyLength = 2048; 
    unsigned long exponent = 65537;
    if (!(ret=RSA_key_set_and_get(keyLength, exponent))) goto Done;
#else
    fprintf(stderr, "Error: use libssl > 1.1!! (OPENSSL_VERSION_NUMBER=%lx)\n", OPENSSL_VERSION_NUMBER);
    ret = 1;
#endif

 Done:
    return ret;
}
