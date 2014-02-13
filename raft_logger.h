#ifndef RAFT_LOGGER_H
#define RAFT_LOGGER_H

#include "include/raft.h"

class RaftLogger {
    
    int size;              /* size of array */
    int count;             /* the amount of elements in the array */
    int front, back;       /* position of the queue */
    int base_log_idx;      /* we compact the log, and thus need to increment the base idx */ 
    raft_entry_t* entries;

    /**
     * @breif This function when called will ensure that we have enough capacity in the current logger state
     */
    void ensurecapacity();

    /**
     * @brief The default constructor
     */
    RaftLogger();

    /**
     * @brief Add entry to log.
     *        Don't add entry if we've already added this entry (based off ID)
     *        Don't add entries with ID=0 
     * @return 0 if unsucessful; 1 otherwise 
     */
    int log_append_entry(raft_entry_t* c);

    /**
     * @brief Retrieve the log entry with a give index
     */
    raft_entry_t* log_get_from_idx(int idx);

    /**
     * @brief Get the log count held (Written) by this logger
     */
    int log_count();

    /**
     * @brief Delete all logs from this log onwards 
     */
    void log_delete(int idx);

    /**
     * @brief Remove oldest entry
     * @return oldest entry 
     */
    void* log_poll();

    /*
     * @return youngest entry 
     */
    raft_entry_t *log_peektail();

    /**
     * @brief Empty the queue. 
     */
    void log_empty();

    virtual ~RaftLogger();

    void log_mark_node_has_committed(int idx);
  
};

#endif //RAFT_LOGGER_H
