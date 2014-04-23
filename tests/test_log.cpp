
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gtest/gtest.h"

#include "raft.h"
#include "raft_msg.h"
#include "raft_logger.h"
#include "raft_private.h"

TEST(RaftLogger,new_is_empty)
{
    RaftLogger *l;

    l = new RaftLogger();
    ASSERT_TRUE(0 == l->log_count());
}

TEST(RaftLogger,append_is_not_empty)
{
    RaftLogger *l;
    raft_entry_t e;

    e.d_id = 1;

    l = new RaftLogger();
    ASSERT_TRUE(1 == l->log_append_entry( e));
    ASSERT_TRUE(1 == l->log_count());
}

TEST(RaftLogger,get_at_idx)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    ASSERT_TRUE(1 == l->log_append_entry( e1));
    e2.d_id = 2;
    ASSERT_TRUE(1 == l->log_append_entry( e2));
    e3.d_id = 3;
    ASSERT_TRUE(1 == l->log_append_entry( e3));
    ASSERT_TRUE(3 == l->log_count());

    ASSERT_TRUE(3 == l->log_count());
    ASSERT_TRUE(e2.d_id == l->log_get_from_idx(2).d_id);
}

TEST(RaftLogger,get_at_idx_returns_null_where_out_of_bounds)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    ASSERT_TRUE(1 == l->log_append_entry( e1));
    ASSERT_THROW(l->log_get_from_idx(2),std::runtime_error);
}

TEST(RaftLogger,mark_node_has_committed_adds_nodes)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    l->log_append_entry( e1);
    ASSERT_TRUE(0 == l->log_get_from_idx(1).d_num_nodes);
    l->log_mark_node_has_committed( 1);
    ASSERT_TRUE(1 == l->log_get_from_idx(1).d_num_nodes);
    l->log_mark_node_has_committed( 1);
    ASSERT_TRUE(2 == l->log_get_from_idx(1).d_num_nodes);
}

TEST(RaftLogger,delete)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    ASSERT_TRUE(1 == l->log_append_entry( e1));
    e2.d_id = 2;
    ASSERT_TRUE(1 == l->log_append_entry( e2));
    e3.d_id = 3;
    ASSERT_TRUE(1 == l->log_append_entry( e3));
    ASSERT_TRUE(3 == l->log_count());

    l->log_delete(3);
    ASSERT_TRUE(2 == l->log_count());
    ASSERT_THROW(l->log_get_from_idx(3),std::runtime_error);
    l->log_delete(2);
    ASSERT_TRUE(1 == l->log_count());
    ASSERT_THROW(l->log_get_from_idx(2),std::runtime_error);
    l->log_delete(1);
    ASSERT_TRUE(0 == l->log_count());
    ASSERT_THROW(l->log_get_from_idx(1),std::runtime_error);
}

TEST(RaftLogger,delete_onwards)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    ASSERT_TRUE(1 == l->log_append_entry( e1));
    e2.d_id = 2;
    ASSERT_TRUE(1 == l->log_append_entry( e2));
    e3.d_id = 3;
    ASSERT_TRUE(1 == l->log_append_entry( e3));
    ASSERT_TRUE(3 == l->log_count());

    /* even 3 gets deleted */
    l->log_delete(2);
    ASSERT_TRUE(1 == l->log_count());
    ASSERT_TRUE(e1.d_id == l->log_get_from_idx(1).d_id);
    ASSERT_THROW(l->log_get_from_idx(2),std::runtime_error);
    ASSERT_THROW(l->log_get_from_idx(3),std::runtime_error);
}

TEST(RaftLogger,peektail)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.d_id = 1;
    ASSERT_TRUE(1 == l->log_append_entry( e1));
    e2.d_id = 2;
    ASSERT_TRUE(1 == l->log_append_entry( e2));
    e3.d_id = 3;
    ASSERT_TRUE(1 == l->log_append_entry( e3));
    ASSERT_TRUE(3 == l->log_count());
    ASSERT_TRUE(e3.d_id == l->log_peektail().d_id);
}

#if 0
// TODO: duplicate testing not implemented yet
void T_estlog_cant_append_duplicates()
{
    RaftLogger *l;
    raft_entry_t e;

    e.d_id = 1;

    l = new RaftLogger();
    ASSERT_TRUE(1 == l->log_append_entry( &e));
    ASSERT_TRUE(1 == l->log_count());
}
#endif

