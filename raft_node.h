#ifndef INCLUDED_RAFT_NODE_H
#define INCLUDED_RAFT_NODE_H

#include <boost/any.hpp>

class RaftNode {

  int next_idx;
  boost::any d_udata;

public:

  RaftNode(const boost::any& udata);
  int is_leader();
  int get_next_idx();
  void set_next_idx(int nextIdx);
  boost::any& get_udata();

};

#endif  //INCLUDED_RAFT_NODE_H
