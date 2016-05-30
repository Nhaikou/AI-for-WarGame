// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// War Game for GameAIGameFramework.
//
// Copyright (c) 2016 Mikko Romppainen. All Rights reserved.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <JohnDoe.h>
#include <MikkoRomppainen.h>
#include <PassCourse.h>
#include <NikoKinnunen.h>
#include "GameApp.h"

int main(int argc, char *argv[])
{
	yam2d::Ref<GameApp> app = new GameApp(argc, argv, 1280, 800);
	app->disableLayer("ObjectSpawns");
	app->disableLayer("GroundTypeColliders");
	app->disableLayer("GroundMoveSpeed");
	app->setDefaultGame("Level0.tmx", "PassCourse", "NikoKinnunen", "NikoKinnunen", 4);
	app->setPlayer1Controller(PassCourse::createNewPlayer());
	app->setPlayer2Controller(NikoKinnunen::createNewPlayer());

	return app->run();
}
