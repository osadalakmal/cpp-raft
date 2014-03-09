#ifndef INCLUDED_RAFT_H
#define INCLUDED_RAFT_H

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
    int* udata_address;
} raft_node_configuration_t;


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
typedef int* raft_node_t;

struct raft_entry_t {
    /* entry's term */
    unsigned int term;
    /* the entry's unique ID */
    unsigned int id;
    /* entry data */
    char* data;
    /* length of entry data */
    unsigned int len;
    /* number of nodes that have this entry */
    unsigned int num_nodes;
};


#endif //INCLUDED_RAFT_H

