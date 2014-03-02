#include "state_mach.h"

using namespace Raft;

State::State() {
}

State::~State() {
}

bool State::is_follower() {
  return d_state == RAFT_STATE_FOLLOWER;
}

bool State::is_leader() {
  return d_state == RAFT_STATE_LEADER;
}

bool State::is_candidate() {
  return d_state == RAFT_STATE_CANDIDATE;
}

void State::set(RAFT_STATE state) { d_state = state; }

RAFT_STATE State::get() { return d_state; }

