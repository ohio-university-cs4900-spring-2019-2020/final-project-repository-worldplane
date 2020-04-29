<<<<<<< HEAD:FinalClient/src/GLViewFinalClient.h
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
   \class GLViewFinalClient
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewFinalClient : public GLView
{
public:
   static GLViewFinalClient* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewFinalClient();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createFinalClientWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void updateHealthLabel();

protected:
   GLViewFinalClient( const std::vector< std::string >& args );
   virtual void onCreate();   

   SoundManager* smgr;
   NetMessengerClient* client;
   PhysicsCreate* physEngine;
   WOPhysXTriangularMesh* triangleMesh;
   WOFTGLString* wcHealthStr;
   WOFTGLString* jetHealthStr;
   WOFTGLString* ggStr = nullptr;
};

/** \} */

} //namespace Aftr
=======
#pragma once

#include "GLView.h"
#include "NetMessengerClient.h"
#include "SoundManager.h"
#include "irrKlang.h"
#include "NetMsgCreate.h"
#include "WOPhysicX.h"
#include "WOFTGLString.h"

namespace Aftr
{
   class Camera;

/**
   \class GLViewNewModule
   \author Scott Nykl 
   \brief A child of an abstract GLView. This class is the top-most manager of the module.

   Read \see GLView for important constructor and init information.

   \see GLView

    \{
*/

class GLViewNewModule : public GLView
{
public:
   static GLViewNewModule* New( const std::vector< std::string >& outArgs );
   virtual ~GLViewNewModule();
   virtual void updateWorld(); ///< Called once per frame
   virtual void loadMap(); ///< Called once at startup to build this module's scene
   virtual void createNewModuleWayPoints();
   virtual void onResizeWindow( GLsizei width, GLsizei height );
   virtual void onMouseDown( const SDL_MouseButtonEvent& e );
   virtual void onMouseUp( const SDL_MouseButtonEvent& e );
   virtual void onMouseMove( const SDL_MouseMotionEvent& e );
   virtual void onKeyDown( const SDL_KeyboardEvent& key );
   virtual void onKeyUp( const SDL_KeyboardEvent& key );
   void updateHealthLabel();

protected:
   GLViewNewModule( const std::vector< std::string >& args );
   virtual void onCreate();   

   SoundManager* smgr;
   NetMessengerClient* client;
   NetMsgCreate* nmc;
   PhysicsCreate* physEngine;
   WOFTGLString* wcHealthStr;
   WOFTGLString* jetHealthStr;
   WOFTGLString* ggStr = nullptr;
};

/** \} */

} //namespace Aftr
>>>>>>> master:src/GLViewNewModule.h
