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
            // すでに使用済みの数字は使えない
            if (fu.is_used(val)) continue;

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

// 数詞覆面算用の変数：単語から数値への対応
map<string, int> dict;

// 単語集 words から num 個の単語を選んでできる覆面算を作るための再帰関数
void rec_makeup(const vector<string>& words,
                int num,
                vector<string>& problem,
                vector<vector<string>>& res) {
    // 終端条件: 必要単語数を揃えた
    if (problem.size() == num) {
        // 数字が合わない場合はスキップ (数詞覆面算限定)
        int sum = 0;
        for (int i = 0; i < problem.size() - 1; ++i) {
            sum += dict[problem[i]];
        }
        if (sum != dict[problem.back()]) return;

        // 覆面算を解く
        Fukumenzan fu(problem);
        const vector<Fukumenzan>& sols = solve(fu);

        // 一意解ならば答えに格納する
        if (sols.size() == 1) res.push_back(problem);
        return;
    }

    // 新たな単語を加える
    for (const string& wd : words) {
        problem.push_back(wd);
        rec_makeup(words, num, problem, res);
        problem.pop_back();
    }
}

// 単語集 words から num 個の単語を選んでできる覆面算を作る
vector<vector<string>> makeup(const vector<string>& words, int num) {
    // 答えを格納する配列
    vector<vector<string>> res;

    // 再帰的に解く
    vector<string> problem;
    rec_makeup(words, num, problem, res);
    return res;
}

int main() {
    // 入力
    int num_words, num_rows;  // 単語数、行数
    cin >> num_words >> num_rows;
    vector<string> words;  // 使用してよい単語
    for (int i = 0; i < num_words; ++i) {
        string wd;
        int val;
        cin >> wd >> val;

        // 単語リストと、単語から数字への対応の更新
        words.push_back(wd);
        dict[wd] = val;
    }

    // 一意解となる覆面算を求める
    const vector<vector<string>>& res = makeup(words, num_rows);
    for (int i = 0; i < res.size(); ++i) {
        cout << i << " th problem: " << endl;
        for (int row = 0; row < res[i].size(); ++row) {
            cout << res[i][row] << " ";
        }
        cout << endl;
    }
}
