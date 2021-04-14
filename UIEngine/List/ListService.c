#include "ListService.h"

VOID
InitializeList (
  LIST_T *List,
  UINT32 NodeSize
  )
{
  List->NodeSize = NodeSize;
  List->Head = NULL;
  List->Tail = NULL;
}

NODE_T *
InsertHead (
  LIST_T *List
  )
{
  NODE_T *NewNode = (NODE_T *)AllocatePool (List->NodeSize + NODE_META_SIZE);
  ZeroMem (NewNode, List->NodeSize + NODE_META_SIZE);

  NODE_T **NewNodePre  = NULL;
  NODE_T **NewNodeNext = NULL;
  NODE_T **HeadPre     = NULL;

  if (NewNode != NULL) {
    NewNodePre   = (NODE_T **)((UINT8 *)NewNode + PRE_NODE_P_OFFSET(List));
    NewNodeNext  = (NODE_T **)((UINT8 *)NewNode + NEXT_NODE_P_OFFSET(List));

    *NewNodePre  = NULL;

    if (List->Head == NULL) {
      *NewNodeNext = NULL;
      List->Head   = NewNode;
      List->Tail   = NewNode;
    } else {
      HeadPre      = (NODE_T **)((UINT8 *)List->Head + PRE_NODE_P_OFFSET(List)); 
      *NewNodeNext = List->Head;
      *HeadPre     = NewNode;
      List->Head   = NewNode;
    }
  }

  return NewNode;
}

NODE_T *
GetNextNode (
  LIST_T *List,
  NODE_T *Node
  )
{
  if (List->Head == NULL) {
    return NULL;
  }
  return *(NODE_T **)((UINT8 *)Node + NEXT_NODE_P_OFFSET(List));
}

VOID
DestroyList (
  LIST_T *List
  )
{
  NODE_T *NodeTemp = NULL;
  if (List->Head != NULL) {

    NodeTemp = List->Head;

    while (NodeTemp != NULL) {
      List->Head = GetNextNode(List, List->Head);
      FreePool (NodeTemp);
      NodeTemp = List->Head;
    }
  }
}
