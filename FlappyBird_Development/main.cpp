#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#pragma warning (disable:4996)

#pragma comment(linker,"/subsystem:console")

// Include SDL lib
#include "Headers/SDL2-2.0.8/include/SDL.h"
#include "Headers/SDL2-2.0.8/include/SDL_image.h"
#include "Headers/SDL2-2.0.8/include/SDL_mixer.h"
#pragma comment(lib,"Headers\\SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"Headers\\SDL2-2.0.8\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"Headers\\SDL2-2.0.8\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"Headers\\SDL2-2.0.8\\lib\\x86\\SDL2_image.lib")
#pragma comment(lib,"Headers\\SDL2-2.0.8\\lib\\x86\\SDL2_mixer.lib")


const int Screen_Width = 800;
const int Screen_Height = 800;


#include "Headers/Pipe.h"
#include "Headers/Bird.h"
#include "Headers/Collision.h"
#include "Headers/Score.h"

SDL_Rect Pipes[20];
int Npipes=20;
int IndexPipe;
int Score = 0;

bird::bird Bird;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* BirdSprite_Text;
SDL_Texture* PipeBodySprite_Text;
SDL_Texture* PipeHeadSprite_Text;
SDL_Texture* Background_Text;
SDL_Texture* ScoreSheet_Text;

Mix_Music *Music;
Mix_Chunk *Ding;


int main(int argc, char** argv)
{
	// Init
	HWND windowHandle = GetConsoleWindow();
	ShowWindow(windowHandle, SW_HIDE);
	srand(time(0));
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Flap Flap", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Screen_Width, Screen_Height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface* BirdSprite = IMG_Load("Images/Bird.png");
	SDL_Surface* PipeBodySprite = IMG_Load("Images/Pipe_Body.png");
	SDL_Surface* PipeHeadSprite= IMG_Load("Images/Pipe_Head.png");
	SDL_Surface* Background = IMG_Load("Images/Background.png");
	SDL_Surface* ScoreSheet = IMG_Load("Images/ScoreSheet.png");
	BirdSprite_Text = SDL_CreateTextureFromSurface(renderer, BirdSprite);
	PipeBodySprite_Text = SDL_CreateTextureFromSurface(renderer, PipeBodySprite);
	PipeHeadSprite_Text = SDL_CreateTextureFromSurface(renderer, PipeHeadSprite);
	Background_Text = SDL_CreateTextureFromSurface(renderer, Background);
	ScoreSheet_Text = SDL_CreateTextureFromSurface(renderer, ScoreSheet);

	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	Music = Mix_LoadMUS("Sounds/BackgroundMusic.mp3");
	Mix_PlayMusic(Music, -1);

	Ding = Mix_LoadWAV("Sounds/Ding.wav");

	pipe::initPipe(Pipes,Npipes);
	bird::initBird(&Bird);

	int LastUpdate = clock();
	int CurrentTime = clock();
	bool alive = true;

	// Game loop
	for (;;)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) return 0;
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_SPACE)
				{
					Bird.Velocity = -45;
				}
				else if (event.key.keysym.sym == SDLK_r)
				{
					pipe::initPipe(Pipes, Npipes);
					bird::initBird(&Bird);
					alive = true;
					Score = 0;
				}
			}
		}
		// Check collision
		if (collision::Collide(Bird, Pipes, Npipes))alive = false;
		if (!alive) continue;
	
		SDL_RenderClear(renderer);
		CurrentTime = clock();
		if (CurrentTime - LastUpdate > 30)
		{
			bird::updatePos(&Bird);
			pipe::updatePipe(Pipes, Npipes);
			if (score::score(Pipes, Npipes, 300))
			{
				Score += 1;
				if (Score%10==0) Mix_PlayChannel(-1, Ding, 0);
			}
			
			LastUpdate = CurrentTime;
		}
		SDL_RenderCopyEx(renderer, Background_Text, NULL, NULL, NULL, NULL, SDL_FLIP_NONE);
		pipe::showPipe(Pipes, Npipes, PipeBodySprite_Text, PipeHeadSprite_Text, renderer);
		score::showScore(Score,ScoreSheet_Text,renderer);
		SDL_Rect tmp = { 300, Bird.Position , Bird.Size, Bird.Size };
		SDL_RenderCopyEx(renderer, BirdSprite_Text, NULL, &tmp, Bird.Velocity / 3, NULL, SDL_FLIP_NONE);
		SDL_RenderPresent(renderer);
	}
	return 0;
}