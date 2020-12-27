# これはなに
NES エミュレータを作ってみたかった

# ビルド方法
```
$ mkdir ./build
$ cd build 
$ cmake ..
$ cmake --build .
```
# ディレクトリ構成
```
.
├── Include
├── Programs
│   └── src
├── Tests
│   └── TestBinaries
└── readme.md
```
## ./Include
EmotionalNES ライブラリの公開ヘッダーを格納します。
## Programs/src
EmotionalNES ライブラリのソースコードを格納します。とりあえずフラットに全部置きます。やばくなってきたら整理する……
# Tests
{単体, 統合}テストを格納します。適宜ディレクトリを切ることにします。
# Tests/TestBinaries
テスト用のバイナリを配置します。主に配布されている nes のテスト用 ROM を配置して、テストから参照します。