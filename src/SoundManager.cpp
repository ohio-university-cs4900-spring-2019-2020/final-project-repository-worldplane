#include "SoundManager.h"

using namespace Aftr;
using namespace std;


// Initialize soundengine.
SoundManager* SoundManager::init() {
	SoundManager* smgr = new SoundManager();
	irrklang::ISoundEngine* tmpEngine = irrklang::createIrrKlangDevice();

	if (tmpEngine == NULL) {
		cout << "Create Sound Engine Error!!!" << endl;
	}
	smgr->setSoundEngine(tmpEngine);

	return smgr;
}

// Play 2D sound. Add to the vector.
void SoundManager::play2D(const char* soundFileName, bool playLooped, bool startPaused, bool track) {
	if (!this->engine)
		return;

	this->sound2D.push_back(this->engine->play2D(soundFileName, playLooped, startPaused, track));
}

// Play 3D sound. Add to the vector.
void SoundManager::play3D(const char* soundFileName, Vector position, bool playLooped, bool startPaused, bool track) {
	if (!this->engine)
		return;

	this->sound3D.push_back(this->engine->play3D(soundFileName, this->convertFromVector(position), playLooped, startPaused, track));
}

// Stop a 2D sound.
void SoundManager::stopSound2D() {
	if (sound2D.empty()) {
		return;
	}

	sound2D.at(0)->stop();
	sound2D.erase(sound2D.begin());
}

// Stop a 3D sound.
void SoundManager::stopSound3D() {
	if (sound3D.empty()) {
		return;
	}

	sound3D.at(0)->stop();
	sound3D.erase(sound3D.begin());
}

// Convert position type from Aftr::Vector into irrklang vec3df.
irrklang::vec3df SoundManager::convertFromVector(Vector position) {
	return irrklang::vec3df(position.x, position.y, position.z);
}


void SoundManager::setSoundEngine(irrklang::ISoundEngine* soundEngine) {
	this->engine = soundEngine;
}


irrklang::ISoundEngine* SoundManager::getSoundEngine() {
	return this->engine;
}

std::vector<irrklang::ISound*> SoundManager::getSound2D() {
	return this->sound2D;
}

std::vector<irrklang::ISound*> SoundManager::getSound3D() {
	return this->sound3D;
}