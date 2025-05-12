#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <iomanip>

using namespace std;

const int N = 8;
const int EMPTY = 0, FORWARD = 1, BACKWARD = 2;

int input[9][9] = {
    {-1, -1, -1, 1, -1, -1, -1, -1, -1},
    {-1, 2, 1, -1, 1, 3, -1, 3, -1},
    {-1, 1, -1, 1, 1, -1, 2, -1, 1},
    {1, -1, -1, -1, -1, 1, -1, 2, -1},
    {1, -1, 2, 3, -1, 2, 2, -1, 1},
    {-1, 2, -1, 3, 1, 2, 1, 2, -1},
    {1, -1, 1, -1, 3, 2, 1, -1, -1},
    {-1, 1, -1, 2, -1, -1, -1, 1, -1},
    {-1, -1, -1, -1, 1, -1, 1, 1, -1}
};

int grid[N][N];
int parent[81];

int find(int x) {
    if (parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
}

bool unite(int a, int b) {
    int ra = find(a), rb = find(b);
    if (ra == rb) return false;
    parent[rb] = ra;
    return true;
}

bool checkNodeConstraints(int i, int j, bool final = false) {
    int req = input[i][j];
    if (req == -1) return true;

    int cnt = 0;

    if (i > 0 && j > 0 && grid[i - 1][j - 1] == BACKWARD) cnt++;
    if (i > 0 && j < N && grid[i - 1][j] == FORWARD) cnt++;
    if (i < N && j > 0 && grid[i][j - 1] == FORWARD) cnt++;
    if (i < N && j < N && grid[i][j] == BACKWARD) cnt++;

    if (cnt > req || (final && cnt != req)) return false;
    return true;
}

bool checkAllNodes(bool final = false) {
    for (int i = 0; i <= N; ++i)
        for (int j = 0; j <= N; ++j)
            if (!checkNodeConstraints(i, j, final)) return false;
    return true;
}

bool solve(int x = 0, int y = 0) {
    if (x == N) return checkAllNodes(true);

    int nx = (y + 1 == N) ? x + 1 : x;
    int ny = (y + 1 == N) ? 0 : y + 1;

    for (int val : {FORWARD, BACKWARD}) {
        grid[x][y] = val;

        for (int i = 0; i <= N; ++i)
            for (int j = 0; j <= N; ++j)
                parent[i * (N + 1) + j] = i * (N + 1) + j;

        bool valid = true;
        for (int i = 0; i < N && valid; ++i) {
            for (int j = 0; j < N && valid; ++j) {
                int a, b;
                if (grid[i][j] == FORWARD) {
                    a = i * (N + 1) + j + 1;
                    b = (i + 1) * (N + 1) + j;
                } else {
                    a = i * (N + 1) + j;
                    b = (i + 1) * (N + 1) + j + 1;
                }
                if (!unite(a, b)) valid = false;
            }
        }

        if (valid && checkAllNodes() && solve(nx, ny)) return true;
    }

    grid[x][y] = EMPTY;
    return false;
}

void printGrid() {
    cout << "\n    ";
    for (int j = 0; j < N; ++j) cout << j << "   ";
    cout << "\n";

    for (int i = 0; i <= N; ++i) {
        cout << "  ";
        for (int j = 0; j <= N; ++j) {
            if (input[i][j] == -1) cout << ".   ";
            else cout << input[i][j] << "   ";
        }
        cout << "\n";

        if (i == N) break;
        cout << "    ";
        for (int j = 0; j < N; ++j) {
            if (grid[i][j] == EMPTY) cout << ".   ";
            else cout << (grid[i][j] == FORWARD ? "/" : "\\") << "   ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void loadInput() {
    cout << "Введіть новий input (рядок за рядком, 9x9, через пробіл):\n";
    for (int i = 0; i <= N; ++i)
        for (int j = 0; j <= N; ++j)
            cin >> input[i][j];
    cout << "Input оновлено.\n";
}

int main() {
    while (true) {
        cout << "\n--- МЕНЮ ---\n";
        cout << "1. Знайти розв’язок\n";
        cout << "2. Вивести поточну сітку\n";
        cout << "3. Завантажити новий input\n";
        cout << "4. Вийти\n";
        cout << "Ваш вибір: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                fill(&grid[0][0], &grid[0][0] + sizeof(grid)/sizeof(int), EMPTY);
                if (solve()) {
                    cout << "Розв’язок знайдено!\n";
                    printGrid();
                } else {
                    cout << "Розв’язок не знайдено.\n";
                }
                break;
            case 2:
                printGrid();
                break;
            case 3:
                loadInput();
                break;
            case 4:
                cout << "До побачення!\n";
                return 0;
            default:
                cout << "Невірний вибір. Спробуйте ще раз.\n";
        }
    }
}
