# BN_*() 関数の実行に失敗する場合

## 症状1

以下のように BN_priv_rand_range() (または BN_rand_range() など)を実行し、

```c
rc=BN_priv_rand_range(bn_p, bn_range_p);
if (1!=rc){
    (void)fprintf(stderr, "BN_priv_rand_range failed with %d: %s.\n",
        rc, ERR_error_string(ERR_get_error(), NULL));
    goto Done;
}
```

ERR_error_string()でエラーメッセージを表示させると、以下のように表示される場合

```text
error:0308A071:bignum routines:bnrand_range:too many iterations
```

## 症状1への対処方法

```c
BIGNUM* bn_p = NULL;
```

となっている場合には、以下のように変更。

```c
BIGNUM* bn_p = BN_secure_new();
```

なお、重要度の低い BIGNUM の場合には BN_secure_new() でなく BN_new() としてもよい。

## 症状1の補足

以下の表のように、```bn_p=NULL``` の場合でも、例えば、BN_hex2bn()、BN_dec2bn() などの関数では、BIGNUM を生成し、そのアドレスを bn_p に格納してくれる。

しかしながら、そのような挙動にならない関数においては、BN_secure_new() や BN_new() で BIGNUM を初期化しておく必要がある。

Table. 1 BIGNUMポインタ型変数(bn_p)の初期化とBN_*()関数の組み合わせに対するコンパイル/実行結果の関係

| Initialization of `bn_p` | None | `Null` | `BN_new()` `BN_secure_new()` |
| :--- | :--- | :--- | :--- |
| BN_hex2bn(bn_p,) BN_dec2bn(bn_p,) etc. | Segmentation fault \*1 | OK | OK |
| BN_priv_rand_range(bn_p), BN_rand_range(bn_p) etc. | OK \*1 | Runtime error \*2 | OK  |

- OK: No runtime error.
- \*1 Compiler warns.
- \*2 Compiler does not warn.

上記の実行環境

```text
5.4.72-microsoft-standard-WSL2
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
OpenSSL 1.1.1k  25 Mar 2021
```
---

* [リストに戻る](../README.md)
