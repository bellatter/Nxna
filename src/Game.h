#ifndef NXNA_GAME_H
#define NXNA_GAME_H

#include "GameTime.h"

namespace Nxna
{
	namespace Content
	{
		class ContentManager;
	}

	namespace Graphics
	{
		class GraphicsDevice;
	}

	class IGraphicsDeviceManager;
	class GraphicsDeviceManager;

	class Game
	{
		friend class GraphicsDeviceManager;

		float m_targetElapsedTime;
		bool m_exitRequested;
		Nxna::Content::ContentManager* m_content;
		Nxna::Graphics::GraphicsDevice* m_device;
		GraphicsDeviceManager* m_graphicsDeviceManager;
		GameTime m_gameTime;
		GameTime m_realTime;
		bool m_isFixedTimeStep;

	public:
		Game();
		virtual ~Game();

		bool IsFixedTimeStep() { return m_isFixedTimeStep; }
		void IsFixedTimeStep(bool isFixedTimeStep) { m_isFixedTimeStep = isFixedTimeStep; }

		Nxna::Content::ContentManager* GetContent() { return m_content; }
		Nxna::Graphics::GraphicsDevice* GetGraphicsDevice() { return m_device; }

		float GetTargetElapsedTime() { return m_targetElapsedTime; }
		void SetTargetElapsedTime(float targetElapsedTime) { m_targetElapsedTime = targetElapsedTime; }

		void Run();
		void Exit();

	protected:

		virtual void Initialize()
		{
			// XNA seems to call LoadContent() within Initialize()
			LoadContent(); 
		}

		virtual void LoadContent() { }
		virtual void Update(const Nxna::GameTime& time) { }
		virtual void Draw(const Nxna::GameTime& time) { }
		virtual void UnloadContent() { }
		virtual void OnExiting() { }

	private:
		void handleEvents();
		void updateTime();
	};
}

#endif // NXNA_GAME_H