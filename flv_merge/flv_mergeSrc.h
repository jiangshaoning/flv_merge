

#ifndef SInt64
#define SInt64 long long
#endif
#ifndef UInt32
#define UInt32 unsigned int
#endif
#ifndef UInt8
#define UInt8 unsigned char
#endif

#define FLV_HEADER_SIZE 9
#define FLV_TAG_HEADER_SIZE 11
#define MAX_DATA_SIZE 16777220


class EventThread : public TplEventArgs<EventThread>
{
	SOUI_CLASS_NAME(EventThread, L"on_event_thread")
public:
	EventThread(SObject *pSender) :TplEventArgs<EventThread>(pSender){}
	enum{ EventID = EVT_EXTERNAL_BEGIN + 30000 };

	bool bCnLang; //语言
	int nData; //进度条的百分比
	SStringT value; //进度表显示数字
};

typedef struct FLVContext
{
	int				  hasAudio;
    unsigned char     soundFormat;
    unsigned char     soundRate;
    unsigned char     soundSize;
    unsigned char     soundType;
	
    unsigned char     videoCodecID;
	UInt32	  		  timeLen;
	
    FILE              *fileSource;
    struct FLVContext *next;
}FLVContext;

typedef struct MergeTaskList
{
    FLVContext *first;
    FLVContext *last;
    UInt32     count;

    void (*add_task)(struct MergeTaskList *taskList, FLVContext *flvCtx);
	int(*do_merge_tasks)(struct MergeTaskList *taskList, const char* const mergeFileName, EventThread *pEvt);
    void (*clear_tasks)(struct MergeTaskList *taskList);
}MergeTaskList;


int InitFLVContext(FLVContext **flvCtx, const char* const fileName, bool bCnLang);
int InitMergeTaskList(MergeTaskList *taskList);
void SetTaskCancel(int cancel);


