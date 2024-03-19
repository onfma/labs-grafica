#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#include "glut.h"
#include <iostream>

#include <string> 
#include <sstream>
#include <cmath>

#define dim 300
float point_ratio = 300;

unsigned char prevKey;


class CartesianGrid {
public:
    CartesianGrid(int rows, int columns, float gridSize) : rows(rows), columns(columns) {
        ratio_col = 1.8 / columns;
        ratio_row = 1.8 / rows;
    }

    void drawGrid() {
        glLineWidth(0.5);

        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        for (int i = 0; i <= rows; ++i) {
            float y = i * ratio_row + init_y;
            glVertex2f(-0.9, y);
            glVertex2f(0.9, y);
        }
        for (int i = 0; i <= columns; ++i) {
            float x = i * ratio_col + init_x;
            glVertex2f(x, -0.9);
            glVertex2f(x, 0.9);
        }

        glEnd();
        float point_size = fmin(fmin(ratio_col, ratio_row) / 3 * point_ratio, 25.0);        //to prevent point overlap
        glPointSize(point_size);
        glRasterPos2d(-0.95, -0.95);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '0');
        glRasterPos2d(-0.95, 0.9);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');
        glRasterPos2d(0.9, -0.95);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

        glFlush();
    }

    void writePixel(int i, int j) {
        if (i < 0 || i >= columns || j < 0 || j >= rows) {
            std::cerr << "Error: Point coordinates (" << i << ", " << j << ") are outside the grid bounds." << std::endl;
            return;
        }

        float x = i * ratio_col + init_x;
        float y = j * ratio_row + init_y;

        glBegin(GL_POINTS);
        glVertex2f(x, y);
        glEnd();
        glFlush();
    }
    void drawLine(int x1, int y1, int x2, int y2) {
        glLineWidth(2);

        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);

        float x1_m = x1* ratio_col + init_x;
        float y1_m = y1 * ratio_row + init_y;
        float x2_m = x2 * ratio_col + init_x;
        float y2_m = y2 * ratio_row + init_y;
        glVertex2f(x1_m, y1_m);
        glVertex2f(x2_m, y2_m);

        glEnd();

        glColor3f(0.0, 0.0, 0.0);

        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            writePixel(x1, y1);

            if (x1 == x2 && y1 == y2) break;

            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
        }
    }

    void drawCircle(int radius) {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        writePixel(x, y);
        writePixel(y, x);

        while (y >= x) {
            x++;

            if (d > 0) {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else {
                d = d + 4 * x + 6;
            }

            writePixel(x, y);
            writePixel(y, x);
        }
    }

private:
    int rows;
    int columns;
    double ratio_col, ratio_row;
    double init_x = -0.9, init_y = -0.9;
};


void Display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    CartesianGrid grid(15, 15, 50);
    grid.drawGrid();
    grid.writePixel(2, 5);
    grid.drawLine(2, 5, 15, 8);
    grid.drawCircle(10);
}

void Init(void) {

    glClearColor(1.0, 1.0, 1.0, 1.0);

    glLineWidth(1);

    glPointSize(1);

    glPolygonMode(GL_FRONT, GL_LINE);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    point_ratio = fmin((GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y) {
}

int main(int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitWindowSize(dim, dim);

    glutInitWindowPosition(100, 100);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutCreateWindow(argv[0]);

    Init();

    glutReshapeFunc(Reshape);

    glutKeyboardFunc(KeyboardFunc);

    glutMouseFunc(MouseFunc);

    glutDisplayFunc(Display);

    glutMainLoop();

    return 0;
}

