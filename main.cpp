#include "saper.hpp"

int main()
{
    int size, mines;
    if (init_dialog(size, mines))
    {
        cout << "Invalid board parameters!" << endl;
        return 1;
    }

    board b(size, mines);
    cursor c(size);

    // Initialize the ncurses library
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);

    // Set mines after the first move
    bool first_move = true;

    while (true)
    {
        clear();
        b.print(c.get_x(), c.get_y());
        refresh();

        c.move();

        if (c.get_action() == 4)
            break;

        if (c.get_action() == 1 || c.get_action() == 2)
        {
            if (first_move)
            {
                b.set_mines(c.get_x(), c.get_y());
                first_move = false;
            }

            if (!b.modify_field(c.get_x(), c.get_y(), c.get_action()))
            {
                b.uncover_mines();
                clear();
                b.print(c.get_x(), c.get_y());
                refresh();
                printw("Game Over!");
                getch();
                break;
            }

            if (b.check_win())
            {
                clear();
                b.print(c.get_x(), c.get_y());
                refresh();
                printw("You Win!");
                getch();
                break;
            }
        }

        // Solve the board automatically (one move at a time)
        if (c.get_action() == 3 && !first_move)
        {
            int result = b.solver();
            if (result == 1)
            {
                clear();
                b.print(c.get_x(), c.get_y());
                if (b.check_win())
                {
                    printw("You Win!");
                    refresh();
                    getch();
                    break;
                }
                refresh();
            }
            else if (result == 2)
            {
                clear();
                b.print(c.get_x(), c.get_y());
                printw("Solver made a move.");
                refresh();
                getch();
            }
            else
            {
                clear();
                b.print(c.get_x(), c.get_y());
                printw("Game Over");
                refresh();
                getch();
                break;
            }
        }
        c.set_action(0); // Reset action
    }
    // End the ncurses session
    endwin();

    return 0;
}
