#include "ServerApp.h"


void ServerApp::setup()
{
    // Settings
    mVDSettings = VDSettings::create();
    mVDSettings->mLiveCode = false;
    mVDSettings->mRenderThumbs = false;
    mVDSession = VDSession::create(mVDSettings);
    // Utils
    mVDUtils = VDUtils::create(mVDSettings);
	// instanciate the console class
	mVDAnimation = VDAnimation::create(mVDSettings, mVDSession);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation, mVDSession);


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

	ui::initialize();

	updateWindowTitle();
	int w = mVDUtils->getWindowsResolution();
	setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
	setWindowPos(ivec2(0, 50));
    //  bpm = 180.0f;
    //setFrameRate(mVDSession->getTargetFps());
	

	setWindowSize(640, 480);
	setFrameRate(12.0f);
}

void ServerApp::update()
{
	mVDSettings->iFps = getAverageFps();
	mVDSettings->sFps = toString(floor(mVDSettings->iFps));
	updateWindowTitle();
}
void ServerApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + mVDSettings->sFps + " fps) Server");
}
void ServerApp::draw()
{
	gl::clear(Color::black());
	
	//imgui
	gl::setMatricesWindow(getWindowSize());
	xPos = margin;
	yPos = margin;

#pragma region WebSockets
	// websockets
	ui::SetNextWindowSize(ImVec2(largeW, largeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
	ui::Begin("WebSockets server");
	{
		if (mVDSettings->mIsWebSocketsServer)
		{
			ui::Text("WS Server %d", mVDSettings->mWebSocketsPort);
			ui::Text("IP %s", mVDSettings->mWebSocketsHost.c_str());
		}
		else
		{
			ui::Text("WS Client %d", mVDSettings->mWebSocketsPort);
			ui::Text("IP %s", mVDSettings->mWebSocketsHost.c_str());
			if (ui::Button("Connect")) { mVDRouter->wsConnect(); }
			ui::SameLine();
		}
		if (ui::Button("Ping")) { mVDRouter->wsPing(); }
	}

	ui::End();
	xPos += largeW + margin;
#pragma endregion WebSockets

#pragma region MIDI

	// MIDI window
/* MAC rtmidi FIX TODO
	ui::SetNextWindowSize(ImVec2(largeW, largeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
	ui::Begin("MIDI");
	{
		if (ui::Button("Enable")) mVDRouter->midiSetup();
		if (ui::CollapsingHeader("MidiIn", "20", true, true))
		{
			ui::Columns(2, "data", true);
			ui::Text("Name"); ui::NextColumn();
			ui::Text("Connect"); ui::NextColumn();
			ui::Separator();

			for (int i = 0; i < mVDRouter->getMidiInPortsCount(); i++)
			{
				ui::Text(mVDRouter->getMidiInPortName(i).c_str()); ui::NextColumn();

				if (mVDRouter->isMidiInConnected(i))
				{
                    ui::Text("Disconnect %d", i);
				}
				else
				{
                    ui::Text("Connect %d", i);

				}

				if (ui::Button(buf))
				{
					if (mVDRouter->isMidiInConnected(i))
					{
						mVDRouter->closeMidiInPort(i);
					}
					else
					{
						mVDRouter->openMidiInPort(i);
					}
				}
				ui::NextColumn();
				ui::Separator();
			}
			ui::Columns(1);
		}
	}
	ui::End();
	xPos += largeW + margin;
*/
#pragma endregion MIDI

#pragma region OSC

	if (mVDSettings->mOSCEnabled)
	{
		ui::SetNextWindowSize(ImVec2(largeW, largeH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
		ui::Begin("OSC router");
		{
			ui::Text("Sending to host %s", mVDSettings->mOSCDestinationHost.c_str());
			ui::SameLine();
			ui::Text(" on port %d", mVDSettings->mOSCDestinationPort);
			ui::Text("Sending to 2nd host %s", mVDSettings->mOSCDestinationHost2.c_str());
			ui::SameLine();
			ui::Text(" on port %d", mVDSettings->mOSCDestinationPort2);
			ui::Text(" Receiving on port %d", mVDSettings->mOSCReceiverPort);

			
		}
		ui::End();

		xPos += largeW + margin;


	}
#pragma endregion OSC
#pragma region Info

	ui::SetNextWindowSize(ImVec2(largePreviewW, largeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
    ui::Text( "Fps %c %d###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], (int)mVDSettings->iFps);
	ui::Begin(buf);
	{
		ImGui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// fps
		static ImVector<float> values; if (values.empty()) { values.resize(100); memset(&values.front(), 0, values.size()*sizeof(float)); }
		static int values_offset = 0;
		static float refresh_time = -1.0f;
		if (ui::GetTime() > refresh_time + 1.0f / 6.0f)
		{
			refresh_time = ui::GetTime();
			values[values_offset] = mVDSettings->iFps;
			values_offset = (values_offset + 1) % values.size();
		}
		if (mVDSettings->iFps < 12.0) ui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ui::PlotLines("FPS", &values.front(), (int)values.size(), values_offset, mVDSettings->sFps.c_str(), 0.0f, 300.0f, ImVec2(0, 30));
		if (mVDSettings->iFps < 12.0) ui::PopStyleColor();


		if (ui::Button("Save Settings"))
		{
			// save settings
			mVDSettings->save();
		}

	}
	ui::End();
	xPos = 0;
	yPos += largeH + margin;
#pragma endregion Info

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
//CINDER_APP(ServerApp, RendererGl(RendererGl::AA_NONE))
CINDER_APP(ServerApp, RendererGl(RendererGl::Options().msaa(4)))



