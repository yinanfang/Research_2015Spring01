#include "commonFunctions.h"

#include "OVR.h"
#include "OVR_CAPI.h"
#include "OVR_CAPI_GL.h"
#include "Kernel/OVR_Math.h"
#include "Kernel/OVR_Std.h"

using namespace algebra3;


void initOculus();
void initOculusRender();
void releaseOculus();
void updateRiftTracking();
void getRidOfHealthWarning();
void oculusBeginRendering();
void oculusEndRendering();
void popGLMatricesForEye();
void pushGLMatricesForEye(int ieye);
void drawConfiguration(int ieye);
void key_oculusRender(int k, int x, int y);
void simpleClearScreen();

