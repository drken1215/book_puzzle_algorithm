#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// 文字列 S, T の編集距離を求める
int solve(const string& S, const string& T) {
    // 2 つの文字列のサイズ
    int M = S.size();
    int N = T.size();

    // 動的計画法のための配列
    // 配列全体を無限大を表す値で初期化する (ここでは N + M)
    vector<vector<int>> dp(M + 1, vector<int>(N + 1, N + M));

    // 動的計画法の初期条件
    dp[0][0] = 0;

    // 各頂点への最短距離を順に求めていく
    for (int x = 0; x <= M; ++x) {
        for (int y = 0; y <= N; ++y) {
            // 上の頂点から来る経路を考慮する
            if (x > 0) {
                dp[x][y] = min(dp[x][y], dp[x - 1][y] + 1);
            }

            // 左の頂点から来る経路を考慮する
            if (y > 0) {
                dp[x][y] = min(dp[x][y], dp[x][y - 1] + 1);
            }

            // 左上の頂点から来る経路を考慮する
            if (x > 0 && y > 0) {
                // 文字が一致する場合は辺の長さは 0 になる
                int length = 1;
                if (S[x - 1] == T[y - 1]) length = 0;

                // 更新
                dp[x][y] = min(dp[x][y], dp[x - 1][y - 1] + length);
            }
        }
    }

    // 右下頂点の値を返す
    return dp[M][N];
}

int main() {
    // 入力
    string S, T;
    cout << "First String: ";
    cin >> S;
    cout << "Second String: ";
    cin >> T;

    // 文字列 S、T の編集距離を求める
    cout << solve(S, T) << endl;
}
