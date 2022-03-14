#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
using namespace std;

// 「座標」を表す型を整数値のペアとして定義する
using Cor = pair<int, int>;

// 二進法表記で表した配置において、座標 pos の情報が含まれる桁を求める
int get_digit(const Cor& pos) {
    return (pos.first * 4 + pos.second) * 4;
}

// 配置 pl の座標 pos の数字を求める
int get_val(uint64_t pl, const Cor& pos) {
    // マス (x, y) についての情報のある桁より下の桁を削除する
    pl >>= get_digit(pos);

    // 下 4 桁をとる
    return pl & 0b1111;
}

// 「配置」の入力データを受け取る
// emp には空きマスの座標が入力される
uint64_t input(Cor& emp) {
    uint64_t pl = 0;
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            uint64_t val;
            cin >> val;
            if (val == 0)
                emp = Cor(x, y);
            else
                pl += val << get_digit(Cor(x, y));
        }
    }
    return pl;
}

// 「配置」を 4 × 4 の形にして出力
void print(uint64_t pl) {
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            cout << get_val(pl, Cor(x, y)) << " ";
        }
        cout << endl;
    }
}

// 配置 pl のスライド操作後の配置を求める
// val: スライドするタイルの数字
// pos: 配置 pl における、スライドするタイルの座標
// emp: 配置 pl における、空きマスの座標
uint64_t slide(uint64_t pl, int val,
                const Cor& pos, const Cor& emp) {
    // マス pos の数字 val を削除する
    pl -= uint64_t(val) << get_digit(pos);

    // マス emp に数字 val を追加する
    pl += uint64_t(val) << get_digit(emp);
    return pl;
}

// 数字 val のタイルの、本来の位置とのマンハッタン距離を計算する
int calc_distance(int val, const Cor& pos) {
    // 目標配置におけるパネル val の座標
    int x = (val - 1) / 4, y = (val - 1) % 4;

    // マンハッタン距離を求める
    return abs(x - pos.first) + abs(y - pos.second);
}

// 最初の配置 pl の推定値を求める
int estimate(uint64_t pl) {
    int res = 0;

    // 配置 pl の座標 (x, y) のタイルを順に見ていく
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            // パネルの値
            int val = get_val(pl, Cor(x, y));

            // 空きマスはスキップ
            if (val == 0) continue;

            // マンハッタン距離を加算
            res += calc_distance(val, Cor(x, y));
        }
    }
    return res;
}

// 推定値 est の配置から、
// 数字 val のタイルを、pos から emp へスライドした配置の推定値を求める
int estimate(int est, int val, const Cor& pos, const Cor& emp) {
    return est + calc_distance(val, emp) - calc_distance(val, pos);
}

// max_depth: 深さ制限値
// depth: 現在の深さ
// pl: 現在の配置
// emp: 現在の配置 pl の空きマスの座標
// est: 現在の配置 pl の推定値
// pre_dir: 現在の配置 pl に至る直前のスライド操作の向き
// res: 配置 pl から目標配置へ至るまでの配置の遷移が格納される (逆順)
void dfs(int max_depth,  int depth,
         uint64_t pl, const Cor& emp,
         int est, int pre_dir,
         vector<uint64_t>& res) {
    // 上下左右への移動を定義しておく (0: 下、1: 右、2: 上、3: 左)
    static const int DX[4] = {1, 0, -1, 0};
    static const int DY[4] = {0, 1, 0, -1};

    // もしすでに解が見つかっていた場合は探索を打ち切る
    if (!res.empty()) return;

    // 終端条件(1): 現在の配置 pl が目標配置に一致した場合
    if (est == 0) {
        res.push_back(pl);
        return;
    }

    // 終端条件(2): 深さが限界に達した場合は、探索を打ち切る
    if (depth >= max_depth) return;

    // 上下左右へのスライド操作を順に試す
    for (int dir = 0; dir < 4; ++dir) {
        // 直前の配置に戻る操作は実施しない
        int reverse_dir = (dir + 2) % 4;
        if (reverse_dir == pre_dir) continue;

        // スライドするタイルの座標を計算し、盤面外ならスキップ
        int nx = emp.first + DX[dir];
        int ny = emp.second + DY[dir];
        Cor pos(nx, ny);
        if (nx < 0 || nx >= 4 || ny < 0 || ny >= 4) continue;

        // 新たな配置を求める
        int val = get_val(pl, pos);  // スライドするタイルの数字
        uint64_t next_pl = slide(pl, val, pos, emp);

        // 新たな配置 next_pl の推定値を求める (差分のみ計算)
        int next_est = estimate(est, val, pos, emp);

        // 「枝刈り条件」に引っ掛からなければ、探索を進める
        if (depth + next_est <= max_depth) {
            // 解が見つかった場合、res は配置の履歴を表す
            // 解が見つからなかった場合、res は空ベクトルである
            dfs(max_depth, depth + 1, next_pl, pos, next_est, dir, res);

            // 解が見つかった場合の処理
            if (!res.empty()) {
                // res の末尾に現在の配置 pl を追加する
                res.push_back(pl);
                return;
            }
        }
    }
}

// 15 パズルソルバー
// pl: 初期配置、emp: 空きマスの座標
vector<uint64_t> solve(const uint64_t& pl, const Cor& emp) {
    // 最初の推定値を計算する
    int est = estimate(pl);

    // 深さ制限値 max_depth を増やしながら深さ優先探索する
    for (int max_depth = 0; max_depth <= 80; ++max_depth) {
        vector<uint64_t> res;
        dfs(max_depth, 0, pl, emp, est, -1, res);

        // 解が見つかったら探索を打ち切る
        if (!res.empty()) {
            reverse(res.begin(), res.end());
            return res;
        }
    }
    return vector<uint64_t>();
}

int main() {
    // 入力
    cout << "15puzzle input: " << endl;
    Cor emp;  // 空きマスの座標
    uint64_t pl = input(emp);

    // 15 パズルを解く
    vector<uint64_t> res = solve(pl, emp);

    // 出力
    for (int i = 0; i < res.size(); ++i) {
        cout << "-----" << endl;
        cout << i << " th move: " << endl;
        print(res[i]);
    }
}
