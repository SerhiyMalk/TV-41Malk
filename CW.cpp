/* ----------------------------------------------------------------<Header>-
Name: HashiSolver.cpp
Title: Hashiwokakero Puzzle Solver
Group: TV-41
Student: [Serhiy]
Written: 2025-05-14
Revised: 2025-05-14
Description: Розв'язує головоломку Hashiwokakero з обмеженнями на містки:
             - Тільки горизонтальні або вертикальні мости
             - До 2 мостів між парами островів
             - Мости не повинні перетинатись
             - Кількість мостів до острова має дорівнювати його числу
------------------------------------------------------------------</Header>-*/

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
const int GRID_SIZE = 9;

int g_grid[GRID_SIZE][GRID_SIZE] = {
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

vector<Island> g_islands;
vector<Bridge> g_bridges;

/* ---------------------------------------------------------------------[<]-
Function: init_islands
Synopsis: Ініціалізує вектор островів з вихідної сітки.
---------------------------------------------------------------------[>] */
void init_islands() {
    g_islands.clear();
    for (int i = 0; i < GRID_SIZE; ++i)
        for (int j = 0; j < GRID_SIZE; ++j)
            if (g_grid[i][j] > 0)
                g_islands.push_back({i, j, g_grid[i][j]});
}

/* ---------------------------------------------------------------------[<]-
Function: in_bounds
Synopsis: Перевіряє, чи координати у межах сітки.
---------------------------------------------------------------------[>] */
bool in_bounds(int x, int y) {
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

/* ---------------------------------------------------------------------[<]-
Function: count_current_bridges
Synopsis: Повертає кількість поточних мостів, підключених до острова.
---------------------------------------------------------------------[>] */
int count_current_bridges(const Island& isl) {
    int sum = 0;
    for (auto& b : g_bridges)
        if ((b.x1 == isl.x && b.y1 == isl.y) || (b.x2 == isl.x && b.y2 == isl.y))
            sum += b.count;
    return sum;
}

/* ---------------------------------------------------------------------[<]-
Function: count_existing_bridge
Synopsis: Повертає кількість мостів між двома островами.
---------------------------------------------------------------------[>] */
int count_existing_bridge(const Island& a, const Island& b) {
    for (auto& br : g_bridges)
        if ((br.x1 == a.x && br.y1 == a.y && br.x2 == b.x && br.y2 == b.y) ||
            (br.x1 == b.x && br.y1 == b.y && br.x2 == a.x && br.y2 == a.y))
            return br.count;
    return 0;
}

/* ---------------------------------------------------------------------[<]-
Function: bridges_intersect
Synopsis: Перевіряє, чи перетинаються два мости.
---------------------------------------------------------------------[>] */
bool bridges_intersect(const Bridge& A, const Bridge& B) {
    if (A.x1 == A.x2 && B.y1 == B.y2) {
        int row = A.x1;
        int cmin = min(A.y1, A.y2), cmax = max(A.y1, A.y2);
        int col = B.y1;
        int rmin = min(B.x1, B.x2), rmax = max(B.x1, B.x2);
        return (col > cmin && col < cmax && row > rmin && row < rmax);
    }
    if (A.y1 == A.y2 && B.x1 == B.x2)
        return bridges_intersect(B, A);
    return false;
}

/* ---------------------------------------------------------------------[<]-
Function: is_valid_bridge
Synopsis: Перевіряє, чи можна встановити міст між двома островами.
---------------------------------------------------------------------[>] */
bool is_valid_bridge(Island& a, Island& b, int c) {
    if (a.x != b.x && a.y != b.y) return false;

    int ca = count_current_bridges(a), cb = count_current_bridges(b);
    if (ca + c > a.value || cb + c > b.value) return false;

    int existing = count_existing_bridge(a, b);
    if (existing + c > 2) return false;

    int dx = (b.x > a.x ? 1 : (b.x < a.x ? -1 : 0));
    int dy = (b.y > a.y ? 1 : (b.y < a.y ? -1 : 0));
    int x = a.x + dx, y = a.y + dy;
    while (x != b.x || y != b.y) {
        if (!in_bounds(x, y) || g_grid[x][y] != 0) return false;
        x += dx; y += dy;
    }

    Bridge cand = {a.x, a.y, b.x, b.y, c};
    for (auto& br : g_bridges)
        if (bridges_intersect(br, cand)) return false;

    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: solve_puzzle
Synopsis: Рекурсивно намагається знайти розв’язок.
---------------------------------------------------------------------[>] */
bool solve_puzzle(int idx) {
    if (idx == g_islands.size()) return true;
    Island& cur = g_islands[idx];
    int ca = count_current_bridges(cur);
    if (ca == cur.value) return solve_puzzle(idx + 1);

    for (int i = 0; i < g_islands.size(); ++i) {
        if (i == idx) continue;
        Island& other = g_islands[i];
        for (int c = 1; c <= 2; ++c) {
            if (ca + c > cur.value) break;
            if (is_valid_bridge(cur, other, c)) {
                bool found = false;
                for (auto& br : g_bridges) {
                    if ((br.x1 == cur.x && br.y1 == cur.y && br.x2 == other.x && br.y2 == other.y) ||
                        (br.x1 == other.x && br.y1 == other.y && br.x2 == cur.x && br.y2 == cur.y)) {
                        br.count += c;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    g_bridges.push_back({cur.x, cur.y, other.x, other.y, c});

                if (solve_puzzle(ca + c == cur.value ? idx + 1 : idx)) return true;

                for (auto it = g_bridges.begin(); it != g_bridges.end(); ++it) {
                    if ((it->x1 == cur.x && it->y1 == cur.y && it->x2 == other.x && it->y2 == other.y) ||
                        (it->x1 == other.x && it->y1 == other.y && it->x2 == cur.x && it->y2 == cur.y)) {
                        it->count -= c;
                        if (it->count == 0) g_bridges.erase(it);
                        break;
                    }
                }
            }
        }
    }
    return false;
}

/* ---------------------------------------------------------------------[<]-
Function: print_solution
Synopsis: Виводить всі з'єднання мостів.
---------------------------------------------------------------------[>] */
void print_solution() {
    cout << "\n--- МІСТКИ ---\n";
    for (auto& br : g_bridges) {
        cout << "(" << br.x1 << "," << br.y1 << ") <-> (" << br.x2 << "," << br.y2 << ")";
        if (br.count == 2) cout << " (подвійний)";
        cout << "\n";
    }
}

/* ---------------------------------------------------------------------[<]-
Function: load_custom_input
Synopsis: Дозволяє користувачу вручну ввести нову сітку.
---------------------------------------------------------------------[>] */
void load_custom_input() {
    cout << "Введіть нову сітку 9x9 (цілі числа, через пробіл):\n";
    for (int i = 0; i < GRID_SIZE; ++i)
        for (int j = 0; j < GRID_SIZE; ++j)
            cin >> g_grid[i][j];
    cout << "Сітка оновлена.\n";
}

/* ---------------------------------------------------------------------[<]-
Function: main
Synopsis: Користувацьке меню програми.
---------------------------------------------------------------------[>] */
int main() {
    while (true) {
        cout << "\n--- МЕНЮ ---\n";
        cout << "1. Знайти розв’язок\n";
        cout << "2. Вивести список містків\n";
        cout << "3. Ввести нову сітку\n";
        cout << "4. Вийти\n";
        cout << "Ваш вибір: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                g_bridges.clear();
                init_islands();
                if (solve_puzzle(0)) {
                    cout << "Розв’язок знайдено!\n";
                    print_solution();
                } else {
                    cout << "Розв’язок не знайдено.\n";
                }
                break;
            case 2:
                print_solution();
                break;
            case 3:
                load_custom_input();
                break;
            case 4:
                cout << "До побачення!\n";
                return 0;
            default:
                cout << "Невірний вибір. Спробуйте ще раз.\n";
        }
    }
}
