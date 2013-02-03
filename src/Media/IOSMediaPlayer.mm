#include "IOSMediaPlayer.h"
#include "Song.h"
#import <AVFoundation/AVFoundation.h>

namespace Nxna
{
namespace Media
{
	void* IOSMediaPlayer::m_currentPlayer = nullptr;

	bool IOSMediaPlayer::Play(Song* song)
	{
		if (song->m_handle == nullptr)
		{
			NSString* urlStr = [[NSString alloc] initWithUTF8String:song->m_path.c_str()];
			NSURL* url = [NSURL fileURLWithPath:urlStr];
	
			NSError* error;
			AVAudioPlayer* player = [[AVAudioPlayer alloc] initWithContentsOfURL: url error:&error];
			song->m_handle = player;
			
			if (player != nil)
			{
				m_currentPlayer = player;
				[player play];
			}
			else
			{
				NSLog([error description]);
				return false;
			}
		}
		else
		{
			if (m_currentPlayer != nullptr)
			{
				AVAudioPlayer* oldPlayer = static_cast<AVAudioPlayer*>(m_currentPlayer);
				[m_currentPlayer stop];
				m_currentPlayer = nullptr;
			}
		
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(song->m_handle);
			
			if (player != nil)
			{
				player.currentTime = 0;
				player.volume = 1.0f;
				[player play];
				m_currentPlayer = player;
			}
			else
				return false;
		}
	
		return true;
	}
	
	void IOSMediaPlayer::Stop()
	{
		if (m_currentPlayer != nullptr)
		{
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(m_currentPlayer);
			[player stop];
			m_currentPlayer = nullptr;
		}
	}
	
	bool IOSMediaPlayer::IsRepeating()
	{
		if (m_currentPlayer != nullptr)
		{
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(m_currentPlayer);
			return player.numberOfLoops < 0;		
		}
		
		return false;
	}
	
	void IOSMediaPlayer::IsRepeating(bool repeating)
	{
		if (m_currentPlayer != nullptr)
		{
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(m_currentPlayer);
			player.numberOfLoops = (repeating ? -1 : 0);		
		}
	}
	
	void IOSMediaPlayer::SetVolume(float volume)
	{
		if (m_currentPlayer != nullptr)
		{
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(m_currentPlayer);
			player.volume = volume;		
		}
	}
	
	float IOSMediaPlayer::GetVolume()
	{
		if (m_currentPlayer != nullptr)
		{
			AVAudioPlayer* player = static_cast<AVAudioPlayer*>(m_currentPlayer);
			return player.volume;		
		}		
		
		return 0;
	}
}
}
