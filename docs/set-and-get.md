# コンパイル時に鍵構造体要素の参照に対して以下のような "incomplete type" エラーが出る場合

例えば、以下など。

~~~text
error: dereferencing pointer to incomplete type ‘RSA’ {aka ‘struct rsa_st’}
error: dereferencing pointer to incomplete type ‘DSA’ {aka ‘struct dsa_st’}
error: dereferencing pointer to incomplete type ‘DH’ {aka ‘struct dh_st’}
~~~

## 対処方法

Ver. 1.1 からは、鍵要素の取り出しは `*_get*()` 関数 (`RSA_get0_key()` など) 書込み/書換えは `*_set*()` 関数(`RSA_set0_key()` など)を使う。

以下は説明のための疑似コード。実験用のコードは[こちら](../src/set_and_get.c)。

## `xxx = key->n` から `*_get*(key, )` への変更例：

~~~c
    /* 宣言 */
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    /* *_get*() で受ける BIGNUM は const にし、ポインタ定数として宣言する。 */
    const BIGNUM *cbn_to_get_p = NULL;
#endif

    /* RSA鍵の要素 n のアドレスを取り出す場合 */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    /*** compile error が出る箇所 ***/
    xxx = key->n;  /* ここで key は RSA, DSA, DHなどの鍵構造体 */
#else
    RSA_get0_key(key, &cbn_to_get_p, NULL, NULL);
    xxx = cbn_to_get_p;
#endif

    /* メモリ解放 */
    /* _get*() で受けた const BIGNUM ポインタ定数は BN_free() しないこと。*/
    /* ({RSA,DSA,DH}_free() などで解放されるため) */
    // BN_free(cbn_to_get_p)
~~~

### 補足

* const BIGNUM を `BN_free()` するとコンパイル時に"‘BN_free’ discards ‘const’ qualifier from pointer target type"などの警告が出る。
  * 警告を無視して `*_get*()` で受けた const BIGNUM ポインタ定数を BN_free() すると、実行時に double free エラーとなる。


## `key->n = yyy` から `*_set*(key, )` への変更例：

~~~c
    /* 宣言 */
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    /* *_set*() で渡す BIGNUM には const は付けず、ポインタ変数として宣言し、NULL を入れておく。 */
    BIGNUM *bn_to_set_p = NULL;  /* *_set*()用 */
#endif
    BIGNUM *bn_p = NULL;         /* *_set*()以外用のBIGNUMポインタ変数 */

    /* RSA鍵の要素 n を更新する場合 */
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    /*** compile error が出る箇所 ***/
    key->n = yyy;  /* ここで key は RSA, DSA, DHなどの鍵構造体 */
#else
    bn_to_set_p = yyy;
    RSA_set0_key(key, bn_to_set_p, NULL, NULL);
    /* _set*() で渡した BIGNUM ポインタ変数は ({RSA,DSA,DH}_free() などで解放されるため)、使用後のポインタに NULL を入れておくこと(もし、入れないのであれば以降で BN_free()しないこと)。*/
    bn_to_set_p = NULL
#endif

    /* メモリ解放 */
    /* memory leak を防止するため、宣言された(constでない) BIGNUM ポインタ変数はいずれも BN_free() されいる(かNULLとなっている)こと。 */
    BN_free(bn_p)
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    BN_free(bn_to_set_p)  /* 使用後 NULL を入れておく(or BN_free()しない)こと。 */
#endif
~~~

### 補足

* `*_put*()` すると既に保持されている鍵要素用のメモリは free される。

---

* [リストに戻る](../README.md)
