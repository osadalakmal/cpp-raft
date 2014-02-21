#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include "raft.h"

class RaftLogger;

class RaftServer {

  /* Persistent state: */
  int current_term; //the server's best guess of what the current term is starts at zero

  int voted_for; // The candidate the server voted for in its current term, or Nil if it hasn't voted for any.

  RaftLogger* log; /* the log which is replicated */

  /* Volatile state: */

  int commit_idx; // idx of highest log entry known to be committed

  int last_applied_idx; // idx of highest log entry applied to state machine


  /* follower/leader/candidate indicator */
  int state;

  /* most recently append idx, also indicates size of log */
  int current_idx;

  /* amount of time left till timeout */
  int timeout_elapsed;

  /* who has voted for me. This is an array with N = 'num_nodes' elements */
  int *votes_for_me;

  raft_node_t* nodes;
  int num_nodes;

  int election_timeout;
  int request_timeout;

  /* callbacks */
  raft_cbs_t cb;
  void* cb_ctx;

  /* my node ID */
  int nodeid;
  public:
  RaftServer();

  virtual ~RaftServer();

  void raft_set_callbacks(raft_cbs_t* funcs, void* cb_ctx);

  void raft_election_start();

  void raft_become_leader();

  void raft_become_candidate();
  void raft_become_follower();

  int raft_periodic( int msec_since_last_period);
  raft_entry_t* raft_get_entry_from_idx( int etyidx);

  int raft_recv_appendentries_response(int node, msg_appendentries_response_t* r);
  int raft_recv_appendentries(
      const int node,
      msg_appendentries_t* ae);
  int raft_recv_requestvote( int node, msg_requestvote_t* vr);

  int raft_recv_requestvote_response( int node,
      msg_requestvote_response_t* r);

  int raft_send_entry_response(        int node, int etyid, int was_committed);

  int raft_recv_entry( int node, msg_entry_t* e);

  int raft_send_requestvote( int node);

  int raft_append_entry( raft_entry_t* c);

  int raft_apply_entry();

  void raft_send_appendentries( int node);

  void raft_send_appendentries_all();
  void raft_set_configuration(        raft_node_configuration_t* nodes, int my_idx);

  int raft_get_nvotes_for_me();

  void raft_vote( int node);

  raft_node_t* raft_get_node(int nodeid);

  int raft_is_follower();

  int raft_is_leader();

  int raft_is_candidate();

  void raft_set_election_timeout( int millisec);

  void raft_set_request_timeout( int millisec);

  int raft_get_nodeid();

  int raft_get_election_timeout();

  int raft_get_request_timeout();

  int raft_get_num_nodes();

  int raft_get_timeout_elapsed();

  int raft_get_log_count();

  int raft_get_voted_for();

  void raft_set_current_term( int term);

  int raft_get_current_term();

  void raft_set_current_idx( int idx);

  int raft_get_current_idx();

  int raft_get_my_id();

  void raft_set_commit_idx( int idx);

  void raft_set_last_applied_idx( int idx);

  int raft_get_last_applied_idx();

  int raft_get_commit_idx();

  void raft_set_state( int state);

  int raft_get_state();

};

inline int raft_votes_is_majority(const int num_nodes, const int nvotes) {
  int half;

  if (num_nodes < nvotes)
    return 0;
  half = num_nodes / 2;
  return half + 1 <= nvotes;
}

#endif //RAFT_SERVER_H
