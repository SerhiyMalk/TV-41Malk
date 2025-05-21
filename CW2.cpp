/* ----------------------------------------------------------------<Header>-
Name: CW2.cpp
Title: Grid Constraint Puzzle Solver
Group: TV-41
Student: [Serhiy]
Written: 2025-04-10
Revised: 2025-05-12
Description: Solves a diagonal grid based puzzle with numeric node constraints.
             Each cell contains either a forward (/) or backward (\) slash such that
             no cycles are formed and all numeric constraints are satisfied.
------------------------------------------------------------------</Header>-*/

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <iomanip>

using namespace std;

const int GRID_SIZE = 8;
const int EMPTY = 0;
const int FORWARD = 1;
const int BACKWARD = 2;

int g_input[GRID_SIZE + 1][GRID_SIZE + 1] = {
    {-1, 1, 1, -1, -1, -1, -1, -1, -1},                   
    {-1, -1, -1, 2, 3, 2, -1, 3, -1},
    {-1, 2, 1, 1, -1, -1, 2, -1, -1},
    {-1, 2, 3, -1, 2, -1, 2, -1, 1},
    {-1, 3, 2, 3, -1, 3, -1, 2, 1},
    {1, -1, -1, 2, 2, -1, -1, 2, -1},
    {-1, 3, -1, -1, -1, 3, 1, 2, -1},
    {1, 1, 2, -1, 1, -1, 2, 2, 1},
    {-1, -1, -1, -1, -1, -1, 1, -1, -1}
};

int g_grid[GRID_SIZE][GRID_SIZE];
int g_parent[81];

/* ---------------------------------------------------------------------[<]-
Function: find
Synopsis: Finds the root representative of a disjoint set using path compression.
          Used for detecting cycles in the connectivity graph.
---------------------------------------------------------------------[>] */
int find(int x) {
    if (g_parent[x] != x) {
        g_parent[x] = find(g_parent[x]);
    }
    return g_parent[x];
}

/* ---------------------------------------------------------------------[<]-
Function: unite
Synopsis: Unites two disjoint sets. Returns false if they were already connected
          (which implies a cycle), otherwise connects them and returns true.
---------------------------------------------------------------------[>] */
bool unite(int a, int b) {
    int ra = find(a);
    int rb = find(b);
    if (ra == rb) return false;
    g_parent[rb] = ra;
    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: check_node_constraints
Synopsis: Checks if a grid node (intersection point) satisfies the number of 
          connected slashes as per the given constraint. If 'final' is true,
          it requires exact match; otherwise, only upper-bound validation.
---------------------------------------------------------------------[>] */
bool check_node_constraints(int i, int j, bool final = false) {
    int required = g_input[i][j];
    if (required == -1) return true;

    int count = 0;
    if (i > 0 && j > 0 && g_grid[i - 1][j - 1] == BACKWARD) count++;
    if (i > 0 && j < GRID_SIZE && g_grid[i - 1][j] == FORWARD) count++;
    if (i < GRID_SIZE && j > 0 && g_grid[i][j - 1] == FORWARD) count++;
    if (i < GRID_SIZE && j < GRID_SIZE && g_grid[i][j] == BACKWARD) count++;

    if (count > required || (final && count != required)) return false;
    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: check_all_nodes
Synopsis: Applies check_node_constraints() to all intersections in the grid.
          If 'final' is true, all constraints must be exactly satisfied.
---------------------------------------------------------------------[>] */
bool check_all_nodes(bool final = false) {
    for (int i = 0; i <= GRID_SIZE; ++i) {
        for (int j = 0; j <= GRID_SIZE; ++j) {
            if (!check_node_constraints(i, j, final)) return false;
        }
    }
    return true;
}

/* ---------------------------------------------------------------------[<]-
Function: solve
Synopsis: Recursive backtracking function that attempts to fill the grid
          with forward or backward slashes such that:
          - Node constraints are not violated.
          - No cycles are formed using disjoint set union.
---------------------------------------------------------------------[>] */
bool solve(int x = 0, int y = 0) {
    if (x == GRID_SIZE) return check_all_nodes();

    int next_x = (y + 1 == GRID_SIZE) ? x + 1 : x;
    int next_y = (y + 1 == GRID_SIZE) ? 0 : y + 1;

    for (int val : {FORWARD, BACKWARD}) {
        g_grid[x][y] = val;

        for (int i = 0; i <= GRID_SIZE; ++i) {
            for (int j = 0; j <= GRID_SIZE; ++j) {
                g_parent[i * (GRID_SIZE + 1) + j] = i * (GRID_SIZE + 1) + j;
            }
        }

        bool valid = true;
        for (int i = 0; i < GRID_SIZE && valid; ++i) {
            for (int j = 0; j < GRID_SIZE && valid; ++j) {
                int a, b;
                if (g_grid[i][j] == FORWARD) {
                    a = i * (GRID_SIZE + 1) + j + 1;
                    b = (i + 1) * (GRID_SIZE + 1) + j;
                } else {
                    a = i * (GRID_SIZE + 1) + j;
                    b = (i + 1) * (GRID_SIZE + 1) + j + 1;
                }
                if (!unite(a, b)) valid = false;
            }
        }

        if (valid && check_all_nodes() && solve(next_x, next_y)) return true;
    }

    g_grid[x][y] = EMPTY;
    return false;
}

/* ---------------------------------------------------------------------[<]-
Function: print_grid
Synopsis: Prints the current state of the grid. Displays:
          - Number constraints at the intersections.
          - Diagonal slashes placed in the cells.
---------------------------------------------------------------------[>] */
void print_grid() {
    cout << "\n    ";
    for (int j = 0; j < GRID_SIZE; ++j) cout << j << "   ";
    cout << "\n";

    for (int i = 0; i <= GRID_SIZE; ++i) {
        cout << "  ";
        for (int j = 0; j <= GRID_SIZE; ++j) {
            if (g_input[i][j] == -1) cout << ".   ";
            else cout << g_input[i][j] << "   ";
        }
        cout << "\n";

        if (i == GRID_SIZE) break;
        cout << "    ";
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (g_grid[i][j] == EMPTY) cout << ".   ";
            else cout << (g_grid[i][j] == FORWARD ? "/   " : "\\   ");
        }
        cout << "\n";
    }
    cout << "\n";
}

/* ---------------------------------------------------------------------[<]-
Function: load_input
Synopsis: Allows the user to manually enter new input data for the puzzle.
          Each cell of the (9x9) input grid is entered line by line.
---------------------------------------------------------------------[>] */
void load_input() {
    cout << "Введіть новий input (рядок за рядком, 9x9, через пробіл):\n";
    for (int i = 0; i <= GRID_SIZE; ++i) {
        for (int j = 0; j <= GRID_SIZE; ++j) {
            cin >> g_input[i][j];
        }
    }
    cout << "Input оновлено.\n";
}

/* ---------------------------------------------------------------------[<]-
Function: main
Synopsis: Displays a menu-driven interface:
          1. Solve the current puzzle.
          2. Print the current grid.
          3. Load a new input configuration.
          4. Exit the program.
---------------------------------------------------------------------[>] */
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
                fill(&g_grid[0][0], &g_grid[0][0] + sizeof(g_grid) / sizeof(int), EMPTY);
                if (solve()) {
                    cout << "Розв’язок знайдено!\n";
                    print_grid();
                } else {
                    cout << "Розв’язок не знайдено.\n";
                }
                break;
            case 2:
                print_grid();
                break;
            case 3:
                load_input();
                break;
            case 4:
                cout << "До побачення!\n";
                return 0;
            default:
                cout << "Невірний вибір. Спробуйте ще раз.\n";
        }
    }
}
