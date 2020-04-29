#include "NetMsgSimpleWO.h"
#include <sstream>
#include <iostream>
#include "GLViewFinalServer.h"
#include "ManagerGLView.h"
#include "WorldContainer.h"
#include "Model.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgSimpleWO);

bool NetMsgSimpleWO::toStream(NetMessengerStreamBuffer & os) const
{
	os << pos.x << pos.y << pos.z << id << new_indicator << dma[0] << dma[1] << dma[2] << dma[3] << dma[4] << dma[5] << dma[6] << dma[7] << dma[8] << dma[9] << dma[10] << dma[11] << dma[12] << dma[13] << dma[14] << dma[15];
	return true;
}

bool NetMsgSimpleWO::fromStream(NetMessengerStreamBuffer & is)
{
	is >> pos.x >> pos.y >> pos.z >> id >> new_indicator >> dma[0] >> dma[1] >> dma[2] >> dma[3] >> dma[4] >> dma[5] >> dma[6] >> dma[7] >> dma[8] >> dma[9] >> dma[10] >> dma[11] >> dma[12] >> dma[13] >> dma[14] >> dma[15];
	return true;
}

void NetMsgSimpleWO::onMessageArrived()
{
	if (new_indicator == 0) {
		WO* wo = ((GLViewFinalServer*)ManagerGLView::getGLView())->getActorLst()->at(id);
		wo->getModel()->setDisplayMatrix(dma);
		wo->setPosition(pos);
	}
	else if (new_indicator == 11) { // missle addition in wheelcar
		std::string missile(ManagerEnvironmentConfiguration::getSMM() + "/models/rocket/missle/missiles.obj");
		WOPhysicX* missileWO = WOPhysicX::New(missile, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		missileWO->setPosition(pos);
		missileWO->setLabel("Missile");
		missileWO->getModel()->setDisplayMatrix(dma);
		missileWO->setEngine(((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine);
		physx::PxRigidDynamic* da = ((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine->createDynamicMissile(missileWO, physx::PxVec3(0.0f, 0.0f, 90.0f));
		((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine->addToScene(da);
		((GLViewFinalServer*)ManagerGLView::getGLView())->getWorldContainer()->push_back(missileWO);
		((GLViewFinalServer*)ManagerGLView::getGLView())->getActorLst()->push_back(missileWO);
	}
	else if (new_indicator == 12) { // missle addition in jet
		std::string missile(ManagerEnvironmentConfiguration::getSMM() + "/models/rocket/missle/missiles.obj");
		WOPhysicX* missileWO = WOPhysicX::New(missile, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		missileWO->setPosition(pos);
		missileWO->setLabel("Missile");
		missileWO->getModel()->setDisplayMatrix(dma);
		missileWO->setEngine(((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine);
		physx::PxRigidDynamic* da = ((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine->createDynamicMissile(missileWO, physx::PxVec3(0.0f, 0.0f, -5.0f));
		((GLViewFinalServer*)ManagerGLView::getGLView())->physEngine->addToScene(da);
		((GLViewFinalServer*)ManagerGLView::getGLView())->getWorldContainer()->push_back(missileWO);
		((GLViewFinalServer*)ManagerGLView::getGLView())->getActorLst()->push_back(missileWO);
		
	}
	else if (new_indicator == 22) { // missle delete
		((GLViewFinalServer*)ManagerGLView::getGLView())->getWorldContainer()->eraseViaWOIndex(id);
		((GLViewFinalServer*)ManagerGLView::getGLView())->getActorLst()->eraseViaWOIndex(id);
	}
}

std::string NetMsgSimpleWO::toString() const
{
	std::stringstream ss;
	ss << NetMsg::toString();
	ss << "Payload: \n"
		<< "p:{" << pos.x << "," << pos.y << "," << pos.z << "} id:" << id << ",new_indicator:" << new_indicator << ",Matrix:" << dma[0] << dma[1] << dma[2] << dma[3] << dma[4] << dma[5] << dma[6] << dma[7] << dma[8] << dma[9] << dma[10] << dma[11] << dma[12] << dma[13] << dma[14] << dma[15] << "\n";
	return ss.str();
}
