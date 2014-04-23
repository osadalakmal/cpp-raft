
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gtest/gtest.h"

#include "raft.h"
#include "raft_server.h"
#include "raft_logger.h"
#include "raft_private.h"
#include "mock_send_functions.h"

static int NODE_ID_1 = 1;
static int NODE_ID_2 = 2;
static int NODE_ID_3 = 3;

TEST(RaftScenario,leader_appears)
{
    int i,j;
    RaftServer *r[3];
    void* sender[3];
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),&NODE_ID_3},
                {(-1),NULL}};

    senders_new();

    for (j=0;j<3;j++)
    {
        r[j] = new RaftServer();
        sender[j] = sender_new(cfg[j].udata_address);
        sender_set_raft(sender[j], r[j]);
        r[j]->set_election_timeout(500);
        r[j]->set_configuration(cfg,j);
        raft_cbs_t* cbs = new raft_cbs_t();
        cbs->send = sender_send;
        cbs->log = NULL;
        r[j]->set_callbacks(cbs, sender[j]);
    }

    for (i=0;i<20;i++)
    {
        one_more_time:

        for (j=0;j<3;j++)
            sender_poll_msgs(sender[j]);

        for (j=0;j<3;j++)
            if (sender_msgs_available(sender[j]))
                    goto one_more_time;

        for (j=0;j<3;j++)
            r[j]->periodic(100);
    }

    int leaders = 0;
    for (j=0;j<3;j++)
    {
        if (r[j]->get_state().is_leader())
            leaders += 1;
    }

    ASSERT_TRUE(0 != leaders);
    ASSERT_TRUE(1 == leaders);
}

