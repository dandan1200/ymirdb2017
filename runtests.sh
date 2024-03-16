gcc -o ymirdb ymirdb.c -Wall -g -fprofile-arcs -ftest-coverage
./ymirdb < test_cases/test_general_entries_1.in > test_cases/test_general_entries_1.out
echo "test 1 done"
./ymirdb < test_cases/test_general_entries_2.in > test_cases/test_general_entries_2.out
echo "test 2 done"
./ymirdb < test_cases/test_general_entries_3.in > test_cases/test_general_entries_3.out
echo "test 3 done"
./ymirdb < test_cases/test_list_keys_entries.in > test_cases/test_list_keys_entries.out
echo "test 4 done"
./ymirdb < test_cases/test_min_max_sum_len_drop.in > test_cases/test_min_max_sum_len_drop.out
echo "test 5 done"
./ymirdb < test_cases/test_pick_pluck_pop.in > test_cases/test_pick_pluck_pop.out
echo "test 6 done"
./ymirdb < test_cases/test_push_append_del.in > test_cases/test_push_append_del.out
echo "test 7 done"
./ymirdb < test_cases/test_rev_sort_uniq.in > test_cases/test_rev_sort_uniq.out
echo "test 8 done"
./ymirdb < test_cases/test_set_and_get.in > test_cases/test_set_and_get.out
echo "test 9 done"
./ymirdb < test_cases/test_snapshot_rollback_checkout.in > test_cases/test_snapshot_rollback_checkout.out
echo "test 10 done"
./ymirdb < test_cases/test_errors.in > test_cases/test_errors.out
echo "test 11 done"
./ymirdb < test_cases/test_forward_backward.in > test_cases/test_forward_backward.out
echo "test 12 done"
./ymirdb < test_cases/test_general_entries_4.in > test_cases/test_general_entries_4.out
echo "test 13 done"
gcov ./ymirdb
