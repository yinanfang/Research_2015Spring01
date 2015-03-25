#include "oculusRender.h"

ovrHmd             hmd;
ovrEyeRenderDesc   EyeRenderDesc[2];
ovrRecti           EyeRenderViewport[2];
HWND window;
HDC DC;
ovrFovPort eyesFov[2];
ovrGLTexture EyeTexture[2];
ovrTexture EyeTextures[2];
ovrPosef headPose[2];
ovrTrackingState ss;
OVR::Posef HeadPose;
GLuint FBOId;
bool HaveVisionTracking = false, HavePositionTracker = false, HaveHMDConnected = false;
float hmdAY, hmdAX, hmdAZ;
float scalex = 1, scaley = 0.8;
float translatex = 0, translatey = 0;

Mat matProjection, matModelView;
extern mat4 projection, modelview, camera;
float scalingFactor = 100;

float movey = 0;
float viewRadius = 150;
int viewAngle = 30;


void getRidOfHealthWarning() {
  ovrHSWDisplayState hswDisplayState;
  ovrHmd_GetHSWDisplayState(hmd, &hswDisplayState);
  if(hswDisplayState.Displayed) ovrHmd_DismissHSWDisplay(hmd);
}

void printOVRMatrix4f(OVR::Matrix4f mat) {
  for (int iIter = 0; iIter < 4; iIter++) {
    for(int jIter = 0; jIter < 4; jIter++) {
      printf("%f ", mat.M[iIter][jIter]);
    }
    printf("\n");
  }
}

void initOculus(){
  window = FindWindow("GLUT", "HMD_BASED_TELEPRESENCE" );
  DC = GetDC(window);

  ovr_Initialize();
  hmd = ovrHmd_Create(0);

  // Start tracking
  ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | 
			   ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
  //Configure Stereo settings.
  OVR::Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,  hmd->DefaultEyeFov[0], 1.0f);
  OVR::Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0f);
  OVR::Sizei RenderTargetSize;
  RenderTargetSize.w = recommenedTex0Size.w + recommenedTex1Size.w;
  RenderTargetSize.h = max ( recommenedTex0Size.h, recommenedTex1Size.h );

  // framebuffer
  glGenFramebuffers(1, &FBOId);
  glBindFramebuffer(GL_FRAMEBUFFER, FBOId);

  // Create Depth Buffer...
  GLuint l_DepthBufferId;
  glGenRenderbuffers(1, &l_DepthBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, l_DepthBufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RenderTargetSize.w, RenderTargetSize.h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, l_DepthBufferId);

  // set up texture
  GLuint texId;
  glGenTextures(1, &texId);
  glBindTexture(GL_TEXTURE_2D, texId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, RenderTargetSize.w, 
	       RenderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE,0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Set the texture as our colour attachment #0...
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, 0);

  // Set the list of draw buffers...
  GLenum l_GLDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, l_GLDrawBuffers); // "1" is the size of DrawBuffers

  // Check if everything is OK...
  GLenum l_Check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  if (l_Check!=GL_FRAMEBUFFER_COMPLETE) {
    printf("There is a problem with the FBO.\n");
    exit(EXIT_FAILURE);
  }

  // Unbind...
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  EyeRenderViewport[0].Pos = OVR::Vector2i(0,0);
  EyeRenderViewport[0].Size = OVR::Sizei(RenderTargetSize.w / 2, RenderTargetSize.h);
  EyeRenderViewport[1].Pos  = OVR::Vector2i((RenderTargetSize.w + 1) / 2, 0);
  EyeRenderViewport[1].Size = EyeRenderViewport[0].Size;


  EyeTexture[0].OGL.Header.API = ovrRenderAPI_OpenGL;
  EyeTexture[0].OGL.Header.TextureSize = RenderTargetSize;
  EyeTexture[0].OGL.Header.RenderViewport = EyeRenderViewport[0];
  EyeTexture[0].OGL.TexId = texId;

  EyeTexture[1] = EyeTexture[0];
  EyeTexture[1].OGL.Header.RenderViewport = EyeRenderViewport[1];

  EyeTextures[0] = EyeTexture[0].Texture;
  EyeTextures[1] = EyeTexture[1].Texture;

}

void initOculusRender(){
  // Configure OpenGL.
  ovrGLConfig cfg;
  cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
  cfg.OGL.Header.RTSize = OVR::Sizei(hmd->Resolution.w, hmd->Resolution.h);
  cfg.OGL.Header.Multisample = 1;
  cfg.OGL.Window = window;
  cfg.OGL.DC = DC;

  int distortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

  eyesFov[0] = hmd->DefaultEyeFov[0];
  eyesFov[1] = hmd->DefaultEyeFov[1];

  EyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, eyesFov[0]);
  EyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, eyesFov[1]);

  ovrBool result = ovrHmd_ConfigureRendering(hmd, &cfg.Config, distortionCaps,
					     eyesFov, EyeRenderDesc);

  ovrHmd_AttachToWindow(hmd, window, NULL, NULL);
  printf("Oculus initialized\n");
}

void releaseOculus(){
  ovrHmd_Destroy(hmd);
  ovr_Shutdown();
}

void updateRiftTracking() {
  ss = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());

  // Report vision tracking
  bool hadVisionTracking = HaveVisionTracking;
  HaveVisionTracking = (ss.StatusFlags & ovrStatus_PositionTracked) != 0;
  if (HaveVisionTracking && !hadVisionTracking) printf("Vision Tracking Acquired \n");
  if (!HaveVisionTracking && hadVisionTracking) printf("Lost Vision Tracking \n");

  // Report position tracker
  bool hadPositionTracker = HavePositionTracker;
  HavePositionTracker = (ss.StatusFlags & ovrStatus_PositionConnected) != 0;
  if (HavePositionTracker && !hadPositionTracker) printf("Position Tracker Connected \n");
  if (!HavePositionTracker && hadPositionTracker) printf("Position Tracker Disconnected \n");

  // Report position tracker
  bool hadHMDConnected = HaveHMDConnected;
  HaveHMDConnected = (ss.StatusFlags & ovrStatus_HmdConnected) != 0;
  if (HaveHMDConnected && !hadHMDConnected) printf("HMD Connected \n");
  if (!HaveHMDConnected && hadHMDConnected) printf("HMD Disconnected \n");

  HeadPose = ss.HeadPose.ThePose;
  HeadPose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&hmdAY, &hmdAX, &hmdAZ);
}

void oculusBeginRendering() {
#if RECORDING
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
  ovrHmd_BeginFrame(hmd, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, FBOId);
#endif

  glutReshapeWindow(hmd->Resolution.w, hmd->Resolution.h);  
  glClearColor( 0.0f,0.0f,0.0f, 0.0f);
  glClearDepth( 1.0);   
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

}

void pushGLMatricesForEye(int ieye) {
  ovrEyeType eye = hmd->EyeRenderOrder[ieye];
  headPose[eye] = ovrHmd_GetEyePose(hmd, eye);
  ovrPosef eyepose = headPose[eye];

#if RECORDING
  glViewport(ieye*RIFT_WIN_COLS/2,0,RIFT_WIN_COLS/2, RIFT_WIN_ROWS);
#else
#define FLIP_LEFT_RIGHT_EYES 1
#if FLIP_LEFT_RIGHT_EYES
  glViewport(EyeRenderViewport[1-eye].Pos.x, EyeRenderViewport[1-eye].Pos.y, EyeRenderViewport[eye].Size.w, EyeRenderViewport[eye].Size.h);
#else
  glViewport(EyeRenderViewport[eye].Pos.x, EyeRenderViewport[eye].Pos.y, EyeRenderViewport[eye].Size.w, EyeRenderViewport[eye].Size.h);
#endif
#endif

  // obtaining projection and view matrices
  OVR::Quatf orientation = OVR::Quatf(eyepose.Orientation);
  OVR::Matrix4f proj = ovrMatrix4f_Projection(EyeRenderDesc[eye].Fov, 0.01f, 10000.0f, true);
  OVR::Matrix4f view = OVR::Matrix4f(orientation);

  // Converting projection and modelview matrices from oculus format to opengl/shader programs format:
  {
    // Converting for fixed pipeline opengl:
    {
      // Setting projection matrix
      assert(glGetError() == GL_NO_ERROR);
      glMatrixMode(GL_PROJECTION); glPushMatrix(); assert(glGetError() == GL_NO_ERROR);
      glLoadIdentity();
      glMultMatrixf(&(proj.Transposed().M[0][0]));

      // Setting ModelView matrix
      glMatrixMode(GL_MODELVIEW); glPushMatrix(); assert(glGetError() == GL_NO_ERROR);
      glLoadIdentity();
      glTranslatef(EyeRenderDesc[eye].ViewAdjust.x + HeadPose.Translation.x*1, EyeRenderDesc[eye].ViewAdjust.y + HeadPose.Translation.y*1, EyeRenderDesc[eye].ViewAdjust.z - HeadPose.Translation.z*1);
      glMultMatrixf(&(view.Transposed().M[0][0]));

#define PRINT_MATRICES 0
#if PRINT_MATRICES
      printf("OVR proj matrix \n");
      printOVRMatrix4f(proj);
      printf("Projection matrix: \n");
      printGLMatrix(GL_PROJECTION_MATRIX);

      printf("OVR modelview matrix \n");
      printOVRMatrix4f(view);
      printf("ModelView matrix: \n");
      printGLMatrix(GL_MODELVIEW_MATRIX);
#endif
    }

    // Converting for shader programs:
    {
      // Need to convert OVR Matrix4f proj to the projection matrix that we pass to the shader programs
      projection = mat4(vec4(proj.M[0][0], proj.M[0][1], proj.M[0][2], proj.M[0][3]),  
			vec4(proj.M[1][0], proj.M[1][1], proj.M[1][2], proj.M[1][3]),  
			vec4(proj.M[2][0], proj.M[2][1], proj.M[2][2], proj.M[2][3]),  
			vec4(proj.M[3][0], proj.M[3][1], proj.M[3][2], proj.M[3][3]));

      // Need to convert OVR Matrix4f view to the modelview matrix that we pass to the shader programs

      view.M[3][0] += (EyeRenderDesc[eye].ViewAdjust.x + HeadPose.Translation.x)*scalingFactor; 
      view.M[3][1] += (EyeRenderDesc[eye].ViewAdjust.y + HeadPose.Translation.y)*scalingFactor;
      view.M[3][2] += (EyeRenderDesc[eye].ViewAdjust.z + HeadPose.Translation.z)*scalingFactor;

#define TRANSPOSE 1
#if TRANSPOSE
      OVR::Matrix4f viewTransposed = view.Transposed();
#else
      OVR::Matrix4f viewTransposed = view;
#endif

      modelview = mat4(vec4(viewTransposed.M[0][0], viewTransposed.M[0][1], viewTransposed.M[0][2], 0),  
		       vec4(viewTransposed.M[1][0], viewTransposed.M[1][1], viewTransposed.M[1][2], 0),  
		       vec4(viewTransposed.M[2][0], viewTransposed.M[2][1], viewTransposed.M[2][2], 0),  
		       vec4(0, 0, 0, 1)) * translation3D(-vec3(viewTransposed.M[0][3], viewTransposed.M[1][3], viewTransposed.M[2][3]));

#define PRINT_MATRICES 0
#if PRINT_MATRICES
      printf("OVR proj matrix \n");
      printOVRMatrix4f(proj);
      printf("OVR modelview matrix \n");
      printOVRMatrix4f(view);

      printf("shader matrix projection \n");
      printMat4(projection);
      printf("shader matrix modelview \n");
      printMat4(modelview);
#endif
    }
  }
}

void popGLMatricesForEye() {
  glMatrixMode(GL_MODELVIEW); glPopMatrix(); assert(glGetError() == GL_NO_ERROR);
  glMatrixMode(GL_PROJECTION); glPopMatrix(); assert(glGetError() == GL_NO_ERROR);
}

void oculusEndRendering() {
  glBindFramebuffer(GL_FRAMEBUFFER,0);

#if RECORDING
#else
  ovrHmd_EndFrame(hmd, headPose, EyeTextures);
#endif

}

void key_oculusRender(int k, int x, int y) {

  switch(k) {
  case '4': if(movey>1) movey = movey*0.9; printf("movey: %f \n", movey); break;
  case '5': movey = movey*1.1; printf("movey: %f \n", movey); break;
  case '1': viewAngle = ((viewAngle-10) % 360); printf("viewAngle: %d \n", viewAngle); break;
  case '2': viewAngle = ((viewAngle+10) % 360); printf("viewAngle: %d \n", viewAngle); break;
  case '7': if(viewRadius>1) viewRadius = viewRadius*0.9; printf("viewRadius: %f \n", viewRadius); break;
  case '8': viewRadius = viewRadius*1.1; printf("viewRadius: %f \n", viewRadius); break;
  case 'm': break;
  default: printf("Invalid entry for this mode \n"); break;
  }

}
