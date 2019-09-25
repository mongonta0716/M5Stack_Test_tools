# M5Stack_Test_tools

M5Stackのテスト用アプリやツールを集めました。

Test Apps and tools for M5Stack.

# Contents

## BenckMark
SPIFFSとmicroSDカードのベンチマークを行うSD-Updater対応のアプリです。
ソースのconstを調整するとバイト単位でファイルサイズや試行回数を調整できます。

### Requirement

コンパイルする場合は、以下のライブラリが必要です。
* [M5Stack](https://github.com/m5stack/M5Stack)
* [M5Stack-SD-Updater](https://github.com/tobozo/M5Stack-SD-Updater)

## tools/png2code.py
pngファイルをM5Stack TFT_eSPIライブラリのSprite::pushImageで扱えるようにC言語ヘッダファイルへ変換するツールです。
Python3の実行環境が必要です。
下記のソースを参考にさせていただきました。

https://github.com/m5stack/M5Stack/tree/master/tools
https://github.com/iikagenlabo/png2img

### Requirement
使用する場合はnumpyとImageのモジュールが必要です。

```pip install numpy Image```

### Usage

変換したいpngファイルを指定します。実行後にファイル名.hのヘッダファイルが作成されます。

```python example.png```



# Licence
[MIT](https://github.com/mongonta0716/M5Stack_Test_tools/blob/master/LICENSE)

# Author
[Takao Akaki](https://twitter.com/mongonta555)
