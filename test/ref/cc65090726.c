/*
  !!DESCRIPTION!! 
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

struct Record {
  struct Record *PtrComp;
  int x;
};

typedef struct Record RecordType;
typedef RecordType *RecordPtr;

void Proc3(RecordPtr *PtrParOut)
{
	/* whatever */
}

void Proc1(RecordPtr PtrParIn)
{
#define  NextRecord  (*(PtrParIn->PtrComp))
	Proc3((RecordPtr *)NextRecord.PtrComp);
	Proc3(&NextRecord.PtrComp);
	Proc3(&PtrParIn->PtrComp->PtrComp);

#ifdef CAST_STRUCT_PTR
	Proc3((RecordPtr *) PtrParIn->PtrComp->PtrComp);
	Proc3((RecordPtr *) (*(PtrParIn->PtrComp)).PtrComp);
	Proc3((RecordPtr *) NextRecord.PtrComp);
#else
	Proc3(PtrParIn->PtrComp->PtrComp);
	Proc3((*(PtrParIn->PtrComp)).PtrComp);
	Proc3(NextRecord.PtrComp);
#endif
	
#undef   NextRecord
}

int main(void)
{
    printf("it works :)\n");

    return 0;
}
