#include "debug_Global.h"
#include "SuperMario.h"
#include "MaiMarioSprite.h"
#include "CDxSound.h"


int LevelEndSoundTime[] = {5000, 3000, 5000};

bool GameInitialize(HINSTANCE hInstance)
{
	//构造CEnging类对象
	WinSize.cx=NORMAL_SCREEN_WIDTH;
	WinSize.cy=NORMAL_SCREEN_HEIGHT;
	pEngine= new CEngine(hInstance,TEXT("SuperMario"),
		TEXT("SuperMario Version 1.0 By Ncu(C)2012-2013"),
		WinSize.cx,WinSize.cy);
	//设置游戏绘制速度为每秒大约15帧
	pEngine->SetFrameSpeed(15);
	if(pEngine==NULL) 
		return FALSE;
	return TRUE;
}

void GameBegin(HWND hWindow)
{
	//初始化随机数的种子
	srand(GetTickCount());
	//-----------------------------
	//创建内存缓冲设备及内存缓冲位图
	//------------------------------
	bufferDC=CreateCompatibleDC(GetDC(hWindow));
	bufferBitmap=CreateCompatibleBitmap(GetDC(hWindow),
		pEngine->GetWidth(),
		pEngine->GetHeight()
		);
	SelectObject(bufferDC,bufferBitmap);
	LoadGame();
}

void LoadGame()
{
	currentLevel = 1;
	inPlayScore = 0;
	coin = NULL;
	LifeRemain = 5;
	TotalScores = 0;
	InPlayingTimeSpend = 0;
	dragDistance = 0;
	inWelcome = true;
	inLevelBegin = false;
	inPlaying = false;
	isDelayCollision = true;
	UpGgradeLevel = false;
	isDragingFlag = false;
	isGettingCoin = 0;
	newTimeCount = oldTimeCount = 0;

	WelcomeScreenLogo = new CGdiBmp(TEXT(".\\res\\LogoMario.bmp"));
	mario_Info = new CGdiBmp(TEXT(".\\res\\mario_Info.bmp"));
	about = new CGdiBmp(TEXT(".\\res\\about.bmp"));

	PlayerBoundary.left = 0;
	PlayerBoundary.top = 0;
	//may be problems here
	PlayerBoundary.right = WinSize.cx;
	PlayerBoundary.bottom = WinSize.cy;

	ScreenBoundry.left = 0;
	ScreenBoundry.top = 0;
	ScreenBoundry.right = WinSize.cx;
	ScreenBoundry.bottom = WinSize.cy;

	SPRITEINFO playerinfo = {
		".\\res\\mario_.bmp",
		8,8,1,1,0,0,CSprite::DIR_RIGHT,0,MARIO_MOVE_SPEED,0,
		true,true,RGB(192,128,192)
	};
	CreatePlayer(playerinfo);

	LoadCoinSprite();

	/* Load Music Resource below */
	if(!ds.CreateDS(pEngine->GetWindow())) return;
	JumpSound.LoadWave(ds, ".\\res\\sound\\Jump.wav");
	DeadSound.LoadWave(ds, ".\\res\\sound\\Dead.wav");
	GameTotallyEndSound.LoadWave(ds, ".\\res\\sound\\GameTotallyEnd.wav");
	GameOverSound.LoadWave(ds, ".\\res\\sound\\GameOver.wav");
	DragFlagPloeSound.LoadWave(ds, ".\\res\\sound\\DragFlagPole.wav");
	StepOnEnemySound.LoadWave(ds, ".\\res\\sound\\StepOnEnemy.wav");
	KnockBrickSound.LoadWave(ds, ".\\res\\sound\\KnockBrick.wav");
	GoldCoinSound.LoadWave(ds, ".\\res\\sound\\GoldCoin.wav");

	map = new CMap();
	checkMapAndSound();
}

void CreatePlayer(SPRITEINFO spInfo)
{
	//Player=new CSprite(spInfo.BmpPath);
	Player=new MaiMarioSprite(spInfo.BmpPath);
	int row=(spInfo.TotalFrames/spInfo.RowFrames);

	SIZE bmpSize={
		Player->getDIB()->getBmpWidth(),
		Player->getDIB()->getBmpHeight()
	};
	SIZE frameSize={
		Player->getDIB()->getBmpWidth()/spInfo.RowFrames,
		Player->getDIB()->getBmpHeight()/row
	};

	POINT pos = {0, 0};

	Player->setFrameSize(frameSize);
	Player->setPos(pos);
	Player->setDirection(spInfo.Dir);
	Player->setSpeed(spInfo.Speed);
	Player->setRestLife(spInfo.RestLife);
	Player->setTotalFrames(spInfo.TotalFrames);
	Player->setRowFrames(spInfo.RowFrames);
	Player->setTransRGB(spInfo.TransRGB);
	Player->setActive(false);
	Player->setRatio(spInfo.Ratio);
	Player->setGlobalPos(pos);
}

void GamePaint(HDC OffDC)
{
	if(inWelcome){
		DrawWelcome(OffDC);
		return;
	}

	if(inLevelBegin){
		DrawBeforeLevelInfo(OffDC);
		MaiMarioSprite::getMainSprite()->initMarioSir();
		if(0 == oldTimeCount)
			oldTimeCount = GetTickCount();
		newTimeCount = GetTickCount();
		if(newTimeCount - oldTimeCount >= 3000){
			inLevelBegin = false;
			inPlaying = true;
			newTimeCount = oldTimeCount = 0;
		}
		return;
	}

	if(inPlaying){
		//map->MoveMap();
		BGSound.Play(true);
		map->DrawMap(OffDC);
		Player->DrawAnimation(OffDC);
		if(0 == oldTimeCount)
			oldTimeCount = GetTickCount();
		newTimeCount = GetTickCount();
		DrawInPlayingScore(OffDC);
		coin->DrawAnimation(OffDC);
		coin->UpdateFrame(true);
	}

	if(!inLevelBegin && !inPlaying){
		DrawGameTotallyEnd(OffDC);
		return;
	}
}

void UpdatePlayerPosition()
{
	if(!inLevelBegin && inPlaying){
		
		int playDestWidth=((int)(Player->getRatio()*Player->getFrameSize().cx+0.5));
		RECT keyPlayerBoundary;
		keyPlayerBoundary.left=0;
		keyPlayerBoundary.top=0;
		keyPlayerBoundary.right=WinSize.cx-2*playDestWidth;
		keyPlayerBoundary.bottom=WinSize.cy;

		Player->UpdatePosition(keyPlayerBoundary,true);
		MaiMarioSprite::getMainSprite()->CheckIfDeath();
		Player->UpdateFrame(true);
	}
}

void checkMapAndSound()
{
	if(map->getMapLevel() == 0){
		map->LoadTxtMap(".\\res\\map\\level1.dat");
		currentLevel = 1;
		map->setMapLevel(currentLevel);
		BGSound.LoadWave(ds, ".\\res\\sound\\BGground.wav");
		LevelEndSound.LoadWave(ds, ".\\res\\sound\\EnterCastle.wav");
	}
	else if(currentLevel != map->getMapLevel()){
		map->releaseMapMatrix();
		switch(currentLevel){
		case 1:
			map->LoadTxtMap(".\\res\\map\\level1.dat");
			BGSound.LoadWave(ds, ".\\res\\sound\\BGground.wav");
			LevelEndSound.LoadWave(ds, ".\\res\\sound\\EnterCastle.wav");
			break;
		case 2:
			map->LoadTxtMap(".\\res\\map\\level2.dat");
			BGSound.LoadWave(ds, ".\\res\\sound\\BGUnderGround.wav");
			LevelEndSound.LoadWave(ds, ".\\res\\sound\\EnterTube.wav");
			break;
		case 3:
			map->LoadTxtMap(".\\res\\map\\level3.dat");
			BGSound.LoadWave(ds, ".\\res\\sound\\BGground.wav");
			LevelEndSound.LoadWave(ds, ".\\res\\sound\\EnterCastle.wav");
			break;
		default:
			map->LoadTxtMap(".\\res\\map\\level1.dat");
			BGSound.LoadWave(ds, ".\\res\\sound\\BGground.wav");
			LevelEndSound.LoadWave(ds, ".\\res\\sound\\EnterCastle.wav");
			break;
		}
		map->setMapLevel(currentLevel);
	}
}

void GameRunning()
{
	checkMapAndSound();
	LoadCoinSprite();
	if(inPlaying){
		UpdatePlayerPosition();
	}

	//获取游戏窗口句柄及绘图设备
	HWND hWindow = pEngine -> GetWindow();
	HDC  hDC = GetDC(hWindow);
	//在内存缓冲设备中绘制
	GamePaint(bufferDC);
	//将内存设备中绘制的内容绘制到屏幕上
	BitBlt(hDC,0,0,pEngine->GetWidth(),pEngine->GetHeight(),bufferDC,0,0,SRCCOPY);
	ReleaseDC(hWindow,hDC); //释放设备

	if(inPlaying){	//各种例外事件的判断
		//如果马里奥 大叔 死掉了~~~
		if(MaiMarioSprite::getMainSprite()->GetIsGoingToDeath()){
			MaiMarioSprite::getMainSprite()->SetIsGoingToDeath(false);
			BGSound.Stop();
			DeadSound.Play();
			BGSound.SetToBegin();
			Sleep(2000);
			inPlaying = false;
			inLevelBegin = true;
			LifeRemain--;
			if(LifeRemain == 0){
				inPlaying = false;
				inLevelBegin = false;
				inWelcome = false;
			}
		}
		//如果升级
		if(UpGgradeLevel){
			currentLevel++;
			BGSound.Stop();
			if(4 != currentLevel)
				LevelEndSound.Play();
			inPlaying = false;
			inLevelBegin = true;
			UpGgradeLevel = false;
			oldTimeCount = 0;
			coin = NULL;
			coinCount = 0;
			if(currentLevel == 4){
				inPlaying = false;
				inLevelBegin = false;
				GameTotallyEndSound.Play();
			}
			else
				Sleep(LevelEndSoundTime[currentLevel]);
		}
		//如果拽旗子
		if(isDragingFlag){
			DragFlagPloeSound.Play();
		}
		//如果吃金币
		if(isGettingCoin){
			GoldCoinSound.Play();
			UpdateCoinPosition();
		}
	}
}

void KeyControl(bool isUp, HDC OffDC)
{
	if(!isUp){
		if(GetAsyncKeyState(VK_RETURN) < 0){
			if(!inLevelBegin && !inPlaying && !inWelcome){
				ReleaseAll();
				LoadGame();
			}
			else if(inWelcome && !inLevelBegin && !inPlaying){
				inWelcome = false;
				inLevelBegin = true;
			}
			else if(!inWelcome && inLevelBegin && !inPlaying){
				inLevelBegin = false;
				inPlaying = true;
			}
		}
	}
	else{
		Player->setActive(false);
	}
}

void GameStop()
{
	//释放所有资源
	delete pEngine;
	DeleteObject(bufferBitmap);
	bufferBitmap=NULL;
	DeleteDC(bufferDC);
	ReleaseAll();
}

void ReleaseAll()
{
	delete map;
	Player->getDIB()->Destroy();
	/* destory music deblow */
	if(BGSound.IsAlive()) BGSound.Release();
	if(JumpSound.IsAlive()) JumpSound.Release();
	if(DeadSound.IsAlive()) DeadSound.Release();
	if(GameTotallyEndSound.IsAlive()) GameTotallyEndSound.Release();
	if(GameOverSound.IsAlive()) GameOverSound.Release();
	if(LevelEndSound.IsAlive()) LevelEndSound.Release();
	if(DragFlagPloeSound.IsAlive()) DragFlagPloeSound.Release();
	if(StepOnEnemySound.IsAlive()) StepOnEnemySound.Release();
	if(KnockBrickSound.IsAlive()) KnockBrickSound.Release();
}

void LoadCoinSprite()
{
	if(coin != NULL)
		return;

	int transparent;
	if(2 == currentLevel){
		coin = new CSprite(".\\res\\coin_black.bmp");
		transparent = RGB(0, 0, 0);
	}
	else{
		coin = new CSprite(".\\res\\coin_blue.bmp");
		transparent = RGB(0, 0, 192);
	}
	int row = 1;
	int speed = 5;
	int dir = CSprite::DIR_LEFT;
	int totalFrames = 3;
	int rowFrames = 3;
	bool active = true;
	int ratio = 1;
	SIZE bmpSize = {coin->getDIB()->getBmpWidth(), coin->getDIB()->getBmpHeight()};
	SIZE frameSize = {16, 16};

	coin->setFrameSize(frameSize);
	coin->setDirection(dir);
	coin->setSpeed(speed);
	coin->setTotalFrames(totalFrames);
	coin->setRowFrames(rowFrames);
	coin->setTransRGB(transparent);
	coin->setRatio(ratio);
	coin->setActive(active);
	coin->setDead(false);
	coin->setVisible(false);
}

void UpdateCoinPosition()
{
	POINT p = coin->getPos();
	float oriX, nowX;
//	printf("%d %d\n", p.x, p.y);
	if(coinTickold == 0){
		coinTickold = GetTickCount();
		oriX = MaiMarioSprite::getMainSprite()->getPosX();
	}
	coinTicknew = GetTickCount();
	nowX = MaiMarioSprite::getMainSprite()->getPosX();

	if(coinTicknew - coinTickold <= 250){
		p.y -= 10;
	}
	else if(coinTicknew - coinTickold <= 500){
		p.y += 10;
	}
	p.x += (long)(oriX - nowX);
//	printf("**%d\n", (long)(oriX - nowX));
	coin->setPos(p);
	if(coinTicknew - coinTickold >= 500){
		isGettingCoin = false;
		coinTickold = 0;
		coin->setVisible(false);
	}
}

void DrawWelcome(HDC OffDC)
{
	RECT rect;
	rect.left=10;
	rect.top=380;
	rect.right=WinSize.cx-10;
	rect.bottom=30;

	WelcomeScreenLogo->DrawSingleFrame(OffDC, -1, 0, 2, 0, 400, 185, 1.9138756, 0, NULL);
	CGdiBmp::DrawWords(OffDC, "Begin[Enter]      About[A]", RGB(255, 255, 255), 20, TEXT("黑体"), rect, DT_CENTER);
}
void DrawBeforeLevelInfo(HDC OffDC)
{
	RECT rect;
	rect.left=10;
	rect.right=10;
	rect.top=10;
	rect.right=WinSize.cx-10;
	rect.bottom=48;

	HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
	RECT BGRect;
	BGRect.left = 0;
	BGRect.top = 0;
	BGRect.right = 800;
	BGRect.bottom = 800;
	FillRect(OffDC, &BGRect, blackBrush);

	//统计分数，目前只有拽旗子和使用时间的统计
	if(0 == InPlayingTimeSpend)
		TotalScores = 0;
	else
		TotalScores = dragDistance * 5 + 400 - InPlayingTimeSpend + coinCount * 20;

	char Buf[64];
	sprintf(Buf, "MARIO : %03d           WORLD : %d", TotalScores, currentLevel);
	CGdiBmp::DrawWords(OffDC, Buf, RGB(255, 255, 255), 19, TEXT("黑体"), rect, DT_CENTER);
	mario_Info->DrawSingleFrame(OffDC, 160, 110, 0, 0, 16, 16, 2, 0, RGB(192, 128, 192));

	rect.left = 190;
	rect.top = 110;
	rect.right = 250;
	rect.bottom = 150;
	sprintf(Buf, "X %d", LifeRemain);
	CGdiBmp::DrawWords(OffDC, Buf, RGB(255, 255, 255), 30, TEXT("黑体"), rect, DT_CENTER);
}
void DrawGameTotallyEnd(HDC OffDC)
{
	RECT rect;
	rect.left=10;
	rect.right=10;
	rect.top=10;
	rect.right=WinSize.cx-10;
	rect.bottom=48;

	HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
	RECT BGRect;
	BGRect.left = 0;
	BGRect.top = 0;
	BGRect.right = SCREEN_WIDTH;
	BGRect.bottom = SCREEN_HEIGHT;
	FillRect(OffDC, &BGRect, blackBrush);

	char Buf[64] = "YOU ";
	if(0 != LifeRemain)
		strcat(Buf, "WIN!    ");
	else
		strcat(Buf, "LOSE!    ");
	strcat(Buf, "Push Enter To Restart!");
	CGdiBmp::DrawWords(OffDC, Buf, RGB(255, 255, 255), 20, TEXT("微软雅黑"), rect, DT_CENTER);
}
void DrawInPlayingScore(HDC OffDC)
{
	/* draw Scores and timeRest */
	char Buf[64];
	InPlayingTimeSpend = (newTimeCount - oldTimeCount) / 1000;
	sprintf(Buf, "MARIO : %03d           TIME : %03d", inPlayScore, 400 - InPlayingTimeSpend);
	RECT rect;
	rect.left=10;
	rect.right=10;
	rect.top=10;
	rect.right=WinSize.cx-10;
	rect.bottom=48;
	CGdiBmp::DrawWords(OffDC, Buf, RGB(255, 255, 255), 19, TEXT("黑体"), rect, DT_CENTER);
}
