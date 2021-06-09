# コンパイル時に "storage size of ‘xxx’ isn’t known" エラーが出る場合

具体的には以下のようなエラーが出る場合、

~~~text
 error: storage size of ‘ctx’ isn’t known
~~~

例えば、以下の疑似コードのように `*_CTX` などの型に直接宣言されている object をポインタで宣言し、最後に `*_CTX_free()` するように変更する。
なお、疑似コード中の `EVP_MD_` となっている箇所は、エラー箇所の型に合わせて変更する（例えば `EVP_CIPHER_` など）。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    EVP_MD_CTX ctx;
#else
    EVP_MD_CTX *ctx = NULL;  /* ポインタ宣言に変更 */
    if (!(ctx = EVP_MD_CTX_new())) {
        /* (必要に応じて、メモリ割り当てに失敗した場合のエラー処理を入れておく) */
    };
#endif

    /* Ver 1.1 未満のレガシーコード中の &ctx は ctx に変更 */

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    EVP_MD_CTX_cleanup(&ctx); /* 廃止 */
#else
    EVP_MD_CTX_free(ctx);  /* メモリ解放を追加 */
#endif
~~~

以下は、 `HMAC_` の例。ただし、`HMAC_` の後継は `EVP_MAC_` となっているため、その[サンプルコード](https://www.openssl.org/docs/manmaster/man3/EVP_MAC_CTX_new.html)も参考にしながら  `EVP_MAC_` に更新する）。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    HMAC_CTX ctx;
#else
    HMAC_CTX *ctx = NULL;  /* ポインタ宣言に変更 */
    if (!(ctx = HMAC_CTX_new())) {
        /* (必要に応じて、メモリ割り当てに失敗した場合のエラー処理を入れておく) */
    };
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
	HMAC_CTX_init(&ctx);
    HMAC_Init(&ctx, ...);
#else
	HMAC_CTX_reset(ctx);
    HMAC_Init_ex(ctx, ..., NULL);
#endif

    /* Ver 1.1 未満のレガシーコード中の &ctx は ctx に変更 */

#if OPENSSL_VERSION_NUMBER < 0x10100000L
    HMAC_CTX_cleanup(&ctx); /* 廃止 */
#else
    HMAC_CTX_free(ctx);  /* メモリ解放を追加 */
#endif
~~~

---

* [リストに戻る](../README.md)
