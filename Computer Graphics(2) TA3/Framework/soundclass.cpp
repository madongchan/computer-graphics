///////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "soundclass.h"
#include <cstring> // memcpy, strncmp
#include <stdio.h> // FILE, fopen_s, etc.


SoundClass::SoundClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_secondaryBuffer1 = 0;
}


SoundClass::SoundClass(const SoundClass& other)
{
}


SoundClass::~SoundClass()
{
}


bool SoundClass::Initialize(HWND hwnd)
{
	bool result;


	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = LoadWaveFile("./data/bgm.wav", &m_secondaryBuffer1);
	if (!result)
	{
		return false;
	}

	// Play the wave file now that it has been loaded.
	result = PlayWaveFile();
	if (!result)
	{
		return false;
	}

	return true;
}


void SoundClass::Shutdown()
{
	// Release the secondary buffer.
	ShutdownWaveFile(&m_secondaryBuffer1);

	// Shutdown the Direct Sound API.
	ShutdownDirectSound();

	return;
}


bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// (이 부분은 DirectSound가 믹싱할 기준 포맷이므로 그대로 둡니다)
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void SoundClass::ShutdownDirectSound()
{
	// Release the primary sound buffer pointer.
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}


// WAV 파일 청크 파싱을 위한 헬퍼 구조체
struct ChunkHeader
{
	char chunkId[4];
	unsigned long chunkSize;
};

// 'fmt ' 청크의 핵심 데이터 (WAVEFORMATEX의 앞부분과 동일)
struct FmtChunkData
{
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long bytesPerSecond;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
};


bool SoundClass::LoadWaveFile(const char* filename, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	ChunkHeader chunkHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	bool fmtChunkFound = false;
	bool dataChunkFound = false;
	unsigned long dataChunkSize = 0;
	long dataChunkPosition = 0; // 오디오 데이터 시작 위치

	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// 1. "RIFF" 청크 확인
	count = fread(&chunkHeader, sizeof(ChunkHeader), 1, filePtr);
	if (count != 1 || strncmp(chunkHeader.chunkId, "RIFF", 4) != 0)
	{
		fclose(filePtr);
		return false;
	}

	// 2. "WAVE" 포맷 확인
	char waveFormatId[4];
	count = fread(waveFormatId, sizeof(char), 4, filePtr);
	if (count != 4 || strncmp(waveFormatId, "WAVE", 4) != 0)
	{
		fclose(filePtr);
		return false;
	}

	// 3. "fmt "와 "data" 청크 탐색 (메타데이터 건너뛰기)
	while (true)
	{
		// 청크 헤더 읽기
		count = fread(&chunkHeader, sizeof(ChunkHeader), 1, filePtr);
		if (count != 1)
		{
			// 파일 끝에 도달 (data 청크를 못찾음)
			fclose(filePtr);
			return false;
		}

		// "fmt " 청크 발견
		if (strncmp(chunkHeader.chunkId, "fmt ", 4) == 0)
		{
			FmtChunkData fmtData;
			// 'fmt ' 청크의 핵심 데이터 읽기
			count = fread(&fmtData, sizeof(FmtChunkData), 1, filePtr);
			if (count != 1) { fclose(filePtr); return false; }

			// PCM 포맷이 아니면 실패
			if (fmtData.audioFormat != WAVE_FORMAT_PCM)
			{
				fclose(filePtr);
				return false;
			}

			// DirectSound가 사용할 waveFormat 구조체 채우기
			waveFormat.wFormatTag = fmtData.audioFormat;
			waveFormat.nSamplesPerSec = fmtData.sampleRate;
			waveFormat.wBitsPerSample = fmtData.bitsPerSample;
			waveFormat.nChannels = fmtData.numChannels;
			waveFormat.nBlockAlign = fmtData.blockAlign;
			waveFormat.nAvgBytesPerSec = fmtData.bytesPerSecond;
			waveFormat.cbSize = 0;

			fmtChunkFound = true;

			// 'fmt ' 청크의 나머지 데이터 건너뛰기 (크기가 16바이트보다 클 경우)
			if (chunkHeader.chunkSize > sizeof(FmtChunkData))
			{
				fseek(filePtr, chunkHeader.chunkSize - sizeof(FmtChunkData), SEEK_CUR);
			}
		}
		// "data" 청크 발견
		else if (strncmp(chunkHeader.chunkId, "data", 4) == 0)
		{
			dataChunkSize = chunkHeader.chunkSize;
			dataChunkPosition = ftell(filePtr); // 데이터 시작 위치 저장
			dataChunkFound = true;

			// 'data' 청크의 내용(실제 오디오 데이터)은 나중에 읽으므로 일단 건너뜀
			fseek(filePtr, dataChunkSize, SEEK_CUR);
		}
		// "LIST", "INFO" 등 기타 청크
		else
		{
			// 청크 크기만큼 파일 포인터를 이동시켜 건너뜀
			fseek(filePtr, chunkHeader.chunkSize, SEEK_CUR);
		}

		// 두 청크를 모두 찾았으면 탐색 종료
		if (fmtChunkFound && dataChunkFound)
		{
			break;
		}
	}

	// "fmt " 또는 "data" 청크를 찾지 못했으면 실패
	if (!fmtChunkFound || !dataChunkFound)
	{
		fclose(filePtr);
		return false;
	}

	// 4. DirectSound 버퍼 생성
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = dataChunkSize; // 실제 데이터 크기
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat; // 파일에서 읽어온 포맷
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		fclose(filePtr);
		return false;
	}

	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		tempBuffer->Release();
		fclose(filePtr);
		return false;
	}

	tempBuffer->Release();
	tempBuffer = 0;

	// 5. 오디오 데이터 읽기
	// 아까 저장해둔 'data' 청크의 시작 위치로 이동
	fseek(filePtr, dataChunkPosition, SEEK_SET);

	waveData = new unsigned char[dataChunkSize];
	if (!waveData)
	{
		fclose(filePtr);
		return false;
	}

	count = fread(waveData, 1, dataChunkSize, filePtr);
	if (count != dataChunkSize)
	{
		delete[] waveData;
		fclose(filePtr);
		return false;
	}

	fclose(filePtr); // 파일 닫기

	// 6. 버퍼에 데이터 쓰기
	result = (*secondaryBuffer)->Lock(0, dataChunkSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		delete[] waveData;
		return false;
	}

	memcpy(bufferPtr, waveData, dataChunkSize);

	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		delete[] waveData;
		return false;
	}

	delete[] waveData;
	waveData = 0;

	return true;
}


void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}


bool SoundClass::PlayWaveFile()
{
	HRESULT result;


	// Set position at the beginning of the sound buffer.
	result = m_secondaryBuffer1->SetCurrentPosition(0);
	if (FAILED(result))
	{
		return false;
	}

	// Set volume of the buffer to 100%.
	result = m_secondaryBuffer1->SetVolume(DSBVOLUME_MAX);
	if (FAILED(result))
	{
		return false;
	}

	// Play the contents of the secondary sound buffer.
	result = m_secondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING); // (반복 재생)
	if (FAILED(result))
	{
		return false;
	}

	return true;
}