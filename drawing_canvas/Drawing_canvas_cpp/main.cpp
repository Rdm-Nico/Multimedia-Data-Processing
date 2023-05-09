#define _CRT_SECURE_NO_WARNINGS
#define MEM_DEBUG 
#include <stdbool.h>

#include "canvas.h"
#include "string.h"
#ifdef MEM_DEBUG
#include <crtdbg.h>
#endif
#include <stdlib.h>

/*
type_of_object parameter_1 parameter_2 ... parameter_n

rectangle 0 0 79 24 *
point 5 15 ?
circle 10 10 4 @
circle 70 10 4 @
line 40 15 40 20 |
*/

/*void draw_file(FILE *f, canvas *c)
{
	char type[30];
	while (fscanf(f, "%29s", type) == 1) {
		if (strcmp(type, "point") == 0) {
			int x, y;
			char ch;
			fscanf(f, "%d%d %c", &x, &y, &ch);
			canvas_set(c, x, y, ch);
		}
		else if (strcmp(type, "line") == 0) {
			int x0, y0, x1, y1;
			char ch;
			fscanf(f, "%d%d%d%d %c", &x0, &y0, &x1, &y1, &ch);
			canvas_line(c, x0, y0, x1, y1, ch);
		}
		else if (strcmp(type, "rectangle") == 0) {
			int x0, y0, x1, y1;
			char ch;
			fscanf(f, "%d%d%d%d %c", &x0, &y0, &x1, &y1, &ch);
			canvas_rectangle(c, x0, y0, x1, y1, ch);
		}
		else if (strcmp(type, "circle") == 0) {
			int xm, ym, r;
			char ch;
			fscanf(f, "%d%d%d %c", &xm, &ym, &r, &ch);
			canvas_circle(c, xm, ym, r, ch);
		}
	}
}*/

struct shape {
	int x_, y_;
	char ch_;
	char *name_;

	shape(int x, int y, char ch, const char *name) 
		: x_(x), y_(y), ch_(ch)
	{
		name_ = strcpy((char*)malloc(strlen(name) + 1), name);
	}
	virtual ~shape() {
		free(name_);
	}
	virtual void draw(canvas *c) const = 0; // pure virtual 
											// (rende shape una classe astratta)
};

struct point : public shape {
	point(int x, int y, char ch, const char *name)
		: shape(x, y, ch, name) {}
	
	void draw(canvas *c) const override {
		c->set(x_, y_, ch_);
	}
};

struct rectangle : public shape {
	int width_, height_;

	rectangle(int x, int y, int width, int height, char ch, const char *name) 
		: shape(x, y, ch, name), width_(width), height_(height) {}

	void draw(canvas *c) const override {
		c->rectangle(x_, y_, x_ + width_, y_ + height_, ch_);
	}
};

struct line : public shape {
	int x1_, y1_;

	line(int x, int y, int x1, int y1, char ch, const char *name)
		: shape(x, y, ch, name), x1_(x1), y1_(y1) {}

	void draw(canvas *c) const override {
		c->line(x_, y_, x1_, y1_, ch_);
	}
};

struct circle : public shape {
	int r_;

	circle(int x, int y, int r, char ch, const char *name) 
		: shape(x, y, ch, name), r_(r) {}

	void draw(canvas *c) const override {
		c->circle(x_, y_, r_, ch_);
	}
};

int main(void)
{
	canvas* c = new canvas(80, 25);

	rectangle *rect = new rectangle(0, 0, 79, 24, '*', "rettangolo1");
	circle *circ = new circle(10, 10, 4, '@', "cerchio1");
	line *ln = new line(40, 15, 40, 20, '|', "linea1");

	shape *arr[] = { rect, circ, ln};

	for (size_t i = 0; i < 3; ++i) {
		arr[i]->draw(c);
	}

	c->out(stdout);

	for (size_t i = 0; i < 3; ++i) {
		delete arr[i];
	}
	delete c;
	if (MEM_DEBUG)
	{
		_CrtDumpMemoryLeaks(); 	//controlla se ci sono memory leaks
	}
}