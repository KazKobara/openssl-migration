# ついでに移行しておくとよい関数群

`[]`内は利用可能なversionを示す。

- `BN_init()` [< 1.1.0 removed] -> `BN_new()` or `BN_secure_new()`
  - `BN_secure_new()` uses secure heap.
- `BN_free()` -> `BN_clear_free()`
  - `BN_clear_free()` overwrites the data before `BN_free()`.

- malloc 系関数 -> OPENSSL_secure_malloc*()
  - `OPENSSL_secure_clear_free()` [>= 1.1.0g]

- `BN_pseudo_rand()`, `BN_rand()` -> `BN_priv_rand()` [>= 1.1.1] for a secret.
- `BN_pseudo_rand_range()`, `BN_rand_range()` -> `BN_priv_rand_range()` [>= 1.1.1] for a secret.
- `RAND_pseudo_bytes()`, `RAND_bytes()` -> `RAND_priv_bytes()` [>= 1.1.1] for a secret.

---

- [リストに戻る](../README.md)
