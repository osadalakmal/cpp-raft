#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include "raft.h"

void raft_set_callbacks(raft_server_t* me_,
        raft_cbs_t* funcs, void* cb_ctx);

void raft_free(raft_server_t* me_);

void raft_election_start(raft_server_t* me_);

void raft_become_leader(raft_server_t* me_);

void raft_become_candidate(raft_server_t* me_);
void raft_become_follower(raft_server_t* me_);

int raft_periodic(raft_server_t* me_, int msec_since_last_period);
raft_entry_t* raft_get_entry_from_idx(raft_server_t* me_, int etyidx);

int raft_recv_appendentries_response(raft_server_t* me_,
        int node, msg_appendentries_response_t* r);
int raft_recv_appendentries(
        raft_server_t* me_,
        const int node,
        msg_appendentries_t* ae);
int raft_recv_requestvote(raft_server_t* me_, int node, msg_requestvote_t* vr);

int raft_votes_is_majority(const int num_nodes, const int nvotes);

int raft_recv_requestvote_response(raft_server_t* me_, int node,
        msg_requestvote_response_t* r);

int raft_send_entry_response(raft_server_t* me_,
        int node, int etyid, int was_committed);

int raft_recv_entry(raft_server_t* me_, int node, msg_entry_t* e);

int raft_send_requestvote(raft_server_t* me_, int node);

int raft_append_entry(raft_server_t* me_, raft_entry_t* c);

int raft_apply_entry(raft_server_t* me_);

void raft_send_appendentries(raft_server_t* me_, int node);

void raft_send_appendentries_all(raft_server_t* me_);
void raft_set_configuration(raft_server_t* me_,
        raft_node_configuration_t* nodes, int my_idx);

int raft_get_nvotes_for_me(raft_server_t* me_);

void raft_vote(raft_server_t* me_, int node);

raft_node_t* raft_get_node(raft_server_t *me_, int nodeid);

int raft_is_follower(raft_server_t* me_);

int raft_is_leader(raft_server_t* me_);

int raft_is_candidate(raft_server_t* me_);
#endif //RAFT_SERVER_H
