#ifndef INCLUDED_RAFT_NODE_H
#define INCLUDED_RAFT_NODE_H

class RaftNode {

  void* d_udata;
  int next_idx;

public:

  RaftNode(void* udata);
  int is_leader();
  int get_next_idx();
  void set_next_idx(int nextIdx);
  void* get_udata();

};

#endif  //INCLUDED_RAFT_NODE_H
