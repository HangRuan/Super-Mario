#pragma once
#include "CEngine.h"
#include "CSprite.h"
#include "CDxSound.h"
#include "CMap.h"
#include "CGdiBmp.h"
#include "Mai.h"
#include "MaiMarioSprite.h"

using namespace std;

struct SPRITEINFO{
	LPTSTR		BmpPath;
	int			TotalFrames;
	int			RowFrames;
	double		Ratio;
	int			Number;
	int			Score;
	POINT		Position;
	int			Dir;
	int			Speed;
	int			RestLife;
	bool		Active;
	bool		Visible;
	COLORREF	TransRGB;
	int			Mirror;
};

CEngine*	pEngine;
HINSTANCE	gameInstance;
HDC			bufferDC;
HBITMAP		bufferBitmap;
SIZE		WinSize;

CSprite*	Player;
CSprite*	coin;

CMap*		map;
RECT		PlayerBoundary;
RECT		ScreenBoundry;

bool		inLevelBegin;		//是否在游戏开始前显示的界面中
bool		inWelcome;			//是否在欢迎界面中
bool		inPlaying;			//是否在游戏进行的画面中
int			currentLevel;
int			inPlayScore;
int			TotalScores;
int			LifeRemain;
bool		UpGgradeLevel;
bool		isDragingFlag;
bool		isGettingCoin;
int			coinCount;
int			InPlayingTimeSpend;
int			dragDistance;

long		newTimeCount;
long		oldTimeCount;
long		coinTickold;
long		coinTicknew;
bool		isDelayCollision;

/* picture Resource */
CGdiBmp*	WelcomeScreenLogo;
CGdiBmp*	mario_Info;
CGdiBmp*	about;

/* music below */
CDxSound		ds;
CDxSoundBuffer	BGSound;
CDxSoundBuffer	JumpSound;
CDxSoundBuffer	DeadSound;
CDxSoundBuffer	GameTotallyEndSound;
CDxSoundBuffer	GameOverSound;
CDxSoundBuffer	LevelEndSound;
CDxSoundBuffer	DragFlagPloeSound;
CDxSoundBuffer	StepOnEnemySound;
CDxSoundBuffer	KnockBrickSound;
CDxSoundBuffer	GoldCoinSound;


void CreatePlayer(SPRITEINFO spInfo);
void UpdatePlayerPosition();

void LoadGame();
void LoadCoinSprite();
void ReleaseAll();

void checkMapAndSound();
void UpdateCoinPosition();

void DrawWelcome(HDC OffDC);
void DrawBeforeLevelInfo(HDC OffDC);
void DrawGameTotallyEnd(HDC OffDC);
void DrawInPlayingScore(HDC OffDC);
