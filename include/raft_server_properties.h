#ifndef RAFT_SERVER_PROPERTIES_H
#define RAFT_SERVER_PROPERTIES_H

#include "raft.h"

void raft_set_election_timeout(raft_server_t* me_, int millisec);

void raft_set_request_timeout(raft_server_t* me_, int millisec);

int raft_get_nodeid(raft_server_t* me_);

int raft_get_election_timeout(raft_server_t* me_);

int raft_get_request_timeout(raft_server_t* me_);

int raft_get_num_nodes(raft_server_t* me_);

int raft_get_timeout_elapsed(raft_server_t* me_);

int raft_get_log_count(raft_server_t* me_);

int raft_get_voted_for(raft_server_t* me_);

void raft_set_current_term(raft_server_t* me_, int term);

int raft_get_current_term(raft_server_t* me_);

void raft_set_current_idx(raft_server_t* me_, int idx);

int raft_get_current_idx(raft_server_t* me_);

int raft_get_my_id(raft_server_t* me_);

void raft_set_commit_idx(raft_server_t* me_, int idx);

void raft_set_last_applied_idx(raft_server_t* me_, int idx);

int raft_get_last_applied_idx(raft_server_t* me_);

int raft_get_commit_idx(raft_server_t* me_);

void raft_set_state(raft_server_t* me_, int state);

int raft_get_state(raft_server_t* me_);
#endif //RAFT_SERVER_PROPERTIES_H
