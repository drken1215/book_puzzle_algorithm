#include <iostream>
#include <vector>
#include <string>
#include <utility>
using namespace std;

// 整数 val が文字列 str に整合するか
bool is_valid(int64_t val, const string& str) {
    // 整数 val を文字列に変換する
    string sval = to_string(val);

    // 整数 val の桁数が文字列 str の長さに一致する必要がある
    if (sval.size() != str.size()) return false;

    // 1 文字ずつ整合性を確認する
    for (int i = 0; i < sval.size(); ++i) {
        // □ については不問
        if (str[i] == '*') continue;

        // 一致しない場合は false
        if (sval[i] != str[i]) return false;
    }
    return true;
}

// 数字 v を文字列 str の右から k 番めに当てはめても整合するか
bool is_valid_sub(int v, int k, const string& str) {
    // 右から k 番めの文字を取得する
    char c = str[str.size() - 1 - k];

    // 虫食いの場合は問題ない
    if (c == '*') return true;

    // たとえば c = '8' のとき、real_val = 8 になる
    int real_val = c - '0';

    // 数値が一致するかどうかを判定する
    return (v == real_val);
}

// □ に入れた数字から、整数全体を復元する
int64_t decode(const vector<int>& vec) {
    int64_t res = 0;  // 答え
    int64_t order = 1;  // 1, 10, 100, ...
    for (int v : vec) {
        res += order * v;
        order *= 10;
    }
    return res;
}

// 虫食算ソルバーを管理するクラス
class Mushikuizan {
 private:
    // 被乗数、乗数、積
    string multiplicand_, multiplier_, product_;

    // 部分積
    vector<string> middle_;

    // 答えを表す配列 (被乗数と乗数のペアを格納)
    vector<pair<int64_t, int64_t>> res_;

 public:
    // コンストラクタ
    Mushikuizan(const string& multiplicand,
                const string& multiplier,
                const string& product,
                const vector<string>& middle) :
        multiplicand_(multiplicand), multiplier_(multiplier),
        product_(product), middle_(middle) {
    }

    // 乗数の □ に数字を入れていく再帰関数
    // plicand: 被乗数に入れた数字、vec: 乗数の □ に数字を入れた結果
    void rec_plier(int64_t plicand, vector<int>& vec) {
        // 終端条件: 乗数の □ のすべてに数字が入ったとき
        if (vec.size() == multiplier_.size()) {
            // 乗数を求める
            int64_t plier = decode(vec);

            // 積の整合性を確認する
            if (!is_valid(plicand * plier, product_)) return;

            // 答えを格納する
            res_.emplace_back(plicand, plier);
            return;
        }

        // 新しい □ に数字を入れる (乗数に 0 は入らないものとする)
        for (int add = 1; add <= 9; ++add) {
            // すでに数字が入っていて矛盾している場合はスキップ
            if (!is_valid_sub(add, vec.size(), multiplier_))
                continue;

            // □ に数字 add を入れて整合性を確認する
            if (!is_valid(plicand * add, middle_[vec.size()]))
                continue;

            // vec に数字 add を加えて再帰呼び出し
            vec.push_back(add);
            rec_plier(plicand, vec);
            vec.pop_back();
        }
    }

    // 被乗数の □ に数字を入れていく関数
    // vec: 被乗数の □ に数字を入れた結果
    void rec_plicand(vector<int>& vec) {
        // 終端条件: 被乗数の □ のすべてに数字が入ったとき
        if (vec.size() == multiplicand_.size()) {
            // 今度は乗数の □ に数字を入れていく
            vector<int> vec2;
            rec_plier(decode(vec), vec2);
            return;
        }

        // 新しい □ に数字を入れる
        for (int add = 0; add <= 9; ++add) {
            // 左端に 0 は入れられない
            if (vec.size() == multiplicand_.size() - 1 && add == 0) continue;

            // すでに数字が入っていて矛盾している場合はスキップ
            if (!is_valid_sub(add, vec.size(), multiplicand_))
                continue;

            // vec に数値 add を加えて再帰呼び出し
            vec.push_back(add);
            rec_plicand(vec);
            vec.pop_back();
        }
    }

    // 虫食算を解く (被乗数と乗数のペアを格納した配列を返す)
    vector<pair<int64_t, int64_t>> solve() {
        // 答えを表す配列を空にする
        res_.clear();

        // 被乗数の □ の方から数字を入れていく
        vector<int> vec;  // 空の配列
        rec_plicand(vec);
        return res_;
    }
};

int main() {
    // 入力
    cout << "Mushikuizan Input: " << endl;
    int A, B;  // 被乗数、乗数の桁数
    cin >> A >> B;
    string hijou, jou, seki;  // 被乗数、乗数、積
    vector<string> middle(B);  // 中間部
    cin >> hijou >> jou;
    for (int i = 0; i < B; ++i) cin >> middle[i];
    cin >> seki;

    // 再帰的に解く
    Mushikuizan mu(hijou, jou, seki, middle);
    const vector<pair<int64_t, int64_t>>& res = mu.solve();

    // 解を出力
    cout << "The num of solutions: " << res.size() << endl;
    for (int i = 0; i < res.size(); ++i) {
        cout << i << " th solution: "
             << res[i].first << " * " << res[i].second
             << " = " << res[i].first * res[i].second << endl;
    }
}

