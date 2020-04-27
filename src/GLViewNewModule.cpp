#include "GLViewNewModule.h"

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
#include "NewModuleWayPoints.h"
#include "NetMessengerClient.h"
#include "io.h"
#include "WOPhysXTriangularMesh.h"
#include "WOGridECEFElevation.h"

using namespace Aftr;
using namespace std;

GLViewNewModule* GLViewNewModule::New(const std::vector< std::string >& args)
{
	GLViewNewModule* glv = new GLViewNewModule(args);
	glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
	glv->onCreate();

	return glv;
}


GLViewNewModule::GLViewNewModule(const std::vector< std::string >& args) : GLView(args)
{
	//Initialize any member variables that need to be used inside of LoadMap() here.
	//Note: At this point, the Managers are not yet initialized. The Engine initialization
	//occurs immediately after this method returns (see GLViewNewModule::New() for
	//reference). Then the engine invoke's GLView::loadMap() for this module.
	//After loadMap() returns, GLView::onCreate is finally invoked.

	//The order of execution of a module startup:
	//GLView::New() is invoked:
	//    calls GLView::init()
	//       calls GLView::loadMap() (as well as initializing the engine's Managers)
	//    calls GLView::onCreate()

	//if (ManagerEnvironmentConfiguration::getVariableValue("NetServerListenPort") == "12683") {
	//	this->client = NetMessengerClient::New("127.0.0.1", "12682");
	//	cout << "This is client A..." << endl;
	//}
	//else {
   // this->client = NetMessengerClient::New("127.0.0.1", "2683");
   // cout << "This is client B..." << endl;
//}
	physEngine = new PhysicsCreate();
	cout << "##Creating physic engine##" << endl;

	triangleMesh = WOPhysXTriangularMesh::New();
	triangleMesh->init(this->physEngine);

	this->nmc = new NetMsgCreate();
	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.


}


void GLViewNewModule::onCreate()
{
	//GLViewNewModule::onCreate() is invoked after this module's LoadMap() is completed.
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
		cout << "Should put the 'Debug' folder under 'NewModule' directly." << endl;
		return;
	}

	//Set background music with 2D Sound when starting the program.
	this->smgr = SoundManager::init();
	this->smgr->play2D("../irrKlang-64bit-1.6.0/music/ophelia.mp3", true, false, true);
	this->smgr->getSound2D().at(0)->setVolume(0.5f);

	this->triangleMesh->createGrid();
}


GLViewNewModule::~GLViewNewModule()
{
	//Implicitly calls GLView::~GLView()
}


void GLViewNewModule::updateWorld()
{
	GLView::updateWorld(); //Just call the parent's update world first.
						   //If you want to add additional functionality, do it after
						   //this call.

	float dt = ManagerSDLTime::getTimeSinceLastPhysicsIteration() / 500.f;
	physx::PxScene* scene = this->physEngine->getScene();
	scene->simulate(dt);
	scene->fetchResults(true);

	physx::PxU32 numAAs = 0;
	physx::PxActor** aas = scene->getActiveActors(numAAs);

	//poses that have changed since the last update
	for (physx::PxU32 i = 0; i < numAAs; i++)
	{
		physx::PxRigidActor* actor = static_cast<physx::PxRigidActor*>(aas[i]);
		physx::PxTransform trans = actor->getGlobalPose();
		WOPhysicX* wo = static_cast<WOPhysicX*>(aas[i]->userData);
		Mat4 matrix = wo->onCreateNVPhysXActor(&trans);

		// Network
		//if (this->client->isTCPSocketOpen()) {
		//	this->nmc->setCamPos(cam->getPosition());
		//	this->nmc->setCamLookDir(cam->getLookDirection());
		//	this->nmc->setObjPos(wo->getPosition());
		//	this->nmc->setRotateZ(0.0f);
		//	this->nmc->setActorIndex(actorLst->getIndexOfWO(wo));
		//	this->nmc->setModelPath("");
		//	this->nmc->setDisplayMatrix(matrix);
		//	this->nmc->setMoveCam(false);
		//	//this->client->sendNetMsgSynchronousTCP(*this->nmc);
		//}
	}
}


void GLViewNewModule::onResizeWindow(GLsizei width, GLsizei height)
{
	GLView::onResizeWindow(width, height); //call parent's resize method.
}


void GLViewNewModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseDown(e);
	this->nmc->setCamPos(cam->getPosition());
	this->nmc->setCamLookDir(cam->getLookDirection());
	this->nmc->setMoveCam(true);
	this->nmc->setModelPath("");
	//this->client->sendNetMsgSynchronousTCP(*this->nmc);

}


void GLViewNewModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
	GLView::onMouseUp(e);
	this->nmc->setCamPos(cam->getPosition());
	this->nmc->setCamLookDir(cam->getLookDirection());
	this->nmc->setMoveCam(true);
	this->nmc->setModelPath("");
	//this->client->sendNetMsgSynchronousTCP(*this->nmc);
}


void GLViewNewModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
	GLView::onMouseMove(e);
	this->nmc->setCamPos(cam->getPosition());
	this->nmc->setCamLookDir(cam->getLookDirection());
	this->nmc->setMoveCam(true);
	this->nmc->setModelPath("");
	//this->client->sendNetMsgSynchronousTCP(*this->nmc);
}


void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{
	GLView::onKeyDown(key);
	if (key.keysym.sym == SDLK_0)
		this->setNumPhysicsStepsPerRender(1);

	if (key.keysym.sym == SDLK_1)
	{
		std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
		WOPhysicX* cubeWO = WOPhysicX::New(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl", Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		cubeWO->setPosition(Vector(100, 200, 20));
		cubeWO->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
		worldLst->push_back(cubeWO);
		actorLst->push_back(cubeWO);
		cubeWO->setEngine(this->physEngine);
		physx::PxRigidDynamic* da = this->physEngine->createDynamic(cubeWO);
		this->physEngine->addToScene(da);
	}

	if (key.keysym.sym == SDLK_s) {
		this->physEngine->shutdown();
	}
}


void GLViewNewModule::onKeyUp(const SDL_KeyboardEvent& key)
{
	GLView::onKeyUp(key);
}


void Aftr::GLViewNewModule::loadMap()
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
	std::string human(ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl");

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

	//physx::PxRigidStatic* sa = this->physEngine->createPlane(wo);
	//this->physEngine->addToScene(sa);

	createNewModuleWayPoints();
}


void GLViewNewModule::createNewModuleWayPoints()
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
