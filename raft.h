#ifndef INCLUDED_RAFT_H
#define INCLUDED_RAFT_H

#pragma GCC diagnostic ignored "-fpermissive"

/**
 * Copyright (c) 2013, Willem-Hendrik Thiart
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file. 
 *
 * @file
 * @author Willem Thiart himself@willemthiart.com
 * @version 0.1
 */

typedef struct {
    /** The ID that this node used to have.
     * So that we can tell which nodes were removed/added when the
     * configuration changes */
    int old_id;

    /** User data pointer for addressing.
     * Examples of what this could be:
     * - void* pointing to implementor's networking data
     * - a (IP,Port) tuple */
    void* udata_address;
} raft_node_configuration_t;

typedef struct {
    /* candidate's term */
    int term;

    /* candidate requesting vote */
    int candidate_id;

    /* idx of candidate's last log entry */
    int last_log_idx;

    /* term of candidate's last log entry */
    int last_log_term;
} msg_requestvote_t;

typedef struct {
    /* the entry's unique ID */
    unsigned int id;

    /* entry data */
    unsigned char* data;

    /* length of entry data */
    unsigned int len;
} msg_entry_t;

typedef struct {
    /* the entry's unique ID */
    unsigned int id;

    /* whether or not the entry was committed */
    int was_committed;
} msg_entry_response_t;

typedef struct {
    /* currentTerm, for candidate to update itself */
    int term;

    /* true means candidate received vote */
    int vote_granted;
} msg_requestvote_response_t;

typedef struct {
    int term;
    int leader_id;
    int prev_log_idx;
    int prev_log_term;
    int n_entries;
    msg_entry_t* entries;
    int leader_commit;
} msg_appendentries_t;

typedef struct {
    /* currentTerm, for leader to update itself */
    int term;

    /* success true if follower contained entry matching
     * prevLogidx and prevLogTerm */
    int success;

    /* Non-Raft fields follow: */
    /* Having the following fields allows us to do less book keeping in
     * regards to full fledged RPC */
    /* This is the highest log IDX we've received and appended to our log */
    int current_idx;
    /* The first idx that we received within the appendentries message */
    int first_idx;
} msg_appendentries_response_t;

enum {
    RAFT_MSG_REQUESTVOTE,
    RAFT_MSG_REQUESTVOTE_RESPONSE,
    RAFT_MSG_APPENDENTRIES,
    RAFT_MSG_APPENDENTRIES_RESPONSE,
    RAFT_MSG_ENTRY,
    RAFT_MSG_ENTRY_RESPONSE,
};

typedef int (
    *func_send_f
)   (
    void *cb_ctx,
    void *udata,
    int node,
    int msg_type,
    const unsigned char *send_data,
    const int len
);

#ifndef HAVE_FUNC_LOG
#define HAVE_FUNC_LOG
typedef void (
    *func_log_f
)    (
    void *cb_ctx,
    void *src,
    const char *buf,
    ...
);
#endif

/**
 * Apply this log to the state macine */
typedef int (
    *func_applylog_f
)   (
    void *cb_ctx,
    void *udata,
    const unsigned char *data,
    const int len
);

typedef struct {
    func_send_f send;
    func_log_f log;
    func_applylog_f applylog;
} raft_cbs_t;

typedef void* raft_server_t;
typedef void* raft_node_t;

typedef struct {
    /* entry's term */
    unsigned int term;
    /* the entry's unique ID */
    unsigned int id;
    /* entry data */
    unsigned char* data;
    /* length of entry data */
    unsigned int len;
    /* number of nodes that have this entry */
    unsigned int num_nodes;
} raft_entry_t;


#endif //INCLUDED_RAFT_H

