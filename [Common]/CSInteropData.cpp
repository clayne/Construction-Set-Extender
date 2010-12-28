#include "CSInteropData.h"
#include "windows.h"

#pragma warning(disable:4800)


bool PerformPipeOperation(HANDLE PipeHandle, int Operation, CSECSInteropData* Buffer, DWORD* BytesReadWrittenBuf)
{
	bool PipeOperation = false;
	
	if (PipeHandle != INVALID_HANDLE_VALUE)
	{
		switch (Operation)
		{
		case kPipeOperation_Read:
			PipeOperation = ReadFile(PipeHandle,
									Buffer,
									sizeof(CSECSInteropData),
									BytesReadWrittenBuf,
									NULL);				
			break;
		case kPipeOperation_Write:
			PipeOperation = WriteFile(PipeHandle,
									Buffer,
									sizeof(CSECSInteropData),
									BytesReadWrittenBuf,
									NULL);	
			break;
		}											
	}

	return PipeOperation;
}