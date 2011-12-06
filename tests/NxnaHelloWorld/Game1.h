#ifndef GAME1_H
#define GAME1_H

#include "Nxna.h"

class Game1 : public Nxna::Game
{
	Nxna::GraphicsDeviceManager* m_graphics;

public:
	Game1();

protected:

	virtual void Initialize() override;
	virtual void LoadContent() override;
	virtual void Update(const Nxna::GameTime& time) override;
	virtual void Draw(const Nxna::GameTime& time) override;

	virtual void HandleEvents() override;
};

#endif // GAME1_H
