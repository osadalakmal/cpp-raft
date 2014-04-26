#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "raft.h"
#include "raft_node.h"

RaftNode::RaftNode(const boost::any& udata) : d_udata(udata), next_idx(0)
{ }

int RaftNode::is_leader()
{
    // TODO
    return 0;
}

int RaftNode::get_next_idx()
{
    return next_idx;
}

void RaftNode::set_next_idx(int nextIdx)
{
    next_idx = nextIdx;
}

boost::any& RaftNode::get_udata()
{
    return d_udata;
}
