#include "raft_logger.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

RaftLogger::RaftLogger() : entries() {
}

int RaftLogger::log_append_entry(raft_entry_t* c) {
    if (0 == c->id)
        return 0;

    entries.push_back(c);
    entries.back()->num_nodes = 0;
    return 1;

}

raft_entry_t* RaftLogger::log_get_from_idx(int idx) {
  if (entries.empty() || idx < 0 || static_cast<size_t>(idx) > entries.size()) {
    return NULL;
  } else {
    return entries[idx-1];
  }
}

int RaftLogger::log_count()
{
    return entries.size();
}

void RaftLogger::log_delete(int idx)
{
  entries.erase(entries.begin() + idx - 1, entries.end());
}

/**
 * Remove oldest entry
 * @return oldest entry */
void *RaftLogger::log_poll()
{
  if (entries.empty()) {
    return NULL;
  } else {
    void* retVal = (void *) entries.back();
    entries.pop_back();
    return retVal;
  }
}

/*
 * @return youngest entry */
raft_entry_t *RaftLogger::log_peektail()
{
  if (!entries.empty()) {
    return entries.back();
  } else {
    return NULL;
  }
}

void RaftLogger::log_empty()
{
  entries.clear();
}

RaftLogger::~RaftLogger()
{
}

void RaftLogger::log_mark_node_has_committed(int idx)
{
    raft_entry_t* e;

    if ((e = log_get_from_idx(idx)))
    {
        e->num_nodes += 1;
    }
}
