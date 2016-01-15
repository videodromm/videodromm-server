#include "ServerApp.h"


void ServerApp::setup()
{
	// parameters
	mVDSettings = VDSettings::create();
	mVDSettings->mLiveCode = false;
	mVDSettings->mRenderThumbs = false;
	// utils
	mVDUtils = VDUtils::create(mVDSettings);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings);
	// instanciate the console class
	mConsole = AppConsole::create(mVDSettings, mVDUtils);

	// MPE
	mServerFramesProcessed = 0;
	mClient = MPEClient::create(this);
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

	if (!mClient->isConnected() && (getElapsedFrames() % 60) == 0)
	{
		mClient->start();
	}
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
	gl::clear();
	mClient->draw();
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

	ui::SetNextWindowSize(ImVec2(largeW, largeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
	ui::Begin("MIDI");
	{
		sprintf_s(buf, "Enable");
		if (ui::Button(buf)) mVDRouter->midiSetup();
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
					sprintf_s(buf, "Disconnect %d", i);
				}
				else
				{
					sprintf_s(buf, "Connect %d", i);
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

			static char str0[128] = "/live/play";
			static int i0 = 0;
			static float f0 = 0.0f;
			ui::InputText("address", str0, IM_ARRAYSIZE(str0));
			ui::InputInt("track", &i0);
			ui::InputFloat("clip", &f0, 0.01f, 1.0f);
			if (ui::Button("Send")) { mVDRouter->sendOSCIntMessage(str0, i0); }
		}
		ui::End();

		xPos += largeW + margin;


	}
#pragma endregion OSC
#pragma region Info

	ui::SetNextWindowSize(ImVec2(largePreviewW, largeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(xPos, yPos), ImGuiSetCond_Once);
	sprintf_s(buf, "Fps %c %d###fps", "|/-\\"[(int)(ImGui::GetTime() / 0.25f) & 3], (int)mVDSettings->iFps);
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

	// console
	if (showConsole)
	{
		ui::SetNextWindowSize(ImVec2((largeW + margin) * 3, largePreviewH), ImGuiSetCond_Once);
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
