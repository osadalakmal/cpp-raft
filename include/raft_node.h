#ifndef INCLUDED_RAFT_NODE_H
#define INCLUDED_RAFT_NODE_H

class RaftNode {

  void* d_udata;
  int next_idx;

public:

  RaftNode(void* udata);
  int raft_node_is_leader();
  int raft_node_get_next_idx();
  void raft_node_set_next_idx(int nextIdx);
  void* raft_node_get_udata();

};

#endif  //INCLUDED_RAFT_NODE_H
