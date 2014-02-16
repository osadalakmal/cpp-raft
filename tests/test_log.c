
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "CuTest.h"

#include "raft.h"
#include "raft_logger.h"
#include "raft_private.h"

void TestLog_new_is_empty(CuTest * tc)
{
    RaftLogger *l;

    l = new RaftLogger();
    CuAssertTrue(tc, 0 == l->log_count());
}

void TestLog_append_is_not_empty(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e;

    e.id = 1;

    l = new RaftLogger();
    CuAssertTrue(tc, 1 == l->log_append_entry( &e));
    CuAssertTrue(tc, 1 == l->log_count());
}

void TestLog_get_at_idx(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e1));
    e2.id = 2;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e2));
    e3.id = 3;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e3));
    CuAssertTrue(tc, 3 == l->log_count());

    CuAssertTrue(tc, 3 == l->log_count());
    CuAssertTrue(tc, e2.id == l->log_get_from_idx(2)->id);
}

void TestLog_get_at_idx_returns_null_where_out_of_bounds(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e1));
    CuAssertTrue(tc, NULL == l->log_get_from_idx(2));
}

void TestLog_mark_node_has_committed_adds_nodes(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    l->log_append_entry( &e1);
    CuAssertTrue(tc, 0 == l->log_get_from_idx(1)->num_nodes);
    l->log_mark_node_has_committed( 1);
    CuAssertTrue(tc, 1 == l->log_get_from_idx(1)->num_nodes);
    l->log_mark_node_has_committed( 1);
    CuAssertTrue(tc, 2 == l->log_get_from_idx(1)->num_nodes);
}

void TestLog_delete(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e1));
    e2.id = 2;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e2));
    e3.id = 3;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e3));
    CuAssertTrue(tc, 3 == l->log_count());

    l->log_delete(3);
    CuAssertTrue(tc, 2 == l->log_count());
    CuAssertTrue(tc, NULL == l->log_get_from_idx(3));
    l->log_delete(2);
    CuAssertTrue(tc, 1 == l->log_count());
    CuAssertTrue(tc, NULL == l->log_get_from_idx(2));
    l->log_delete(1);
    CuAssertTrue(tc, 0 == l->log_count());
    CuAssertTrue(tc, NULL == l->log_get_from_idx(1));
}

void TestLog_delete_onwards(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e1));
    e2.id = 2;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e2));
    e3.id = 3;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e3));
    CuAssertTrue(tc, 3 == l->log_count());

    /* even 3 gets deleted */
    l->log_delete(2);
    CuAssertTrue(tc, 1 == l->log_count());
    CuAssertTrue(tc, e1.id == l->log_get_from_idx(1)->id);
    CuAssertTrue(tc, NULL == l->log_get_from_idx(2));
    CuAssertTrue(tc, NULL == l->log_get_from_idx(3));
}

void TestLog_peektail(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e1, e2, e3;

    l = new RaftLogger();
    e1.id = 1;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e1));
    e2.id = 2;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e2));
    e3.id = 3;
    CuAssertTrue(tc, 1 == l->log_append_entry( &e3));
    CuAssertTrue(tc, 3 == l->log_count());
    CuAssertTrue(tc, e3.id == l->log_peektail()->id);
}

#if 0
// TODO: duplicate testing not implemented yet
void T_estlog_cant_append_duplicates(CuTest * tc)
{
    RaftLogger *l;
    raft_entry_t e;

    e.id = 1;

    l = new RaftLogger();
    CuAssertTrue(tc, 1 == l->log_append_entry( &e));
    CuAssertTrue(tc, 1 == l->log_count());
}
#endif

