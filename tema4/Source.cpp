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
#include <vector>

#define dim 500
float point_ratio = 500;
double w = 500, h = 500;

unsigned char prevKey;


class CartesianGrid {
public:
    int size = 0;

    CartesianGrid(int size){
        if (size <= 0 || size != static_cast<int>(size)) {
            std::cerr << "Error: Size must be a positive integer." << std::endl;
        }
        else {
            this->size = size;
            ratio_col = fmin(((2 - 4 * epsilon / w) / size), 2 * ((h / w) - (epsilon * 2 / w)) / size);
            ratio_row = fmin(((2 - 4 * epsilon / h) / size), 2 * ((w / h) - (epsilon * 2 / h)) / size);
        }
    }

    void drawGrid() {
        glLineWidth(0.5);

        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
        double max_x, max_y;
        max_x = init_x + ratio_col * size;
        max_y = init_y + ratio_row * size;
        for (int i = 0; i <= size; ++i) {
            float y = i * ratio_row + init_y ;
            glVertex2f(init_x, y);
            glVertex2f(max_x, y);
        }
        for (int i = 0; i <= size; ++i) {
            float x = i * ratio_col + init_x;
            glVertex2f(x, init_y);
            glVertex2f(x, max_y);
        }

        glEnd();
        float point_size = fmin(fmax(ratio_col, ratio_row)/ 3 * point_ratio, 25.0);     //to avoid point itersection
        glPointSize(point_size);
        glRasterPos2d(init_x - epsilon / w, init_y - epsilon/h);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '0');
        glRasterPos2d(init_x - epsilon / w, max_y);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');
        glRasterPos2d(max_x, init_y - epsilon / h);
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

        glFlush();
    }

    void writePixel(int i, int j) {
        if (i < 0 || i > size || j < 0 || j > size || i != static_cast<int>(i) || j != static_cast<int>(j)) {
            std::cerr << "Error: Point coordinates (" << i << ", " << j << ") values are not right." << std::endl;
            return;
        }

        float x = i * ratio_col + init_x;
        float y = j * ratio_row + init_y;

        glBegin(GL_POINTS);
        glVertex2f(x, y);
        glEnd();
        glFlush();
    }
    void drawMartor(int x1, int y1, int x2, int y2) {
        glLineWidth(2);
        glColor3f(1.0, 0.0, 0.0);

        glBegin(GL_LINES);
        float x1_d, y1_d, x2_d, y2_d;
        x1_d = x1 * ratio_col + init_x;
        y1_d = y1 * ratio_row + init_y;
        x2_d = x2 * ratio_col + init_x;
        y2_d = y2 * ratio_row + init_y;
        glVertex2f(x1_d, y1_d);
        glVertex2f(x2_d, y2_d);
        glEnd();

        glColor3f(0.0, 0.0, 0.0);
    }
    void drawLine(int x1, int y1, int x2, int y2, int thickness) {
        drawMartor(x1, y1, x2, y2);

        int dx = x2 - x1;
        int dy = y2 - y1;

        int sx = (dx > 0) ? 1 : -1;
        int sy = (dy > 0) ? 1 : -1;

        dx = abs(dx);
        dy = abs(dy);

        if (dx >= dy) { 
            int dE = 2 * dy;
            int dNE = 2 * (dy - dx);
            int d = 2 * dy - dx;
            int x = x1, y = y1;
            while (x != x2 + sx) {
                for (int i = -thickness / 2; i <= thickness / 2; ++i) {
                    writePixel(x, y + i);
                }

                if (d <= 0) {
                    d += dE;
                }
                else {
                    d += dNE;
                    y += sy;
                }
                x += sx;
            }
        }
        else { 
            int dN = 2 * dx;
            int dEN = 2 * (dx - dy);
            int d = 2 * dx - dy;
            int x = x1, y = y1;
            while (y != y2 + sy) {
                for (int i = -thickness / 2; i <= thickness / 2; ++i) {
                    writePixel(x + i, y);
                }

                if (d <= 0) {
                    d += dN;
                }
                else {
                    d += dEN;
                    x += sx;
                }
                y += sy;
            }
        }
    }
    void drawCircleMartor(int radius, int cx = 0, int cy = 0) {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 1000; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(1000);
            float x = (radius * ratio_col) * cosf(theta) + (cx * ratio_col + init_x);
            float y = (radius * ratio_row) * sinf(theta) + (cy * ratio_row + init_y);
            glVertex2f(x, y);
        }
        glEnd();
        glColor3f(0.0, 0.0, 0.0);
    }

    void drawCircle(int radius, int cx = 0, int cy = 0, int thickness = 1) {
        drawCircleMartor(radius, cx, cy);
        writePixel(cx, cy);

        //SE
        int x = 0, y = radius;
        int d = 1 - radius;
        int dE = 3;
        int dSE = -2 * radius + 5;
        while (x <= y) {
            for (int i = x - thickness / 2; i <= x + thickness / 2; i++) {
                for (int j = y - thickness / 2; j <= y + thickness / 2; j++) {
                    writePixel(cx + i, cy + j);
                    writePixel(cx + j, cy + i);
                }
            }
            if (d < 0) {
                d += dE;
                dE += 2;
                dSE += 2;
            }
            else {
                d += dSE;
                dSE += 4;
                y--;
            }
            x++;
        }

        //SW
        x = radius;
        y = 0;
        d = 1 - radius;
        int dW = 3;
        int dSW = -2 * radius + 5;
        while (-y <= x) {
            for (int i = x - thickness / 2; i <= x + thickness / 2; i++) {
                for (int j = y - thickness / 2; j <= y + thickness / 2; j++) {
                    writePixel(cx + i, cy + j);
                    writePixel(cx - j, cy - i);
                }
            }
            if (d < 0) {
                d += dW;
                dW += 2;
                dSW += 2;
            }
            else {
                d += dSW;
                dSW += 4;
                x--;
            }
            y--;
        }

        //NW
        d = 1 - radius;
        x = 0;
        y = -radius;
        dW = 3;
        int dNW = -2 * radius + 5;
        while (x >= y) {
            for (int i = x - thickness / 2; i <= x + thickness / 2; i++) {
                for (int j = y - thickness / 2; j <= y + thickness / 2; j++) {
                    writePixel(cx + i, cy + j);
                    writePixel(cx + j, cy + i);
                }
            }
            if (d < 0) {
                d += dW;
                dW += 2;
                dNW += 2;
            }
            else {
                d += dNW;
                dW += 2;
                dNW += 4;
                y++;
            }
            x--;
        }

        //NE
        d = 1 - radius;
        x = -radius;
        y = 0;
        dE = 3;
        int dNE = -2 * radius + 5;
        while (x <= -y) {
            for (int i = x - thickness / 2; i <= x + thickness / 2; i++) {
                for (int j = y - thickness / 2; j <= y + thickness / 2; j++) {
                    writePixel(cx + i, cy + j);
                    writePixel(cx - j, cy - i);
                }
            }
            if (d < 0) {
                d += dE;
                dE += 2;
                dNE += 2;
            }
            else {
                d += dNE;
                dE += 2;
                dNE += 4;
                x++;
            }
            y++;
        }
    }

private:
    double ratio_col = 0;
    double ratio_row = 0;
    double epsilon = 25;
    double init_x = -1 + epsilon * 2 / w, init_y = -1 + epsilon * 2 / h;
};

void Display1() {
    glClear(GL_COLOR_BUFFER_BIT);
    CartesianGrid grid(15);
    grid.drawGrid();

    grid.drawLine(0, 0, 15, 7, 1);
    grid.drawLine(0, 15, 15, 10, 3);
}

void Display2() {
    glClear(GL_COLOR_BUFFER_BIT);
    CartesianGrid grid(15);
    grid.drawGrid();

    grid.drawCircle(13);
}
void Display3() {
    glClear(GL_COLOR_BUFFER_BIT);
    CartesianGrid grid(70);
    grid.drawGrid();

    grid.drawCircle(20, 30, 40, 2);
}

void Display4() {
    glClear(GL_COLOR_BUFFER_BIT);
    CartesianGrid grid(50);
    grid.drawGrid();

    grid.drawLine(0, 0, 15, 8, 1);
    grid.drawLine(0, 15, 15, 11, 3);
}


void Display(void) {

    switch (prevKey) {
    case '1':
        Display1();
        break;
    case '2':
        Display2();
        break;
    case '3':
        Display3();
        break;
    case '4':
        Display4();
        break;
    default:
        break;
    }

    glFlush();

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

void Reshape(int newW, int newH) {
    glViewport(0, 0, (GLsizei)newW, (GLsizei)newH);
    point_ratio = fmin((GLsizei)newW, (GLsizei)newH);
    h = (GLsizei)newH;
    w = (GLsizei)newW;
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
