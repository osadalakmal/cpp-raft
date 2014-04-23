#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gtest/gtest.h"

#include "../raft.h"
#include "../raft_logger.h"
#include "../raft_server.h"
#include "../raft_node.h"
#include "../raft_private.h"
#include "mock_send_functions.h"

static int NODE_ID_1 = 1;
static int NODE_ID_2 = 2;
static int NODE_ID_3 = 3;
static int NODE_ID_4 = 4;
static int NODE_ID_5 = 5;

using namespace ::testing;

// TODO: leader doesn't timeout and cause election

TEST(RaftServer,voted_for_records_who_we_voted_for)
{
    RaftServer r;
    r.vote(2);
    ASSERT_TRUE(2 == r.get_voted_for());
}

TEST(RaftServer,idx_starts_at_1)
{
    RaftServer r;
    ASSERT_TRUE(1 == r.get_current_idx());
}

TEST(RaftServer,currentterm_defaults_to_0)
{
    RaftServer r;
    ASSERT_TRUE(0 == r.get_current_term());
}

TEST(RaftServer,set_currentterm_sets_term)
{
    RaftServer r;
    r.set_current_term(5);
    ASSERT_TRUE(5 == r.get_current_term());
}

TEST(RaftServer,voting_results_in_voting)
{
    RaftServer r;
    r.vote(1);
    ASSERT_TRUE(1 == r.get_voted_for());
    r.vote(9);
    ASSERT_TRUE(9 == r.get_voted_for());
}

TEST(RaftServer,election_start_increments_term)
{
    RaftServer r;
    r.set_current_term(1);
    r.election_start();
    ASSERT_TRUE(2 == r.get_current_term());
}

TEST(RaftServer,set_state)
{
    RaftServer r;
    r.get_state().set(RAFT_STATE_LEADER);
    ASSERT_TRUE(RAFT_STATE_LEADER == r.get_state().get());
}

TEST(RaftServer,starts_as_follower)
{
    RaftServer r;
    ASSERT_TRUE(RAFT_STATE_FOLLOWER == r.get_state().get());
}

TEST(RaftServer,starts_with_election_timeout_of_1000ms)
{
    RaftServer r;
    ASSERT_TRUE(1000 == r.get_election_timeout());
}

TEST(RaftServer,starts_with_request_timeout_of_200ms)
{
    RaftServer r;
    ASSERT_TRUE(200 == r.get_request_timeout());
}

TEST(RaftServer,entry_append_cant_append_if_id_is_zero)
{

    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.d_data = str;
    ety.d_len = 3;
    ety.d_id = 0;
    ety.d_term = 1;

    RaftServer r;
    ASSERT_TRUE(1 == r.get_current_idx());
    r.append_entry(ety);
    ASSERT_TRUE(1 == r.get_current_idx());
}

TEST(RaftServer,entry_append_increases_logidx)
{

    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.d_data = str;
    ety.d_len = 3;
    ety.d_id = 1;
    ety.d_term = 1;

    RaftServer r;
    ASSERT_TRUE(1 == r.get_current_idx());
    r.append_entry(ety);
    ASSERT_TRUE(2 == r.get_current_idx());
}

TEST(RaftServer,append_entry_means_entry_gets_current_term)
{

    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.d_data = str;
    ety.d_len = 3;
    ety.d_id = 1;
    ety.d_term = 1;

    RaftServer r;
    ASSERT_TRUE(1 == r.get_current_idx());
    r.append_entry(ety);
    ASSERT_TRUE(2 == r.get_current_idx());
}

#if 0
/* TODO: no support for duplicate detection yet */
void T_estRaft_server_append_entry_not_sucessful_if_entry_with_id_already_appended()
{

    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.d_data = str;
    ety.d_len = 3;
    ety.d_id = 1;
    ety.d_term = 1;

    RaftServer r;
    ASSERT_TRUE(1 == r.get_current_idx());
    r.append_entry(ety);
    r.append_entry(ety);
    ASSERT_TRUE(2 == r.get_current_idx());

    /* different ID so we can be successful */
    ety.d_id = 2;
    r.append_entry(ety);
    ASSERT_TRUE(3 == r.get_current_idx());
}
#endif

TEST(RaftServer,entry_is_retrieveable_using_idx)
{

    raft_entry_t e1;
    raft_entry_t e2;
    raft_entry_t ety_appended;
    char* str = const_cast<char*>("aaa");
    char* str2 = const_cast<char*>("bbb");

    RaftServer r;

    e1.d_term = 1;
    e1.d_id = 1;
    e1.d_data = str;
    e1.d_len = 3;
    r.append_entry(e1);

    /* different ID so we can be successful */
    e2.d_term = 1;
    e2.d_id = 2;
    e2.d_data = str2;
    e2.d_len = 3;
    r.append_entry(e2);

    ASSERT_NO_THROW(ety_appended = r.get_entry_from_idx(2));
    ASSERT_TRUE(!strncmp(reinterpret_cast<char*>(ety_appended.d_data),reinterpret_cast<char*>(str2),3));
}

TEST(RaftServer,wont_apply_entry_if_we_dont_have_entry_to_apply)
{

    raft_entry_t ety;
    raft_entry_t *ety_appended;
    char* str = const_cast<char*>("aaa");

    void *sender = sender_new(NULL);
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    RaftServer r;
    r.set_callbacks(&funcs,sender);

    r.set_commit_idx(0);
    r.set_last_applied_idx(0);

    r.apply_entry();
    ASSERT_TRUE(0 == r.get_last_applied_idx());
    ASSERT_TRUE(0 == r.get_commit_idx());

    ety.d_term = 1;
    ety.d_id = 1;
    ety.d_data = str;
    ety.d_len = 3;
    r.append_entry(ety);
    r.apply_entry();
    ASSERT_TRUE(1 == r.get_last_applied_idx());
    ASSERT_TRUE(1 == r.get_commit_idx());
}

/* If commitidx > lastApplied: increment lastApplied, apply log[lastApplied]
 * to state machine (�5.3) */
TEST(RaftServer,increment_lastApplied_when_lastApplied_lt_commitidx)
{

    raft_entry_t ety;

    RaftServer r;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    void* sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);
    /* must be follower */
    r.get_state().set(RAFT_STATE_FOLLOWER);
    r.set_current_term(1);
    r.set_commit_idx(1);
    r.set_last_applied_idx(0);

    /* need at least one entry */
    ety.d_term = 1;
    ety.d_id = 1;
    ety.d_data = const_cast<char*>("aaa");
    ety.d_len = 3;
    r.append_entry(ety);

    /* let time lapse */
    r.periodic(1);
    ASSERT_TRUE(0 != r.get_last_applied_idx());
    ASSERT_TRUE(1 == r.get_last_applied_idx());
}

TEST(RaftServer,apply_entry_increments_last_applied_idx)
{

    raft_entry_t ety;
    raft_entry_t *ety_appended;
    char* str = const_cast<char*>("aaa");

    ety.d_term = 1;

    RaftServer r;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    void* sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);
    r.set_commit_idx(1);
    r.set_last_applied_idx(0);

    ety.d_id = 1;
    ety.d_data = str;
    ety.d_len = 3;
    r.append_entry(ety);
    r.apply_entry();
    ASSERT_TRUE(1 == r.get_last_applied_idx());
}

TEST(RaftServer,periodic_elapses_election_timeout)
{
    RaftServer r;
    /* we don't want to set the timeout to zero */
    r.set_election_timeout(1000);
    ASSERT_TRUE(0 == r.get_timeout_elapsed());

    r.periodic(0);
    ASSERT_TRUE(0 == r.get_timeout_elapsed());

    r.periodic(100);
    ASSERT_TRUE(100 == r.get_timeout_elapsed());
}

TEST(RaftServer,election_timeout_sets_to_zero_when_elapsed_time_greater_than_timeout)
{
    RaftServer r;
    r.set_election_timeout(1000);

    /* greater than 1000 */
    r.periodic(2000);
    /* less than 1000 as the timeout would be randomised */
    ASSERT_TRUE(r.get_timeout_elapsed() < 1000);
}

TEST(RaftServer,cfg_sets_num_nodes)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};
    RaftServer r;
    r.set_configuration(cfg,0);

    ASSERT_TRUE(2 == r.get_num_nodes());
}

TEST(RaftServer,cant_get_node_we_dont_have)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);

    ASSERT_TRUE(r.get_last_node() != r.get_node(0));
    ASSERT_TRUE(r.get_last_node() != r.get_node(1));
    ASSERT_TRUE(r.get_last_node() == r.get_node(2));
}

/* If term > currentTerm, set currentTerm to term (step down if candidate or
 * leader) */
TEST(RaftServer,votes_are_majority_is_true)
{
    /* 1 of 3 = lose */
    ASSERT_TRUE(0 == raft_votes_is_majority(3,1));

    /* 2 of 3 = win */
    ASSERT_TRUE(1 == raft_votes_is_majority(3,2));

    /* 2 of 5 = lose */
    ASSERT_TRUE(0 == raft_votes_is_majority(5,2));

    /* 3 of 5 = win */
    ASSERT_TRUE(1 == raft_votes_is_majority(5,3));

    /* 2 of 1?? This is an error */
    ASSERT_TRUE(0 == raft_votes_is_majority(1,2));
}

TEST(RaftServer,dont_increase_votes_for_me_when_receive_request_vote_response_is_not_granted)
{

    msg_requestvote_response_t rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_current_term(1);
    ASSERT_TRUE(0 == r.get_nvotes_for_me());

    memset(&rvr, 0, sizeof(msg_requestvote_response_t));
    rvr.term = 1;
    rvr.vote_granted = 0;
    r.recv_requestvote_response(1,&rvr);
    ASSERT_TRUE(0 == r.get_nvotes_for_me());
}

TEST(RaftServer,increase_votes_for_me_when_receive_request_vote_response)
{

    msg_requestvote_response_t rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_current_term(1);
    ASSERT_TRUE(0 == r.get_nvotes_for_me());

    memset(&rvr, 0, sizeof(msg_requestvote_response_t));
    rvr.term = 1;
    rvr.vote_granted = 1;
    r.recv_requestvote_response(1,&rvr);
    ASSERT_TRUE(1 == r.get_nvotes_for_me());
}

/* Reply false if term < currentTerm (�5.1) */
TEST(RaftServer,recv_requestvote_reply_false_if_term_less_than_current_term)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_requestvote_t rv(1,0,0,0);
    msg_requestvote_response_t *rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);
    r.set_current_term(2);

    r.recv_requestvote(1,&rv);

    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rvr);
    ASSERT_TRUE(0 == rvr->vote_granted);
}

/* If votedFor is null or candidateId, and candidate's log is at
 * least as up-to-date as local log, grant vote (�5.2, �5.4) */
TEST(RaftServer,dont_grant_vote_if_we_didnt_vote_for_this_candidate)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_requestvote_t rv(1,1,0,1);
    msg_requestvote_response_t *rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    r.vote(0);
    r.recv_requestvote(1,&rv);

    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rvr);
    ASSERT_TRUE(0 == rvr->vote_granted);
}

TEST(RaftFollower,becomes_follower_is_follower)
{
    RaftServer r;

    r.become_follower();
    ASSERT_TRUE(r.get_state().is_follower());
}

TEST(RaftFollower,becomes_follower_clears_voted_for)
{
    RaftServer r;
    r.vote(1);
    ASSERT_TRUE(1 == r.get_voted_for());
    r.become_follower();
    ASSERT_TRUE(-1 == r.get_voted_for());
}

/* 5.1 */
TEST(RaftFollower,recv_appendentries_reply_false_if_term_less_than_currentterm)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_appendentries_response_t *aer;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    RaftServer r;
    r.set_configuration(cfg,0);
    sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    /* term is low */
    MsgAppendEntries ae(1,0,0,0,0,0);

    /*  higher current term */
    r.set_current_term(5);
    r.recv_appendentries(1,&ae);

    /*  response is false */
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(0 == aer->success);
}

/* TODO: check if test case is needed */
TEST(RaftFollower,recv_appendentries_updates_currentterm_if_term_gt_currentterm)
{

    void *sender;
    msg_appendentries_response_t *aer;

    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    /*  older currentterm */
    r.set_current_term(1);

    /*  newer term for appendentry */
    /* no prev log idx */
    MsgAppendEntries ae (2,0,0,0,0,0);

    /*  appendentry has newer term, so we change our currentterm */
    r.recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    /* term has been updated */
    ASSERT_TRUE(2 == r.get_current_term());
}

TEST(RaftFollower,doesnt_log_after_appendentry_if_no_entries_are_specified)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    void* sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    r.get_state().set(RAFT_STATE_FOLLOWER);
    ASSERT_TRUE(0 == r.get_log_count());

    /* receive an appendentry with commit */
    MsgAppendEntries ae(1,0,4,1,0,5);

    r.recv_appendentries(1,&ae);
    ASSERT_TRUE(0 == r.get_log_count());
}

TEST(RaftFollower,increases_log_after_appendentry)
{

    void *sender;
    msg_appendentries_response_t *aer;
    char* str = const_cast<char*>("aaa");

    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    r.get_state().set(RAFT_STATE_FOLLOWER);

    /*  log size s */
    ASSERT_TRUE(0 == r.get_log_count());

    /* receive an appendentry with commit */

    /* include one entry */
    msg_entry_t ety(1,reinterpret_cast<unsigned char*>(str),3);
    MsgAppendEntries ae(1,0,0,1,1,5);
    ae.addEntry(ety);

    r.recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    ASSERT_TRUE(1 == r.get_log_count());
}

/*  5.3 */
TEST(RaftFollower,recv_appendentries_reply_false_if_doesnt_have_log_at_prev_log_idx_which_matches_prev_log_term)
{

    void *sender;
    void *msg;
    char* str = const_cast<char*>("aaa");
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    msg_appendentries_response_t *aer;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    /* term is different from appendentries */
    r.set_current_term(2);
    r.set_commit_idx(1);
    r.set_last_applied_idx(1);
    // TODO at log manually?

    /* log idx that server doesn't have */
    /* prev_log_term is less than current term (ie. 2) */
    MsgAppendEntries ae(2,0,1,1,0,0);

    /* trigger reply */
    r.recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));

    /* reply is false */
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(0 == aer->success);
}

/* 5.3 */
TEST(RaftFollower,recv_appendentries_delete_entries_if_conflict_with_new_entries)
{

    void *sender;
    msg_appendentries_response_t *aer;
    raft_entry_t ety_appended;

    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    r.set_configuration(cfg,0);
    sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    r.set_current_term(1);

    raft_entry_t ety;

    /* increase log size */
    char* str1 = const_cast<char *>("111");
    ety.d_data = str1;
    ety.d_len = 3;
    ety.d_id = 1;
    ety.d_term = 1;
    r.append_entry(ety);
    ASSERT_TRUE(1 == r.get_log_count());

    /* this log will be overwritten by the appendentries below */
    raft_entry_t ety_extra;
    char* str2 = const_cast<char *>("222");
    ety_extra.d_data = str2;
    ety_extra.d_len = 3;
    ety_extra.d_id = 2;
    ety_extra.d_term = 1;
    r.append_entry(ety_extra);
    ASSERT_TRUE(2 == r.get_log_count());
    ASSERT_NO_THROW(ety_appended = r.get_entry_from_idx(2));
    ASSERT_TRUE(!memcmp(ety_appended.d_data,str2,3));


    /* include one entry */
    char* str3 = const_cast<char *>("333");
    /* pass a appendentry that is newer  */
    msg_entry_t mety(3,reinterpret_cast<unsigned char*>(str3),3);
    MsgAppendEntries ae(2,0,1,1,1,0);
    ae.addEntry(mety);

    r.recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    ASSERT_TRUE(2 == r.get_log_count());
    ASSERT_NO_THROW(ety_appended = r.get_entry_from_idx(1));
    ASSERT_TRUE(!memcmp(ety_appended.d_data,str1,3));
}

TEST(RaftFollower,recv_appendentries_add_new_entries_not_already_in_log)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    RaftServer r;
    sender = sender_new(NULL);
    r.set_configuration(cfg,0);
    r.set_current_term(1);
    r.set_callbacks(&funcs,sender);

    /* include entries */
    msg_entry_t e[2];
    memset(&e,0,sizeof(msg_entry_t) * 2);
    e[0].id(1);
    e[1].id(2);
    MsgAppendEntries ae(1,0,0,1,2,0);
    ae.addEntry(e[0]);
    ae.addEntry(e[1]);
    r.recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    ASSERT_TRUE(2 == r.get_log_count());
}

/* If leaderCommit > commitidx, set commitidx =
 *  min(leaderCommit, last log idx) */
TEST(RaftFollower,recv_appendentries_set_commitidx_to_prevLogIdx)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    /* include entries */
    msg_entry_t e[4];
    memset(&e,0,sizeof(msg_entry_t) * 4);
    e[0].id(1);
    e[1].id(2);
    e[2].id(3);
    e[3].id(4);
    MsgAppendEntries ae(1,0,0,1,4,0);
    ae.addEntry(e[0]);
    ae.addEntry(e[1]);
    ae.addEntry(e[2]);
    ae.addEntry(e[3]);
    r.recv_appendentries(1,&ae);

    /* receive an appendentry with commit */
    ae.setTerm(1);
    ae.setPrevLogTerm(1);
    ae.setPrevLogIdx(4);
    ae.setLeaderCommit(5);
    /* receipt of appendentries changes commit idx */
    r.recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    /* set to 4 because commitIDX is lower */
    ASSERT_TRUE(4 == r.get_commit_idx());
}

TEST(RaftFollower,recv_appendentries_set_commitidx_to_LeaderCommit)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);


    /* include entries */
    msg_entry_t e[4];
    memset(&e,0,sizeof(msg_entry_t) * 4);
    e[0].id(1);
    e[1].id(2);
    e[2].id(3);
    e[3].id(4);
    MsgAppendEntries ae(1,0,0,1,4,0);
    ae.addEntry(e[0]);
    ae.addEntry(e[1]);
    ae.addEntry(e[2]);
    ae.addEntry(e[3]);
    r.recv_appendentries(1,&ae);

    /* receive an appendentry with commit */
    ae.setTerm(1);
    ae.setPrevLogTerm(1);
    ae.setPrevLogIdx(3);
    ae.setLeaderCommit(3);
    /* receipt of appendentries changes commit idx */
    r.recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != aer);
    ASSERT_TRUE(1 == aer->success);
    /* set to 3 because leaderCommit is lower */
    ASSERT_TRUE(3 == r.get_commit_idx());
}

TEST(RaftFollower,becomes_candidate_when_election_timeout_occurs)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    RaftServer r;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    void* sender = sender_new(NULL);
    r.set_callbacks(&funcs,sender);

    /*  1 second election timeout */
    r.set_election_timeout(1000);

    r.set_configuration(cfg,0);

    /*  1.001 seconds have passed */
    r.periodic(1001);

    /* is a candidate now */
    ASSERT_TRUE(1 == r.get_state().is_candidate());
}

/* Candidate 5.2 */
TEST(RaftFollower,dont_grant_vote_if_candidate_has_a_less_complete_log)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    msg_requestvote_t rv(1,1,1,1);
    msg_requestvote_response_t *rvr;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* server's term and idx are more up-to-date */
    r.set_current_term(1);
    r.set_current_idx(2);

    /* vote not granted */
    r.recv_requestvote(1,&rv);
    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rvr);
    ASSERT_TRUE(0 == rvr->vote_granted);
}

TEST(RaftCandidate,becomes_candidate_is_candidate)
{
    RaftServer r;

    r.become_candidate();
    ASSERT_TRUE(r.get_state().is_candidate());
}

/* Candidate 5.2 */
TEST(RaftFollower,becoming_candidate_increments_current_term)
{
    RaftServer r;

    ASSERT_TRUE(0 == r.get_current_term());
    r.become_candidate();
    ASSERT_TRUE(1 == r.get_current_term());
}

/* Candidate 5.2 */
TEST(RaftFollower,becoming_candidate_votes_for_self)
{
    RaftServer r;

    ASSERT_TRUE(-1 == r.get_voted_for());
    r.become_candidate();
    ASSERT_TRUE(r.get_nodeid() == r.get_voted_for());
    ASSERT_TRUE(1 == r.get_nvotes_for_me());
}

/* Candidate 5.2 */
TEST(RaftFollower,becoming_candidate_resets_election_timeout)
{
    RaftServer r;
    r.set_election_timeout(1000);
    ASSERT_TRUE(0 == r.get_timeout_elapsed());

    r.periodic(900);
    ASSERT_TRUE(900 == r.get_timeout_elapsed());

    r.become_candidate();
    /* time is selected randomly */
    ASSERT_TRUE(r.get_timeout_elapsed() < 900);
}

TEST(RaftFollower,receiving_appendentries_resets_election_timeout)
{
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    void* sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_election_timeout(1000);

    r.periodic(900);

    MsgAppendEntries ae;
    ae.setTerm(1);
    r.recv_appendentries(1,&ae);
    ASSERT_TRUE(0 == r.get_timeout_elapsed());
}
 
/* Candidate 5.2 */
TEST(RaftFollower,becoming_candidate_requests_votes_from_other_servers)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),&NODE_ID_3},
                {(-1),NULL}};
    msg_requestvote_t* rv;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* set term so we can check it gets included in the outbound message */
    r.set_current_term(2);
    r.set_current_idx(5);

    /* becoming candidate triggers vote requests */
    r.become_candidate();

    /* 2 nodes = 2 vote requests */
    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rv);
    ASSERT_TRUE(2 != rv->term());
    ASSERT_TRUE(3 == rv->term());
    /*  TODO: there should be more items */
    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rv);
    ASSERT_TRUE(3 == rv->term());
}

/* Candidate 5.2 */
TEST(RaftCandidate,election_timeout_and_no_leader_results_in_new_election)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    msg_requestvote_response_t vr;

    memset(&vr,0,sizeof(msg_requestvote_response_t));
    vr.term = 0;
    vr.vote_granted = 1;

    sender = sender_new(NULL);

    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);
    r.set_election_timeout(1000);

    /* server wants to be leader, so becomes candidate */
    r.become_candidate();
    ASSERT_TRUE(1 == r.get_current_term());

    /* clock over (ie. 1000 + 1), causing new election */
    r.periodic(1001);
    ASSERT_TRUE(2 == r.get_current_term());

    /*  receiving this vote gives the server majority */
//    r.recv_requestvote_response(1,&vr);
//    ASSERT_TRUE(1 == r.get_state().is_leader());
}

/* Candidate 5.2 */
TEST(RaftCandidate,receives_majority_of_votes_becomes_leader)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),&NODE_ID_3},
                {(-1),&NODE_ID_4},
                {(-1),&NODE_ID_5},
                {(-1),NULL}};

    msg_requestvote_response_t vr;

    sender = sender_new(NULL);

    RaftServer r;
    r.set_configuration(cfg,0);
    ASSERT_TRUE(5 == r.get_num_nodes());
    r.set_callbacks(&funcs,sender);

    /* vote for self */
    r.become_candidate();
    ASSERT_TRUE(1 == r.get_current_term());
    ASSERT_TRUE(1 == r.get_nvotes_for_me());

    /* a vote for us */
    memset(&vr,0,sizeof(msg_requestvote_response_t));
    vr.term = 1;
    vr.vote_granted = 1;
    /* get one vote */
    r.recv_requestvote_response(1,&vr);
    ASSERT_TRUE(2 == r.get_nvotes_for_me());
    ASSERT_TRUE(0 == r.get_state().is_leader());

    /* get another vote
     * now has majority (ie. 3/5 votes) */
    r.recv_requestvote_response(2,&vr);
    ASSERT_TRUE(1 == r.get_state().is_leader());
}

/* Candidate 5.2 */
TEST(RaftCandidate,will_not_respond_to_voterequest_if_it_has_already_voted)
{

    void *sender;
    void *msg;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    msg_requestvote_response_t* rvr;
    msg_requestvote_t rv(0,0,0,0);

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    r.vote(0);

    memset(&rv,0,sizeof(msg_requestvote_t));
    r.recv_requestvote(1,&rv);

    /* we've vote already, so won't respond with a vote granted... */
    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(0 == rvr->vote_granted);
}

/* Candidate 5.2 */
TEST(RaftCandidate,requestvote_includes_logidx)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_requestvote_t* rv;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.get_state().set(RAFT_STATE_CANDIDATE);

    r.set_callbacks(&funcs,sender);
    r.set_current_term(5);
    r.set_current_idx(3);
    r.send_requestvote(1);

    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != rv);
    ASSERT_TRUE(3 == rv->last_log_idx());
    ASSERT_TRUE(5 == rv->term());
}

/* Candidate 5.2 */
TEST(RaftCandidate,recv_appendentries_frm_leader_results_in_follower)
{

    void *sender;
    void *msg;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);

    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    r.get_state().set(RAFT_STATE_CANDIDATE);
    ASSERT_TRUE(0 == r.get_state().is_follower());

    /* receive recent appendentries */
    MsgAppendEntries ae(1,0,0,0,0,0);
    r.recv_appendentries(1,&ae);
    ASSERT_TRUE(1 == r.get_state().is_follower());
}

/* Candidate 5.2 */
TEST(RaftCandidate,recv_appendentries_frm_invalid_leader_doesnt_result_in_follower)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* server's log is newer */
    r.set_current_term(1);
    r.set_current_idx(2);

    /*  is a candidate */
    r.get_state().set(RAFT_STATE_CANDIDATE);
    ASSERT_TRUE(0 == r.get_state().is_follower());

    /*  invalid leader determined by "leaders" old log */
    MsgAppendEntries ae(1,0,1,1,0,0);

    /* appendentry from invalid leader doesn't make candidate become follower */
    r.recv_appendentries(1,&ae);
    ASSERT_TRUE(1 == r.get_state().is_candidate());
}

TEST(RaftLeader,becomes_leader_is_leader)
{
    RaftServer r;

    r.become_leader();
    ASSERT_TRUE(r.get_state().is_leader());
}

TEST(RaftLeader,becomes_leader_clears_voted_for)
{
    RaftServer r;
    r.vote(1);
    ASSERT_TRUE(1 == r.get_voted_for());
    r.become_leader();
    ASSERT_TRUE(-1 == r.get_voted_for());
}

TEST(RaftLeader,when_becomes_leader_all_nodes_have_nextidx_equal_to_lastlog_idx_plus_1)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),&NODE_ID_3},
                {(-1),NULL}};

    MsgAppendEntries* ae;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* candidate to leader */
    r.get_state().set(RAFT_STATE_CANDIDATE);
    r.become_leader();

    int i;
    for (i=0; i<r.get_num_nodes(); i++)
    {
        if (i==0) continue;
        NodeIter p = r.get_node(i);
        ASSERT_TRUE(r.get_current_idx() + 1 == p->get_next_idx());
    }
}

/* 5.2 */
TEST(RaftLeader,when_it_becomes_a_leader_sends_empty_appendentries)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),&NODE_ID_3},
                {(-1),NULL}};

    MsgAppendEntries* ae;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* candidate to leader */
    r.get_state().set(RAFT_STATE_CANDIDATE);
    r.become_leader();

    /* receive appendentries messages for both nodes */
    ae = reinterpret_cast<MsgAppendEntries*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != ae);
    ae = reinterpret_cast<MsgAppendEntries*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != ae);
}

/* 5.2
 * Note: commit means it's been appended to the log, not applied to the FSM */
TEST(RaftLeader,responds_to_entry_msg_when_entry_is_committed)
{

    void *sender;
    msg_entry_response_t *cr;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* I am the leader */
    r.get_state().set(RAFT_STATE_LEADER);
    ASSERT_TRUE(0 == r.get_log_count());

    /* entry message */
    msg_entry_t ety(1,(unsigned char*)"entry",strlen("entry"));

    /* receive entry */
    r.recv_entry(1,&ety);
    ASSERT_TRUE(1 == r.get_log_count());

    /* trigger response through commit */
    r.apply_entry();

    /* leader sent response to entry message */
    cr = reinterpret_cast<msg_entry_response_t*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != cr);
}

/* 5.3 */
TEST(RaftLeader,sends_appendentries_with_NextIdx_when_PrevIdx_gt_NextIdx)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    MsgAppendEntries* ae;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* i'm leader */
    r.get_state().set(RAFT_STATE_LEADER);

    NodeIter p = r.get_node(0);
    p->set_next_idx(4);

    /* receive appendentries messages */
    r.send_appendentries(0);
    ae = reinterpret_cast<MsgAppendEntries*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != ae);
}

/* 5.3 */
TEST(RaftLeader,retries_appendentries_with_decremented_NextIdx_log_inconsistency)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    MsgAppendEntries* ae;

    sender = sender_new(NULL);
    RaftServer r;
    r.set_callbacks(&funcs,sender);
    r.set_configuration(cfg,0);

    /* i'm leader */
    r.get_state().set(RAFT_STATE_LEADER);

    /* receive appendentries messages */
    r.send_appendentries(0);
    ae = reinterpret_cast<MsgAppendEntries*>(sender_poll_msg_data(sender));
    ASSERT_TRUE(NULL != ae);
}

/*
 * If there exists an N such that N > commitidx, a majority
 * of matchidx[i] = N, and log[N].term == currentTerm:
 * set commitidx = N (�5.2, �5.4).  */
TEST(RaftLeader,append_entry_to_log_increases_idxno)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    msg_entry_t ety(1,(unsigned char*)"entry",strlen("entry"));
    void *sender = sender_new(NULL);
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);
    r.get_state().set(RAFT_STATE_LEADER);
    ASSERT_TRUE(0 == r.get_log_count());

    r.recv_entry(1,&ety);
    ASSERT_TRUE(1 == r.get_log_count());
}

TEST(RaftLeader,increase_commit_idx_when_majority_have_entry_and_atleast_one_newer_entry)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_appendentries_response_t aer;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};
    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);
    r.set_callbacks(&funcs,sender);

    /* I'm the leader */
    r.get_state().set(RAFT_STATE_LEADER);
    r.set_current_term(1);
    r.set_commit_idx(0);
    /* the last applied idx will became 1, and then 2 */
    r.set_last_applied_idx(0);

    /* append entries - we need two */
    raft_entry_t ety;
    ety.d_term = 1;
    ety.d_id = 1;
    ety.d_data = "aaaa";
    ety.d_len = 4;
    r.append_entry(ety);
    ety.d_id = 2;
    r.append_entry(ety);

    memset(&aer,0,sizeof(msg_appendentries_response_t));

    /* FIRST entry log application */
    /* send appendentries -
     * server will be waiting for response */
    r.send_appendentries(0);
    r.send_appendentries(1);
    /* receive mock success responses */
    aer.term = 1;
    aer.success = 1;
    aer.current_idx = 1;
    aer.first_idx = 1;
    r.recv_appendentries_response(0,&aer);
    r.recv_appendentries_response(1,&aer);
    /* leader will now have majority followers who have appended this log */
    ASSERT_TRUE(0 != r.get_commit_idx());
    ASSERT_TRUE(2 != r.get_commit_idx());
    ASSERT_TRUE(1 == r.get_commit_idx());
    ASSERT_TRUE(1 == r.get_last_applied_idx());

    /* SECOND entry log application */
    /* send appendentries -
     * server will be waiting for response */
    r.send_appendentries(0);
    r.send_appendentries(1);
    /* receive mock success responses */
    aer.term = 5;
    aer.success = 1;
    aer.current_idx = 2;
    aer.first_idx = 2;
    r.recv_appendentries_response(0,&aer);
    r.recv_appendentries_response(1,&aer);
    /* leader will now have majority followers who have appended this log */
    ASSERT_TRUE(2 == r.get_commit_idx());
    ASSERT_TRUE(2 == r.get_last_applied_idx());
}

TEST(RaftLeader,steps_down_if_received_appendentries_is_newer_than_itself)
{

    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    sender = sender_new(NULL);
    RaftServer r;
    r.set_configuration(cfg,0);

    r.get_state().set(RAFT_STATE_LEADER);
    r.set_current_term(5);
    r.set_current_idx(5);
    r.set_callbacks(&funcs,sender);

    MsgAppendEntries ae(5,0,6,5,0,0);
    r.recv_appendentries(1,&ae);

    ASSERT_TRUE(1 == r.get_state().is_follower());
}

/* TODO: If a server receives a request with a stale term number, it rejects the request. */
#if 0
void T_estRaft_leader_sends_appendentries_when_receive_entry_msg()
#endif
