#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include "state_mach.h"
#include "raft.h"
#include "raft_msg.h"
#include <cstddef>
#include <vector>

class RaftLogger;
class RaftNode;
typedef std::vector<RaftNode>::iterator NodeIter;

class RaftServer {

  /* Persistent state: */
  ///the server's best guess of what the current term is starts at zero
  int current_term;

  /// The candidate the server voted for in its current term, or Nil if it hasn't voted for any.
  int voted_for; 

  /// the log which is replicated
  RaftLogger* log; 

  /* Volatile state: */
  /// idx of highest log entry known to be committed
  int commit_idx; 

  /// idx of highest log entry applied to state machine
  int last_applied_idx; 


  /// follower/leader/candidate indicator
  Raft::State d_state;

  /// most recently append idx, also indicates size of log
  int current_idx;

  /// amount of time left till timeout
  int timeout_elapsed;

  /// who has voted for me. This is an array with N = 'num_nodes' elements
  std::vector<int> votes_for_me;

  std::vector<RaftNode> nodes;

  int election_timeout;
  int request_timeout;

  /// callbacks
  raft_cbs_t cb;
  void* cb_ctx;

  /// my node ID
  size_t nodeid;
  public:

  /**
   * Initialise a new raft server
   *
   * Request timeout defaults to 200 milliseconds
   * Election timeout defaults to 1000 milliseconds
   * @return newly initialised raft server */
  RaftServer();

  virtual ~RaftServer();

  /**
   * Set callbacks
   * @param funcs Callbacks
   * @param cb_ctx The context that we include with all callbacks */
  void set_callbacks(raft_cbs_t* funcs, void* cb_ctx);

  void election_start();

  void become_leader();

  void become_candidate();
  void become_follower();

  /**
   * Run actions that are dependent on time passing
   * @return 0 on error */
  int periodic( int msec_since_last_period);

  /**
   * @param idx The entry's index
   * @return entry from index */
  raft_entry_t* get_entry_from_idx( int etyidx);

  /**
   * Receive a response from an appendentries message we sent
   * @param node Who sent us the response
   * @param r The appendentries response 
   * @return 0 on error */
  int recv_appendentries_response(int node, msg_appendentries_response_t* r);
  /**
   * Receive an appendentries message
   * @param node Who sent us the response
   * @param ae The appendentries message 
   * @return 0 on error */
  int recv_appendentries(
      const int node,
      msg_appendentries_t* ae);
  /**
   * Receive a requestvote message
   * @param node Who sent us the message
   * @param vr The requestvote message
   * @return 0 on error */
  int recv_requestvote( int node, msg_requestvote_t* vr);

  /**
   * Receive a response from a requestvote message we sent
   * @param node Who sent us the response
   * @param r The requestvote response 
   * @param node The node this response was sent by */
  int recv_requestvote_response( int node,
      msg_requestvote_response_t* r);

  int send_entry_response(        int node, int etyid, int was_committed);

  /**
   * Receive an entry message from client.
   * Append the entry to the log
   * Send appendentries to followers 
   * @param node The node this response was sent by
   * @param e The entry message */
  int recv_entry( int node, msg_entry_t* e);

  int send_requestvote( int node);

  int append_entry( raft_entry_t* c);

  int apply_entry();

  void send_appendentries( int node);

  void send_appendentries_all();
  /**
   * Set configuration
   * @param nodes Array of nodes, end of array is marked by NULL entry
   * @param my_idx Which node is myself */
  void set_configuration(        raft_node_configuration_t* nodes, int my_idx);

  /**
   * @return number of votes this server has received this election */
  int get_nvotes_for_me();

  void vote( int node);

  /**
   * @param node The node's index
   * @return node pointed to by node index
   */
  NodeIter get_node(size_t nodeid);

  /**
   * @return 1 if follower; 0 otherwise */
  int is_follower();

  /**
   * @return 1 if node is leader; 0 otherwise */
  int is_leader();

  /**
   * @return 1 if candidate; 0 otherwise */
  int is_candidate();

  /**
   * Set election timeout
   * @param millisec Election timeout in milliseconds */
  void set_election_timeout( int millisec);

  /**
   * Set request timeout in milliseconds
   * @param millisec Request timeout in milliseconds */
  void set_request_timeout( int millisec);

  /**
   * @return the server's node ID */
  int get_nodeid();

  /**
   * @return currently configured election timeout in milliseconds */
  int get_election_timeout();

  int get_request_timeout();

  /**
   * @return number of nodes that this server has */
  size_t get_num_nodes();

  /**
   * @return currently elapsed timeout in milliseconds */
  int get_timeout_elapsed();

  /**
   * @return number of items within log */
  int get_log_count();

  /**
   * @return node ID of who I voted for */
  int get_voted_for();

  void set_current_term( int term);

  /**
   * @return current term */
  int get_current_term();

  void set_current_idx( int idx);

  /**
   * @return current log index */
  int get_current_idx();

  int get_my_id();

  void set_commit_idx( int idx);

  void set_last_applied_idx( int idx);

  /**
   * @return index of last applied entry */
  int get_last_applied_idx();

  int get_commit_idx();

  inline Raft::State& get_state() { return d_state; };

  inline NodeIter get_last_node() { return nodes.end(); }

};

inline int raft_votes_is_majority(const int num_nodes, const int nvotes) {
  int half;

  if (num_nodes < nvotes)
    return 0;
  half = num_nodes / 2;
  return half + 1 <= nvotes;
}

#endif //RAFT_SERVER_H
