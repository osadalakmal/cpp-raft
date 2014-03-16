#ifndef RAFT_MSG_INCLUDED_H
#define RAFT_MSG_INCLUDED_H

class msg_requestvote_t {
    /* candidate's term */
    int d_term;

    /* candidate requesting vote */
    int d_candidate_id;

    /* idx of candidate's last log entry */
    int d_last_log_idx;

    /* term of candidate's last log entry */
    int d_last_log_term;

public:

    msg_requestvote_t(int term, int candidate_id, int last_log_idx, int last_log_term):
      d_term(term), d_candidate_id(candidate_id), d_last_log_idx(last_log_idx), d_last_log_term(last_log_term) {}

    inline int term() { return d_term; }
    inline int candidate_id() { return d_candidate_id; }
    inline int last_log_idx() { return d_last_log_idx; }
    inline int last_log_term() { return d_last_log_term; }

    inline void term(int term) { d_term = term; }
    inline void candidate_id(int candidate_id) { d_candidate_id = candidate_id; }
    inline void last_log_idx(int last_log_idx) { d_last_log_idx = last_log_idx; }
    inline void last_log_term(int last_log_term) { d_last_log_term = last_log_term; }

};

class msg_entry_t {
    /* the entry's unique ID */
    unsigned int d_id;

    /* entry data */
    unsigned char* d_data;

    /* length of entry data */
    unsigned int d_len;

public:

    msg_entry_t(): d_id(0), d_data(NULL), d_len(0) { }

    msg_entry_t(unsigned int id, unsigned char* data, unsigned int len):
    	d_id(id), d_data(data), d_len(len) { }

	unsigned char* data() const {
		return d_data;
	}

	void data(unsigned char* data) {
		this->d_data = data;
	}

	unsigned int id() const {
		return d_id;
	}

	void id(unsigned int id) {
		this->d_id = id;
	}

	unsigned int len() const {
		return d_len;
	}

	void len(unsigned int len) {
		this->d_len = len;
	}
};

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

class msg_appendentries_t {
    int d_term;
    int d_leader_id;
    int d_prev_log_idx;
    int d_prev_log_term;
    int d_n_entries;
    msg_entry_t* d_entries;
    int d_leader_commit;

public:

    msg_appendentries_t() : d_term(0), d_leader_id(0), d_prev_log_idx(0),
    	d_prev_log_term(0), d_n_entries(0), d_entries(NULL), d_leader_commit(0) {

    }

    msg_appendentries_t(int term, int leader_id, int prev_log_idx,
    		int prev_log_term, int n_entries, msg_entry_t* entries,
    		int leader_commit) : d_term(term), d_leader_id(leader_id), d_prev_log_idx(prev_log_idx),
        	d_prev_log_term(prev_log_term), d_n_entries(n_entries), d_entries(entries),
        	d_leader_commit(leader_commit) {

	}

	const msg_entry_t* getEntries() const {
		return d_entries;
	}

	void setEntries(const msg_entry_t*& entries) {
		d_entries = entries;
	}

	int getLeaderCommit() const {
		return d_leader_commit;
	}

	void setLeaderCommit(int leaderCommit) {
		d_leader_commit = leaderCommit;
	}

	int getLeaderId() const {
		return d_leader_id;
	}

	void setLeaderId(int leaderId) {
		d_leader_id = leaderId;
	}

	int getNEntries() const {
		return d_n_entries;
	}

	void setNEntries(int nEntries) {
		d_n_entries = nEntries;
	}

	int getPrevLogIdx() const {
		return d_prev_log_idx;
	}

	void setPrevLogIdx(int prevLogIdx) {
		d_prev_log_idx = prevLogIdx;
	}

	int getPrevLogTerm() const {
		return d_prev_log_term;
	}

	void setPrevLogTerm(int prevLogTerm) {
		d_prev_log_term = prevLogTerm;
	}

	int getTerm() const {
		return d_term;
	}

	void setTerm(int term) {
		d_term = term;
	}
};

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

#endif
