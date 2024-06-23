#ifndef SAPER_HPP
#define SAPER_HPP

#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <ncurses.h>
#include <algorithm>

using namespace std;

class field
{
    bool flag;
    bool cover;
    char contents[4];

public:
    field();
    void set_mine();
    void set_contents();
    void set_flag(bool f);
    void set_cover(bool c);
    char get_contents();
    bool get_flag();
    bool get_cover();
    void print1(int x, int y);
    void print2(int x, int y);
};

class board
{
    unsigned int size;
    unsigned int mines;
    field **arr;
    unsigned int uncovered = 0;

    void ring_mine(int x, int y);

public:
    board(unsigned int s, unsigned int m);
    ~board();
    void print(int x, int y);
    void set_mines(int c_x, int c_y);
    bool check_win();
    bool check_fail(int x, int y);
    void uncover_mines();
    void uncover_fields(int x, int y);
    bool modify_field(int x, int y, int action);
    int solver();
};

class cursor
{
    unsigned int range;
    int x, y;
    int action;

public:
    cursor(unsigned int r);
    bool move();
    int get_x();
    int get_y();
    int get_action();
    int get_range();
    void set_range(unsigned int r);
    void set_action(int a);
};

bool init_dialog(int &size, int &mines);

#endif // SAPER_HPP
