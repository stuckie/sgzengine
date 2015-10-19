/*
	SGZEngine
	Copyright (C) 2008 - 2009 Steven Campbell

	This engine is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this engine; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Steven Campbell
	stuckie@stuckiegamez.co.uk
*/

/**
	\file SDLAudio.cpp
	\brief The SDL Audio Object

*/

#include "SDLAudio.h"
#include "../../../common/debug/CDebug.h"

using namespace SGZ;

bool SDLAudio::createInterface()
{
	/** Create a client interface to talk on */
	if(SDL_Init(SDL_INIT_AUDIO)==-1)
	{
		Logger.log( SGZLOG::Warning, "AudioMAN: Error setting up SDL Audio!\n");
		return false;
	}

	#ifdef PLAT_PC
	if(Mix_OpenAudio(44100, AUDIO_S16, 2, 512)==-1)
	#endif
	{
		Logger.log( SGZLOG::Warning, "AudioMAN: Error Opening Mixer!\n");
		return false;
	}
	Logger.log( SGZLOG::Debug, "AudioMAN: SDLAudio Initialised!\n");

	/** Give us 10 channels to play with from the outset */
	Mix_AllocateChannels(10);

	return true;
}

bool SDLAudio::destroyInterface()
{
	/** Destroy a client interface */
	SListIterator<SDLMusic*> MusicListITR = MusicList.getIterator();

	MusicListITR.start();
	while (MusicListITR.valid())
	{
		Mix_FreeMusic(MusicListITR.item()->Data);
		MusicList.remove(MusicListITR);
		MusicListITR.forth();
	}

	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	SFXListITR.start();
	while (SFXListITR.valid())
	{
		Mix_FreeChunk(SFXListITR.item()->Data);
		SFXList.remove(SFXListITR);
		SFXListITR.forth();
	}

	Mix_CloseAudio();

	return true;
}

bool SDLAudio::updateAudio()
{
	/** Update/Poll Audio where applicable */
	/** SDL does not poll audio */
	return true;
}

bool SDLAudio::loadSFXFile( const std::string &Filename, const std::string &SFXName )
{
	/** Check to see if we've already loaded it */
	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
		if((SFXListITR.item()->Name.compare(SFXName))==0)
		{
			Logger.log( SGZLOG::Debug, "AudioMAN: SFX \"" + SFXName + "\" already loaded!\n");
			return false;
		}

	/** Load a sound effect to play */
	Logger.log( SGZLOG::Debug, "AudioMAN: Loading SFX \"" + Filename + "\" as " + SFXName + "\n");
	SDLSound *Effect;
	Effect = new SDLSound;
	Effect->Name = SFXName;
	Effect->Data = Mix_LoadWAV(Filename.c_str());
	if(Effect->Data == NULL)
	{
		Logger.log( SGZLOG::Warning, "AudioMAN: Failed to load SFX: " + Filename + "\n");
		delete Effect;
		return false;
	}
	else
		SFXList.append(Effect);
	return true;
}

bool SDLAudio::loadMUSFile( const std::string &Filename, const std::string &MUSName )
{
	/** Check to see if we've already loaded it */
	SListIterator<SDLMusic*> MusicListITR = MusicList.getIterator();

	for( MusicListITR.start(); MusicListITR.valid(); MusicListITR.forth() )
		if((MusicListITR.item()->Name.compare(MUSName))==0)
		{
			Logger.log( SGZLOG::Debug, "AudioMAN: SFX: " + MUSName + " already loaded!\n");
			return false;
		}

	/** Load a music to play */
	Logger.log( SGZLOG::Debug, "AudioMAN: Loading MUS: " + Filename + " as " + MUSName + "\n");
	SDLMusic *Music;
	Music = new SDLMusic;
	Music->Name = MUSName;
	Music->Data = Mix_LoadMUS(Filename.c_str());
	if(Music->Data == NULL)
	{
		Logger.log( SGZLOG::Warning, "AudioMAN: Failed to load MUS: " + Filename + "\n");
		delete Music;
		return false;
	}
	else
		MusicList.append(Music);
	return true;
}

int SDLAudio::playSFX( const std::string &SFXName, const int loops, const int channel )
{
	/** Play sound effect */
	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
		if((SFXListITR.item()->Name.compare(SFXName))==0)
		{
			return Mix_PlayChannel(channel, SFXListITR.item()->Data, loops);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find SFX \""+ SFXName + "\" to play! Is it loaded?\n");
	return -1;
}

bool SDLAudio::playMUS( const std::string &MUSName, const int loops )
{
	/** Play Music */
	SListIterator<SDLMusic*> MusicListITR = MusicList.getIterator();

	for( MusicListITR.start(); MusicListITR.valid(); MusicListITR.forth() )
		if((MusicListITR.item()->Name.compare(MUSName))==0)
		{
			Mix_PlayMusic(MusicListITR.item()->Data, loops);
			Logger.log( SGZLOG::Debug, "AudioMAN: Playing " + MUSName + ", " + /*loops + */" times\n");
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find MUS \""+ MUSName + "\" to play! Is it loaded?\n");
	return false;
}

bool SDLAudio::removeSFX( const std::string &SFXName )
{
	/** remove sound effect */
	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
		if((SFXListITR.item()->Name.compare(SFXName))==0)
		{
			Mix_FreeChunk(SFXListITR.item()->Data);
			SFXList.remove(SFXListITR);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find SFX \""+ SFXName +"\" to unload!\n");
	return false;
}

bool SDLAudio::removeMUS( const std::string &MUSName )
{
	/** remove Music */
	Logger.log( SGZLOG::Debug, "AudioMAN: Removing " + MUSName);
	SListIterator<SDLMusic*> MusicListITR = MusicList.getIterator();

	for( MusicListITR.start(); MusicListITR.valid(); MusicListITR.forth() )
		if((MusicListITR.item()->Name.compare(MUSName))==0)
		{
			Mix_FreeMusic(MusicListITR.item()->Data);
			MusicList.remove(MusicListITR);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find MUS \""+ MUSName +"\" to unload!\n");
	return false;
}

int SDLAudio::fadeInSFX( const std::string &SFXName, const int time, const int loops, const int channel)
{
	/** Play SFX */
	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
		if((SFXListITR.item()->Name.compare(SFXName))==0)
		{
			return Mix_FadeInChannel(channel, SFXListITR.item()->Data, loops, time);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find SFX \""+ SFXName +"\" to Fade In!\n");
	return -1;
}

bool SDLAudio::fadeInMUS( const std::string &MUSName, const int time, const int loops)
{
	/** Play Music */
	SListIterator<SDLMusic*> MusicListITR = MusicList.getIterator();

	for( MusicListITR.start(); MusicListITR.valid(); MusicListITR.forth() )
		if((MusicListITR.item()->Name.compare(MUSName))==0)
		{
			Mix_FadeInMusic(MusicListITR.item()->Data, loops, time);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find MUS \"" + MUSName + "\" to Fade In!\n");
	return false;
}

bool SDLAudio::fadeOutCHAN( const int channel, const int time)
{
	Mix_FadeOutChannel(channel, time);

	return true;
}

bool SDLAudio::fadeOutMUS( const int time )
{
	Mix_FadeOutMusic(time);

	return true;
}

bool SDLAudio::stopMUS()
{
	/** Stop Playing */
	Mix_HaltMusic();
	return true;
}

bool SDLAudio::stopCHAN( const int channel )
{
	Mix_HaltChannel(channel);

	return true;
}

bool SDLAudio::pauseMUS()
{
	/** Pause the file */
	Mix_PauseMusic();
	return true;
}

bool SDLAudio::pauseCHAN( const int channel )
{
	Mix_Pause(channel);
	return true;
}

bool SDLAudio::resumeMUS()
{
	Mix_ResumeMusic();
	return true;
}

bool SDLAudio::resumeCHAN( const int channel )
{
	Mix_Resume(channel);
	return true;
}

bool SDLAudio::rewindMUS()
{
	Mix_RewindMusic();
	return true;
}

bool SDLAudio::setMUSPOS( const double position )
{
	Mix_SetMusicPosition(position);
	return true;
}

bool SDLAudio::setVolumeMUS(const int volume )
{
	Mix_VolumeMusic(volume);
	return true;
}

bool SDLAudio::setVolumeSFX( const std::string &SFXName, const int volume )
{
	SListIterator<SDLSound*> SFXListITR = SFXList.getIterator();

	if((SFXName.compare("-1"))==0)
	{
		for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
			Mix_VolumeChunk(SFXListITR.item()->Data, volume);

		return true;
	}
	else
	{
		for( SFXListITR.start(); SFXListITR.valid(); SFXListITR.forth() )
			if((SFXListITR.item()->Name.compare(SFXName))==0)
			{
				Mix_VolumeChunk(SFXListITR.item()->Data, volume);
				return true;
			}
	}

	Logger.log( SGZLOG::Warning, "AudioMAN: Can't find SFX \""+ SFXName +"\" to Set Volume!\n");
	return false;
}

int SDLAudio::isCHANPlaying( const int channel )
{
	return Mix_Playing(channel);
}

bool SDLAudio::isMUSPlaying()
{
	return Mix_PlayingMusic() ? true : false;
}

bool SDLAudio::setChannels( const int chans )
{
	Mix_AllocateChannels(chans);
	return true;
}

bool SDLAudio::setPanning( const int channel, const int left, const int right )
{
	Mix_SetPanning(channel, left, right);
	return true;
}

bool SDLAudio::setDistance( const int channel, const int distance )
{
	Mix_SetDistance(channel, distance);
	return true;
}

bool SDLAudio::setPosition( const int channel, const int angle, const int distance )
{
	Mix_SetPosition(channel, angle, distance);
	return true;
}

bool SDLAudio::setReverseStereo( const int channel, const bool flip )
{
	Mix_SetReverseStereo(channel, flip);
	return true;
}

bool SDLAudio::createGROUP( const int num, const std::string& name )
{
	/** Check to see if we've already created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			Logger.log( SGZLOG::Debug, "AudioMAN: Group \"" + name +"\" already created!\n");
			return false;
		}

	/** Create a channel group */
	Logger.log( SGZLOG::Debug, "AudioMAN: Creating SFX Group \"" + name +"\"\n");
	SDLChanGroup *Group;
	Group = new SDLChanGroup;
	Group->Name = name;
	Group->groupNum = num;
	GroupList.append(Group);
	return true;
}

bool SDLAudio::removeGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			GroupList.remove(GroupListITR);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \""+name+"\" doesn't exist!\n");
	return false;
}

bool SDLAudio::reserveCHANS( const int num )
{
	Mix_ReserveChannels(num);
	return true;
}

bool SDLAudio::groupCHAN( const int chan, const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			Mix_GroupChannel(chan, GroupListITR.item()->groupNum);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return false;
}

bool SDLAudio::groupCHANS( const int from, const int to, const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			Mix_GroupChannels(from, to, GroupListITR.item()->groupNum);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return false;
}

int SDLAudio::countGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			return Mix_GroupCount(GroupListITR.item()->groupNum);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return 0;
}

int SDLAudio::CHANinGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();
	if (name.compare("all")==0) { return Mix_GroupAvailable(-1); }

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			return Mix_GroupAvailable(GroupListITR.item()->groupNum);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return 0;
}

int SDLAudio::oldestGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			return Mix_GroupOldest(GroupListITR.item()->groupNum);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return 0;
}

int SDLAudio::newestGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			return Mix_GroupNewer(GroupListITR.item()->groupNum);
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return 0;
}

bool SDLAudio::fadeGROUP( const std::string &name, const int time )
{
	 /** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			Mix_FadeOutGroup(GroupListITR.item()->groupNum, time);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return false;
}

bool SDLAudio::haltGROUP( const std::string &name )
{
	/** Check to see if we've created it */
	SListIterator<SDLChanGroup*> GroupListITR = GroupList.getIterator();

	for( GroupListITR.start(); GroupListITR.valid(); GroupListITR.forth() )
		if((GroupListITR.item()->Name.compare(name))==0)
		{
			Mix_HaltGroup(GroupListITR.item()->groupNum);
			return true;
		}

	Logger.log( SGZLOG::Warning, "AudioMAN: Group \"" + name + "\" doesn't exist!\n");
	return false;
}
