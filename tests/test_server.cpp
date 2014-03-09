
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "CuTest.h"

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


// TODO: leader doesn't timeout and cause election

int raft_get_commit_idx(raft_server_t* me_);

void TestRaft_server_voted_for_records_who_we_voted_for(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->vote(2);
    CuAssertTrue(tc, 2 == r->get_voted_for());
}

void TestRaft_server_idx_starts_at_1(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    CuAssertTrue(tc, 1 == r->get_current_idx());
}

void TestRaft_server_currentterm_defaults_to_0(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    CuAssertTrue(tc, 0 == r->get_current_term());
}

void TestRaft_server_set_currentterm_sets_term(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->set_current_term(5);
    CuAssertTrue(tc, 5 == r->get_current_term());
}

void TestRaft_server_voting_results_in_voting(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->vote(1);
    CuAssertTrue(tc, 1 == r->get_voted_for());
    r->vote(9);
    CuAssertTrue(tc, 9 == r->get_voted_for());
}

void TestRaft_election_start_increments_term(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->set_current_term(1);
    r->election_start();
    CuAssertTrue(tc, 2 == r->get_current_term());
}

void TestRaft_set_state(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->get_state().set(RAFT_STATE_LEADER);
    CuAssertTrue(tc, RAFT_STATE_LEADER == r->get_state().get());
}

void TestRaft_server_starts_as_follower(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    CuAssertTrue(tc, RAFT_STATE_FOLLOWER == r->get_state().get());
}

void TestRaft_server_starts_with_election_timeout_of_1000ms(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    CuAssertTrue(tc, 1000 == r->get_election_timeout());
}

void TestRaft_server_starts_with_request_timeout_of_200ms(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    CuAssertTrue(tc, 200 == r->get_request_timeout());
}

void TestRaft_server_entry_append_cant_append_if_id_is_zero(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.data = str;
    ety.len = 3;
    ety.id = 0;
    ety.term = 1;

    r = new RaftServer();
    CuAssertTrue(tc, 1 == r->get_current_idx());
    r->append_entry(&ety);
    CuAssertTrue(tc, 1 == r->get_current_idx());
}

void TestRaft_server_entry_append_increases_logidx(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.data = str;
    ety.len = 3;
    ety.id = 1;
    ety.term = 1;

    r = new RaftServer();
    CuAssertTrue(tc, 1 == r->get_current_idx());
    r->append_entry(&ety);
    CuAssertTrue(tc, 2 == r->get_current_idx());
}

void TestRaft_server_append_entry_means_entry_gets_current_term(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.data = str;
    ety.len = 3;
    ety.id = 1;
    ety.term = 1;

    r = new RaftServer();
    CuAssertTrue(tc, 1 == r->get_current_idx());
    r->append_entry(&ety);
    CuAssertTrue(tc, 2 == r->get_current_idx());
}

#if 0
/* TODO: no support for duplicate detection yet */
void T_estRaft_server_append_entry_not_sucessful_if_entry_with_id_already_appended(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    char* str = const_cast<char*>("aaa");

    ety.data = str;
    ety.len = 3;
    ety.id = 1;
    ety.term = 1;

    r = new RaftServer();
    CuAssertTrue(tc, 1 == r->get_current_idx());
    r->append_entry(&ety);
    r->append_entry(&ety);
    CuAssertTrue(tc, 2 == r->get_current_idx());

    /* different ID so we can be successful */
    ety.id = 2;
    r->append_entry(&ety);
    CuAssertTrue(tc, 3 == r->get_current_idx());
}
#endif

void TestRaft_server_entry_is_retrieveable_using_idx(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t e1;
    raft_entry_t e2;
    raft_entry_t *ety_appended;
    char* str = const_cast<char*>("aaa");
    char* str2 = const_cast<char*>("bbb");

    r = new RaftServer();

    e1.term = 1;
    e1.id = 1;
    e1.data = str;
    e1.len = 3;
    r->append_entry(&e1);

    /* different ID so we can be successful */
    e2.term = 1;
    e2.id = 2;
    e2.data = str2;
    e2.len = 3;
    r->append_entry(&e2);

    CuAssertTrue(tc, NULL != (ety_appended = r->get_entry_from_idx(2)));
    CuAssertTrue(tc, !strncmp(reinterpret_cast<char*>(ety_appended->data),reinterpret_cast<char*>(str2),3));
}

void TestRaft_server_wont_apply_entry_if_we_dont_have_entry_to_apply(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    raft_entry_t *ety_appended;
    char* str = const_cast<char*>("aaa");

    r = new RaftServer();
    r->set_commit_idx(0);
    r->set_last_applied_idx(0);

    r->apply_entry();
    CuAssertTrue(tc, 0 == r->get_last_applied_idx());
    CuAssertTrue(tc, 0 == r->get_commit_idx());

    ety.term = 1;
    ety.id = 1;
    ety.data = str;
    ety.len = 3;
    r->append_entry(&ety);
    r->apply_entry();
    CuAssertTrue(tc, 1 == r->get_last_applied_idx());
    CuAssertTrue(tc, 1 == r->get_commit_idx());
}

/* If commitidx > lastApplied: increment lastApplied, apply log[lastApplied]
 * to state machine (�5.3) */
void TestRaft_server_increment_lastApplied_when_lastApplied_lt_commitidx(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;

    r = new RaftServer();
    /* must be follower */
    r->get_state().set(RAFT_STATE_FOLLOWER);
    r->set_current_term(1);
    r->set_commit_idx(1);
    r->set_last_applied_idx(0);

    /* need at least one entry */
    ety.term = 1;
    ety.id = 1;
    ety.data = const_cast<char*>("aaa");
    ety.len = 3;
    r->append_entry(&ety);

    /* let time lapse */
    r->periodic(1);
    CuAssertTrue(tc, 0 != r->get_last_applied_idx());
    CuAssertTrue(tc, 1 == r->get_last_applied_idx());
}

void TestRaft_server_apply_entry_increments_last_applied_idx(CuTest* tc)
{
    RaftServer *r;
    raft_entry_t ety;
    raft_entry_t *ety_appended;
    char* str = const_cast<char*>("aaa");

    ety.term = 1;

    r = new RaftServer();
    r->set_commit_idx(1);
    r->set_last_applied_idx(0);

    ety.id = 1;
    ety.data = str;
    ety.len = 3;
    r->append_entry(&ety);
    r->apply_entry();
    CuAssertTrue(tc, 1 == r->get_last_applied_idx());
}

void TestRaft_server_periodic_elapses_election_timeout(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    /* we don't want to set the timeout to zero */
    r->set_election_timeout(1000);
    CuAssertTrue(tc, 0 == r->get_timeout_elapsed());

    r->periodic(0);
    CuAssertTrue(tc, 0 == r->get_timeout_elapsed());

    r->periodic(100);
    CuAssertTrue(tc, 100 == r->get_timeout_elapsed());
}

void TestRaft_server_election_timeout_sets_to_zero_when_elapsed_time_greater_than_timeout(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->set_election_timeout(1000);

    /* greater than 1000 */
    r->periodic(2000);
    /* less than 1000 as the timeout would be randomised */
    CuAssertTrue(tc, r->get_timeout_elapsed() < 1000);
}

void TestRaft_server_cfg_sets_num_nodes(CuTest * tc)
{
    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};
    RaftServer* r = new RaftServer();
    r->set_configuration(cfg,0);

    CuAssertTrue(tc, 2 == r->get_num_nodes());
}

void TestRaft_server_cant_get_node_we_dont_have(CuTest * tc)
{
    RaftServer *r;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();
    r->set_configuration(cfg,0);

    CuAssertTrue(tc, r->get_last_node() != r->get_node(0));
    CuAssertTrue(tc, r->get_last_node() != r->get_node(1));
    CuAssertTrue(tc, r->get_last_node() == r->get_node(2));
}

/* If term > currentTerm, set currentTerm to term (step down if candidate or
 * leader) */
void TestRaft_votes_are_majority_is_true(
    CuTest * tc
)
{
    /* 1 of 3 = lose */
    CuAssertTrue(tc, 0 == raft_votes_is_majority(3,1));

    /* 2 of 3 = win */
    CuAssertTrue(tc, 1 == raft_votes_is_majority(3,2));

    /* 2 of 5 = lose */
    CuAssertTrue(tc, 0 == raft_votes_is_majority(5,2));

    /* 3 of 5 = win */
    CuAssertTrue(tc, 1 == raft_votes_is_majority(5,3));

    /* 2 of 1?? This is an error */
    CuAssertTrue(tc, 0 == raft_votes_is_majority(1,2));
}

void TestRaft_server_dont_increase_votes_for_me_when_receive_request_vote_response_is_not_granted(
    CuTest * tc
)
{
    RaftServer *r;
    msg_requestvote_response_t rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_current_term(1);
    CuAssertTrue(tc, 0 == r->get_nvotes_for_me());

    memset(&rvr, 0, sizeof(msg_requestvote_response_t));
    rvr.term = 1;
    rvr.vote_granted = 0;
    r->recv_requestvote_response(1,&rvr);
    CuAssertTrue(tc, 0 == r->get_nvotes_for_me());
}

void TestRaft_server_increase_votes_for_me_when_receive_request_vote_response(
    CuTest * tc
)
{
    RaftServer *r;
    msg_requestvote_response_t rvr;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_current_term(1);
    CuAssertTrue(tc, 0 == r->get_nvotes_for_me());

    memset(&rvr, 0, sizeof(msg_requestvote_response_t));
    rvr.term = 1;
    rvr.vote_granted = 1;
    r->recv_requestvote_response(1,&rvr);
    CuAssertTrue(tc, 1 == r->get_nvotes_for_me());
}

/* Reply false if term < currentTerm (�5.1) */
void TestRaft_server_recv_requestvote_reply_false_if_term_less_than_current_term(
    CuTest * tc
)
{
    RaftServer *r;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    sender = sender_new(NULL);
    r->set_callbacks(&funcs,sender);
    r->set_current_term(2);

    r->recv_requestvote(1,&rv);

    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rvr);
    CuAssertTrue(tc, 0 == rvr->vote_granted);
}

/* If votedFor is null or candidateId, and candidate's log is at
 * least as up-to-date as local log, grant vote (�5.2, �5.4) */
void TestRaft_server_dont_grant_vote_if_we_didnt_vote_for_this_candidate(
    CuTest * tc
)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    r->vote(0);
    r->recv_requestvote(1,&rv);

    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rvr);
    CuAssertTrue(tc, 0 == rvr->vote_granted);
}

void TestRaft_follower_becomes_follower_is_follower(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();

    r->become_follower();
    CuAssertTrue(tc, r->get_state().is_follower());
}

void TestRaft_follower_becomes_follower_clears_voted_for(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->vote(1);
    CuAssertTrue(tc, 1 == r->get_voted_for());
    r->become_follower();
    CuAssertTrue(tc, -1 == r->get_voted_for());
}

/* 5.1 */
void TestRaft_follower_recv_appendentries_reply_false_if_term_less_than_currentterm(CuTest * tc)
{
    RaftServer *r;
    void *sender;
    raft_cbs_t funcs = {
        sender_send,
        NULL
    };
    msg_appendentries_t ae;
    msg_appendentries_response_t *aer;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    r = new RaftServer();
    r->set_configuration(cfg,0);
    sender = sender_new(NULL);
    r->set_callbacks(&funcs,sender);

    /* term is low */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;

    /*  higher current term */
    r->set_current_term(5);
    r->recv_appendentries(1,&ae);

    /*  response is false */
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 0 == aer->success);
}

/* TODO: check if test case is needed */
void TestRaft_follower_recv_appendentries_updates_currentterm_if_term_gt_currentterm(CuTest * tc)
{
    RaftServer *r;
    void *sender;
    msg_appendentries_t ae;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    sender = sender_new(NULL);
    r->set_callbacks(&funcs,sender);

    /*  older currentterm */
    r->set_current_term(1);

    /*  newer term for appendentry */
    memset(&ae,0,sizeof(msg_appendentries_t));
    /* no prev log idx */
    ae.prev_log_idx = 0;
    ae.term = 2;

    /*  appendentry has newer term, so we change our currentterm */
    r->recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    /* term has been updated */
    CuAssertTrue(tc, 2 == r->get_current_term());
}

void TestRaft_follower_doesnt_log_after_appendentry_if_no_entries_are_specified(CuTest * tc)
{
    RaftServer *r;

    msg_appendentries_t ae;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();
    r->set_configuration(cfg,0);

    r->get_state().set(RAFT_STATE_FOLLOWER);

    /*  log size s */
    CuAssertTrue(tc, 0 == r->get_log_count());

    /* receive an appendentry with commit */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_term = 1;
    ae.prev_log_idx = 4;
    ae.leader_commit = 5;
    ae.n_entries = 0;

    r->recv_appendentries(1,&ae);
    CuAssertTrue(tc, 0 == r->get_log_count());
}

void TestRaft_follower_increases_log_after_appendentry(CuTest * tc)
{
    RaftServer *r;
    void *sender;
    msg_appendentries_t ae;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    sender = sender_new(NULL);
    r->set_callbacks(&funcs,sender);

    r->get_state().set(RAFT_STATE_FOLLOWER);

    /*  log size s */
    CuAssertTrue(tc, 0 == r->get_log_count());

    /* receive an appendentry with commit */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_term = 1;
    /* first appendentries msg */
    ae.prev_log_idx = 0;
    ae.leader_commit = 5;
    /* include one entry */
    msg_entry_t ety(1,reinterpret_cast<unsigned char*>(str),3);
    ae.entries = &ety;
    ae.n_entries = 1;

    r->recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    CuAssertTrue(tc, 1 == r->get_log_count());
}

/*  5.3 */
void TestRaft_follower_recv_appendentries_reply_false_if_doesnt_have_log_at_prev_log_idx_which_matches_prev_log_term(CuTest * tc)
{
    RaftServer *r;
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

    msg_appendentries_t ae;
    msg_appendentries_response_t *aer;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    /* term is different from appendentries */
    r->set_current_term(2);
    r->set_commit_idx(1);
    r->set_last_applied_idx(1);
    // TODO at log manually?

    /* log idx that server doesn't have */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 2;
    ae.prev_log_idx = 1;
    /* prev_log_term is less than current term (ie. 2) */
    ae.prev_log_term = 1;
    /* include one entry */
    msg_entry_t ety(1,reinterpret_cast<unsigned char*>(str),3);
    ae.entries = &ety;
    ae.n_entries = 1;

    /* trigger reply */
    r->recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));

    /* reply is false */
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 0 == aer->success);
}

/* 5.3 */
void TestRaft_follower_recv_appendentries_delete_entries_if_conflict_with_new_entries(CuTest * tc)
{
    RaftServer *r;
    void *sender;
    msg_appendentries_t ae;
    msg_appendentries_response_t *aer;
    raft_entry_t *ety_appended;

    raft_cbs_t funcs = {
        sender_send,
        NULL
    };

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();
    r->set_configuration(cfg,0);
    sender = sender_new(NULL);
    r->set_callbacks(&funcs,sender);

    r->set_current_term(1);

    raft_entry_t ety;

    /* increase log size */
    char* str1 = const_cast<char *>("111");
    ety.data = str1;
    ety.len = 3;
    ety.id = 1;
    ety.term = 1;
    r->append_entry(&ety);
    CuAssertTrue(tc, 1 == r->get_log_count());

    /* this log will be overwritten by the appendentries below */
    raft_entry_t ety_extra;
    char* str2 = const_cast<char *>("222");
    ety_extra.data = str2;
    ety_extra.len = 3;
    ety_extra.id = 2;
    ety_extra.term = 1;
    r->append_entry(&ety_extra);
    CuAssertTrue(tc, 2 == r->get_log_count());
    CuAssertTrue(tc, NULL != (ety_appended = r->get_entry_from_idx(2)));
    CuAssertTrue(tc, !memcmp(ety_appended->data,str2,3));


    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 2;
    ae.prev_log_idx = 1;
    ae.prev_log_term = 1;
    /* include one entry */
    char* str3 = const_cast<char *>("333");
    /* pass a appendentry that is newer  */
    msg_entry_t mety(3,reinterpret_cast<unsigned char*>(str3),3);
    ae.entries = &mety;
    ae.n_entries = 1;

    r->recv_appendentries(1,&ae);
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    CuAssertTrue(tc, 2 == r->get_log_count());
    CuAssertTrue(tc, NULL != (ety_appended = r->get_entry_from_idx(1)));
    CuAssertTrue(tc, !memcmp(ety_appended->data,str1,3));
}

void TestRaft_follower_recv_appendentries_add_new_entries_not_already_in_log(CuTest * tc)
{
    RaftServer *r;
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


    r = new RaftServer();
    sender = sender_new(NULL);
    r->set_configuration(cfg,0);
    r->set_current_term(1);
    r->set_callbacks(&funcs,sender);

    msg_appendentries_t ae;
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_idx = 0;
    ae.prev_log_term = 1;
    /* include entries */
    msg_entry_t e[2];
    memset(&e,0,sizeof(msg_entry_t) * 2);
    e[0].id(1);
    e[1].id(2);
    ae.entries = e;
    ae.n_entries = 2;
    r->recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    CuAssertTrue(tc, 2 == r->get_log_count());
}

/* If leaderCommit > commitidx, set commitidx =
 *  min(leaderCommit, last log idx) */
void TestRaft_follower_recv_appendentries_set_commitidx_to_prevLogIdx(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    msg_appendentries_t ae;
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_idx = 0;
    ae.prev_log_term = 1;
    /* include entries */
    msg_entry_t e[4];
    memset(&e,0,sizeof(msg_entry_t) * 4);
    e[0].id(1);
    e[1].id(2);
    e[2].id(3);
    e[3].id(4);
    ae.entries = e;
    ae.n_entries = 4;
    r->recv_appendentries(1,&ae);

    /* receive an appendentry with commit */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_term = 1;
    ae.prev_log_idx = 4;
    ae.leader_commit = 5;
    /* receipt of appendentries changes commit idx */
    r->recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    /* set to 4 because commitIDX is lower */
    CuAssertTrue(tc, 4 == r->get_commit_idx());
}

void TestRaft_follower_recv_appendentries_set_commitidx_to_LeaderCommit(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    msg_appendentries_t ae;
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_idx = 0;
    ae.prev_log_term = 1;
    /* include entries */
    msg_entry_t e[4];
    memset(&e,0,sizeof(msg_entry_t) * 4);
    e[0].id(1);
    e[1].id(2);
    e[2].id(3);
    e[3].id(4);
    ae.entries = e;
    ae.n_entries = 4;
    r->recv_appendentries(1,&ae);

    /* receive an appendentry with commit */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_term = 1;
    ae.prev_log_idx = 3;
    ae.leader_commit = 3;
    /* receipt of appendentries changes commit idx */
    r->recv_appendentries(1,&ae);

    msg_appendentries_response_t *aer;
    aer = reinterpret_cast<msg_appendentries_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != aer);
    CuAssertTrue(tc, 1 == aer->success);
    /* set to 3 because leaderCommit is lower */
    CuAssertTrue(tc, 3 == r->get_commit_idx());
}

void TestRaft_follower_becomes_candidate_when_election_timeout_occurs(CuTest * tc)
{
    RaftServer *r;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};

    r = new RaftServer();

    /*  1 second election timeout */
    r->set_election_timeout(1000);

    r->set_configuration(cfg,0);

    /*  1.001 seconds have passed */
    r->periodic(1001);

    /* is a candidate now */
    CuAssertTrue(tc, 1 == r->get_state().is_candidate());
}

/* Candidate 5.2 */
void TestRaft_follower_dont_grant_vote_if_candidate_has_a_less_complete_log(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* server's term and idx are more up-to-date */
    r->set_current_term(1);
    r->set_current_idx(2);

    /* vote not granted */
    r->recv_requestvote(1,&rv);
    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rvr);
    CuAssertTrue(tc, 0 == rvr->vote_granted);
}

void TestRaft_candidate_becomes_candidate_is_candidate(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();

    r->become_candidate();
    CuAssertTrue(tc, r->get_state().is_candidate());
}

/* Candidate 5.2 */
void TestRaft_follower_becoming_candidate_increments_current_term(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();

    CuAssertTrue(tc, 0 == r->get_current_term());
    r->become_candidate();
    CuAssertTrue(tc, 1 == r->get_current_term());
}

/* Candidate 5.2 */
void TestRaft_follower_becoming_candidate_votes_for_self(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();

    CuAssertTrue(tc, -1 == r->get_voted_for());
    r->become_candidate();
    CuAssertTrue(tc, r->get_nodeid() == r->get_voted_for());
    CuAssertTrue(tc, 1 == r->get_nvotes_for_me());
}

/* Candidate 5.2 */
void TestRaft_follower_becoming_candidate_resets_election_timeout(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->set_election_timeout(1000);
    CuAssertTrue(tc, 0 == r->get_timeout_elapsed());

    r->periodic(900);
    CuAssertTrue(tc, 900 == r->get_timeout_elapsed());

    r->become_candidate();
    /* time is selected randomly */
    CuAssertTrue(tc, r->get_timeout_elapsed() < 900);
}

void TestRaft_follower_receiving_appendentries_resets_election_timeout(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->set_election_timeout(1000);

    r->periodic(900);

    msg_appendentries_t ae;
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    r->recv_appendentries(1,&ae);
    CuAssertTrue(tc, 0 == r->get_timeout_elapsed());
}
 
/* Candidate 5.2 */
void TestRaft_follower_becoming_candidate_requests_votes_from_other_servers(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* set term so we can check it gets included in the outbound message */
    r->set_current_term(2);
    r->set_current_idx(5);

    /* becoming candidate triggers vote requests */
    r->become_candidate();

    /* 2 nodes = 2 vote requests */
    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rv);
    CuAssertTrue(tc, 2 != rv->term());
    CuAssertTrue(tc, 3 == rv->term());
    /*  TODO: there should be more items */
    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rv);
    CuAssertTrue(tc, 3 == rv->term());
}

/* Candidate 5.2 */
void TestRaft_candidate_election_timeout_and_no_leader_results_in_new_election(CuTest * tc)
{
    RaftServer *r;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);
    r->set_election_timeout(1000);

    /* server wants to be leader, so becomes candidate */
    r->become_candidate();
    CuAssertTrue(tc, 1 == r->get_current_term());

    /* clock over (ie. 1000 + 1), causing new election */
    r->periodic(1001);
    CuAssertTrue(tc, 2 == r->get_current_term());

    /*  receiving this vote gives the server majority */
//    r->recv_requestvote_response(1,&vr);
//    CuAssertTrue(tc, 1 == r->get_state().is_leader());
}

/* Candidate 5.2 */
void TestRaft_candidate_receives_majority_of_votes_becomes_leader(CuTest * tc)
{
    RaftServer *r;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    CuAssertTrue(tc, 5 == r->get_num_nodes());
    r->set_callbacks(&funcs,sender);

    /* vote for self */
    r->become_candidate();
    CuAssertTrue(tc, 1 == r->get_current_term());
    CuAssertTrue(tc, 1 == r->get_nvotes_for_me());

    /* a vote for us */
    memset(&vr,0,sizeof(msg_requestvote_response_t));
    vr.term = 1;
    vr.vote_granted = 1;
    /* get one vote */
    r->recv_requestvote_response(1,&vr);
    CuAssertTrue(tc, 2 == r->get_nvotes_for_me());
    CuAssertTrue(tc, 0 == r->get_state().is_leader());

    /* get another vote
     * now has majority (ie. 3/5 votes) */
    r->recv_requestvote_response(2,&vr);
    CuAssertTrue(tc, 1 == r->get_state().is_leader());
}

/* Candidate 5.2 */
void TestRaft_candidate_will_not_respond_to_voterequest_if_it_has_already_voted(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    r->vote(0);

    memset(&rv,0,sizeof(msg_requestvote_t));
    r->recv_requestvote(1,&rv);

    /* we've vote already, so won't respond with a vote granted... */
    rvr = reinterpret_cast<msg_requestvote_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, 0 == rvr->vote_granted);
}

/* Candidate 5.2 */
void TestRaft_candidate_requestvote_includes_logidx(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->get_state().set(RAFT_STATE_CANDIDATE);

    r->set_callbacks(&funcs,sender);
    r->set_current_term(5);
    r->set_current_idx(3);
    r->send_requestvote(1);

    rv = reinterpret_cast<msg_requestvote_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != rv);
    CuAssertTrue(tc, 3 == rv->last_log_idx());
    CuAssertTrue(tc, 5 == rv->term());
}

/* Candidate 5.2 */
void TestRaft_candidate_recv_appendentries_frm_leader_results_in_follower(CuTest * tc)
{
    RaftServer *r;
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

    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    r->get_state().set(RAFT_STATE_CANDIDATE);
    CuAssertTrue(tc, 0 == r->get_state().is_follower());

    /* receive recent appendentries */
    msg_appendentries_t ae;
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    r->recv_appendentries(1,&ae);
    CuAssertTrue(tc, 1 == r->get_state().is_follower());
}

/* Candidate 5.2 */
void TestRaft_candidate_recv_appendentries_frm_invalid_leader_doesnt_result_in_follower(CuTest * tc)
{
    RaftServer *r;
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

    msg_appendentries_t ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* server's log is newer */
    r->set_current_term(1);
    r->set_current_idx(2);

    /*  is a candidate */
    r->get_state().set(RAFT_STATE_CANDIDATE);
    CuAssertTrue(tc, 0 == r->get_state().is_follower());

    /*  invalid leader determined by "leaders" old log */
    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 1;
    ae.prev_log_idx = 1;
    ae.prev_log_term = 1;

    /* appendentry from invalid leader doesn't make candidate become follower */
    r->recv_appendentries(1,&ae);
    CuAssertTrue(tc, 1 == r->get_state().is_candidate());
}

void TestRaft_leader_becomes_leader_is_leader(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();

    r->become_leader();
    CuAssertTrue(tc, r->get_state().is_leader());
}

void TestRaft_leader_becomes_leader_clears_voted_for(CuTest * tc)
{
    RaftServer *r;

    r = new RaftServer();
    r->vote(1);
    CuAssertTrue(tc, 1 == r->get_voted_for());
    r->become_leader();
    CuAssertTrue(tc, -1 == r->get_voted_for());
}

void TestRaft_leader_when_becomes_leader_all_nodes_have_nextidx_equal_to_lastlog_idx_plus_1(CuTest * tc)
{
    RaftServer *r;
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

    msg_appendentries_t* ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* candidate to leader */
    r->get_state().set(RAFT_STATE_CANDIDATE);
    r->become_leader();

    int i;
    for (i=0; i<r->get_num_nodes(); i++)
    {
        if (i==0) continue;
        NodeIter p = r->get_node(i);
        CuAssertTrue(tc, r->get_current_idx() + 1 == p->get_next_idx());
    }
}

/* 5.2 */
void TestRaft_leader_when_it_becomes_a_leader_sends_empty_appendentries(CuTest * tc)
{
    RaftServer *r;
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

    msg_appendentries_t* ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* candidate to leader */
    r->get_state().set(RAFT_STATE_CANDIDATE);
    r->become_leader();

    /* receive appendentries messages for both nodes */
    ae = reinterpret_cast<msg_appendentries_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != ae);
    ae = reinterpret_cast<msg_appendentries_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != ae);
}

/* 5.2
 * Note: commit means it's been appended to the log, not applied to the FSM */
void TestRaft_leader_responds_to_entry_msg_when_entry_is_committed(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* I am the leader */
    r->get_state().set(RAFT_STATE_LEADER);
    CuAssertTrue(tc, 0 == r->get_log_count());

    /* entry message */
    msg_entry_t ety(1,(unsigned char*)"entry",strlen("entry"));

    /* receive entry */
    r->recv_entry(1,&ety);
    CuAssertTrue(tc, 1 == r->get_log_count());

    /* trigger response through commit */
    r->apply_entry();

    /* leader sent response to entry message */
    cr = reinterpret_cast<msg_entry_response_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != cr);
}

/* 5.3 */
void TestRaft_leader_sends_appendentries_with_NextIdx_when_PrevIdx_gt_NextIdx(CuTest * tc)
{
    RaftServer *r;
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


    msg_appendentries_t* ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* i'm leader */
    r->get_state().set(RAFT_STATE_LEADER);

    NodeIter p = r->get_node(0);
    p->set_next_idx(4);

    /* receive appendentries messages */
    r->send_appendentries(0);
    ae = reinterpret_cast<msg_appendentries_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != ae);
}

/* 5.3 */
void TestRaft_leader_retries_appendentries_with_decremented_NextIdx_log_inconsistency(CuTest * tc)
{
    RaftServer *r;
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


    msg_appendentries_t* ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_callbacks(&funcs,sender);
    r->set_configuration(cfg,0);

    /* i'm leader */
    r->get_state().set(RAFT_STATE_LEADER);

    /* receive appendentries messages */
    r->send_appendentries(0);
    ae = reinterpret_cast<msg_appendentries_t*>(sender_poll_msg_data(sender));
    CuAssertTrue(tc, NULL != ae);
}

/*
 * If there exists an N such that N > commitidx, a majority
 * of matchidx[i] = N, and log[N].term == currentTerm:
 * set commitidx = N (�5.2, �5.4).  */
void TestRaft_leader_append_entry_to_log_increases_idxno(CuTest * tc)
{
    RaftServer *r;

    /* 2 nodes */
    raft_node_configuration_t cfg[] = {
                {(-1),&NODE_ID_1},
                {(-1),&NODE_ID_2},
                {(-1),NULL}};


    msg_entry_t ety(1,(unsigned char*)"entry",strlen("entry"));

    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->get_state().set(RAFT_STATE_LEADER);
    CuAssertTrue(tc, 0 == r->get_log_count());

    r->recv_entry(1,&ety);
    CuAssertTrue(tc, 1 == r->get_log_count());
}

void TestRaft_leader_increase_commit_idx_when_majority_have_entry_and_atleast_one_newer_entry(CuTest * tc)
{
    RaftServer *r;
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
    r = new RaftServer();
    r->set_configuration(cfg,0);
    r->set_callbacks(&funcs,sender);

    /* I'm the leader */
    r->get_state().set(RAFT_STATE_LEADER);
    r->set_current_term(1);
    r->set_commit_idx(0);
    /* the last applied idx will became 1, and then 2 */
    r->set_last_applied_idx(0);

    /* append entries - we need two */
    raft_entry_t ety;
    ety.term = 1;
    ety.id = 1;
    ety.data = "aaaa";
    ety.len = 4;
    r->append_entry(&ety);
    ety.id = 2;
    r->append_entry(&ety);

    memset(&aer,0,sizeof(msg_appendentries_response_t));

    /* FIRST entry log application */
    /* send appendentries -
     * server will be waiting for response */
    r->send_appendentries(0);
    r->send_appendentries(1);
    /* receive mock success responses */
    aer.term = 1;
    aer.success = 1;
    aer.current_idx = 1;
    aer.first_idx = 1;
    r->recv_appendentries_response(0,&aer);
    r->recv_appendentries_response(1,&aer);
    /* leader will now have majority followers who have appended this log */
    CuAssertTrue(tc, 0 != r->get_commit_idx());
    CuAssertTrue(tc, 2 != r->get_commit_idx());
    CuAssertTrue(tc, 1 == r->get_commit_idx());
    CuAssertTrue(tc, 1 == r->get_last_applied_idx());

    /* SECOND entry log application */
    /* send appendentries -
     * server will be waiting for response */
    r->send_appendentries(0);
    r->send_appendentries(1);
    /* receive mock success responses */
    aer.term = 5;
    aer.success = 1;
    aer.current_idx = 2;
    aer.first_idx = 2;
    r->recv_appendentries_response(0,&aer);
    r->recv_appendentries_response(1,&aer);
    /* leader will now have majority followers who have appended this log */
    CuAssertTrue(tc, 2 == r->get_commit_idx());
    CuAssertTrue(tc, 2 == r->get_last_applied_idx());
}

void TestRaft_leader_steps_down_if_received_appendentries_is_newer_than_itself(CuTest * tc)
{
    RaftServer *r;
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

    msg_appendentries_t ae;

    sender = sender_new(NULL);
    r = new RaftServer();
    r->set_configuration(cfg,0);

    r->get_state().set(RAFT_STATE_LEADER);
    r->set_current_term(5);
    r->set_current_idx(5);
    r->set_callbacks(&funcs,sender);

    memset(&ae,0,sizeof(msg_appendentries_t));
    ae.term = 5;
    ae.prev_log_idx = 6;
    ae.prev_log_term = 5;
    r->recv_appendentries(1,&ae);

    CuAssertTrue(tc, 1 == r->get_state().is_follower());
}

/* TODO: If a server receives a request with a stale term number, it rejects the request. */
#if 0
void T_estRaft_leader_sends_appendentries_when_receive_entry_msg(CuTest * tc)
#endif
