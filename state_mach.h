#ifndef INCLUDED_STATE_MACH
#define INCLUDED_STATE_MACH

enum RAFT_STATE {
    RAFT_STATE_NONE,
    RAFT_STATE_FOLLOWER,
    RAFT_STATE_CANDIDATE,
    RAFT_STATE_LEADER
};

namespace Raft {

class State {

  RAFT_STATE d_state;

  public:

  /**
   * @brief This creates a defualt constrcuted state object.
   *        The default state is NONE
   */
  State();

  /**
   * @brief The default destructor
   */
  virtual ~State();

  /**
   * @brief Return true iff state is leader
   */
  bool is_leader();

  /**
   * @brief Returns true iff state is follower
   */
  bool is_follower();

  /**
   * @brief Returns true iff state is candidate
   */
  bool is_candidate();

  /**
   * @brief Sets the state of this object
   */
  void set(RAFT_STATE state);

  /**
   * @brief Gets the current state of the object
   */
  RAFT_STATE get();

};

} //namespace Raft

#endif
