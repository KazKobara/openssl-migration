# OpenSSL (libssl libcrypto) の version を 1.1未満(1.0.2以前) から 1.1 以降に変更する方法や注意点など

## 必要な知識

* OpenSSL (主要バージョンの違いについては[こちら](https://ja.wikipedia.org/wiki/OpenSSL))
* 構造体
* const

## 「レガシーコード --link--> 新ライブラリ (lib{ssl,crypto} ver. 1.1*以降)」を実現したい場合

以前 Ver. 1.0.2 以前の libssl libcrypto を link していたレガシーコードを、Ver. 1.1 以降向けのコードにアップデートさせたい場合について、以下の状況毎に説明。

コンパイルエラー毎の対応策:

* [鍵構造体要素の参照に対して `"incomplete type"` エラーが出る場合](./docs/set-and-get.md)
* [`"storage size of ‘xxx’ isn’t known"` エラーが出る場合](./docs/direct-to-pointer.md)
* [`"error: invalid application of ‘sizeof’ to incomplete type ‘BIGNUM’ {aka ‘struct bignum_s’}"` が出る場合](./docs/bignum.md)
* [`"DEPRECATEDIN_1_1_0"` が出る場合](./docs/deprecated.md)

リンクエラーへの対応策:

* [`"undefined reference to"` エラーが出る場合](./docs/deprecated.md)

## 「レガシーコード --link--> 旧ライブラリ (lib{ssl,crypto} ver. 1.0*以降)」を実現したい場合

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

## 「最新コード --link--> 旧ライブラリ (lib{ssl,crypto} ver. 1.0.*以前)」を実現したい場合

libssl, libcrypto ver. 1.1 以降を link することを想定して書いたコード(最新コード)に 1.0 以前を link させたい場合。例えば、実行環境を変えたら Ver. 1.1 未満のライブラリしか利用できなかった場合など。この場合は、最新コードに [OpenSSL 1.1.0 Changes](https://wiki.openssl.org/index.php/OpenSSL_1.1.0_Changes) に記載されている以下の範囲のようなコードを追加すればよい。

~~~c
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* リンクする OpenSSL の version が 1.1 未満の場合には、この部分が追加される。
（Ver. 1.1以降で追加された関数を Ver. 1.1未満のライブラリで実現する関数群）*/
#endif
~~~

---

## 免責事項

### 当サイトの情報の正確性について

当サイトのコンテンツや情報において、可能な限り正確な情報を掲載するよう努めています。しかし、誤情報が入り込んだり、情報が古くなったりすることもあります。必ずしも正確性を保証するものではありません。また合法性や安全性なども保証しません。

### 損害等の責任について

当サイトに掲載された内容によって生じた損害等の一切の責任を負いかねますので、ご了承ください。
また当サイトからリンクやバナーなどによって他のサイトに移動された場合、移動先サイトで提供される情報、サービス等について一切の責任も負いません。