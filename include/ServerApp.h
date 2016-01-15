#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "MPEApp.hpp"
#include "MPEClient.h"

// UserInterface
#include "CinderImGui.h"
// parameters
#include "VDSettings.h"
// Utils
#include "VDUtils.h"
// Message router
#include "VDRouter.h"
// Console
#include "AppConsole.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mpe;
using namespace VideoDromm;

// NEXT STEP: Create multiple targets
// https://github.com/wdlindmeier/Most-Pixels-Ever-Cinder/wiki/MPE-Setup-Tutorial-for-Cinder#3-create-multiple-targets

class ServerApp : public App, public MPEApp
{
public:
	void						setup() override;
	void 						cleanup() override;
	void						mpeReset();
	void 						keyDown(KeyEvent event) override;

	void						update() override;
	void						mpeFrameUpdate(long serverFrameNumber);

	void						draw() override;
	void						mpeFrameRender(bool isNewFrame);

	void 						updateWindowTitle();

	// Most Pixels Ever
	MPEClientRef				mClient;
	long						mServerFramesProcessed;
	// Settings
	VDSettingsRef				mVDSettings;
	// Utils
	VDUtilsRef					mVDUtils;
	// Message router
	VDRouterRef					mVDRouter;
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
	// console
	AppConsoleRef				mConsole;
	bool						showConsole;
	void						ShowAppConsole(bool* opened);

};