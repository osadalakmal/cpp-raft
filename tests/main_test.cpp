

/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include "CuTest.h"


extern void TestLog_new_is_empty(CuTest*);
extern void TestLog_append_is_not_empty(CuTest*);
extern void TestLog_get_at_idx(CuTest*);
extern void TestLog_get_at_idx_returns_null_where_out_of_bounds(CuTest*);
extern void TestLog_mark_node_has_committed_adds_nodes(CuTest*);
extern void TestLog_delete(CuTest*);
extern void TestLog_delete_onwards(CuTest*);
extern void TestLog_peektail(CuTest*);
extern void TestRaft_node_set_nextIdx(CuTest*);
extern void TestRaft_scenario_leader_appears(CuTest*);
extern void TestRaft_server_voted_for_records_who_we_voted_for(CuTest*);
extern void TestRaft_server_idx_starts_at_1(CuTest*);
extern void TestRaft_server_currentterm_defaults_to_0(CuTest*);
extern void TestRaft_server_set_currentterm_sets_term(CuTest*);
extern void TestRaft_server_voting_results_in_voting(CuTest*);
extern void TestRaft_election_start_increments_term(CuTest*);
extern void TestRaft_set_state(CuTest*);
extern void TestRaft_server_starts_as_follower(CuTest*);
extern void TestRaft_server_starts_with_election_timeout_of_1000ms(CuTest*);
extern void TestRaft_server_starts_with_request_timeout_of_200ms(CuTest*);
extern void TestRaft_server_entry_append_cant_append_if_id_is_zero(CuTest*);
extern void TestRaft_server_entry_append_increases_logidx(CuTest*);
extern void TestRaft_server_append_entry_means_entry_gets_current_term(CuTest*);
extern void TestRaft_server_entry_is_retrieveable_using_idx(CuTest*);
extern void TestRaft_server_wont_apply_entry_if_we_dont_have_entry_to_apply(CuTest*);
extern void TestRaft_server_increment_lastApplied_when_lastApplied_lt_commitidx(CuTest*);
extern void TestRaft_server_apply_entry_increments_last_applied_idx(CuTest*);
extern void TestRaft_server_periodic_elapses_election_timeout(CuTest*);
extern void TestRaft_server_election_timeout_sets_to_zero_when_elapsed_time_greater_than_timeout(CuTest*);
extern void TestRaft_server_cfg_sets_num_nodes(CuTest*);
extern void TestRaft_server_cant_get_node_we_dont_have(CuTest*);
extern void TestRaft_votes_are_majority_is_true(CuTest*);
extern void TestRaft_server_dont_increase_votes_for_me_when_receive_request_vote_response_is_not_granted(CuTest*);
extern void TestRaft_server_increase_votes_for_me_when_receive_request_vote_response(CuTest*);
extern void TestRaft_server_recv_requestvote_reply_false_if_term_less_than_current_term(CuTest*);
extern void TestRaft_server_dont_grant_vote_if_we_didnt_vote_for_this_candidate(CuTest*);
extern void TestRaft_follower_becomes_follower_is_follower(CuTest*);
extern void TestRaft_follower_becomes_follower_clears_voted_for(CuTest*);
extern void TestRaft_follower_recv_appendentries_reply_false_if_term_less_than_currentterm(CuTest*);
extern void TestRaft_follower_recv_appendentries_updates_currentterm_if_term_gt_currentterm(CuTest*);
extern void TestRaft_follower_doesnt_log_after_appendentry_if_no_entries_are_specified(CuTest*);
extern void TestRaft_follower_increases_log_after_appendentry(CuTest*);
extern void TestRaft_follower_recv_appendentries_reply_false_if_doesnt_have_log_at_prev_log_idx_which_matches_prev_log_term(CuTest*);
extern void TestRaft_follower_recv_appendentries_delete_entries_if_conflict_with_new_entries(CuTest*);
extern void TestRaft_follower_recv_appendentries_add_new_entries_not_already_in_log(CuTest*);
extern void TestRaft_follower_recv_appendentries_set_commitidx_to_prevLogIdx(CuTest*);
extern void TestRaft_follower_recv_appendentries_set_commitidx_to_LeaderCommit(CuTest*);
extern void TestRaft_follower_becomes_candidate_when_election_timeout_occurs(CuTest*);
extern void TestRaft_follower_dont_grant_vote_if_candidate_has_a_less_complete_log(CuTest*);
extern void TestRaft_candidate_becomes_candidate_is_candidate(CuTest*);
extern void TestRaft_follower_becoming_candidate_increments_current_term(CuTest*);
extern void TestRaft_follower_becoming_candidate_votes_for_self(CuTest*);
extern void TestRaft_follower_becoming_candidate_resets_election_timeout(CuTest*);
extern void TestRaft_follower_receiving_appendentries_resets_election_timeout(CuTest*);
extern void TestRaft_follower_becoming_candidate_requests_votes_from_other_servers(CuTest*);
extern void TestRaft_candidate_election_timeout_and_no_leader_results_in_new_election(CuTest*);
extern void TestRaft_candidate_receives_majority_of_votes_becomes_leader(CuTest*);
extern void TestRaft_candidate_will_not_respond_to_voterequest_if_it_has_already_voted(CuTest*);
extern void TestRaft_candidate_requestvote_includes_logidx(CuTest*);
extern void TestRaft_candidate_recv_appendentries_frm_leader_results_in_follower(CuTest*);
extern void TestRaft_candidate_recv_appendentries_frm_invalid_leader_doesnt_result_in_follower(CuTest*);
extern void TestRaft_leader_becomes_leader_is_leader(CuTest*);
extern void TestRaft_leader_becomes_leader_clears_voted_for(CuTest*);
extern void TestRaft_leader_when_becomes_leader_all_nodes_have_nextidx_equal_to_lastlog_idx_plus_1(CuTest*);
extern void TestRaft_leader_when_it_becomes_a_leader_sends_empty_appendentries(CuTest*);
extern void TestRaft_leader_responds_to_entry_msg_when_entry_is_committed(CuTest*);
extern void TestRaft_leader_sends_appendentries_with_NextIdx_when_PrevIdx_gt_NextIdx(CuTest*);
extern void TestRaft_leader_retries_appendentries_with_decremented_NextIdx_log_inconsistency(CuTest*);
extern void TestRaft_leader_append_entry_to_log_increases_idxno(CuTest*);
extern void TestRaft_leader_increase_commit_idx_when_majority_have_entry_and_atleast_one_newer_entry(CuTest*);
extern void TestRaft_leader_steps_down_if_received_appendentries_is_newer_than_itself(CuTest*);


void RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestLog_new_is_empty);
    SUITE_ADD_TEST(suite, TestLog_append_is_not_empty);
    SUITE_ADD_TEST(suite, TestLog_get_at_idx);
    SUITE_ADD_TEST(suite, TestLog_get_at_idx_returns_null_where_out_of_bounds);
    SUITE_ADD_TEST(suite, TestLog_mark_node_has_committed_adds_nodes);
    SUITE_ADD_TEST(suite, TestLog_delete);
    SUITE_ADD_TEST(suite, TestLog_delete_onwards);
    SUITE_ADD_TEST(suite, TestLog_peektail);
    SUITE_ADD_TEST(suite, TestRaft_node_set_nextIdx);
    SUITE_ADD_TEST(suite, TestRaft_scenario_leader_appears);
    SUITE_ADD_TEST(suite, TestRaft_server_voted_for_records_who_we_voted_for);
    SUITE_ADD_TEST(suite, TestRaft_server_idx_starts_at_1);
    SUITE_ADD_TEST(suite, TestRaft_server_currentterm_defaults_to_0);
    SUITE_ADD_TEST(suite, TestRaft_server_set_currentterm_sets_term);
    SUITE_ADD_TEST(suite, TestRaft_server_voting_results_in_voting);
    SUITE_ADD_TEST(suite, TestRaft_election_start_increments_term);
    SUITE_ADD_TEST(suite, TestRaft_set_state);
    SUITE_ADD_TEST(suite, TestRaft_server_starts_as_follower);
    SUITE_ADD_TEST(suite, TestRaft_server_starts_with_election_timeout_of_1000ms);
    SUITE_ADD_TEST(suite, TestRaft_server_starts_with_request_timeout_of_200ms);
    SUITE_ADD_TEST(suite, TestRaft_server_entry_append_cant_append_if_id_is_zero);
    SUITE_ADD_TEST(suite, TestRaft_server_entry_append_increases_logidx);
    SUITE_ADD_TEST(suite, TestRaft_server_append_entry_means_entry_gets_current_term);
    SUITE_ADD_TEST(suite, TestRaft_server_entry_is_retrieveable_using_idx);
    SUITE_ADD_TEST(suite, TestRaft_server_wont_apply_entry_if_we_dont_have_entry_to_apply);
    SUITE_ADD_TEST(suite, TestRaft_server_increment_lastApplied_when_lastApplied_lt_commitidx);
    SUITE_ADD_TEST(suite, TestRaft_server_apply_entry_increments_last_applied_idx);
    SUITE_ADD_TEST(suite, TestRaft_server_periodic_elapses_election_timeout);
    SUITE_ADD_TEST(suite, TestRaft_server_election_timeout_sets_to_zero_when_elapsed_time_greater_than_timeout);
    SUITE_ADD_TEST(suite, TestRaft_server_cfg_sets_num_nodes);
    SUITE_ADD_TEST(suite, TestRaft_server_cant_get_node_we_dont_have);
    SUITE_ADD_TEST(suite, TestRaft_votes_are_majority_is_true);
    SUITE_ADD_TEST(suite, TestRaft_server_dont_increase_votes_for_me_when_receive_request_vote_response_is_not_granted);
    SUITE_ADD_TEST(suite, TestRaft_server_increase_votes_for_me_when_receive_request_vote_response);
    SUITE_ADD_TEST(suite, TestRaft_server_recv_requestvote_reply_false_if_term_less_than_current_term);
    SUITE_ADD_TEST(suite, TestRaft_server_dont_grant_vote_if_we_didnt_vote_for_this_candidate);
    SUITE_ADD_TEST(suite, TestRaft_follower_becomes_follower_is_follower);
    SUITE_ADD_TEST(suite, TestRaft_follower_becomes_follower_clears_voted_for);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_reply_false_if_term_less_than_currentterm);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_updates_currentterm_if_term_gt_currentterm);
    SUITE_ADD_TEST(suite, TestRaft_follower_doesnt_log_after_appendentry_if_no_entries_are_specified);
    SUITE_ADD_TEST(suite, TestRaft_follower_increases_log_after_appendentry);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_reply_false_if_doesnt_have_log_at_prev_log_idx_which_matches_prev_log_term);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_delete_entries_if_conflict_with_new_entries);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_add_new_entries_not_already_in_log);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_set_commitidx_to_prevLogIdx);
    SUITE_ADD_TEST(suite, TestRaft_follower_recv_appendentries_set_commitidx_to_LeaderCommit);
    SUITE_ADD_TEST(suite, TestRaft_follower_becomes_candidate_when_election_timeout_occurs);
    SUITE_ADD_TEST(suite, TestRaft_follower_dont_grant_vote_if_candidate_has_a_less_complete_log);
    SUITE_ADD_TEST(suite, TestRaft_candidate_becomes_candidate_is_candidate);
    SUITE_ADD_TEST(suite, TestRaft_follower_becoming_candidate_increments_current_term);
    SUITE_ADD_TEST(suite, TestRaft_follower_becoming_candidate_votes_for_self);
    SUITE_ADD_TEST(suite, TestRaft_follower_becoming_candidate_resets_election_timeout);
    SUITE_ADD_TEST(suite, TestRaft_follower_receiving_appendentries_resets_election_timeout);
    SUITE_ADD_TEST(suite, TestRaft_follower_becoming_candidate_requests_votes_from_other_servers);
    SUITE_ADD_TEST(suite, TestRaft_candidate_election_timeout_and_no_leader_results_in_new_election);
    SUITE_ADD_TEST(suite, TestRaft_candidate_receives_majority_of_votes_becomes_leader);
    SUITE_ADD_TEST(suite, TestRaft_candidate_will_not_respond_to_voterequest_if_it_has_already_voted);
    SUITE_ADD_TEST(suite, TestRaft_candidate_requestvote_includes_logidx);
    SUITE_ADD_TEST(suite, TestRaft_candidate_recv_appendentries_frm_leader_results_in_follower);
    SUITE_ADD_TEST(suite, TestRaft_candidate_recv_appendentries_frm_invalid_leader_doesnt_result_in_follower);
    SUITE_ADD_TEST(suite, TestRaft_leader_becomes_leader_is_leader);
    SUITE_ADD_TEST(suite, TestRaft_leader_becomes_leader_clears_voted_for);
    SUITE_ADD_TEST(suite, TestRaft_leader_when_becomes_leader_all_nodes_have_nextidx_equal_to_lastlog_idx_plus_1);
    SUITE_ADD_TEST(suite, TestRaft_leader_when_it_becomes_a_leader_sends_empty_appendentries);
    SUITE_ADD_TEST(suite, TestRaft_leader_responds_to_entry_msg_when_entry_is_committed);
    SUITE_ADD_TEST(suite, TestRaft_leader_sends_appendentries_with_NextIdx_when_PrevIdx_gt_NextIdx);
    SUITE_ADD_TEST(suite, TestRaft_leader_retries_appendentries_with_decremented_NextIdx_log_inconsistency);
    SUITE_ADD_TEST(suite, TestRaft_leader_append_entry_to_log_increases_idxno);
    SUITE_ADD_TEST(suite, TestRaft_leader_increase_commit_idx_when_majority_have_entry_and_atleast_one_newer_entry);
    SUITE_ADD_TEST(suite, TestRaft_leader_steps_down_if_received_appendentries_is_newer_than_itself);

    CuSuiteRun(suite);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main()
{
    RunAllTests();
    return 0;
}

