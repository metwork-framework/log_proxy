#include <glib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "util.h"

void test_get_current_timestamp()
{
    glong t1 = get_current_timestamp();
    g_assert(t1 > 1576518428); //December 16, 2019
    sleep(1);
    glong t2 = get_current_timestamp();
    g_assert(t2-t1 <= 2);
    g_assert(t2-t1 >= 1);
}

void test_get_unique_hexa_identifier()
{
    gchar *hexa1=get_unique_hexa_identifier();
    g_assert(strlen(hexa1) >= 16); //should be 32
    gchar *hexa2=get_unique_hexa_identifier();
    g_assert(hexa1 != hexa2);
    g_free(hexa1);
    g_free(hexa2);
}

int main(int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);
    setlocale(LC_ALL, "");
    g_test_add_func("/log_proxy/test_get_current_timestamp", test_get_current_timestamp);
    g_test_add_func("/log_proxy/test_get_unique_hexa_identifier", test_get_unique_hexa_identifier);
    int res = g_test_run();
    return res;
}
