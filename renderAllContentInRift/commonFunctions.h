#define PI 3.1415926535f
#define RECORDING 0
#define USE_RELATIVE_PATHS 1
#define DRAW_SCANNED_OBJECTS 0
#define RIFT_WIN_COLS 1920
#define RIFT_WIN_ROWS 1200

#ifndef GL
#define GL
#include <GL\glew.h>
#include <GL\glut.h>
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <GLHelper.h>

using namespace cv;
using namespace std;

float deg2rad(float theta);
float rad2deg(float radAngle);
void printGLMatrix(GLenum mat);



