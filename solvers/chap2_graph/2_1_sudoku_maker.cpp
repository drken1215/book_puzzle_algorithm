#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <random>
using namespace std;

// 盤面を二次元ベクトルで表す
using Field = vector<vector<int>>;

// 数独を解くためのクラス
class Sudoku {
 private:
    Field field_;

 public:
    // コンストラクタ (未確定マスの値を -1 で表す)
    Sudoku() : field_(9, vector<int>(9, -1)) {
    }

    // filed データを返す
    const Field& get() const {
        return field_;
    }

    // マス (x, y) に数字 val を入れる
    void put(int x, int y, int val) {
        field_[x][y] = val;
    }

    // マス (x, y) の数字を削除する
    void reset(int x, int y) {
        field_[x][y] = -1;
    }

    // 入力データを受け取る
    void input() {
        for (int x = 0; x < 9; ++x) {
            string line;
            cin >> line;

            // マス (x, y) を処理する
            for (int y = 0; y < 9; ++y) {
                // 空マスの場合は -1 のままにしておく
                if (line[y] == '*') continue;

                // 数字に変換してマス (x, y) に入れる
                int val = line[y] - '0';
                put(x, y, val);
            }
        }
    }

    // 空きマスを探索する (存在しない場合は false を返す)
    bool find_empty(int& x, int& y);

    // マス (x, y) に入れられる数字の集合を返す
    vector<int> find_choices(int x, int y);
};

// 空きマスを探索する (存在しない場合は false を返す)
bool Sudoku::find_empty(int& x, int& y) {
    for (x = 0; x < 9; ++x)
        for (y = 0; y < 9; ++y)
            if (field_[x][y] == -1)
                return true;
    return false;
}

// マス (x, y) に入れられる数字の集合を返す
vector<int> Sudoku::find_choices(int x, int y) {
    // マス (x, y) と同じ行、列、ブロックにある数字を求める
    set<int> cannot_use;

    // 同じ行に含まれる数字を除外する
    for (int i = 0; i < 9; ++i)
        if (field_[x][i] != -1)
            cannot_use.insert(field_[x][i]);

    // 同じ列に含まれる数字を除外する
    for (int i = 0; i < 9; ++i)
        if (field_[i][y] != -1)
            cannot_use.insert(field_[i][y]);

    // 同じブロックに含まれる数字を除外する
    int x2 = x / 3 * 3, y2 = y / 3 * 3;  // 同じブロックの左上のマス
    for (int i = x2; i < x2 + 3; ++i)
        for (int j = y2; j < y2 + 3; ++j)
            if (field_[i][j] != -1)
                cannot_use.insert(field_[i][j]);

    // マス (x, y) に入れられる数字からなる配列を求める
    vector<int> res;
    for (int val = 1; val <= 9; ++val)
        if (!cannot_use.count(val))
            res.push_back(val);
    return res;
}

// 数独を解くための再帰関数
void dfs(Sudoku& board, vector<Field>& res, bool all = true) {
    // フラグ all が false のときは、解を 1 個求めたら抜ける
    if (!all && !res.empty()) return;

    // 空きマスを探す (存在しない場合は終端条件)
    int x, y;
    if (!board.find_empty(x, y)) {
        // 解に追加
        res.push_back(board.get());
        return;
    }

    // マス (x, y) に入れられる数字の集合を求める
    const vector<int>& can_use = board.find_choices(x, y);

    // 空きマス (x, y) に数字を順に入れていく
    for (int val : can_use) {
        board.put(x, y, val);  // マス (x, y) に数字 val を入れる
        dfs(board, res, all);  // 再帰呼び出し
        board.reset(x, y);  // マス (x, y) の数字を削除
    }
}

// 数独を解く
vector<Field> solve(Sudoku& board, bool all = true) {
    // 答えを格納した配列
    vector<Field> res;

    // 再帰関数を呼び出す
    dfs(board, res, all);
    return res;
}

// 数独の盤面を出力する関数
void print(const Sudoku& board) {
    const Field& field = board.get();
    for (int x = 0; x < 9; ++x) {
        for (int y = 0; y < 9; ++y) {
            if (field[x][y] == -1)
                cout << "*";
            else
                cout << field[x][y];
        }
        cout << endl;
    }
}

int main() {
    // 乱数のシードを固定する
    mt19937 rand_src(1);

    // 数独を入力する
    Sudoku board;
    vector<pair<int, int>> cells;  // ピンク色マスの集合
    for (int x = 0; x < 9; ++x) {
        string line;
        cin >> line;
        for (int y = 0; y < 9; ++y) {
            if (line[y] >= '0' && line[y] <= '9') {
                int val = line[y] - '0';
                board.put(x, y, val);
            } else if (line[y] == 'o') {
                cells.emplace_back(x, y);
            }
        }
    }

    // 数独を最初に解き、解の 1 つを board に入力して初期解とする
    vector<Field> res = solve(board, false);  // 解を 1 つだけ求める
    for (pair<int, int> p : cells)
        board.put(p.first, p.second, res[0][p.first][p.second]);

    // 初期盤面を改めて解く
    res = solve(board);
    int score = res.size();  // 解の個数を初期スコアとする
    cout << "initial problem: " << score << " sols" << endl;
    print(board);

    // 山登り法の実施 (10000 回実行してもダメなら打ち切る)
    for (int iter = 0; iter < 10000; ++iter) {
        // 一意解問題が見つかったら探索を打ち切る
        if (score == 1) break;

        // 新たな盤面を作る
        Sudoku board2 = board;

        // 「ピンク色マスの数字をランダムに変更する」を 2 回実施する
        for (int con = 0; con < 2; ++con) {
            // ピンク色マスをランダムに選び、一旦数字を削除する
            int id = rand_src() % cells.size();
            int x = cells[id].first, y = cells[id].second;
            board2.reset(x, y);

            // 選んだマスに入れられる数字をランダムに選んで入れる
            vector<int> can = board2.find_choices(x, y);
            int val = can[rand_src() % can.size()];
            board2.put(x, y, val);
        }

        // 新たな盤面で数独を解く
        res = solve(board2);
        int new_score = res.size();  // 新たな盤面の解の個数

        // 改善するならば置き換える
        if (new_score < score && new_score != 0) {
            cout << iter << ": " << score << " sols -> "
                 << new_score << " sols" << endl;
            board = board2;  // 盤面を更新
            score = new_score;  // スコアを更新
            print(board);  // 改善後の盤面を出力
        }
    }

    // 最終問題を出力
    cout << "final problem: " << endl;
    print(board);
}

