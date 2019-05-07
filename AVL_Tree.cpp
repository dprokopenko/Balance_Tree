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

struct node // структура для представления узлов дерева
{
	int key, X, Y;
	unsigned char height;
	node* left;
	node* right;
	node(int k) { key = k; left = right = 0; height = 1; }
};

unsigned char height(node* p)
{
	return p?p->height:0;
}

int bfactor(node* p)
{
	return height(p->right)-height(p->left);
}

void fixheight(node* p)
{
	unsigned char hl = height(p->left);
	unsigned char hr = height(p->right);
	p->height = (hl>hr?hl:hr)+1;
}

node* rotateright(node* p) // правый поворот вокруг p
{
	node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}

node* rotateleft(node* q) // левый поворот вокруг q
{
	node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}

node* balance(node* p) // балансировка узла p
{
	fixheight(p);
	if( bfactor(p)==2 )
	{
		if( bfactor(p->right) < 0 )
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if( bfactor(p)==-2 )
	{
		if( bfactor(p->left) > 0  )
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p; // балансировка не нужна
}

node* insert(node* p, int k) // вставка ключа k в дерево с корнем p
{
	if( !p ) return new node(k);
	if( k<p->key )
		p->left = insert(p->left,k);
	else
		p->right = insert(p->right,k);
	return balance(p);
}

bool exist(node *t, int k)
{
    if (t == nullptr)
        return false;
    if (t->key == k)
        return true;
    if (t->key < k)
        return exist(t->right, k);
    else
        return exist(t->left, k);
}

node* findmin(node* p) // поиск узла с минимальным ключом в дереве p
{
	return p->left?findmin(p->left):p;
}

node* removemin(node* p) // удаление узла с минимальным ключом из дерева p
{
	if( p->left==0 )
		return p->right;
	p->left = removemin(p->left);
	return balance(p);
}

node* remove(node* p, int k) // удаление ключа k из дерева p
{
	if( !p ) return 0;
	if( k < p->key )
		p->left = remove(p->left,k);
	else if( k > p->key )
		p->right = remove(p->right,k);
	else //  k == p->key
	{
		node* q = p->left;
		node* r = p->right;
		delete p;
		if( !r ) return q;
		node* min = findmin(r);
		min->right = removemin(r);
		min->left = q;
		return balance(min);
	}
	return balance(p);
}

void draw_line(node *L, node *R)
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

void draw_tree(node *t, int h)
{
    if (t == nullptr)
        return;
    if (t->left != nullptr)
        draw_tree(t->left, h + 1);
    if (t->right != nullptr)
        draw_tree(t->right, h + 1);
    int x = distance * calc[h], y = coordinate + climb * h;
    calc[h]++;
    t->X = x;
    t->Y = y;
    if (t->left != nullptr)
        draw_line(t, t->left);
    if (t->right != nullptr)
        draw_line(t, t->right);
}

void draw_vertex(node *t)
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
    text.setString(to_string(t->key));
    text.setColor(Color::Black);
    text.setCharacterSize(TextSize);
    text.setOrigin(text.getLocalBounds().left + text.getLocalBounds().width / 2.0,
                    text.getLocalBounds().top + text.getLocalBounds().height / 2.0);
    text.setPosition(x, y);
    float WidthText = (R - EPS) * sqrt(2 * (1 - cos(150 * PI / 180.0)));
    float HeightText = (R - EPS) * sqrt(2 * (1 - cos(30 * PI / 180.0)));
    if (-100 < t->key && t->key < 100)
    {
        WidthText = (R - EPS * 3) * sqrt(2 * (1 - cos(90 * PI / 180.0)));
        HeightText = (R - EPS * 3) * sqrt(2 * (1 - cos(90 * PI / 180.0)));
    }
    text.scale(WidthText / text.getLocalBounds().width, HeightText / text.getLocalBounds().height);
    window.draw(circle);
    window.draw(text);
    if (t->left != nullptr)
        draw_vertex(t->left);
    if (t->right != nullptr)
        draw_vertex(t->right);
}

int main()
{
    Clock clock;
    srand(time(nullptr));
    font.loadFromFile("Roboto-Regular.ttf");
    view.reset(FloatRect(0, 0, WIDTH, HEIGHT));
    node *t = nullptr;
    int n = 0;
    for (int i = 0; i < n; ++i)
    {
        int num = rand();
       // t = remove(t, num);
        if (!exist(t, num))
            t = insert(t, num);
    }
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
        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            view.zoom(1 + time / 1000.0);
            Speed += time / 1000.0;
            Speed = min(Speed, (float) 0.3);
        }
        if (Keyboard::isKeyPressed(Keyboard::Up))
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
          //  t = remove(t, num);
            if (!exist(t, num))
                t = insert(t, num);
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
            t = remove(t, num);
        }
        bool ok_rand = false;
        while (Keyboard::isKeyPressed(Keyboard::C))
            ok_rand = true;
        if (ok_rand)
        {
            int num = rand() * rand();
          //  t = remove(t, num);
            if (!exist(t, num))
                t = insert(t, num);
            ++n;
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

