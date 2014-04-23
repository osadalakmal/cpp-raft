#include "raft_logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <vector>

using namespace std;

RaftLogger::RaftLogger() : entries() {
}

int RaftLogger::log_append_entry(const raft_entry_t& c) {
    if (0 == c.d_id)
        return 0;

    entries.push_back(c);
    entries.back().d_num_nodes = 0;
    return 1;

}

raft_entry_t& RaftLogger::log_get_from_idx(int idx) {
  if (entries.empty() || idx < 0 || static_cast<size_t>(idx) > entries.size()) {
    throw std::runtime_error("No Such Log");
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

raft_entry_t& RaftLogger::log_peektail()
{
  if (!entries.empty()) {
    return entries.back();
  } else {
	throw std::runtime_error("No Such Log");
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
    raft_entry_t& e = log_get_from_idx(idx);
    e.d_num_nodes += 1;
}
