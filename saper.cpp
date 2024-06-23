#include "saper.hpp"

field::field() : flag{false}, cover(true)
{
    contents[0] = '[';
    contents[1] = ' ';
    contents[2] = ']';
    contents[3] = '\n';
}

void field::set_mine()
{
    contents[1] = 'X';
}

void field::set_contents()
{
    if (contents[1] != 'X')
    {
        if (contents[1] == ' ')
            contents[1] = '1';
        else
            contents[1]++;
    }
}

void field::set_flag(bool f)
{
    flag = f;
}

void field::set_cover(bool c)
{
    cover = c;
}

char field::get_contents()
{
    return contents[1];
}

bool field::get_flag()
{
    return flag;
}

bool field::get_cover()
{
    return cover;
}

void field::print1(int x, int y)
{
    if (cover)
    {
        attron(COLOR_PAIR(1));
        if (flag)
            mvprintw(x, y, "[F]\n");
        else
            mvprintw(x, y, "[?]\n");
        attroff(COLOR_PAIR(1));
    }
    else
    {
        attron(COLOR_PAIR(3));
        mvprintw(x, y, contents);
        attroff(COLOR_PAIR(3));
    }
}

// Pointed by cursor
void field::print2(int x, int y)
{
    if (cover)
    {
        attron(COLOR_PAIR(2));
        if (flag)
            mvprintw(x, y, "[F]\n");
        else
            mvprintw(x, y, "[?]\n");
        attroff(COLOR_PAIR(2));
    }
    else
    {
        attron(COLOR_PAIR(4));
        mvprintw(x, y, contents);
        attroff(COLOR_PAIR(4));
    }
}

board::board(unsigned int s, unsigned int m) : size(s), mines(m)
{
    arr = new field *[size];
    for (unsigned int i = 0; i < size; i++)
        arr[i] = new field[size];
}

board::~board()
{
    for (unsigned int i = 0; i < size; i++)
        delete[] arr[i];
    delete[] arr;
}

void board::print(int x, int y)
{
    int offset = 4;

    for (unsigned int i = 0; i < size; i++)
        for (unsigned int j = 0; j < size; j++)
        {
            // Pointed by cursor
            if (i == x && j == y)
            {
                arr[i][j].print2(i, j * offset);
            }
            else
            {
                arr[i][j].print1(i, j * offset);
            }
        }

    refresh();
}

void board::set_mines(int c_x, int c_y)
{
    srand(time(NULL));

    unsigned int i = 0;
    while (i < mines)
    {
        int x = rand() % size;
        int y = rand() % size;
        bool mines_free = false;

        // Guarantee first safe move
        for (int j = -1; j <= 1; j++)
            for (int k = -1; k <= 1; k++)
                if (x == c_x + j && y == c_y + k)
                {
                    mines_free = true;
                    j = 2;
                    k = 2;
                }

        if (!mines_free && arr[x][y].get_contents() != 'X')
        {
            arr[x][y].set_mine();
            ring_mine(x, y);
            i++;
        }
    }
}

void board::ring_mine(int x, int y)
{
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
        {
            int x1 = x + i, y1 = y + j;

            // Check if it isn't beyond the board
            if (x1 >= size || y1 >= size || x1 < 0 || y1 < 0)
                continue;
            else
                arr[x1][y1].set_contents();
        }
}

bool board::check_win()
{
    return uncovered == size * size - mines;
}

bool board::check_fail(int x, int y)
{
    return arr[x][y].get_contents() == 'X';
}

void board::uncover_mines()
{
    for (unsigned int i = 0; i < size; i++)
        for (unsigned int j = 0; j < size; j++)
            if (arr[i][j].get_contents() == 'X')
            {
                arr[i][j].set_flag(false);
                arr[i][j].set_cover(false);
            }
}

void board::uncover_fields(int x, int y)
{
    if (arr[x][y].get_cover())
    {
        arr[x][y].set_cover(false);
        uncovered++;

        if (arr[x][y].get_contents() == ' ')
        {
            if (x + 1 < size)
                uncover_fields(x + 1, y);
            if (y + 1 < size)
                uncover_fields(x, y + 1);
            if (x - 1 >= 0)
                uncover_fields(x - 1, y);
            if (y - 1 >= 0)
                uncover_fields(x, y - 1);
        }
    }
}

bool board::modify_field(int x, int y, int action)
{
    switch (action)
    {
    case 1:
        // Uncover the field
        if (!arr[x][y].get_flag() && arr[x][y].get_cover())
        {
            if (arr[x][y].get_contents() == 'X')
            {
                // Game Over
                uncover_mines();
                return false;
            }
            else if (arr[x][y].get_contents() == ' ')
            {
                uncover_fields(x, y);
            }
            else
            {
                arr[x][y].set_cover(false);
                uncovered++;
            }
        }
        break;
    case 2:
        // Set/Cancel the flag
        arr[x][y].set_flag(!arr[x][y].get_flag());
        break;
    default:
        break;
    }

    return true;
}

int board::solver()
{
    vector<pair<int, int>> covered_cells;
    unordered_map<int, double> probability_map;

    // Gather all covered cells
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (arr[i][j].get_cover() && !arr[i][j].get_flag())
            {
                covered_cells.push_back({i, j});
            }
        }
    }

    // Calculate probabilities
    for (auto &cell : covered_cells)
    {
        int x = cell.first;
        int y = cell.second;
        int adjacent_mines = 0;
        double probability = 0.0;

        // Count adjacet flags
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                int nx = x + i;
                int ny = y + j;
                if (nx >= 0 && nx < size && ny >= 0 && ny < size)
                {
                    if (arr[nx][ny].get_flag())
                    {
                        adjacent_mines++;
                    }
                }
            }
        }

        //Calculate probablity based on adjacent mines and covered neighbors
        probability = (double)(arr[x][y].get_contents() - '0' - adjacent_mines) / (8 - adjacent_mines);

        // Store probability for the cell
        probability_map[x * size + y] = probability;
    }

    // sort covered cells by probability in ascending order
    sort(covered_cells.begin(), covered_cells.end(), [&](const pair<int, int> &a, const pair<int, int> &b)
         { return probability_map[a.first * size + a.second] < probability_map[b.first * size + b.second]; });

    // Uncover the field with the lowest probability
    if (!covered_cells.empty())
    {
        int x = covered_cells[0].first;
        int y = covered_cells[0].second;
        if (modify_field(x, y, 1))
            return 1; 
        else 
            return 0; // GAME OVER
    }

    return 2; // No move made
}

cursor::cursor(unsigned int r) : range(r), x(0), y(0), action(0) {}

bool cursor::move()
{
    char key = getchar();

    switch (key)
    {
    case 'w':
        if (x - 1 >= 0)
            x--;
        break;
    case 's':
        if (x + 1 < range)
            x++;
        break;
    case 'a':
        if (y - 1 >= 0)
            y--;
        break;
    case 'd':
        if (y + 1 < range)
            y++;
        break;
    case '1':
        action = 1; // uncover
        break;
    case '2':
        action = 2; // set/cancel flag
        break;
    case '3':
        action = 3;
        break;
    case '4':
        action = 4;
        break;
    default:
        break;
    }

    return true;
}

int cursor::get_x()
{
    return x;
}

int cursor::get_y()
{
    return y;
}

int cursor::get_action()
{
    return action;
}

int cursor::get_range()
{
    return range;
}

void cursor::set_range(unsigned int r)
{
    range = r;
}

void cursor::set_action(int a)
{
    action = a;
}

bool init_dialog(int &size, int &mines)
{
    cout << "MINESWEEPER\n";
    cout << "**********************\n";
    cout << "Objective:\n";
    cout << "*Reveal all fields\n";
    cout << " without mines\n";
    cout << "**********************\n";
    cout << "INSTRUCTION:\n";
    cout << "You have a cursor to\n";
    cout << "move on the board.\n";
    cout << "Flag prevent against\n";
    cout << "accidental uncovering\n";
    cout << "the mine\n";
    cout << "**********************\n";
    cout << "KEY SHORTCUTS:\n";
    cout << "1. up 'w'\n";
    cout << "2. down 's'\n";
    cout << "3. left 'a'\n";
    cout << "4. right 'r'\n";
    cout << "5. reveal field '1'\n";
    cout << "6. set/cancel flag '2'\n";
    cout << "7. hint '3'\n";
    cout << "8. exit '4'\n";
    cout << "**********************\n";
    cout << "CUSTOM THE BOARD:\n";
    cout << "Enter size: ";
    cin >> size;
    cout << "Enter mine number: ";
    cin >> mines;

    return mines > size * size || size <= 0 || mines <= 0;
}
