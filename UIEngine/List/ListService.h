#ifndef __LIST_SERVICE_H__
#define __LIST_SERVICE_H__

#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>

#define NODE_META_SIZE           (sizeof(NODE_T *) + sizeof(NODE_T *))
#define PRE_NODE_P_OFFSET(List)  List->NodeSize
#define NEXT_NODE_P_OFFSET(List) (List->NodeSize + sizeof(NODE_T *))

typedef UINT8 NODE_T;

typedef struct {
  UINT32 NodeSize;
  NODE_T *Head;
  NODE_T *Tail;
} LIST_T;

VOID
InitializeList (
  LIST_T *List,
  UINT32 NodeSize
  );

NODE_T *
InsertHead (
  LIST_T *List
  );

NODE_T *
GetNextNode (
  LIST_T *List,
  NODE_T *Node
  );

VOID
DestroyList (
  LIST_T *List
  );
#endif
