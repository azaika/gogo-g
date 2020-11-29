# gogo-g
5五将棋 を作る

## リンク
- [課題サイト](https://sites.google.com/site/isutbe2020/%E3%82%B0%E3%83%AB%E3%83%BC%E3%83%97%E8%AA%B2%E9%A1%8C/%E3%82%B0%E3%83%AB%E3%83%BC%E3%83%97%E8%AA%B2%E9%A1%8C2?authuser=0) (課題の説明)
- [ルール説明](http://minerva.cs.uec.ac.jp/cgi-bin/uec55shogi/wiki.cgi?page=5%B8%DE%BE%AD%B4%FD%A4%CE%A5%EB%A1%BC%A5%EB) (電通大のページ)

## 各ファイルの説明
- README.md : 今表示されているこのファイル
- .gitignore : git で無視してほしいファイルを列挙する
- source/
  - main.c  : 最初に起動される場所
  - gogo_controller.h : ゲームの全体的な処理を行う場所
  - piece.h : 駒の状態を表したりするための諸々
  - game_state.h : 盤面全体の状態を管理するための諸々
  - ai.h : CPU の思考はここに置いていこう
- build/
  - 開発が便利になる諸々を入れられたら嬉しい (今は空)

## その他
- 他の人に実装して欲しい場所には `// ToDo : 詳細` という形でコメントを書くことにしましょう
  - 逆に何をすれば良いか分からないときはとりあえず ToDo が付いた箇所を実装すると👍
- 座標 **(y, x)** を整数 `c = y*5 + x` として保持する
  - 例えば (2, 4) は `c = 2*5 + 4 = 14` である
- 逆に座標が整数 `c` で与えられたとき, `(c / 5, c % 5)` とすれば座標を復元できる
  - `c=14` ならば `(c / 5, c % 5) = (2, 4)` である
- 以下画像の**左下**を (0, 0), 右下を (0, 4), 右上を (4, 4) とする
  - 出力するときは左上である (4, 0) から順に出力していることに注意

    ![盤面の画像](http://minerva.cs.uec.ac.jp/cgi-bin/uec55shogi/wiki.cgi?page=5%B8%DE%BE%AD%B4%FD%A4%CE%A5%EB%A1%BC%A5%EB&file=syokihaiti%2Ejpg&action=ATTACH)