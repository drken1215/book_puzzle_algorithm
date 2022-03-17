#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
using namespace std;

// 演算子の種類
const int EMPTY = 0;
const int PLUS = 1;
const int MINUS = 2;
const int MUL = 3;
const int DIV = 4;

// 作りたい数
const int TARGET = 100;

// □ に入れた演算子のうち、空白部分を処理する関数
pair<vector<double>, vector<int>> calc_empty(const vector<int>& signs) {
    // 空白部分の処理後の計算式を表すデータ
    vector<double> new_vals;  // 数値
    vector<int> new_signs;  // 演算子

    // 途中経過の値 (小町算の最初の値は 1 である)
    double val = 1;

    // 演算子を順に見ていく
    for (int i = 0; i < signs.size(); ++i) {
        // 新たな数値 (i = 0, ..., 7 のとき add = 2, ..., 9)
        double add = i + 2;

        if (signs[i] == EMPTY) {
            // 空白の場合は数値を連結する (ex: 23 * 10 + 4 = 234)
            val = val * 10 + add;
        } else {
            // そうでない場合は、数値と演算子の組を新たに記録する
            new_vals.push_back(val);
            new_signs.push_back(signs[i]);

            // □ の直後の新たな値を val に記録する
            val = add;
        }
    }

    // 最後の値を push して、答えを返す
    new_vals.push_back(val);
    return make_pair(new_vals, new_signs);
}

// 掛け算、割り算の部分を計算する関数
pair<vector<double>, vector<int>>
calc_mul_div(const vector<double>& vals, const vector<int>& signs) {
    // 掛け算、割り算部分の処理後の計算式を表すデータ
    vector<double> new_vals;  // 数値
    vector<int> new_signs;  // 演算子

    // 途中経過の値
    double val = vals[0];

    // 演算子を順に見ていく
    for (int i = 0; i < signs.size(); ++i) {
        // 新たな数値
        double add = vals[i + 1];

        if (signs[i] == MUL) {
            val *= add;
        } else if (signs[i] == DIV) {
            val /= add;
        } else {
            // 掛け算、割り算でない場合は、数値と演算子の組を新たに記録する
            new_vals.push_back(val);
            new_signs.push_back(signs[i]);

            // □ の直後の新たな値を val に記録する
            val = add;
        }
    }

    // 最後の値を push して、答えを返す
    new_vals.push_back(val);
    return make_pair(new_vals, new_signs);
}

// 足し算、引き算の部分を計算する関数
double calc_plus_minus
(const vector<double>& vals, const vector<int>& signs) {
    // 答えを表す変数
    double res = vals[0];

    // 演算子を順に見ていく
    for (int i = 0; i < signs.size(); ++i) {
        // 新たな数値
        double add = vals[i + 1];

        if (signs[i] == PLUS) res += add;
        else if (signs[i] == MINUS) res -= add;
    }
    return res;
}

// sign: 符号の入れ方を表す 8 次元ベクトル
double calc(const vector<int>& signs) {
    // Step 1: 空白の連結
    pair<vector<double>, vector<int>> step1 = calc_empty(signs);

    // Step 2: 掛け算、割り算
    pair<vector<double>, vector<int>> step2
    = calc_mul_div(step1.first, step1.second);

    // Step 3: 足し算、引き算
    return calc_plus_minus(step2.first, step2.second);
}

// 数式を再現する
string decode(const vector<int>& sign) {
    string res = "1";
    for (int i = 0; i < sign.size(); ++i) {
        // 演算子を追加
        if (sign[i] == PLUS) res += " + ";
        else if (sign[i] == MINUS) res +=  " - ";
        else if (sign[i] == MUL) res += " * ";
        else if (sign[i] == DIV) res += " / ";

        // 数値を追加
        res += to_string(i + 2);
    }
    return res;
}

// 再帰関数
void rec(const vector<int>& vec, vector<string>& res) {
    // 終端条件: vec のサイズが 8
    if (vec.size() == 8) {
        // 計算結果と 100 との差が十分小さいとき一致とみなす
        const double EPS = 1e-9;  // 十分小さい値
        if (abs(calc(vec) - TARGET) < EPS)
            res.push_back(decode(vec));

        // 再帰処理を終える
        return;
    }

    // vec の末尾に 5 値を順に挿入して試す
    for (int add = 0; add < 5; ++add) {
        // vec を新しい配列 vec2 にコピー
        vector<int> vec2 = vec;

        // vec2 の末尾に add を追加
        vec2.push_back(add);

        // 再帰呼び出し
        rec(vec2, res);
    }
}

int main() {
    // 再帰関数の処理用に、空の配列を定義
    vector<int> vec;

    // 再帰的に求める
    vector<string> res;  // 解を格納する配列
    rec(vec, res);

    // 答えを出力する
    cout << "The number of solutions: " << res.size() << endl;
    for (const string& str : res)
        cout << str << " = " << TARGET << endl;
}
