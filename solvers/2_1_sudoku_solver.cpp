#include <iostream>
#include <vector>
#include <string>
#include <set>
using namespace std;

// 盤面を二次元ベクトルで表す
using Field = vector<vector<int>>;

// 数独を解くためのクラス
class Sudoku {
 private:
    Field field;

 public:
    // コンストラクタ (未確定マスの値を -1 で表す)
    Sudoku() : field(9, vector<int>(9, -1)) {
    }

    // filed データを返す
    const Field& get() {
        return field;
    }

    // マス (x, y) に数字 val を入れる
    void put(int x, int y, int val) {
        field[x][y] = val;
    }

    // マス (x, y) の数字を削除する
    void reset(int x, int y) {
        field[x][y] = -1;
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
            if (field[x][y] == -1)
                return true;
    return false;
}

// マス (x, y) に入れられる数字の集合を返す
vector<int> Sudoku::find_choices(int x, int y) {
    // マス (x, y) と同じ行、列、ブロックにある数字を求める
    set<int> cannot_use;

    // 同じ行に含まれる数字を除外する
    for (int i = 0; i < 9; ++i)
        if (field[x][i] != -1)
            cannot_use.insert(field[x][i]);

    // 同じ列に含まれる数字を除外する
    for (int i = 0; i < 9; ++i)
        if (field[i][y] != -1)
            cannot_use.insert(field[i][y]);

    // 同じブロックに含まれる数字を除外する
    int x2 = x / 3 * 3, y2 = y / 3 * 3;  // 同じブロックの左上のマス
    for (int i = x2; i < x2 + 3; ++i)
        for (int j = y2; j < y2 + 3; ++j)
            if (field[i][j] != -1)
                cannot_use.insert(field[i][j]);

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

int main() {
    // 数独を入力する
    cout << "Sudoku Input: " << endl;
    Sudoku board;
    board.input();

    // 数独を解く
    vector<Field> res = solve(board);

    // 解を出力する
    if (res.size() == 0) {
        cout << "No solutions." << endl;
    } else if (res.size() > 1) {
        cout << "More than one solution." << endl;
    } else {
        const Field& answer = res[0];
        for (int x = 0; x < 9; ++x) {
            for (int y = 0; y < 9; ++y)
                cout << answer[x][y] << " ";
            cout << endl;
        }
    }
}
