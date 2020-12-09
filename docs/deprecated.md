# `"DEPRECATEDIN_"` エラーや `"undefined reference to"` エラーが出る場合が出る場合

例えば、以下など。

~~~text
note: declared here
   xx | DEPRECATEDIN_1_1_0(int RAND_pseudo_bytes(unsigned char *buf, int num))
~~~

~~~text
undefined reference to `HMAC_CTX_init'
undefined reference to `HMAC_CTX_cleanup'
undefined reference to `EVP_MD_CTX_cleanup'
~~~

## 対処方法

以下の manpage などで上記エラーが出る関数の後継となる関数や、入出力の型を確認し修正を行う。

* [最新版](https://www.openssl.org/docs/manmaster/man3/)
* [Ver. 1.1.1](https://www.openssl.org/docs/man1.1.1/man3/)
* [Ver. 1.0.2](https://www.openssl.org/docs/man1.0.2/man3/)

例えば、

* 入出力の型を変える必要がなく、かつ、変換前の文字列が一意に定まるなら、ヘッダファイルなどで #define でまとめて変換する。

~~~c
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
#define RAND_pseudo_bytes RAND_priv_bytes
#define CRYPTO_malloc_init OPENSSL_malloc_init
#endif
~~~

または、以下のように個別に場合分けする。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    CRYPTO_malloc_init();
#else
    OPENSSL_malloc_init();
#endif
~~~

* `HMAC_CTX_init` は `HMAC_CTX_init_ex` に移行させるなど `*_ex` が存在しているかを調べてみる。
* `EVP_MD_CTX_cleanup(&ctx)`は[こちら](./docs/direct-to-pointer.md)に従い`EVP_MD_CTX_free(ctx)`に変更。
* `HMAC_CTX_cleanup(&ctx)`は[こちら](./docs/direct-to-pointer.md)に従い`HMAC_CTX_free(ctx)`に変更、または、[サンプルコード](https://www.openssl.org/docs/manmaster/man3/EVP_MAC_CTX_new.html)も参考にしながら  `EVP_MAC_` に更新する。

---

* [リストに戻る](../README.md)
