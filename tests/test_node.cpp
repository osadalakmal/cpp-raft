
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gtest/gtest.h"

#include "raft.h"
#include "raft_logger.h"
#include "raft_node.h"
#include "raft_private.h"

TEST(RaftNode,node_set_nextIdx)
{
    RaftNode *p;

    p = new RaftNode((void*)1);
    p->set_next_idx(3);
    ASSERT_TRUE(3 == p->get_next_idx());
}

