
#include "stdafx.h"
#include "resource.h"
#include "flv_mergeSrc.h"
#include "assert.h"

static int task_cancel = 0;

void SetTaskCancel(int cancel)
{
	task_cancel = cancel;
}

UInt32 FromInt32StringBe(const UInt8* const str)
{
    return ((str[0]<<24)  |
	    (str[1]<<16)   |
	    (str[2]<<8)    |
	    (str[3]));
}

UInt32 FromInt24StringBe(const UInt8* const str)
{
    return ((str[0]<<16)  |
	    (str[1]<<8)   |
	    (str[2]));
}

UInt32 GetTimestamp(const UInt8* const pos)
{
    return ((pos[3]<<24)   |
            (pos[0]<<16)   |
	    (pos[1]<<8)    |
	    (pos[2]));
}

void SetTimestamp(UInt8* const pos, UInt32 newTimestamp)
{
    pos[3] = newTimestamp>>24;
    pos[0] = newTimestamp>>16;
    pos[1] = newTimestamp>>8;
    pos[2] = newTimestamp;
}

double AMF_DecodeNumber(const char *data)
{
	double dVal;
	unsigned char *ci, *co;
	ci = (unsigned char *)data;
	co = (unsigned char *)&dVal;

	co[0] = ci[7];
	co[1] = ci[6];
	co[2] = ci[5];
	co[3] = ci[4];
	co[4] = ci[3];
	co[5] = ci[2];
	co[6] = ci[1];
	co[7] = ci[0];
	
	return dVal;
}

char *AMF_EncodeNumber(char *output, char *outend, double dVal)
{
	if (output + 1 + 8 > outend)
		return NULL;

	*output++ = 0;	/* type: Number */
	unsigned char *ci, *co;
	ci = (unsigned char *)&dVal;
	co = (unsigned char *)output;
	co[0] = ci[7];
	co[1] = ci[6];
	co[2] = ci[5];
	co[3] = ci[4];
	co[4] = ci[3];
	co[5] = ci[2];
	co[6] = ci[1];
	co[7] = ci[0];

  return output + 8;
}


int ReadMetaDuration(FILE *piFlvFile, int size, double *poDuration)
{
	char tbuff[400], *tp=tbuff;
	int tfd=0, tlen=size;
	
	if (piFlvFile != NULL)
	{
		fread(tbuff, sizeof(char), size, piFlvFile);
		while(tlen > 0)
		{
			if (!memcmp(tp,"duration", 8))
				break;
			tp++;
			tlen--;
		}

		if (tlen > 0)
		{
			*poDuration = AMF_DecodeNumber(tp+9);
			return 0;
		}
	}

	return -1;
}

int WriteMetaDuration(char *buff, int size, double piSecs)
{
	char *tp = buff;
	int tlen = 0, tpn = size;
	double secs = (double)piSecs;
	char temp[10] = { 0 };

	while (tlen < tpn)
	{
		if (!memcmp(tp, "duration", 8))
			break;
		tp++;
		tlen++;
	}

	if (tlen > 0 && tlen <tpn)
	{
		tlen += 8;
		AMF_EncodeNumber(temp, temp + 9, secs);
		memcpy(buff + tlen, temp, 8);
	}

	return 0;
}

int ReadFromFile(FILE* inputFile, char* const buffer, int size)
{
    int readLen, realReadLen;
    char* tmp;

    assert(inputFile != NULL && buffer != NULL);

    if (inputFile != NULL && buffer != NULL) {
	readLen = size;
	realReadLen = 0;
	tmp = buffer;
	while ( (realReadLen=fread(tmp, sizeof(char), readLen, inputFile)) > 0) {
	    readLen -= realReadLen;
	    tmp += realReadLen;
	}

	return (readLen==0)?size:-1;
    }

    return -1;	
}

int WriteToFile(FILE* outputFile, char* const buffer, int size)
{
    int writeLen, realWriteLen;
    char* tmp;

    assert(outputFile != NULL && buffer != NULL);

    if (outputFile != NULL && buffer != NULL) {
		writeLen = size;
		realWriteLen = 0;
		tmp = buffer;
		while ( (realWriteLen=fwrite(tmp, sizeof(char), writeLen, outputFile)) > 0) {
		    writeLen -= realWriteLen;
		    tmp += realWriteLen;
		}

	return (writeLen==0)?size:-1;
    }

    return -1;
}

int IsFLVFile(FILE *input)
{
    int len;
    unsigned char buf[FLV_HEADER_SIZE];
    assert(input != NULL);

    if (input == NULL) return 0;

    rewind(input);

    if ( FLV_HEADER_SIZE != (len=fread(buf, sizeof(unsigned char), FLV_HEADER_SIZE, input)) ) {
		return 0;
    }

    if (buf[0] != 'F' || buf[1] != 'L' || buf[2] != 'V' 
	|| buf[3] != 0x01 || FromInt32StringBe(&buf[5]) != 9)
      return 0;

    return 1;
}

int GetFLVFileInfo(FLVContext *flvCtx)
{
    int hasAudioParams, hasVideoParams;
    int skipSize, readLen;
    int dataSize;
    double timeLen;
    UInt8 tagType;
    UInt8 tmp[FLV_TAG_HEADER_SIZE+1];

    assert(flvCtx != NULL);

    if (flvCtx == NULL) return -1;

    if (flvCtx->fileSource != NULL) {
		rewind(flvCtx->fileSource);

		skipSize = 9;
		if (fseek(flvCtx->fileSource, skipSize, SEEK_CUR) != 0)
		  return -1;
		hasVideoParams = hasAudioParams = 0;
		skipSize = 4;
		while (!hasVideoParams || !hasAudioParams) {
			if (fseek(flvCtx->fileSource, skipSize, SEEK_CUR) != 0)
			{
				return -1;
			}
			  
			if (FLV_TAG_HEADER_SIZE + 1 != (readLen = fread(tmp, sizeof(UInt8), FLV_TAG_HEADER_SIZE + 1, flvCtx->fileSource)))
			{
				if (readLen)
					return -1;
				else
					return 0;
			}
			 
			tagType = tmp[0] & 0x1f;

			switch (tagType) {
				case 18:
					dataSize = FromInt24StringBe(&tmp[1]);
					ReadMetaDuration(flvCtx->fileSource, dataSize - 1 + 4, &timeLen);
					flvCtx->timeLen = (unsigned int)timeLen;
					skipSize = 0;
					continue;
				case 8 :
					flvCtx->soundFormat = (tmp[FLV_TAG_HEADER_SIZE] & 0xf0) >> 4 ;
					flvCtx->soundRate   = (tmp[FLV_TAG_HEADER_SIZE] & 0x0c) >> 2 ;
					flvCtx->soundSize   = (tmp[FLV_TAG_HEADER_SIZE] & 0x02) >> 1 ;
					flvCtx->soundType   = (tmp[FLV_TAG_HEADER_SIZE] & 0x01) >> 0 ;
					flvCtx->hasAudio	= hasAudioParams = 1;
					break;
				case 9 :
					flvCtx->videoCodecID = (tmp[FLV_TAG_HEADER_SIZE] & 0x0f);
					hasVideoParams = 1;
					break;
				default :
					break;
			}

			dataSize = FromInt24StringBe(&tmp[1]);

			skipSize = dataSize - 1 + 4;
		}

		return 0;
    } 
	else 
	{
		return -1;
    }
}

void NotifyforProgress(EventThread *pEvt, int data, SStringT value)
{
	pEvt->nData = data;
	pEvt->value = value;
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
}


int AddFileData(FILE* inputFile, FILE* mergeFile, int isFirstFile, int isLastFile, UInt32* lastTimestamp, 
				EventThread *pEvt, UInt32 timeLen)
{
    int readLen;
    UInt32 curTimestamp = 0;
    UInt32 newTimestamp = 0;
    int dataSize;
    UInt8 tmp[20];
    int step = 0;
    char *buf;

    assert(inputFile != NULL && mergeFile != NULL);

    if (NULL == (buf=(char*)malloc(sizeof(char)*MAX_DATA_SIZE))) {
		fprintf(stderr, "malloc error!\n");
		return -1;
    }

    if (inputFile != NULL && mergeFile != NULL)
    {
		rewind(inputFile);

		if (isFirstFile) {
		    if ( FLV_HEADER_SIZE+4 == (readLen=fread(tmp, sizeof(UInt8), FLV_HEADER_SIZE+4, inputFile)) ) 
		    {
				rewind(mergeFile);
				if (readLen != fwrite(tmp, sizeof(char), readLen, mergeFile)) {
				    goto failed;
				}
		    } 
		    else 
		    {
				goto failed;
		    }
		} 
		else 
		{
		    if (fseek(inputFile, FLV_HEADER_SIZE+4, SEEK_CUR) != 0)
		      goto failed;
		}

		while (ReadFromFile(inputFile, (char*)tmp, FLV_TAG_HEADER_SIZE) > 0 && !task_cancel) {
			if(tmp[0] != 18 && tmp[0] != 8 && tmp[0] != 9)
				break;
		    dataSize = FromInt24StringBe(&tmp[1]);

		    curTimestamp = GetTimestamp(&tmp[4]);
		    newTimestamp = curTimestamp + *lastTimestamp;
		    SetTimestamp(&tmp[4], newTimestamp);
		    int tmpTime = newTimestamp /timeLen;
			if(tmpTime > step && step < 1000)
			{
				SStringT percent;
				step = tmpTime;
				percent.Format(_T("%d.%1d%%"), step/10, step%10);
				NotifyforProgress(pEvt, step, percent);
			}
		    if (WriteToFile(mergeFile, (char*)tmp, FLV_TAG_HEADER_SIZE) < 0)
		      goto failed;

		    readLen = dataSize+4;
				
		    if (ReadFromFile(inputFile, buf, readLen) > 0) 
		    {
				if (tmp[0] == 18 && isFirstFile)
				{
					//WriteMetaDuration(buf, readLen, (double)timeLen);
				}
				if (WriteToFile(mergeFile, buf, readLen) < 0)
					goto failed;
			} 
			else 
			{
				goto failed;
		    }
		}

		// update the timestamp and return
		*lastTimestamp = newTimestamp;

		free(buf);
		buf = NULL;
		if(isLastFile && !task_cancel)
		{
			NotifyforProgress(pEvt, 1000, _T("100.0%"));
		}
		return 0;
    }

 failed:
    free(buf);
    buf = NULL;
    return -1;
}

void DeleteFLVContext(FLVContext *flvCtx)
{
    assert(flvCtx != NULL);

    if ( flvCtx != NULL ) {
		if ( flvCtx->fileSource != NULL )
		  fclose(flvCtx->fileSource);
		free(flvCtx);
    }
}

int IsSuitableToMerge(FLVContext *flvCtx1, FLVContext *flvCtx2)
{
    return (flvCtx1->soundFormat == flvCtx2->soundFormat) &&
      (flvCtx1->soundRate == flvCtx2->soundRate) &&
      (flvCtx1->soundSize == flvCtx2->soundSize) &&
      (flvCtx1->soundType == flvCtx2->soundType) &&
      (flvCtx1->videoCodecID == flvCtx2->videoCodecID) ||
	  (flvCtx1->hasAudio != flvCtx2->hasAudio);
}

void AddToMergeTaskList(MergeTaskList *taskList, FLVContext *flvCtx)
{
    assert(taskList != NULL && flvCtx != NULL);

    if (taskList != NULL && flvCtx != NULL) {
		if (taskList->count == 0) {
		    taskList->first = taskList->last = flvCtx;
		} else {
		    taskList->last->next = flvCtx;
		    taskList->last = taskList->last->next;
		}
		taskList->count++;
    }
}

int DoMergeTasks(MergeTaskList *taskList, const char* const mergeFileName, EventThread *pEvt)
{
    assert(taskList != NULL && mergeFileName != NULL);

    FILE *mergeFile;
    UInt32 lastTimestamp = 0;
    FLVContext *curCtx, *nextCtx;
    UInt32 i, timeLen = 0;
	
    if (taskList == NULL) return -1;

    if ( NULL == (mergeFile=fopen(mergeFileName, "wb")) ) {
		MessageBoxEx(NULL, S_CW2T(TR(L"check_output", L"messagebox")), _T(""), MB_OK, pEvt->bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) : NULL);
		fprintf(stderr, "open file %s failed!\n", mergeFileName);
		return -1;
    }

    // insure the flv files are suitable to merge
    curCtx = taskList->first;
    for (i = 0; i < taskList->count-1; i++) {
		nextCtx = curCtx->next;

		if (!IsSuitableToMerge(curCtx, nextCtx)) {
		    fprintf(stderr, "unable to merge the flv files, maybe different parameters!\n");
		    goto failed;
		}

		curCtx = nextCtx;
    }

	curCtx = taskList->first;
    for (i = 0; i < taskList->count; i++) {
    	timeLen += curCtx->timeLen;
		nextCtx = curCtx->next;
		curCtx = nextCtx;
    }

	NotifyforProgress(pEvt, 0, _T("0.0%"));

    // combine them one by one
    curCtx = taskList->first;
    lastTimestamp = 0;
  
    for (i = 0; i < taskList->count && !task_cancel; i++) {
		if (AddFileData(curCtx->fileSource, mergeFile, (i == 0), (i == taskList->count - 1), &lastTimestamp, pEvt, timeLen) != 0) {
		    goto failed;
		}
		curCtx = curCtx->next;
    }

 failed:
	if (MessageBoxEx(NULL, S_CW2T(TR(L"all_done", L"messagebox")), _T(""), MB_OK, pEvt->bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) : NULL) == IDOK){
		NotifyforProgress(pEvt, 0, _T("0.0%"));
	}
    fclose(mergeFile);
    return -1;
}

void ClearTasks(struct MergeTaskList *taskList)
{
    assert(taskList != NULL);

    FLVContext *curCtx, *nextCtx;

    if (taskList != NULL) {

		curCtx = taskList->first;
		while (taskList->count--) {
		    nextCtx = curCtx->next;

		    DeleteFLVContext(curCtx);
				
		    curCtx = nextCtx;
		}
    }
}

int InitMergeTaskList(MergeTaskList *taskList)
{
    assert(taskList != NULL);

    if (taskList == NULL) return -1;

    memset(taskList, 0, sizeof(MergeTaskList));
	
    // set callbacks
    taskList->add_task = AddToMergeTaskList;
    taskList->do_merge_tasks = DoMergeTasks;
    taskList->clear_tasks = ClearTasks;

    return 0;
}


int InitFLVContext(FLVContext **flvCtx, const char* const fileName, bool bCnLang)
{
    assert(flvCtx != NULL);
	SStringT msg, path;

    if (flvCtx == NULL) return -1;

    if ( NULL == (*flvCtx=(FLVContext*)calloc(1, sizeof(FLVContext))) ) {
		fprintf(stderr, "malloc FLVContext error!\n");
		return -1;
    }

	//SStringA pathA = fileName;
	path = S_CA2T(fileName);
    if ( NULL == ((*flvCtx)->fileSource=fopen(fileName, "rb")) ) {	
		msg.Format(_T("%s %s!"), path, S_CW2T(TR(L"open_failed", L"messagebox")));
		//fprintf(stderr, "open file %s failed.\n", fileName);
		goto failed;
    }

    if (!IsFLVFile((*flvCtx)->fileSource)) {
		msg.Format(_T("%s %s"), path, S_CW2T(TR(L"invalid_flv", L"messagebox")));
		//fprintf(stderr, "%s: invalid FLV file!", fileName);
		goto failed;
    }

    if (GetFLVFileInfo(*flvCtx) != 0) {
		msg.Format(_T("%s %s"), path, S_CW2T(TR(L"check_failed", L"messagebox")));
		//fprintf(stderr, "cannot find flv file info!\n");
		goto failed;
    }

    return 0;

failed:
	MessageBoxEx(NULL, msg, _T(""), MB_OK, bCnLang ? MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) : NULL);
    if ((*flvCtx)->fileSource != NULL)
      fclose((*flvCtx)->fileSource);
	
    free(*flvCtx);
    *flvCtx = NULL;
	
    return -1;
}

