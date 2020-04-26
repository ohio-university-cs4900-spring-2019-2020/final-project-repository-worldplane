#include "irrKlang.h"
#include "ik_ISoundEngine.h"
#include "Vector.h"

namespace Aftr {
	class SoundManager {
	public:
		static SoundManager* init();
		virtual void play2D(const char* soundFileName, bool playLooped, bool startPaused, bool track);
		virtual void play3D(const char* soundFileName, Aftr::Vector position, bool playLooped, bool startPaused, bool track);
		virtual void stopSound2D();
		virtual void stopSound3D();
		virtual irrklang::vec3df convertFromVector(Vector position);

		//getters and setters
		virtual irrklang::ISoundEngine* getSoundEngine();
		virtual void setSoundEngine(irrklang::ISoundEngine* soundEngine);
		virtual std::vector<irrklang::ISound*> getSound2D();
		virtual std::vector<irrklang::ISound*> getSound3D();

	protected:
		irrklang::ISoundEngine* engine = nullptr;  //ISoundEngine
		std::vector<irrklang::ISound*> sound2D;    //Vector for 2D musics
		std::vector<irrklang::ISound*> sound3D;    //Vector for 3D musics
	};
}