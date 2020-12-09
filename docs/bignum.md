
# "error: invalid application of ‘sizeof’ to incomplete type ‘BIGNUM’ {aka ‘struct bignum_s’}" が出る場合

例えば、以下など。

~~~text
 error: invalid application of ‘sizeof’ to incomplete type ‘BIGNUM’ {aka ‘struct bignum_s’}
~~~

## 対処方法

BIGNUM変数をポインタ変数としてで宣言 (`BIGNUM *bn = NULL;` など)している場合は、`sizeof(BIGNUM)` を `sizeof(BIGNUM*)` に変更。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    sizeof(BIGNUM)
#else
    sizeof(BIGNUM*)
#endif
~~~

---

* [リストに戻る](../README.md)
