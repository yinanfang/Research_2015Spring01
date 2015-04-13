#include "commonFunctions.h"

#include <gl\glut.h>
#include <sstream>
#include <vector>
#include <CSurface.h>

using namespace algebra3;


void initStaticObjectRenderer();
void key_renderStaticObjectInRift(unsigned char k, int x, int y);
void renderStaticObjectInRift(int number);
mat4 getProjectionMatrix(double, double, double, double);
void printMat4(mat4 currMat);
void renderStaticObjectInRiftTimer(int t);

static int SceneNumber = 0;
static int NUM_BUFFER = 20;