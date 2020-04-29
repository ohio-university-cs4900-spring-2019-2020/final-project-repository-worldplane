#pragma once

#include "GLView.h"
#include "NetMessengerClient.h"
#include "SoundManager.h"
#include "irrKlang.h"
#include "NetMsgSimpleWO.h"
#include "WOPhysicX.h"
#include "WOPhysXTriangularMesh.h"
#include "WOFTGLString.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewFinalServer
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewFinalServer : public GLView
{
public:
   static GLViewFinalServer* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewFinalServer();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createFinalServerWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void updateHealthLabel();
   NetMessengerClient* client;
   PhysicsCreate* physEngine;

protected:
   GLViewFinalServer( const std::vector< std::string >& args );
   virtual void onCreate();   

   SoundManager* smgr;
   WOPhysXTriangularMesh* triangleMesh;
   WOFTGLString* wcHealthStr;
   WOFTGLString* jetHealthStr;
   WOFTGLString* ggStr = nullptr;
};

/** \} */

} //namespace Aftr
