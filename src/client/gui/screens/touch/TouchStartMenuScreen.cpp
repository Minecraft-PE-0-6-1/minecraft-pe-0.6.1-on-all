#include "TouchStartMenuScreen.hpp"
#include "client/gui/screens/ProgressScreen.hpp"
#include "client/gui/screens/OptionsScreen.hpp"
#include "client/gui/screens/PauseScreen.hpp"

#include "client/gui/Font.hpp"
#include "client/gui/components/ScrolledSelectionList.hpp"
#include "client/gui/components/GuiElement.hpp"

#include <MinecraftClient.hpp>
#include "client/renderer/Tesselator.hpp"
#include "client/renderer/Textures.hpp"
#include "client/renderer/TextureData.hpp"
#include "SharedConstants.hpp"
#include "AppPlatform.hpp"
#include "LicenseCodes.hpp"
#include "util/Mth.hpp"

#include "client/gui/screens/DialogDefinitions.hpp"
#include "client/gui/screens/SimpleChooseLevelScreen.hpp"

namespace Touch {

// 
// Start menu screen implementation
//

// Some kind of default settings, might be overridden in ::init
StartMenuScreen::StartMenuScreen()
:	bHost(    2, "Start Game"),
	bJoin(    3, "Join Game"),
	bOptions( 4, "Options"),
	bQuit(    5, "")
{
	ImageDef def;
	bJoin.width = 75;
	def.width = def.height = (float) bJoin.width;

	def.setSrc(IntRectangle(0, 26, (int)def.width, (int)def.width));
	def.name = "gui/touchgui.png";
	IntRectangle& defSrc = *def.getSrc();

	bOptions.setImageDef(def, true);

	defSrc.y += defSrc.h;
	bHost.setImageDef(def, true);

	defSrc.y += defSrc.h;
	bJoin.setImageDef(def, true);
}

StartMenuScreen::~StartMenuScreen()
{
}

void StartMenuScreen::init()
{
	buttons.push_back(&bHost);
	buttons.push_back(&bJoin);
	buttons.push_back(&bOptions);

	// add quit icon (same look as options header)
	{
		ImageDef def;
		def.name = "gui/touchgui.png";
		def.width = 34;
		def.height = 26;
		def.setSrc(IntRectangle(150, 0, (int)def.width, (int)def.height));
		bQuit.setImageDef(def, true);
		bQuit.scaleWhenPressed = false;
		buttons.push_back(&bQuit);
	}

	tabButtons.push_back(&bHost);
	tabButtons.push_back(&bJoin);
	tabButtons.push_back(&bOptions);

	#ifdef DEMO_MODE
		buttons.push_back(&bBuy);
		tabButtons.push_back(&bBuy);
	#endif

	copyright = "\xffMojang AB";//. Do not distribute!";

	// always show base version string
	std::string versionString = Common::getGameVersionString();

	std::string _username = minecraft.options().getStringValue(OPTIONS_USERNAME);
	if (_username.empty()) _username = "unknown";

	username = "Username: " + _username;

	#ifdef DEMO_MODE
        #ifdef __APPLE__
            version = versionString + " (Lite)";
        #else
            version = versionString + " (Demo)";
        #endif
	#else
		version = versionString;
	#endif
    
    #ifdef APPLE_DEMO_PROMOTION
        version = versionString + " (Demo)";
    #endif

	bJoin.active = bHost.active = bOptions.active = true;
}

void StartMenuScreen::setupPositions() {
	int yBase = 2 + height / 3;
	int buttonWidth = bHost.width;
	float spacing = (width - (3.0f * buttonWidth)) / 4;

	//#ifdef ANDROID
	bHost.y =	 yBase;
	bJoin.y =	 yBase;
	bOptions.y = yBase;
	//#endif

	// Center buttons
	bJoin.x		= 0*buttonWidth + (int)(1*spacing);
	bHost.x		= 1*buttonWidth + (int)(2*spacing);
	bOptions.x	= 2*buttonWidth + (int)(3*spacing);
    
	// quit icon top-right (use size assigned in init)
	bQuit.x = width - bQuit.width;
	bQuit.y = 0;

	copyrightPosX = width - minecraft.font()->width(copyright) - 1;
	versionPosX = (width - minecraft.font()->width(version)) / 2;// - minecraft.font()->width(version) - 2;
}

void StartMenuScreen::buttonClicked(::Button* button) {

	if (button->id == bHost.id)
	{
		#if defined(DEMO_MODE) || defined(APPLE_DEMO_PROMOTION)
			minecraft.setScreen( new SimpleChooseLevelScreen("_DemoLevel") );
		#else
			minecraft.screenChooser.setScreen(SCREEN_SELECTWORLD);
		#endif
	}
	if (button->id == bJoin.id)
	{
        #ifdef APPLE_DEMO_PROMOTION
            minecraft.platform()->createUserInput(DialogDefinitions::DIALOG_DEMO_FEATURE_DISABLED);
        #else
            minecraft.locateMultiplayer();
            minecraft.screenChooser.setScreen(SCREEN_JOINGAME);
        #endif
	}
	if (button->id == bOptions.id)
	{
		minecraft.setScreen(new OptionsScreen());
	}
	if (button == &bQuit)
	{
		minecraft.quit();
	}
}

bool StartMenuScreen::isInGameScreen() { return false; }

void StartMenuScreen::render( int xm, int ym, float a )
{
	renderBackground();

	// Show current username in the top-left corner
	drawString(font, username, 2, 2, 0xffffffff);
    
    glEnable2(GL_BLEND);

#if defined(RPI)
	TextureId id = minecraft.textures().loadTexture("gui/pi_title.png");
#else
	TextureId id = minecraft.textures().loadTexture("gui/title.png");
#endif
	const TextureData* data = minecraft.textures().getTemporaryTextureData(id);

	if (data) {
		minecraft.textures().bind(id);

		const float x = (float)width / 2;
		const float y = 4;
		const float wh = 0.5f * Mth::Min((float)width/2.0f, (float)data->w / 2);
		const float scale = 2.0f * wh / (float)data->w;
		const float h = scale * (float)data->h;

		// Render title text
		Tesselator& t = Tesselator::instance;
		glColor4f2(1, 1, 1, 1);
		t.begin();
			t.vertexUV(x-wh, y+h, blitOffset, 0, 1);
			t.vertexUV(x+wh, y+h, blitOffset, 1, 1);
			t.vertexUV(x+wh, y+0, blitOffset, 1, 0);
			t.vertexUV(x-wh, y+0, blitOffset, 0, 0);
		t.draw();

		drawString(font, version, versionPosX, (int)(y+h)+2, /*50,*/ 0xffcccccc);//0x666666);
		drawString(font, copyright, copyrightPosX, height - 10, 0xffffff);
		glColor4f2(1, 1, 1, 1);
		if (Textures::isTextureIdValid(minecraft.textures().loadAndBindTexture("gui/logo/github.png")))
			blit(2, height - 10, 0, 0, 8, 8, 256, 256);
		drawString(font, "Kolyah35/minecraft-pe-0.6.1", 12, height - 10, 0xffcccccc);
		//patch->draw(t, 0, 20);
	}
	Screen::render(xm, ym, a);
    glDisable2(GL_BLEND);
}


void StartMenuScreen::mouseClicked(int x, int y, int buttonNum) {
	const int logoX = 2;
	const int logoW = 8 + 2 + font.width("Kolyah35/minecraft-pe-0.6.1");
	const int logoY = height - 10;
	const int logoH = 10;
	if (x >= logoX && x <= logoX + logoW && y >= logoY && y <= logoY + logoH)
		minecraft.platform()->openURL("https://gitea.sffempire.ru/Kolyah35/minecraft-pe-0.6.1");
	else
		Screen::mouseClicked(x, y, buttonNum);
}

bool StartMenuScreen::handleBackEvent( bool isDown ) {
	minecraft.quit();
	return true;
}

} // namespace Touch

