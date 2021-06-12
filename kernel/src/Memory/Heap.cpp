#include "Heap.h"
#include "../Paging/PageTableManager.h"
#include "../Paging/PageFrameAllocator.h"

void* HeapStart;
void* HeapEnd;
HeapSegHdr* LastHdr;

void InitializeHeap(void* HeapAddress, size_t PageCount){
    void* Position = HeapAddress;

    for (size_t i = 0; i < PageCount; i++){
        GlobalPageTableManager.MapMemory(Position, GlobalAllocator.RequestPage());
        Position = (void*)((size_t)Position + 0x1000);
    }

    size_t HeapLength = PageCount * 0x1000;

    HeapStart = HeapAddress;
    HeapEnd = (void*)((size_t)HeapStart + HeapLength);
    HeapSegHdr* StartSegment = (HeapSegHdr*)HeapAddress;
    StartSegment->Length = HeapLength - sizeof(HeapSegHdr);
    StartSegment->Next = NULL;
    StartSegment->Last = NULL;
    StartSegment->IsFree = true;
    LastHdr = StartSegment;
}

void Free(void* address){
    HeapSegHdr* segment = (HeapSegHdr*)address - 1;
    segment->IsFree = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void* Malloc(size_t Size){
    if (Size % 0x10 > 0){ // it is not a multiple of 0x10
        Size -= (Size % 0x10);
        Size += 0x10;
    }

    if (Size == 0) return NULL;

    HeapSegHdr* CurrentSegment = (HeapSegHdr*) HeapStart;
    while(true){
        if(CurrentSegment->IsFree){
            if (CurrentSegment->Length > Size){
                CurrentSegment->Split(Size);
                CurrentSegment->IsFree = false;
                return (void*)((uint64_t)CurrentSegment + sizeof(HeapSegHdr));
            }
            if (CurrentSegment->Length == Size){
                CurrentSegment->IsFree = false;
                return (void*)((uint64_t)CurrentSegment + sizeof(HeapSegHdr));
            }
        }
        if (CurrentSegment->Next == NULL) break;
        CurrentSegment = CurrentSegment->Next;
    }
    ExpandHeap(Size);
    return Malloc(Size);
}

HeapSegHdr* HeapSegHdr::Split(size_t SplitLength){
    if (SplitLength < 0x10) return NULL;
    int64_t SplitSegmentLength = Length - SplitLength - (sizeof(HeapSegHdr));
    if (SplitSegmentLength < 0x10) return NULL;

    HeapSegHdr* NewSplitHdr = (HeapSegHdr*) ((size_t)this + SplitLength + sizeof(HeapSegHdr));
    Next->Last = NewSplitHdr; // Set the Next segment's Last segment to our new segment
    NewSplitHdr->Next = Next; // Set the new segment's Next segment to out original Next segment
    Next = NewSplitHdr; // Set our new segment to the new segment
    NewSplitHdr->Last = this; // Set our new segment's Last segment to the current segment
    NewSplitHdr->Length = SplitSegmentLength; // Set the new header's Length to the calculated value
    NewSplitHdr->IsFree = IsFree; // make sure the new segment's IsFree is the same as the original
    Length = SplitLength; // set the Length of the original segment to its new Length

    if (LastHdr == this) LastHdr = NewSplitHdr;
    return NewSplitHdr;
}

void ExpandHeap(size_t Length){
    if (Length % 0x1000) {
        Length -= Length % 0x1000;
        Length += 0x1000;
    }

    size_t PageCount = Length / 0x1000;
    HeapSegHdr* NewSegment = (HeapSegHdr*)HeapEnd;

    for (size_t i = 0; i < PageCount; i++){
        GlobalPageTableManager.MapMemory(HeapEnd, GlobalAllocator.RequestPage());
        HeapEnd = (void*)((size_t)HeapEnd + 0x1000);
    }

    NewSegment->IsFree = true;
    NewSegment->Last = LastHdr;
    LastHdr->Next = NewSegment;
    LastHdr = NewSegment;
    NewSegment->Next = NULL;
    NewSegment->Length = Length - sizeof(HeapSegHdr);
    NewSegment->CombineBackward();

}

void HeapSegHdr::CombineForward(){
    if (Next == NULL) return;
    if (!Next->IsFree) return;

    if (Next == LastHdr) LastHdr = this;

    if (Next->Next != NULL){
        Next->Next->Last = this;
    }

    

    Length = Length + Next->Length + sizeof(HeapSegHdr);

    Next = Next->Next;
}

void HeapSegHdr::CombineBackward(){
    if (Last != NULL && Last->IsFree) Last->CombineForward();
}

