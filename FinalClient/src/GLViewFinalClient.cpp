#include "GLViewFinalClient.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "AftrGLRendererBase.h"

//If we want to use way points, we need to include this.
#include "FinalClientWayPoints.h"
#include "NetMessengerClient.h"
#include "io.h"
#include "WOPhysXTriangularMesh.h"
#include "WOGridECEFElevation.h"
#include "WOFTGLString.h"
#include "MGLFTGLString.h"

using namespace Aftr;
using namespace std;

GLViewFinalClient* GLViewFinalClient::New(const std::vector< std::string >& args)
{
	GLViewFinalClient* glv = new GLViewFinalClient(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();

	return glv;
}


GLViewFinalClient::GLViewFinalClient(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewFinalClient::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.

	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

//	if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
//		this->client = NetMessengerClient::New("127.0.0.1", "12682");
//		cout << "This is client A..." << endl;
//	}
//	else {
//    this->client = NetMessengerClient::New("127.0.0.1", "12683");
//    cout << "This is client B..." << endl;
//}
	physEngine = new PhysicsCreate();
	cout << "##Creating physic engine##" << endl;

	triangleMesh = WOPhysXTriangularMesh::New();
	triangleMesh->init(this->physEngine);

	//this->nmc->setPhysics(this->physEngine);
	//GLViewFinalClient::onCreate() is invoked after this module's LoadMap() is completed.


}


void GLViewFinalClient::onCreate()
{
	//GLViewFinalClient::onCreate() is invoked after this module's LoadMap() is completed.
	//At this point, all the managers are initialized. That is, the engine is fully initialized.

	if (this->pe != NULL)
	{
		//optionally, change gravity direction and magnitude here
		//The user could load these values from the module's aftr.conf
		this->pe->setGravityNormalizedVector(Vector(0, 0, -1.0f));
		this->pe->setGravityScalar(Aftr::GRAVITY);
	}
	this->setActorChaseType(STANDARDEZNAV); //Default is STANDARDEZNAV mode
	//this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1

	// Check if the Debug folder in the right place.
	if ((_access("../irrKlang-64bit-1.6.0/music/ophelia.mp3", 0)) == -1) {
		cout << "Sound file path error.." << endl;
		cout << "Should put the 'Debug' folder under 'FinalClient' directly." << endl;
		return;
	}

	//Set background music with 2D Sound when starting the program.
	//this->smgr = SoundManager::init();
	//this->smgr->play2D("../irrKlang-64bit-1.6.0/music/ophelia.mp3", true, false, true);
	//this->smgr->getSound2D().at(0)->setVolume(0.5f);

	/*this->triangleMesh->createGrid();*/
	if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
		client = NetMessengerClient::New("127.0.0.1", "12682");
	}
	else {
		client = NetMessengerClient::New("127.0.0.1", "12683");
	}
}


GLViewFinalClient::~GLViewFinalClient()
{
	//Implicitly calls GLView::~GLView()
}

void GLViewFinalClient::updateWorld()
{
	GLView::updateWorld(); //Just call the parent's update world first.
						   //If you want to add additional functionality, do it after
						   //this call.

}

void GLViewFinalClient::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewFinalClient::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);

}


void GLViewFinalClient::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);

}


void GLViewFinalClient::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);

}


void GLViewFinalClient::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);

	if (actor == nullptr) {
		return;
	}
	// backward
	if (key.keysym.sym == SDLK_s)
	{
		Vector look_dir = actor->getLookDirection();
		actor->moveRelative(look_dir * -1);

		NetMsgSimpleWO msg;
		msg.pos = actor->getPosition();
		msg.dma = actor->getModel()->getDisplayMatrix();
		msg.id = this->actorLst->getIndexOfWO(actor);
		msg.new_indicator = 0;
		client->sendNetMsgSynchronousTCP(msg);

		if (actor->getLabel() == "WheeledCar") {
			this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
			NetMsgSimpleWO msg;
			msg.pos = this->wcHealthStr->getPosition();
			msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
		else {
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = this->jetHealthStr->getPosition();
			msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
	}
	// forward
	if (key.keysym.sym == SDLK_w)
	{
		Vector look_dir = actor->getLookDirection();
		actor->moveRelative(look_dir);
		NetMsgSimpleWO msg;
		msg.pos = actor->getPosition();
		msg.dma = actor->getModel()->getDisplayMatrix();
		msg.id = this->actorLst->getIndexOfWO(actor);
		msg.new_indicator = 0;
		client->sendNetMsgSynchronousTCP(msg);
		if (actor->getLabel() == "WheeledCar") {
			this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
			NetMsgSimpleWO msg;
			msg.pos = this->wcHealthStr->getPosition();
			msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
		else {
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = this->jetHealthStr->getPosition();
			msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
	}
	// left
	if (key.keysym.sym == SDLK_a)
	{
		actor->getModel()->rotateAboutGlobalZ(0.3f);
		Vector look_dir = actor->getLookDirection();
		actor->moveRelative(look_dir * 1.5f);
		NetMsgSimpleWO msg;
		msg.pos = actor->getPosition();
		msg.dma = actor->getModel()->getDisplayMatrix();
		msg.id = this->actorLst->getIndexOfWO(actor);
		msg.new_indicator = 0;
		client->sendNetMsgSynchronousTCP(msg);
		if (actor->getLabel() == "WheeledCar") {
			this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
			NetMsgSimpleWO msg;
			msg.pos = this->wcHealthStr->getPosition();
			msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
		else {
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = this->jetHealthStr->getPosition();
			msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
	}
	// right
	if (key.keysym.sym == SDLK_d)
	{
		actor->getModel()->rotateAboutGlobalZ(-0.3f);
		Vector look_dir = actor->getLookDirection();
		actor->moveRelative(look_dir * 1.5f);
		NetMsgSimpleWO msg;
		msg.pos = actor->getPosition();
		msg.dma = actor->getModel()->getDisplayMatrix();
		msg.id = this->actorLst->getIndexOfWO(actor);
		msg.new_indicator = 0;
		client->sendNetMsgSynchronousTCP(msg);
		if (actor->getLabel() == "WheeledCar") {
			this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
			NetMsgSimpleWO msg;
			msg.pos = this->wcHealthStr->getPosition();
			msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
		else {
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = this->jetHealthStr->getPosition();
			msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
		}
	}
	
	if (actor->getLabel() == "Jet") {
		// upper
		if (key.keysym.sym == SDLK_e)
		{
			actor->moveRelative(Vector(0.f, 0.f, 0.1f));
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = actor->getPosition();
			msg.dma = actor->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(actor);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
			if (actor->getLabel() == "WheeledCar") {
				this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
				NetMsgSimpleWO msg;
				msg.pos = this->wcHealthStr->getPosition();
				msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
				msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
				msg.new_indicator = 0;
				client->sendNetMsgSynchronousTCP(msg);
			}
			else {
				this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
				NetMsgSimpleWO msg;
				msg.pos = this->jetHealthStr->getPosition();
				msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
				msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
				msg.new_indicator = 0;
				client->sendNetMsgSynchronousTCP(msg);
			}
		}
		// lower
		if (key.keysym.sym == SDLK_c)
		{
			actor->moveRelative(Vector(0.f, 0.f, -0.1f));
			this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
			NetMsgSimpleWO msg;
			msg.pos = actor->getPosition();
			msg.dma = actor->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(actor);
			msg.new_indicator = 0;
			client->sendNetMsgSynchronousTCP(msg);
			if (actor->getLabel() == "WheeledCar") {
				this->wcHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5);
				NetMsgSimpleWO msg;
				msg.pos = this->wcHealthStr->getPosition();
				msg.dma = this->wcHealthStr->getModel()->getDisplayMatrix();
				msg.id = this->actorLst->getIndexOfWO(this->wcHealthStr);
				msg.new_indicator = 0;
				client->sendNetMsgSynchronousTCP(msg);
			}
			else {
				this->jetHealthStr->setPosition(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5);
				NetMsgSimpleWO msg;
				msg.pos = this->jetHealthStr->getPosition();
				msg.dma = this->jetHealthStr->getModel()->getDisplayMatrix();
				msg.id = this->actorLst->getIndexOfWO(this->jetHealthStr);
				msg.new_indicator = 0;
				client->sendNetMsgSynchronousTCP(msg);
			}
		}
	}

	if (key.keysym.sym == SDLK_g) {
		std::string missile(ManagerEnvironmentConfiguration::getSMM() + "/models/rocket/missle/missiles.obj");
		WOPhysicX* missileWO = WOPhysicX::New(missile, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		if (actor->getLabel() == "WheeledCar") {
			missileWO->setPosition(Vector(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z + 5));
			missileWO->setLabel("Missile");
			worldLst->push_back(missileWO);
			actorLst->push_back(missileWO);

			NetMsgSimpleWO msg;
			msg.pos = missileWO->getPosition();
			msg.dma = missileWO->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(missileWO);
			msg.new_indicator = 11;
			client->sendNetMsgSynchronousTCP(msg);
		}
		else {
			missileWO->setPosition(Vector(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z - 5));
			missileWO->setLabel("Missile");
			worldLst->push_back(missileWO);
			actorLst->push_back(missileWO);

			NetMsgSimpleWO msg;
			msg.pos = missileWO->getPosition();
			msg.dma = missileWO->getModel()->getDisplayMatrix();
			msg.id = this->actorLst->getIndexOfWO(missileWO);
			msg.new_indicator = 12;
			client->sendNetMsgSynchronousTCP(msg);
		}
	}

	//if (this->client->isTCPSocketOpen()) {
	//	this->nmc->setObjPos(actor->getWO()->getPosition());
	//	this->nmc->setRotateZ(0.0f);
	//	this->nmc->setActorIndex(actorLst->getIndexOfWO(actor->getWO()));
	//	this->nmc->setModelPath(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	//	this->nmc->setDisplayMatrix(actor->getWO()->getModel()->getDisplayMatrix());
	//	this->client->sendNetMsgSynchronousTCP(*this->nmc);
	//}
}


void GLViewFinalClient::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewFinalClient::loadMap()
{
	this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
	this->actorLst = new WorldList();
	this->netLst = new WorldList();

	ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
	ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
	ManagerOpenGLState::enableFrustumCulling = false;
	Axes::isVisible = true;
	this->glRenderer->isUsingShadowMapping(false); //set to TRUE to enable shadow mapping, must be using GL 3.2+

	this->cam->setPosition(0, 0, 100);

	std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
	std::string wheeledCar(ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl");
	std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");
	std::string jet(ManagerEnvironmentConfiguration::getSMM() + "/models/jet_wheels_down_PP.wrl");

	//SkyBox Textures readily available
	std::vector< std::string > skyBoxImageNames; //vector to store texture paths
	skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");

	float ga = 0.1f; //Global Ambient Light level for this module
	ManagerLight::setGlobalAmbientLight(aftrColor4f(ga, ga, ga, 1.0f));
	WOLight* light = WOLight::New();
	light->isDirectionalLight(true);
	light->setPosition(Vector(0, 0, 100));
	//Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
	//for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
	light->getModel()->setDisplayMatrix(Mat4::rotateIdentityMat({ 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD));
	light->setLabel("Light");
	worldLst->push_back(light);

	//Create the SkyBox
	WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
	wo->setPosition(Vector(0, 0, 0));
	wo->setLabel("Sky Box");
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	worldLst->push_back(wo);

	//Create grass
	wo = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wo->setPosition(Vector(0, 0, 0));
	wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
	ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at(0)->getSkins().at(0);
	grassSkin.getMultiTextureSet().at(0)->setTextureRepeats(5.0f);
	grassSkin.setAmbient(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Color of object when it is not in any light
	grassSkin.setDiffuse(aftrColor4f(1.0f, 1.0f, 1.0f, 1.0f)); //Diffuse color components (ie, matte shading color of this object)
	grassSkin.setSpecular(aftrColor4f(0.4f, 0.4f, 0.4f, 1.0f)); //Specular color component (ie, how "shiney" it is)
	grassSkin.setSpecularCoefficient(10); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
	wo->setLabel("Grass");
	worldLst->push_back(wo);

	WOPhysicX* wheeledCarWO = WOPhysicX::New(wheeledCar, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	wheeledCarWO->setPosition(Vector(40, 15, 5));
	wheeledCarWO->setLabel("WheeledCar");
	worldLst->push_back(wheeledCarWO);
	actorLst->push_back(wheeledCarWO);
	if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
		actor = wheeledCarWO;
	}
	//current health label
	wcHealthStr = WOFTGLString::New(ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.TTF", 30);
	wcHealthStr->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 0.5f, 1.5f, 1.0f));
	wcHealthStr->getModelT<MGLFTGLString>()->setSize(10, 10);
	wcHealthStr->getModelT<MGLFTGLString>()->setText("200 / 200");
	wcHealthStr->rotateAboutGlobalX(PI / 2);
	wcHealthStr->rotateAboutGlobalZ(-PI / 2);
	wcHealthStr->setPosition(wheeledCarWO->getPosition().x, wheeledCarWO->getPosition().y, wheeledCarWO->getPosition().z + 5);
	worldLst->push_back(wcHealthStr);
	actorLst->push_back(wcHealthStr);

	WOPhysicX* jetWO = WOPhysicX::New(jet, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
	jetWO->setPosition(Vector(40, 15, 100));
	jetWO->setLabel("Jet");
	worldLst->push_back(jetWO);
	actorLst->push_back(jetWO);
	if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") != "12683") {
		actor = jetWO;
	}
	//current health label
	jetHealthStr = WOFTGLString::New(ManagerEnvironmentConfiguration::getSMM() + "/fonts/COMIC.TTF", 30);
	jetHealthStr->getModelT<MGLFTGLString>()->setFontColor(aftrColor4f(1.0f, 0.5f, 1.5f, 1.0f));
	jetHealthStr->getModelT<MGLFTGLString>()->setSize(10, 10);
	jetHealthStr->getModelT<MGLFTGLString>()->setText("200 / 200");
	jetHealthStr->rotateAboutGlobalX(PI / 2);
	jetHealthStr->rotateAboutGlobalZ(-PI / 2);
	jetHealthStr->setPosition(jetWO->getPosition().x, jetWO->getPosition().y, jetWO->getPosition().z - 5);
	worldLst->push_back(jetHealthStr);
	actorLst->push_back(jetHealthStr);

	//if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
	//	actor = wheeledCarWO;
	//}
	//else {
	//	actor = jetWO;
	//}

	createFinalClientWayPoints();
}


void GLViewFinalClient::createFinalClientWayPoints()
{
	// Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
	WayPointParametersBase params(this);
	params.frequency = 5000;
	params.useCamera = true;
	params.visible = true;
	WOWayPointSpherical* wayPt = WOWP1::New(params, 3);
	wayPt->setPosition(Vector(50, 0, 3));
	worldLst->push_back(wayPt);
}
