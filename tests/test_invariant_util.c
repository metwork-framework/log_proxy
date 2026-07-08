#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

/* Import the function from src/util.c */
extern gchar *util_hash_to_hex(const guint8 *hash, gsize len);

START_TEST(test_hash_to_hex_buffer_bounds)
{
    /* Invariant: Buffer writes never exceed allocated length (len * 8 chars + null) */

    /* Valid input: 20-byte SHA1 hash */
    guint8 valid_hash[20];
    memset(valid_hash, 0xAB, 20);
    gchar *res = util_hash_to_hex(valid_hash, 20);
    ck_assert_ptr_nonnull(res);
    ck_assert_uint_eq(strlen(res), 20 * 8);
    g_free(res);

    /* Boundary: 64-byte hash (SHA-512 sized, 16 uint32s) */
    guint8 boundary_hash[64];
    memset(boundary_hash, 0xFF, 64);
    res = util_hash_to_hex(boundary_hash, 64);
    ck_assert_ptr_nonnull(res);
    ck_assert_uint_eq(strlen(res), 64 * 8);
    g_free(res);

    /* Oversized: 256 bytes - 2x typical max, triggers many loop iterations */
    guint8 large_hash[256];
    memset(large_hash, 0xDE, 256);
    res = util_hash_to_hex(large_hash, 256);
    ck_assert_ptr_nonnull(res);
    ck_assert_uint_eq(strlen(res), 256 * 8);
    g_free(res);

    /* Exploit case: 1024 bytes - 10x typical, would overflow undersized buffer */
    guint8 exploit_hash[1024];
    memset(exploit_hash, 0x41, 1024);
    res = util_hash_to_hex(exploit_hash, 1024);
    ck_assert_ptr_nonnull(res);
    ck_assert_uint_eq(strlen(res), 1024 * 8);
    g_free(res);
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_hash_to_hex_buffer_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}