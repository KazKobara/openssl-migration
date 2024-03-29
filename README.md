# OpenSSL (libssl libcrypto) の version を 1.1未満(1.0.2以前) から 1.1 以降に変更する方法や注意点など

## 関連情報

* [脆弱性のある OpenSSL の version 情報はこちら](https://www.openssl.org/news/vulnerabilities.html)
* [OpenSSL の主要な version](https://ja.wikipedia.org/wiki/OpenSSL)
  * サポート期限切れの version (1.1.0 以下など)は、脆弱性が見つかっても原則修正が行われないため、最新版/脆弱性修正版への移行が必要です。
  * Python 3.10 以降から OpenSSL 1.1.1 以降が求められます [[PEP 644](https://peps.python.org/pep-0644/)]。

## 必要な知識

* c言語
  * 特に、構造体、constなど。

## 【推奨】 Ver.3未満用コードの修正 --link--> Ver.3以降のライブラリ

本家の [migration_guide](https://www.openssl.org/docs/man3.0/man7/migration_guide.html) をご参照下さい。

## 【推奨】 Ver1.1未満用コードの修正 --link--> Ver.1.1以降のライブラリ

Ver. 1.1 未満の libssl libcrypto を link していたレガシーコードを、Ver. 1.1 以降向けのコードに修正する場合。

エラー毎の対応策を以下で説明（他にもお薦めの対応策をお知らせ頂ければ、それらへのリンクを貼らせて頂くか、以下に集約させて頂ければと思います（ただし、作業は時間のあるときになってしまいます…））。

### コンパイルエラー毎の対応策

* [鍵構造体要素の参照に対して `"incomplete type"` エラーが出る場合](./docs/set-and-get.md)
* [`"storage size of ‘xxx’ isn’t known"` エラーが出る場合](./docs/direct-to-pointer.md)
* [`"error: invalid application of ‘sizeof’ to incomplete type ‘BIGNUM’ {aka ‘struct bignum_s’}"` が出る場合](./docs/bignum.md)
* [`"DEPRECATEDIN_"` が出る場合](./docs/deprecated.md)

### リンクエラーへの対応策

* [`"undefined reference to"` エラーが出る場合](./docs/deprecated.md)

### 実行時エラーへの対応策

* [BN_*() 関数の実行に失敗する場合](./docs/bn_runtime_error.md)

### ついでに[移行しておくとよい関数群](./docs/func.md)

---

以下【非推奨】、やむを得ない場合の応急措置となります。

## Ver1.1未満用コード --link--> Ver1.1未満のライブラリ

OS の OpenSSL ライブラリの version が 1.1 以降になったため、Ver. 1.0 以前のライブラリを入手したい場合。

Ubuntu/Debian系:

~~~txt
apt list libssl*
~~~

で入手可能なバージョンを確認し、例えば libssl1.0.2, libssl1.0.0 などを `sudo apt install` する。必要なバージョンが見つからない場合には `/etc/apt/sources.list` に以下のような行を追加し、`sudo apt update` 後に上記を試す。

~~~txt
deb http://security.ubuntu.com/ubuntu xenial-security main
~~~

CentOS/RedHat系:

~~~txt
sudo dnf install compat-openssl10-devel
~~~

* 依存関係を解決するためにインストール済みパッケージの削除を許可する必要がある場合には --allowerasing オプションを付ける。

## Ver1.1以降用コード --link--> Ver1.1未満のライブラリ

libssl, libcrypto ver. 1.1 以降を link することを想定して書いたコードに 1.0 以前を link させたい場合。例えば、実行環境を変えたら Ver. 1.1 未満のライブラリしか利用できなかった場合など。この場合は、最新コードに [OpenSSL 1.1.0 Changes](https://wiki.openssl.org/index.php/OpenSSL_1.1.0_Changes) に記載されている以下の範囲のようなコードを追加すればよい。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* リンクする OpenSSL の version が 1.1 未満の場合には、この部分が追加される。
（Ver. 1.1以降で追加された関数を Ver. 1.1未満のライブラリで実現する関数群）*/
#endif
~~~

---
[homeに戻る](https://kazkobara.github.io/)

---

## 免責事項

### 当サイトの情報の正確性について

当サイトのコンテンツや情報において、可能な限り正確な情報を掲載するよう努めています。しかし、誤情報が入り込んだり、情報が古くなったりすることもあります。必ずしも正確性を保証するものではありません。また合法性や安全性なども保証しません。

### 損害等の責任について

当サイトに掲載された内容によって生じた損害等の一切の責任を負いかねますので、ご了承ください。
また当サイトからリンクやバナーなどによって他のサイトに移動された場合、移動先サイトで提供される情報、サービス等について一切の責任も負いません。
