# BN_*() 関数の実行に失敗する場合

## 症状1

以下のように BN_priv_rand_range() (または BN_rand_range() など)を実行し、

```c
rc=BN_priv_rand_range(bn_p, bn_range_p);
if (1!=rc){
    (void)fprintf(stderr, "BN_priv_rand_range failed with %d: %s.\n",
        rc, ERR_error_string(ERR_get_error(), NULL));
    goto Done;  /* メモリ解放などの後処理を行って return */
}
```

ERR_error_string()でエラーメッセージを表示させると、以下のように表示される場合

```text
error:0308A071:bignum routines:bnrand_range:too many iterations
```

## 症状1への対処方法

```c
BIGNUM * bn_p = NULL;
```

となっている場合には、以下のように変更:

```c
BIGNUM * const bn_p = BN_secure_new();
```

なお、重要度の低い BIGNUM の場合には BN_new() でよい。

```c
BIGNUM * const bn_p = BN_new();
```

そして、エラー処理を追加:

```c
int32_t ret = 1;
if (NULL==bn_p){
    /* エラー処理 */
    (void)fprintf(stderr, "BN_new(): %s.\n",
        ERR_error_string(ERR_get_error(), NULL));
    goto Done;  /* メモリ解放などの後処理を行って return */
}

/* BIGNUM を使った処理 */

    ret = 0;
Done:
    BN_clear_free(bn_p);  /* 重要度の低い BIGNUM の場合には BN_free(bn_p); でよい */
    return ret;
```

## 症状1の補足

以下の表のように、```bn_p=NULL``` の場合でも、例えば、BN_hex2bn()、BN_dec2bn() などの関数では、BIGNUM を生成し、そのアドレスを bn_p に格納してくれる。

しかしながら、そのような挙動にならない関数においては、BN_secure_new() や BN_new() で BIGNUM を初期化しておく必要がある。特に、\*2 の注釈を付けた部分は、コンパイラによっては警告やエラーを出さず、runtime error となるため注意が必要。また、\*1 において正常時にはエラーとならない場合も、ポインタ型変数にゴミが入っているとエラー処理の際に double free などの問題が起こるため避けなければならない。

Table. 1 BIGNUMポインタ型変数(bn_p)の初期化とBN_*()関数の組み合わせに対するコンパイル/実行結果の関係

| Initialization of `bn_p` | None (not recommended) | `NULL` (not recommended) | `BN_new()` `BN_secure_new()` (recommended) |
| :--- | :--- | :--- | :--- |
| BN_hex2bn(&bn_p,) BN_dec2bn(&bn_p,) etc. | Segmentation fault \*1 | OK | OK |
| BN_priv_rand_range(bn_p), BN_rand_range(bn_p) etc. | OK \*1 | Runtime error \*2 | OK  |
| BN_priv_rand(bn_p), BN_rand(bn_p) etc. | OK \*1 | Segmentation fault \*2 | OK  |
| BN_zero(bn_p), BN_one(bn_p), BN_add(bn_p,), BN_sub(bn_p,), BN_lshift(bn_p,) etc. | Segmentation fault \*1 | Segmentation fault \*2 | OK  |

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

- [リストに戻る](../README.md)
