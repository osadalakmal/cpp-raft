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
    const int d_len
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
    const unsigned char *d_data,
    const int d_len
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
    unsigned int d_term;
    /* the entry's unique ID */
    unsigned int d_id;
    /* entry d_data */
    char* d_data;
    /* length of entry d_data */
    unsigned int d_len;
    /* number of nodes that have this entry */
    unsigned int d_num_nodes;

    raft_entry_t() : d_term(0), d_id(0), d_data(0), d_len(0), d_num_nodes(0) {
    }

    raft_entry_t(unsigned int term, unsigned int id, char* data, unsigned int len, unsigned int num_nodes = 0) :
    	d_term(term), d_id(id), d_data(data), d_len(len), d_num_nodes(num_nodes) {
	}

	char* getData() const {
		return d_data;
	}

	void setData(char* data) {
		this->d_data = data;
	}

	unsigned int getId() const {
		return d_id;
	}

	void setId(unsigned int id) {
		this->d_id = id;
	}

	unsigned int getLen() const {
		return d_len;
	}

	void setLen(unsigned int len) {
		this->d_len = len;
	}

	unsigned int getNumNodes() const {
		return d_num_nodes;
	}

	void setNumNodes(unsigned int numNodes) {
		d_num_nodes = numNodes;
	}

	unsigned int getTerm() const {
		return d_term;
	}

	void setTerm(unsigned int term) {
		this->d_term = term;
	}
};


#endif //INCLUDED_RAFT_H

