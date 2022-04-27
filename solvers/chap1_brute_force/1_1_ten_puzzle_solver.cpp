#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <utility>
using namespace std;

// 逆ポーランド記法の計算式を計算する
double calc_poland(const string& exp) {
    // 計算のための配列
    vector<double> space;

    // 逆ポーランド記法 exp の各文字 c を順に見る
    for (char c : exp) {
        if (c >= '0' && c <= '9') {
            // c が数字を表す文字の場合、
            // '7' のような文字リテラルを 7 のような数値に変換する
            int add = c - '0';

            // 配列の末尾に挿入する
            space.push_back(add);
        } else {
            // c が演算子の場合、末尾から 2 つの数を取り出す
            double second = space.back();
            space.pop_back();
            double first = space.back();
            space.pop_back();

            // 演算の実施結果を配列の末尾に挿入する
            if (c == '+')
                space.push_back(first + second);
            else if (c == '-')
                space.push_back(first - second);
            else if (c == '*')
                space.push_back(first * second);
            else
                space.push_back(first / second);
        }
    }
    // 配列の末尾に残っている値を返す
    return space.back();
}

// 逆ポーランド記法の計算式から、通常の計算式を復元する
string decode_poland(const string& exp) {
    // 通常の計算式の復元のための配列
    vector<string> space;

    // 逆ポーランド記法 exp の各文字 c を順に見る
    for (char c : exp) {
        if (c >= '0' && c <= '9') {
            // 数字を表す文字 c を文字列に変換して配列の末尾に挿入する
            space.push_back({c});
        } else {
            // c が演算子の場合、末尾から 2 つの計算式を取り出す
            string second = space.back();
            space.pop_back();
            string first = space.back();
            space.pop_back();

            // 演算子が「*」「/」で、
            // 演算子の前の式が「+」「-」を含むとき括弧をつける
            if (first.find('+') != string::npos ||
                first.find('-') != string::npos) {
                if (c == '*' || c == '/') {
                    first = "(" + first + ")";
                }
            }
            
            // 演算子が「-」「*」「/」で、
            // 演算子の後の式が「+」「-」を含むとき括弧をつける
            if (second.find('+') != string::npos ||
                second.find('-') != string::npos) {
                if (c == '-' || c == '*' || c == '/') {
                    second = "(" + second + ")";
                }
            }

            // 演算子をもとに復元した計算式を配列の末尾に挿入する
            if (c == '+')
                space.push_back(first + " + " + second);
            else if (c == '-')
                space.push_back(first + " - " + second);
            else if (c == '*')
                space.push_back(first + " * " + second);
            else
                space.push_back(first + " / " + second);
        }
    }
    return space.back();
}

// テンパズルソルバー
// val: 4 つの数を格納した配列、target: 作りたい数
set<string> solve(vector<int> val, int target) {
    // 答えを表す計算式を格納する配列
    set<string> res;

    // 逆ポーランド記法の計算式 exp を試すための関数オブジェクト
    const double EPS = 1e-9;  // 十分小さい値
    auto check = [&](const string& exp) -> void {
        // 計算結果と作りたい数との差が十分小さいとき、一致とみなす
        if (abs(calc_poland(exp) - target) < EPS) {
            res.insert(decode_poland(exp));
        }
    };

    // 4 つの数 val の並び替えを順に試していく
    sort(val.begin(), val.end());  // val を辞書順最小にする
    do {
        // 4 つの数字を連結してできる文字列 fours を作る
        string fours = "";
        for (int v : val) fours += to_string(v);

        // 4^3 = 64 通りの演算子の組合せを試していく
        const string ops = "+-*/";  // 4 つの演算子
        for (char op1 : ops) {
            for (char op2 : ops) {
                for (char op3 : ops) {
                    // まず、パターン "xxxoooo" を作る
                    string exp = fours + op1 + op2 + op3;

                    // パターン "xxxxooo" を試す
                    check(exp);

                    // パターン "xxxoxoo" を試す
                    swap(exp[3], exp[4]), check(exp);

                    // パターン "xxxooxo" を試す
                    swap(exp[4], exp[5]), check(exp);

                    // パターン "xxoxoxo" を試す
                    swap(exp[2], exp[3]), check(exp);

                    // パターン "xxoxxoo" を試す
                    swap(exp[4], exp[5]), check(exp);
                }
            }
        }
    } while (next_permutation(val.begin(), val.end()));
    return res;
}

int main() {
    // 4 つの数と、作りたい数の入力
    vector<int> val(4);  // 4 つの数
    int target;  // 作りたい数
    for (int i = 0; i < 4; ++i) {
        cout << i + 1 << " th number: ";
        cin >> val[i];
    }
    cout << "target number: ";
    cin >> target;

    // テンパズルを解く
    const set<string>& res = solve(val, target);

    // 出力
    for (const string& exp : res)
        cout << exp << " = " << target << endl;
}

