#include "raft_logger.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

RaftLogger::RaftLogger() {
  //@TODO Contstantize this
    size = 10; //INITIAL_CAPACITY;
    count = 0;
    back = front = 0;
    entries = reinterpret_cast<raft_entry_t*>(calloc(1,sizeof(raft_entry_t) * size));
}

void RaftLogger::ensurecapacity() {
    int i, j;
    raft_entry_t *temp;

    if (count < size)
        return;

    temp = reinterpret_cast<raft_entry_t*>(calloc(1,sizeof(raft_entry_t) * size * 2));

    for (i = 0, j = front; i < count; i++, j++)
    {
        if (j == size)
            j = 0;
        memcpy(&temp[i], &entries[j], sizeof(raft_entry_t));
    }

    size *= 2;
    entries = temp;
    front = 0;
    back = count;

    /* clean up old entries */
    free(entries);
}

int RaftLogger::log_append_entry(raft_entry_t* c) {
    if (0 == c->id)
        return 0;

    ensurecapacity();

    memcpy(&entries[back],c,sizeof(raft_entry_t));
    entries[back].num_nodes = 0;
    count++;
    back++;
    return 1;

}

raft_entry_t* RaftLogger::log_get_from_idx(int idx) {
    int i;

    if (base_log_idx + count < idx || idx < base_log_idx)
        return NULL;

    /* idx starts at 1 */
    idx -= 1;
    i = (front + idx - base_log_idx) % size;
    return &entries[i];
}

int RaftLogger::log_count()
{
    return count;
}

void RaftLogger::log_delete(int idx)
{
    int end;

    /* idx starts at 1 */
    idx -= 1;
    idx -= base_log_idx;

    for (end = log_count(); idx<end; idx++)
    {
        back--;
        count--;
    }

#if 0
    const void *elem;

    if (arrayqueue_is_empty(me))
        return NULL;

//    __checkwrapping(me);
    in(me)->back--;
    in(me)->count--;
    if (-1 == in(me)->back)
        in(me)->back = in(me)->size;
    elem = entries[in(me)->back];

    return (void *) elem;
#endif
}

/**
 * Remove oldest entry
 * @return oldest entry */
void *RaftLogger::log_poll()
{
    const void *elem;

    if (0 == log_count())
        return NULL;
    elem = &entries[front];
    front++;
    count--;
    base_log_idx++;
    return (void *) elem;
}

/*
 * @return youngest entry */
raft_entry_t *RaftLogger::log_peektail()
{
    if (0 == log_count())
        return NULL;

    if (0 == back)
        return &entries[size-1];
    else
        return &entries[back-1];
}

void RaftLogger::log_empty()
{
    front = 0;
    back = 0;
    count = 0;
}

RaftLogger::~RaftLogger()
{
    free(entries);
}

void RaftLogger::log_mark_node_has_committed(int idx)
{
    raft_entry_t* e;

    if ((e = log_get_from_idx(idx)))
    {
        e->num_nodes += 1;
    }
}
