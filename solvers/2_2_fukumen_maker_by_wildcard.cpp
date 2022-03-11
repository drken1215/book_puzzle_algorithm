#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
using namespace std;

// 覆面算を解くためのクラス
class Fukumenzan {
 private:
    // 覆面算の文字列を格納した配列
    vector<string> problem_;

    // 計算のための変数
    vector<vector<int>> board_;  // 文字に数字を入れていく過程
    set<int> used_;  // すでに使用した数字の集合

    // 未確定であることを表す数字
    const int NOTIN = -1;

 public:
    // コンストラクタ
    Fukumenzan(const vector<string>& input) : problem_(input) { }

    // 覆面算を解く過程を初期化する
    void init() {
        // 文字に数字を入れていく盤面 board 全体を NOTIN で初期化する
        board_.resize(problem_.size());
        for (int i = 0; i < problem_.size(); ++i) {
            board_[i].assign(problem_[i].size(), NOTIN);
        }
        used_.clear();
    }

    // 覆面算全体の行数を返す
    int get_size() const { return problem_.size(); }

    // 覆面算全体の桁数 (最下行の桁数)　を返す
    int get_digit() const { return problem_.back().size(); }

    // 覆面算の上から row 行めの桁数を返す
    int get_digit(int row) const { return problem_[row].size(); }

    // 数字 val がすでに使われているかどうかを返す
    bool is_used(int val) const { return used_.count(val); }

    // 覆面算の上から row 行め、右から digit 桁めに入れた数字を返す
    int get_val(int row, int digit) const {
        // digit 桁めが row 行めの文字列からはみ出す場合は 0 とする
        if (digit >= get_digit(row)) return 0;
        return board_[row][get_digit(row) - 1 - digit];
    }

    // 覆面算の上から row 行め、右から digit 桁めの文字を返す
    char get_char(int row, int digit) const {
        return problem_[row][get_digit(row) - 1 - digit];
    }

    // 覆面算の計算結果を出力する
    void print() const {
        for (const vector<int>& vec : board_) {
            for (int v : vec) cout << v;
            cout << endl;
        }
    }

    // row 行め、右から digit 桁めの文字に数値 val を入れる
    void set_val(int row, int digit, int val);

    // row 行め、右から digit 桁めの文字に入れていた数字を削除する
    void reset_val(int row, int digit, int val);

    // 覆面文字に入れた数字によって計算が合うかどうかを確認する
    bool is_valid();
};

// row 行め、右から digit 桁めの文字に数字 val を入れる
void Fukumenzan::set_val(int row, int digit, int val) {
    // 該当の文字を求める
    char c = get_char(row, digit);

    // ワイルドカード文字の場合はそのマスのみ変更する
    if (c == '?') {
        board_[row][get_digit(row) - 1 - digit] = val;
        return;
    }

    // 文字が c であるすべてのマスに数字 val を入れる
    for (int r = 0; r < get_size(); ++r) {
        for (int d = 0; d < get_digit(r); ++d) {
            if (problem_[r][d] == c) board_[r][d] = val;
        }
    }
    used_.insert(val);  // 数字 val を使用済みとする
}

// row 行め、右から digit 桁めの文字に入れていた数字を削除する
void Fukumenzan::reset_val(int row, int digit, int val) {
    // 該当の文字を求める
    char c = get_char(row, digit);

    // ワイルドカード文字の場合はそのマスのみ変更する
    if (c == '?') {
        board_[row][get_digit(row) - 1 - digit] = NOTIN;
        return;
    }

    // 文字が c であるすべてのマスの数字を削除する
    for (int r = 0; r < get_size(); ++r) {
        for (int d = 0; d < get_digit(r); ++d) {
            if (problem_[r][d] == c) board_[r][d] = -1;
        }
    }
    used_.erase(val);  // 数字 val を今後使用可能とする
}

// 覆面文字に入れた数字によって計算が合うかどうかを確認する
bool Fukumenzan::is_valid() {
    // 最上桁に 0 があってはいけない
    for (const vector<int>& val : board_) {
        if (val[0] == 0) return false;
    }

    // 右から 0 桁めから順に計算していく
    int kuriagari = 0;  // 繰り上がりの値
    for (int digit = 0; digit < get_digit(); ++digit) {
        // 右から digit 桁めの、最下行以外の行の和を計算する
        int sum = 0;
        for (int row = 0; row < get_size(); ++row) {
            // まだ数字が入っていない部分に来たらその場で終了する
            if (get_val(row, digit) == NOTIN) return true;

            // 最下段以外を足す
            if (row != get_size() - 1) sum += get_val(row, digit);
        }

        // 前の桁からの繰り上がりを足して、新たな繰り上がりを計算する
        sum += kuriagari;
        kuriagari = sum / 10;

        // 最下行に来るべき数字 (sum % 10) が合っているかを確かめる
        if (sum % 10 != get_val(get_size() - 1, digit)) return false;
    }

    // 最後に繰り上がりが残ってはいけない
    return (kuriagari == 0);
}

// 深さ優先探索のための再帰関数
// 上から row 行め、右から digit 桁めに数字を入れようとする
void dfs(Fukumenzan& fu, int row, int digit, vector<Fukumenzan>& res) {
    // 終端条件: すべての文字に数字が入ったとき
    if (row == 0 && digit == fu.get_digit()) {
        res.push_back(fu);  // 答えに格納する
        return;
    }

    // 再帰呼び出しのための次のマスを求める
    int next_row = row + 1, next_digit = digit;
    if (next_row == fu.get_size()) {
        next_row = 0, next_digit = digit + 1;
    }

    // すでに数字が入っているかどうかで場合分け
    if (fu.get_val(row, digit) != -1) {
        dfs(fu, next_row, next_digit, res);
    } else {
        for (int val = 0; val <= 9; ++val) {
            // すでに使用済みの数字は使えない (ワイルドカード文字の場合は OK)
            if (fu.get_char(row, digit) != '?' && fu.is_used(val)) {
                continue;
            }

            fu.set_val(row, digit, val);  // 文字に数字を入れる
            if (fu.is_valid()) {
                dfs(fu, next_row, next_digit, res);
            }
            fu.reset_val(row, digit, val);  // 文字から数字を削除する
        }
    }
}

// 覆面算を解く関数
vector<Fukumenzan> solve(Fukumenzan& fu) {
    // 和の桁数が最大でない場合は解なしなので、空の配列を返す
    for (int i = 0; i < fu.get_size() - 1; ++i) {
        if (fu.get_digit(i) > fu.get_digit()) {
            return vector<Fukumenzan>();
        }
    }

    // 初期化する
    fu.init();

    // 深さ優先探索を開始する
    vector<Fukumenzan> res;
    dfs(fu, 0, 0, res);
    return res;
}

// 一意解となる覆面算をすべて求める
vector<vector<string>> makeup(const vector<string>& input,
                              const vector<Fukumenzan>& sols) {
    // 覆面算を分類する
    map<vector<string>, int> groups;

    for (const Fukumenzan& sol : sols) {
        // 数字から文字への対応を求める
        map<int, char> dict;
        for (int row = 0; row < input.size(); ++row) {
            for (int i = 0; i < input[row].size(); ++i) {
                // 左から i 番目の数字と文字
                int v = sol.get_val(row, input[row].size() - 1 - i);
                char c = sol.get_char(row, input[row].size() - 1 - i);
                if (c != '?') dict[v] = c;
            }
        }

        // 改めて '?' を文字に置き換えていく
        vector<string> problem(input.size(), "");
        char new_moji = 'a';
        for (int row = 0; row < input.size(); ++row) {
            for (int i = 0; i < input[row].size(); ++i) {
                int v = sol.get_val(row, input[row].size() - 1 - i);
                char c = sol.get_char(row, input[row].size() - 1 - i);

                if (c != '?') {
                    problem[row] += c;
                } else if (dict.count(v)) {
                    problem[row] += dict[v];
                } else {
                    // 初登場の数字には新たな文字を割り当てる
                    dict[v] = new_moji++;
                    problem[row] += dict[v];
                }
            }
        }
        groups[problem]++;  // 完成した覆面算を登録
    }

    // グループのメンバーが 1 個のみの覆面算を抽出する
    vector<vector<string>> res;
    for (const auto& group : groups) {
        if (group.second == 1) res.push_back(group.first);
    }
    return res;
}

int main() {
    // 入力
    cout << "Fukumenzan Input: " << endl;
    int N;  // 行数
    cin >> N;
    vector<string> input(N);
    for (int i = 0; i < N; ++i) cin >> input[i];

    // 再帰的に解く
    Fukumenzan fu(input);
    const vector<Fukumenzan>& sols = solve(fu);

    // 一意解となる覆面算を求める
    const vector<vector<string>>& res = makeup(input, sols);
    for (int i = 0; i < res.size(); ++i) {
        cout << i << " th problem: " << endl;
        for (int row = 0; row < res[i].size(); ++row) {
            cout << res[i][row];
            if (row < res[i].size() - 2) 
                cout << " + ";
            else if (row == res[i].size() - 2)
                cout << " = ";
        }
        cout << endl;
    }
}
