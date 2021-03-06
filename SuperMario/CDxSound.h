#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <dsound.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib") 

class CDxSound; //由于要先引用，所以需先声明后定义

/************************************************************************/
/*
/* CDxSoundBuffer类
/*                                                                     
/************************************************************************/
class CDxSoundBuffer 
{
protected:
	LPDIRECTSOUNDBUFFER SndBuf;	//缓冲区接口指针
	LONG Volume;				//音量
	LONG Pan;					//左右声道相对音量
	DWORD Freq;					//频率

public:
	CDxSoundBuffer();
	~CDxSoundBuffer();
	//创建辅助缓冲区并设计基本属性
	bool CreateBuffer(CDxSound &ds, WAVEFORMATEX *format, int size);
	//创建主缓冲区并将主缓冲区设定为WAVE格式
	bool CreateMainBuffer(CDxSound &ds, int freq=22050);
	//读入WAVE文件,并将文件数据写入缓冲区
	bool LoadWave(CDxSound &ds, const char *path);
	//释放所有资源
	HRESULT Release();
	//恢复缓冲区
	HRESULT Restore();
	//播放声音(参数决定是否循环)
	bool Play(bool loop = false)
	{
		if (FAILED(SndBuf->Play(0, 0, loop? DSBPLAY_LOOPING: 0)))
			return false;
		return true;
	}
	//停止声音播放
	bool Stop(){ return SndBuf->Stop() == DS_OK; }
	//设置声音到开始位置
	bool SetToBegin(){return SndBuf->SetCurrentPosition(0.0) == DS_OK; }
	//判读缓冲区是否处于活跃状态
	bool IsAlive() const { return SndBuf != 0; }
	//重载"->"操作符, 表示SndBuf对象
	LPDIRECTSOUNDBUFFER operator ->() { return SndBuf; }
} ;

/************************************************************************/
/*
/* CDxSound类
/*                                                                     
/************************************************************************/
class CDxSound {
public:
	CDxSound();
	~CDxSound();
	//创建DirectSound对象、设置合作级别并创建主缓冲区
	bool CreateDS(HWND hWnd, int freq=22050);
	//释放DirectSound对象及DirectSound缓冲区对象
	void ReleaseAll();
	//失去缓冲区内存时则复原
	HRESULT RestoreAll();
	//重载"->"操作符, 表示ds对象
	LPDIRECTSOUND operator ->() { return ds; }

protected:
	LPDIRECTSOUND ds;		// DirectSound对象
	CDxSoundBuffer dsbuf;	// DirectSound缓冲对象
} ;

/************************************************************************/
/*
/* wave格式的读取类
/*                                                                     
/************************************************************************/
class CWave {
public:
	CWave();
	virtual ~CWave();
	//打开并读取WAVE文件
	bool Open(const char *path);
	//读取并查询是否存在data区块
	bool StartRead();
	//读取指定数据
	bool Read(unsigned long size, void *data, unsigned long *nread);
	//关闭文件
	bool Close();
	//获取wave格式属性
	WAVEFORMATEX *GetFormat() { return wfex; }
	//获取区块大小
	DWORD CkSize() const { return ckIn.cksize; }

protected:
	WAVEFORMATEX *wfex; //wave格式结构体指针
	HMMIO hmmio;		//文件句柄	
	MMCKINFO ckIn;		//MMCKINFO对象（查询用子块）
	MMCKINFO ckInRIFF;	//MMCKINFO对象（父快，从文件获取）
} ;