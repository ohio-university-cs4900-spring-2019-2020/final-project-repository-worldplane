#pragma once
#include "NetMsg.h"
#include "Vector.h"
#include "Mat4.h"
#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr {

	class NetMsgCreate : public NetMsg {
	public:
		NetMsgMacroDeclaration(NetMsgCreate);

		NetMsgCreate();
		//virtual ~NetMsgCreate();
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		//getters and setters
		Vector getObjPos();
		void setObjPos(Vector v);
		Vector getCamPos();
		void setCamPos(Vector v);
		Vector getCamLookDir();
		void setCamLookDir(Vector dir);
		float getRotateZ();
		void setRotateZ(float rotate);
		void setActorIndex(int i);
		void setModelPath(std::string p);
		void setDisplayMatrix(Mat4 m);
		Mat4 getDisplayMatrix();
		void setMoveCam(bool b);

	protected:
		Vector objPos; //object position
		Vector camPos; //camera position
		Vector camLookDir; //camera look direction
		float rotateZ; //rotate on Z axis
		int actorIndex;
		std::string modelPath;
		Mat4 m;
		bool moveCam;
	};
}

#endif
