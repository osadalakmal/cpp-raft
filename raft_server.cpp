
/**
 * Copyright (c) 2013, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * @file
 * @brief Implementation of a Raft server
 * @author Willem Thiart himself@willemthiart.com
 * @version 0.1
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/* for varags */
#include <stdarg.h>

#include "state_mach.h"
#include "raft.h"
#include "raft_server.h"
#include "raft_logger.h"
#include "raft_node.h"
#include "raft_private.h"

int raft_get_commit_idx(raft_server_t *me_);
int raft_get_state(raft_server_t *me_);

static void __log(void *src, const char *fmt, ...) {
  char buf[1024];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
}

RaftServer::RaftServer() {
  this->current_term = 0;
  this->voted_for = -1;
  this->current_idx = 1;
  this->timeout_elapsed = 0;
  this->request_timeout = 200;
  this->election_timeout = 1000;
  this->last_applied_idx = 0;
  this->log = new RaftLogger();
  d_state.set(RAFT_STATE_FOLLOWER);
}

void RaftServer::set_callbacks(raft_cbs_t *funcs, void *cb_ctx) {
  memcpy(&this->cb, funcs, sizeof(raft_cbs_t));
  this->cb_ctx = cb_ctx;
}

RaftServer::~RaftServer() { delete this->log; }

void RaftServer::election_start() {

  __log(NULL, "election starting: %d %d, term: %d", this->election_timeout,
        this->timeout_elapsed, this->current_term);

  become_candidate();
}

void RaftServer::become_leader() {
  int i;

  __log(NULL, "becoming leader");

  d_state.set(RAFT_STATE_LEADER);
  this->voted_for = -1;
  for (i = 0; i < this->num_nodes; i++) {
    if (this->nodeid == i)
      continue;
    raft_node_t *p = get_node(i);
    reinterpret_cast<RaftNode *>(p)
        ->set_next_idx(get_current_idx() + 1);
    send_appendentries(i);
  }
}

void RaftServer::become_candidate() {
  int i;

  __log(NULL, "becoming candidate");

  memset(this->votes_for_me, 0, sizeof(int) * this->num_nodes);
  this->current_term += 1;
  vote(this->nodeid);
  d_state.set(RAFT_STATE_CANDIDATE);

  /* we need a random factor here to prevent simultaneous candidates */
  this->timeout_elapsed = rand() % 500;

  /* request votes from nodes */
  for (i = 0; i < this->num_nodes; i++) {
    if (this->nodeid == i)
      continue;
    send_requestvote(i);
  }
}

void RaftServer::become_follower() {

  __log(NULL, "becoming follower");

  d_state.set(RAFT_STATE_FOLLOWER);
  this->voted_for = -1;
}

int RaftServer::periodic(int msec_since_last_period) {

  __log(NULL, "periodic elapsed time: %d", this->timeout_elapsed);

  switch (d_state.get()) {
  case RAFT_STATE_FOLLOWER:
    if (this->last_applied_idx < this->commit_idx) {
      if (0 == apply_entry())
        return 0;
    }
    break;
  default:
    break;
  }

  this->timeout_elapsed += msec_since_last_period;

  if (d_state.get() == RAFT_STATE_LEADER) {
    if (this->request_timeout <= this->timeout_elapsed) {
      send_appendentries_all();
      this->timeout_elapsed = 0;
    }
  } else {
    if (this->election_timeout <= this->timeout_elapsed) {
      election_start();
    }
  }

  return 1;
}

raft_entry_t *RaftServer::get_entry_from_idx(int etyidx) {
  return this->log->log_get_from_idx(etyidx);
}

int
RaftServer::recv_appendentries_response(int node,
                                             msg_appendentries_response_t *r) {
  raft_node_t *p;

  __log(NULL, "received appendentries response from: %d", node);

  p = get_node(node);

  if (1 == r->success) {
    int i;

    for (i = r->first_idx; i <= r->current_idx; i++)
      this->log->log_mark_node_has_committed(i);

    while (1) {
      raft_entry_t *e;

      e = this->log->log_get_from_idx(this->last_applied_idx + 1);

      /* majority has this */
      if (e && this->num_nodes / 2 <= e->num_nodes) {
        if (0 == apply_entry())
          break;
      } else {
        break;
      }
    }
  } else {
    /* If AppendEntries fails because of log inconsistency:
       decrement nextIndex and retry (§5.3) */
    RaftNode *p_raftnode = reinterpret_cast<RaftNode *>(p);
    assert(0 <= p_raftnode->get_next_idx());
    // TODO does this have test coverage?
    // TODO can jump back to where node is different instead of iterating
    p_raftnode->set_next_idx(p_raftnode->get_next_idx() -
                                       1);
    send_appendentries(node);
  }

  return 1;
}

int RaftServer::recv_appendentries(const int node,
                                        msg_appendentries_t *ae) {
  msg_appendentries_response_t r;

  this->timeout_elapsed = 0;

  __log(NULL, "received appendentries from: %d", node);

  r.term = this->current_term;

  /* we've found a leader who is legitimate */
  if (d_state.is_leader() && this->current_term <= ae->term)
    become_follower();

  /* 1. Reply false if term < currentTerm (§5.1) */
  if (ae->term < this->current_term) {
    __log(NULL, "AE term is less than current term");
    r.success = 0;
    goto done;
  }

  /* not the first appendentries we've received */
  if (0 != ae->prev_log_idx) {
    raft_entry_t *e;

    if ((e = get_entry_from_idx(ae->prev_log_idx))) {
      /* 2. Reply false if log doesn’t contain an entry at prevLogIndex
         whose term matches prevLogTerm (§5.3) */
      if (e->term != ae->prev_log_term) {
        __log(NULL, "AE term doesn't match prev_idx");
        r.success = 0;
        goto done;
      }

      /* 3. If an existing entry conflicts with a new one (same index
      but different terms), delete the existing entry and all that
      follow it (§5.3) */
      raft_entry_t *e2;
      if ((e2 = get_entry_from_idx(ae->prev_log_idx + 1))) {
        this->log->log_delete(ae->prev_log_idx + 1);
      }
    } else {
      __log(NULL, "AE no log at prev_idx");
      r.success = 0;
      goto done;
      //assert(0);
    }
  }

  /* 5. If leaderCommit > commitIndex, set commitIndex =
      min(leaderCommit, last log index) */
  if (get_commit_idx() < ae->leader_commit) {
    raft_entry_t *e;

    if ((e = this->log->log_peektail())) {
      set_commit_idx(e->id < ae->leader_commit ? e->id
                                                    : ae->leader_commit);
      while (1 == apply_entry())
        ;
    }
  }

  if (d_state.is_candidate())
    become_follower();

  set_current_term(ae->term);

  int i;

  /* append all entries to log */
  for (i = 0; i < ae->n_entries; i++) {
    msg_entry_t *cmd;
    raft_entry_t *c;

    cmd = &ae->entries[i];

    /* TODO: replace malloc with mempoll/arena */
    c = malloc(sizeof(raft_entry_t));
    c->term = this->current_term;
    c->len = cmd->len;
    c->id = cmd->id;
    c->data = malloc(cmd->len);
    memcpy(c->data, cmd->data, cmd->len);
    if (0 == append_entry(c)) {
      __log(NULL, "AE failure; couldn't append entry");
      r.success = 0;
      goto done;
    }
  }

  r.success = 1;
  r.current_idx = get_current_idx();
  r.first_idx = ae->prev_log_idx + 1;

done:
  if (this->cb.send)
    this->cb.send(this->cb_ctx, this, node, RAFT_MSG_APPENDENTRIES_RESPONSE,
                  (void *)&r, sizeof(msg_appendentries_response_t));
  return 1;
}

int RaftServer::recv_requestvote(int node, msg_requestvote_t *vr) {
  msg_requestvote_response_t r;

  if (get_current_term() < vr->term) {
    this->voted_for = -1;
  }

  if (vr->term < get_current_term() || /* we've already voted */
      -1 != this->voted_for ||              /* we have a more up-to-date log */
      vr->last_log_idx < this->current_idx) {
    r.vote_granted = 0;
  } else {
    vote(node);
    r.vote_granted = 1;
  }

  __log(NULL, "node requested vote: %d replying: %s", node,
        r.vote_granted == 1 ? "granted" : "not granted");

  r.term = get_current_term();
  if (this->cb.send)
    this->cb.send(this->cb_ctx, this, node, RAFT_MSG_REQUESTVOTE_RESPONSE,
                  (void *)&r, sizeof(msg_requestvote_response_t));

  return 0;
}

int RaftServer::recv_requestvote_response(int node,
                                               msg_requestvote_response_t *r) {

  __log(NULL, "node responded to requestvote: %d status: %s", node,
        r->vote_granted == 1 ? "granted" : "not granted");

  if (d_state.is_leader())
    return 0;

  assert(node < this->num_nodes);

  //    if (r->term != get_current_term())
  //        return 0;

  if (1 == r->vote_granted) {
    int votes;

    this->votes_for_me[node] = 1;
    votes = get_nvotes_for_me();
    if (raft_votes_is_majority(this->num_nodes, votes))
      become_leader();
  }

  return 0;
}

int RaftServer::send_entry_response(int node, int etyid,
                                         int was_committed) {
  msg_entry_response_t res;

  __log(NULL, "send entry response to: %d", node);

  res.id = etyid;
  res.was_committed = was_committed;
  if (this->cb.send)
    this->cb.send(this->cb_ctx, this, node, RAFT_MSG_ENTRY_RESPONSE,
                  (void *)&res, sizeof(msg_entry_response_t));
  return 0;
}

int RaftServer::recv_entry(int node, msg_entry_t *e) {
  raft_entry_t ety;
  int res, i;

  __log(NULL, "received entry from: %d", node);

  ety.term = this->current_term;
  ety.id = e->id;
  ety.data = e->data;
  ety.len = e->len;
  res = append_entry(&ety);
  send_entry_response(node, e->id, res);
  for (i = 0; i < this->num_nodes; i++) {
    if (this->nodeid == i)
      continue;
    send_appendentries(i);
  }
  return 0;
}

int RaftServer::send_requestvote(int node) {
  msg_requestvote_t rv;

  __log(NULL, "sending requestvote to: %d", node);

  rv.term = this->current_term;
  rv.last_log_idx = get_current_idx();
  if (this->cb.send)
    this->cb.send(this->cb_ctx, this, node, RAFT_MSG_REQUESTVOTE, (void *)&rv,
                  sizeof(msg_requestvote_t));
  return 1;
}

int RaftServer::append_entry(raft_entry_t *c) {

  if (1 == this->log->log_append_entry(c)) {
    this->current_idx += 1;
    return 1;
  }
  return 0;
}

int RaftServer::apply_entry() {
  raft_entry_t *e;

  if (!(e = this->log->log_get_from_idx(this->last_applied_idx + 1)))
    return 0;

  __log(NULL, "applying log: %d", this->last_applied_idx);

  this->last_applied_idx++;
  if (this->commit_idx < this->last_applied_idx)
    this->commit_idx = this->last_applied_idx;
  if (this->cb.applylog)
    this->cb.applylog(this->cb_ctx, this, e->data, e->len);
  return 1;
}

void RaftServer::send_appendentries(int node) {

  __log(NULL, "sending appendentries to: %d", node);

  if (!(this->cb.send))
    return;

  msg_appendentries_t ae;
  raft_node_t *p = get_node(node);

  ae.term = this->current_term;
  ae.leader_id = this->nodeid;
  ae.prev_log_term = reinterpret_cast<RaftNode *>(p)->get_next_idx();
  // TODO:
  ae.prev_log_idx = 0;
  ae.n_entries = 0;
  this->cb.send(this->cb_ctx, this, node, RAFT_MSG_APPENDENTRIES, (void *)&ae,
                sizeof(msg_appendentries_t));
}

void RaftServer::send_appendentries_all() {
  int i;

  for (i = 0; i < this->num_nodes; i++) {
    if (this->nodeid == i)
      continue;
    send_appendentries(i);
  }
}

void RaftServer::set_configuration(raft_node_configuration_t *nodes,
                                        int my_idx) {
  int num_nodes;

  /* TODO: one memory allocation only please */
  for (num_nodes = 0; nodes->udata_address; nodes++) {
    num_nodes++;
    this->nodes = realloc(this->nodes, sizeof(raft_node_t *) * num_nodes);
    this->num_nodes = num_nodes;
    this->nodes[num_nodes - 1] = new RaftNode(nodes->udata_address);
  }
  this->votes_for_me = calloc(num_nodes, sizeof(int));
  this->nodeid = my_idx;
}

int RaftServer::get_nvotes_for_me() {
  int i, votes;

  for (i = 0, votes = 0; i < this->num_nodes; i++) {
    if (this->nodeid == i)
      continue;
    if (1 == this->votes_for_me[i])
      votes += 1;
  }

  if (this->voted_for == this->nodeid)
    votes += 1;

  return votes;
}

void RaftServer::vote(int node) { this->voted_for = node; }

raft_node_t *RaftServer::get_node(int nodeid) {
  if (nodeid < 0 || this->num_nodes <= nodeid)
    return NULL;
  return this->nodes[nodeid];
}

void RaftServer::set_election_timeout(int millisec) {
  this->election_timeout = millisec;
}

void RaftServer::set_request_timeout(int millisec) {
  this->request_timeout = millisec;
}

int RaftServer::get_nodeid() { return this->nodeid; }

int RaftServer::get_election_timeout() { return this->election_timeout; }

int RaftServer::get_request_timeout() { return this->request_timeout; }

int RaftServer::get_num_nodes() { return this->num_nodes; }

int RaftServer::get_timeout_elapsed() { return this->timeout_elapsed; }

int RaftServer::get_log_count() { return this->log->log_count(); }

int RaftServer::get_voted_for() { return this->voted_for; }

void RaftServer::set_current_term(int term) { this->current_term = term; }

int RaftServer::get_current_term() { return this->current_term; }

void RaftServer::set_current_idx(int idx) { this->current_idx = idx; }

int RaftServer::get_current_idx() { return this->current_idx; }

int RaftServer::get_my_id() { return this->nodeid; }

void RaftServer::set_commit_idx(int idx) { this->commit_idx = idx; }

void RaftServer::set_last_applied_idx(int idx) { this->last_applied_idx = idx;}

int RaftServer::get_last_applied_idx() { return this->last_applied_idx; }

int RaftServer::get_commit_idx() { return this->commit_idx; }

/*--------------------------------------------------------------79-characters-*/
