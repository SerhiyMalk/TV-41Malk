#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int N = 9;
int grid[N][N] = {
    {0, 2, 0, 3, 0, 0, 2, 0, 0},
    {2, 0, 3, 0, 5, 0, 0, 0, 3},
    {0, 0, 0, 0, 0, 1, 0, 2, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 2},
    {0, 3, 0, 0, 4, 0, 0, 3, 0},
    {3, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 5, 0, 4, 0, 0},
    {0, 3, 0, 2, 0, 0, 0, 1, 0},
    {2, 0, 0, 0, 3, 0, 4, 0, 2}
};

struct Island { int x, y, value; };
struct Bridge { int x1, y1, x2, y2, count; };

vector<Island> islands;
vector<Bridge> bridges;

void initialize() {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (grid[i][j] > 0)
                islands.push_back({i, j, grid[i][j]});
}

bool inBounds(int x, int y) {
    return x >= 0 && x < N && y >= 0 && y < N;
}

int countBridges(const Island &isl) {
    int sum = 0;
    for (auto &b : bridges) {
        if ((b.x1 == isl.x && b.y1 == isl.y) || (b.x2 == isl.x && b.y2 == isl.y))
            sum += b.count;
    }
    return sum;
}

int countExistingBridge(const Island &a, const Island &b) {
    for (auto &br : bridges) {
        if ((br.x1 == a.x && br.y1 == a.y && br.x2 == b.x && br.y2 == b.y) ||
            (br.x1 == b.x && br.y1 == b.y && br.x2 == a.x && br.y2 == a.y)) {
            return br.count;
        }
    }
    return 0;
}

bool intersects(const Bridge &A, const Bridge &B) {
    if (A.x1 == A.x2 && B.y1 == B.y2) {
        int row = A.x1;
        int cmin = min(A.y1, A.y2), cmax = max(A.y1, A.y2);
        int col = B.y1;
        int rmin = min(B.x1, B.x2), rmax = max(B.x1, B.x2);
        return (col > cmin && col < cmax && row > rmin && row < rmax);
    }
    if (A.y1 == A.y2 && B.x1 == B.x2)
        return intersects(B, A);
    return false;
}

bool canPlaceBridge(Island &a, Island &b, int c) {
    if (a.x != b.x && a.y != b.y) return false;

    int ca = countBridges(a), cb = countBridges(b);
    if (ca + c > a.value || cb + c > b.value) return false;

    int existing = countExistingBridge(a, b);
    if (existing + c > 2) return false;

    int dx = (b.x > a.x ? 1 : (b.x < a.x ? -1 : 0));
    int dy = (b.y > a.y ? 1 : (b.y < a.y ? -1 : 0));
    int x = a.x + dx, y = a.y + dy;
    while (x != b.x || y != b.y) {
        if (!inBounds(x, y) || grid[x][y] != 0) return false;
        x += dx; y += dy;
    }

    Bridge cand = {a.x, a.y, b.x, b.y, c};
    for (auto &br : bridges)
        if (intersects(br, cand)) return false;

    return true;
}

bool solve(int idx) {
    if (idx == islands.size()) return true;
    Island &cur = islands[idx];
    int ca = countBridges(cur);
    if (ca == cur.value) return solve(idx + 1);

    for (int i = 0; i < islands.size(); ++i) {
        if (i == idx) continue;
        Island &oth = islands[i];
        for (int c = 1; c <= 2; ++c) {
            if (ca + c > cur.value) break;
            if (canPlaceBridge(cur, oth, c)) {
                bool found = false;
                for (auto &br : bridges) {
                    if ((br.x1 == cur.x && br.y1 == cur.y && br.x2 == oth.x && br.y2 == oth.y) ||
                        (br.x1 == oth.x && br.y1 == oth.y && br.x2 == cur.x && br.y2 == cur.y)) {
                        br.count += c;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    bridges.push_back({cur.x, cur.y, oth.x, oth.y, c});

                if (solve(ca + c == cur.value ? idx + 1 : idx)) return true;

                for (auto it = bridges.begin(); it != bridges.end(); ++it) {
                    if ((it->x1 == cur.x && it->y1 == cur.y && it->x2 == oth.x && it->y2 == oth.y) ||
                        (it->x1 == oth.x && it->y1 == oth.y && it->x2 == cur.x && it->y2 == cur.y)) {
                        it->count -= c;
                        if (it->count == 0) bridges.erase(it);
                        break;
                    }
                }
            }
        }
    }
    return false;
}

void printSolution() {
    cout << "Lines:\n";
    for (auto &br : bridges) {
        cout << "(" << br.x1 << "," << br.y1 << ") <-> (" << br.x2 << "," << br.y2 << ")";
        if (br.count == 2) cout << " twice";
        cout << "\n";
    }
}

int main() {
    initialize();
    if (solve(0)) {
        cout << "Solution found:\n";
        printSolution();
    } else {
        cout << "No solution found!\n";
    }
    return 0;
}
