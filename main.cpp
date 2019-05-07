#define _GLIBCXX_USE_CXX11_ABI 0
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

#define distance _distance
#define PI 3.14159265

const int WIDTH = 1000;
const int HEIGHT = 800;
const int R = 30;
const int WidthLine = 5;
const int TextSize = 500;
const int distance = 100;
const int coordinate = 100;
const int climb = 200;
const int EPS = 5;

float Speed = 0.1;

vector <int> calc;

RenderWindow window(VideoMode(WIDTH, HEIGHT), "Trees");

Font font;

View view;

struct Tree
{
    int x, y, X, Y;
    Tree *l, *r;
    Tree(int num)
    {
        y = rand();
        x = num;
        l = nullptr;
        r = nullptr;
    }
};

pair <Tree*, Tree*> split(Tree *t, int num)
{
    if (t == nullptr) {return make_pair(t, t);}
    else
    {
        if (num <= t->x)
        {
            pair <Tree*, Tree*> p = split(t->l, num);
            t->l = p.second;
            return make_pair(p.first, t);
        }
        else
        {
            pair <Tree*, Tree*> p = split(t->r, num);
            t->r = p.first;
            return make_pair(t, p.second);
        }
    }
}

Tree *merge_tree(Tree *l, Tree *r)
{
    if (l == nullptr) {return r;}
    if (r == nullptr) {return l;}
    if (l->y > r->y)
    {
        l->r = merge_tree(l->r, r);
        return l;
    }
    else
    {
        r->l = merge_tree(l, r->l);
        return r;
    }
}

bool exists(Tree* t, int num)
{
    if (t == nullptr) {return false;}
    else
    {
        if (num == t->x) {return true;}
        if (num < t->x) {return exists(t->l, num);}
        else {return exists(t->r, num);}
    }
}

Tree *add(Tree *t, int num)
{
    if (exists(t, num))
        return t;
    pair <Tree*, Tree*> p = split(t, num);
    p.first = merge_tree(p.first, new Tree(num));
    t = merge_tree(p.first, p.second);
    return t;
}

Tree *delete_tree(Tree *t, int num)
{
    pair <Tree*, Tree*> p = split(t, num);
    pair <Tree*, Tree*> q = split(p.second, num + 1);
    t = merge_tree(p.first, q.second);
    return t;
}

void draw_line(Tree *L, Tree *R)
{
    int lenX = abs(L->X - R->X);
    int lenY = abs(L->Y - R->Y);
    int LenLine = sqrt(lenX * lenX + lenY * lenY);
    RectangleShape line;
    line.setSize(Vector2f(LenLine, WidthLine));
    line.setOrigin(LenLine, WidthLine / 2.0);
    line.setPosition(L->X, L->Y);
    line.setFillColor(Color(102, 0, 102));
    if (!lenX)
        line.rotate(-90);
    else
    {
        float angle = atan2(lenY, lenX) * 180 / PI;
        if (R->X > L->X)
            line.rotate(angle - 180);
        else
            line.rotate(-angle);
    }
    window.draw(line);
}

void draw_tree(Tree *t, int h)
{
    if (t == nullptr)
        return;
    if (t->l != nullptr)
        draw_tree(t->l, h + 1);
    if (t->r != nullptr)
        draw_tree(t->r, h + 1);
    int x = distance * calc[h], y = coordinate + climb * h;
    calc[h]++;
    t->X = x;
    t->Y = y;
    if (t->l != nullptr)
        draw_line(t, t->l);
    if (t->r != nullptr)
        draw_line(t, t->r);
}

void draw_vertex(Tree *t)
{
    if (t == nullptr)
        return;
    int x = t->X, y = t->Y;
    CircleShape circle;
    circle.setRadius(R);
    circle.setOrigin(R, R);
    circle.setPosition(x, y);
    circle.setFillColor(Color::Red);
    circle.setOutlineThickness(-3);
    circle.setOutlineColor(Color::Black);
    circle.setPointCount(100);
    Text text;
    text.setFont(font);
    text.setString(to_string(t->x));
    text.setColor(Color::Black);
    text.setCharacterSize(TextSize);
    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0,
                    text.getLocalBounds().top + text.getLocalBounds().height / 2.0);
    text.setPosition(x, y);
    float WidthText = (R - EPS) * sqrt(2 * (1 - cos(150 * PI / 180.0)));
    float HeightText = (R - EPS) * sqrt(2 * (1 - cos(30 * PI / 180.0)));
    if (-100 < t->x && t->x < 100)
    {
        WidthText = (R - EPS * 3) * sqrt(2 * (1 - cos(90 * PI / 180.0)));
        HeightText = (R - EPS * 3) * sqrt(2 * (1 - cos(90 * PI / 180.0)));
    }
    text.scale(WidthText / text.getLocalBounds().width, HeightText / text.getLocalBounds().height);
    window.draw(circle);
    window.draw(text);
    if (t->l != nullptr)
        draw_vertex(t->l);
    if (t->r != nullptr)
        draw_vertex(t->r);
}

int main()
{
    Clock clock;
    srand(time(nullptr));
    font.loadFromFile("Roboto-Regular.ttf");
    view.reset(FloatRect(0, 0, WIDTH, HEIGHT));
    Tree *t = nullptr;
    int n = 20;
    for (int i = 0; i < n; ++i)
        t = add(t, rand());
    t = add(t, 100);
    ++n;
    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time /= 800;
        Event event;
        while (window.pollEvent(event))
            if (event.type == Event::Closed)
            {
                cout << "WINDOW CLOSE" << endl;
                window.close();
            }
        if (Keyboard::isKeyPressed(Keyboard::A))
            view.move(-Speed * time, 0);
        if (Keyboard::isKeyPressed(Keyboard::D))
            view.move(Speed * time, 0);
        if (Keyboard::isKeyPressed(Keyboard::W))
            view.move(0, -Speed * time);
        if (Keyboard::isKeyPressed(Keyboard::S))
            view.move(0, Speed * time);
        if (Keyboard::isKeyPressed(Keyboard::R))
            view.rotate(1);
        if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            view.zoom(1 + time / 1000.0);
            Speed += time / 1000.0;
            Speed = min(Speed, (float) 0.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            view.zoom(1 - time / 1000.0);
            Speed -= time / 1000.0;
            Speed = max(Speed, (float) 0.01);
        }
        bool ok_insert = false;
        while (Keyboard::isKeyPressed(Keyboard::Z))
            ok_insert = true;
        if (ok_insert)
        {
            int num;
            cin >> num;
            cout << endl;
            t = add(t, num);
            ++n;
        }
        bool ok_remove = false;
        while (Keyboard::isKeyPressed(Keyboard::X))
            ok_remove = true;
        if (ok_remove)
        {
            int num;
            cin >> num;
            cout << endl;
            t = delete_tree(t, num);
        }
        window.setView(view);
        window.clear(Color::White);
        calc.clear();
        calc.resize(n);
        draw_tree(t, 0);
        draw_vertex(t);
        window.display();
    }
    return 0;
}

