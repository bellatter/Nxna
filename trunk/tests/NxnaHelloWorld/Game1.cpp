#include <iostream>
#include "Graphics/OpenGL/OpenGLDevice.h"
#include "Platform/OpenGlWindow.h"
#include "Platform/Input.h"
#include "Game1.h"

Game1::Game1()
{
	m_graphics = new Nxna::Platform::OpenGlWindow(this);
}

Nxna::Vector2 pos;
Nxna::Vector2 vel;
Nxna::Graphics::Texture2D* tex;
Nxna::Graphics::SpriteBatch* sb;
const float width = 32;
const float height = 32;

void Game1::Initialize()
{
	m_graphics->SetScreenSize(320, 240, false);

	Game::Initialize();
}

void Game1::LoadContent()
{
	// create the sprite batch
	sb = new Nxna::Graphics::SpriteBatch(GetGraphicsDevice());

	// create a dummy texturel
    
	byte pixels[] = { 128, 128, 128, 128 };

	tex = GetGraphicsDevice()->CreateTexture(1, 1, Nxna::Graphics::SurfaceFormat_Color);
	tex->SetData(0, pixels, 4);

	vel.X = 50.0f;
	vel.Y = 50.0f;

	pos.X = 10;
	pos.Y = 40;
}

void Game1::Update(const Nxna::GameTime& time)
{
	pos += vel * time.ElapsedGameTime;

	Nxna::Graphics::Viewport vp = GetGraphicsDevice()->GetViewport();
    
    /*Nxna::Rectangle vp;
    vp.X = 0;
    vp.Y = 0;
    vp.Width = 100;
    vp.Height = 100;*/
    
	if (pos.X - width * 0.5f < 0)
	{
		pos.X = width * 0.5f;
		vel.X = -vel.X;
	}
	else if (pos.X + width * 0.5f > vp.Width)
	{
		pos.X = vp.Width - width * 0.5f;
		vel.X = -vel.X;
	}
	if (pos.Y - height * 0.5f < 0)
	{
		pos.Y = height * 0.5f;
		vel.Y = -vel.Y;
	}
	else if (pos.Y + height * 0.5f > vp.Height)
	{
		pos.Y = vp.Height - height * 0.5f;
		vel.Y = -vel.Y;
	}
}

void Game1::Draw(const Nxna::GameTime& time)
{
	GetGraphicsDevice()->Clear(Nxna::Color::GetCornflowerBlue());

	Nxna::Rectangle r;
	r.X = pos.X - width * 0.5f;
	r.Y = pos.Y - height *0.5f;
	r.Width = width;
	r.Height = height;
    
    Nxna::Rectangle r2;
    r2.X = 0;
    r2.Y = 0;
    r2.Width = 100;
    r2.Height = 10;
    
    Nxna::Rectangle r3;
    r3.X = 100;
    r3.Y = 100;
    r3.Width = 100;
    r3.Height = 100;
    
    Nxna::Rectangle r4;
    r4.X = 150;
    r4.Y = 150;
    r4.Width = 100;
    r4.Height = 100;
    
    Nxna::Color red(255, 0, 0);
    Nxna::Color blue(0, 0, 255);
    Nxna::Color halfred = red * 0.5f;
    Nxna::Color halfblue = blue * 0.5f;

	sb->Begin();
	sb->Draw(tex, r, nullptr, red);
    sb->Draw(tex, r2, nullptr, red);
    sb->Draw(tex, r3, nullptr, halfblue);
    sb->Draw(tex, r4, nullptr, halfred);
	sb->End();
}

void Game1::HandleEvents()
{
	Nxna::Platform::Input::Refresh();

	if (Nxna::Platform::Input::WasQuitReceived())
		Exit();
}
