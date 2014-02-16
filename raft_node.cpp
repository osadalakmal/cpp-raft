#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "raft.h"
#include "raft_node.h"

RaftNode::RaftNode(void* udata) : d_udata(udata)
{ }

int RaftNode::raft_node_is_leader()
{
    // TODO
    return 0;
}

int RaftNode::raft_node_get_next_idx()
{
    return next_idx;
}

void RaftNode::raft_node_set_next_idx(int nextIdx)
{
    next_idx = nextIdx;
}

void* RaftNode::raft_node_get_udata()
{
    return d_udata;
}
