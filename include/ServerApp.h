#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// UserInterface
#include "CinderImGui.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Utils
#include "VDUtils.h"
// Animation
#include "VDAnimation.h"
// shaders
#include "VDShaders.h"
// fbo
#include "VDFbo.h"
// Message router
#include "VDRouter.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

class ServerApp : public App
{
public:
	void						setup() override;
	void 						cleanup() override;
	void 						keyDown(KeyEvent event) override;

	void						update() override;

	void						draw() override;

	void 						updateWindowTitle();

	// Settings
	VDSettingsRef				mVDSettings;
	// Utils
	VDUtilsRef					mVDUtils;
	// Message router
	VDRouterRef					mVDRouter;
    // Session
    VDSessionRef                mVDSession;
    // Animation
    VDAnimationRef              mVDAnimation;
	// imgui
	int							w;
	int							h;
	int							displayHeight;
	int							xPos;
	int							yPos;
	int							largeW;
	int							largeH;
	int							largePreviewW;
	int							largePreviewH;
	int							margin;
	int							inBetween;
	char						buf[64];
	
};