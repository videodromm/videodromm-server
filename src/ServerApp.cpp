#include "ServerApp.h"


void ServerApp::setup()
{
	// instanciate the console class
	mConsole = AppConsole::create(mVDSettings, mVDUtils);

	// parameters
	mVDSettings = VDSettings::create();
	mVDSettings->mLiveCode = false;
	mVDSettings->mRenderThumbs = false;
	// utils
	mVDUtils = VDUtils::create(mVDSettings);

	// MPE
	mServerFramesProcessed = 0;
	//mClient = MPEClient::create(this);
	// imgui
	margin = 3;
	inBetween = 3;
	// mPreviewFboWidth 80 mPreviewFboHeight 60 margin 10 inBetween 15 mPreviewWidth = 160;mPreviewHeight = 120;
	w = mVDSettings->mPreviewFboWidth + margin;
	h = mVDSettings->mPreviewFboHeight * 2.3;
	largeW = (mVDSettings->mPreviewFboWidth + margin) * 4;
	largeH = (mVDSettings->mPreviewFboHeight + margin) * 5;
	largePreviewW = mVDSettings->mPreviewWidth + margin;
	largePreviewH = (mVDSettings->mPreviewHeight + margin) * 2.4;
	displayHeight = mVDSettings->mMainDisplayHeight - 50;

	showConsole = true;
	ui::initialize();
	updateWindowTitle();
	int w = mVDUtils->getWindowsResolution();
	setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
	setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
	// tempo init
	mVDUtils->tapTempo();

}

void ServerApp::mpeReset()
{
	// Reset the state of your app.
	// This will be called when any client connects.
	mServerFramesProcessed = 0;
}

void ServerApp::update()
{
	mVDSettings->iFps = getAverageFps();
	mVDSettings->sFps = toString(floor(mVDSettings->iFps));
	updateWindowTitle();

	/*if (!mClient->isConnected() && (getElapsedFrames() % 60) == 0)
	{
		mClient->start();
	}*/
}
void ServerApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + mVDSettings->sFps + " fps) " + toString(mServerFramesProcessed));
}
void ServerApp::mpeFrameUpdate(long serverFrameNumber)
{
	mServerFramesProcessed = serverFrameNumber;

}

void ServerApp::draw()
{
	//mClient->draw();
	//imgui
	gl::setMatricesWindow(getWindowSize());
	xPos = margin;
	yPos = margin + 30;
	// console
	if (showConsole)
	{
		ui::SetNextWindowSize(ImVec2((w + margin) * mVDSettings->MAX, largePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		ShowAppConsole(&showConsole);
		if (mVDSettings->newMsg)
		{
			mVDSettings->newMsg = false;
			mConsole->AddLog(mVDSettings->mMsg.c_str());
		}
	}
}
// From imgui by Omar Cornut
void ServerApp::ShowAppConsole(bool* opened)
{
	mConsole->Run("Console", opened);
}
void ServerApp::mpeFrameRender(bool isNewFrame)
{
	gl::clear(Color(0.2, 0.0, 0.3));
	// Your render code.
}
void ServerApp::keyDown(KeyEvent event)
{

	// warp editor did not handle the key, so handle it here
	switch (event.getCode()) {
	case KeyEvent::KEY_ESCAPE:
		// quit the application
		quit();
		break;

	}

}
void ServerApp::cleanup()
{
	CI_LOG_V("shutdown");

	// save params
	mVDSettings->save();
	ui::Shutdown();
}
// If you're deploying to iOS, set the Render antialiasing to 0 for a significant
// performance improvement. This value defaults to 4 (AA_MSAA_4) on iOS and 16 (AA_MSAA_16)
// on the Desktop.
#if defined( CINDER_COCOA_TOUCH )
CINDER_APP(ServerApp, RendererGl(RendererGl::AA_NONE))
#else
CINDER_APP(ServerApp, RendererGl)
#endif
