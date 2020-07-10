#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <fcntl.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "util.h"
#include "control.h"

//tests on util.c
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

void test_get_file_size()
{
    g_unlink("example_file");
    GError *err = NULL;
    g_file_set_contents("example_file", "hello\n", 6, &err);
    g_assert(get_file_size("example_file") == 6);
    g_unlink("example_file");
    g_assert(get_file_size("example_file") == -1);
}

void test_get_file_inode()
{
    g_creat("example_file", O_RDWR);
    g_assert(get_file_inode("example_file") > 0);
    g_unlink("example_file");
    g_assert(get_file_inode("example_file") == -1);
}

void test_get_fd_inode()
{
    int fd = g_open("example_file", O_CREAT, 0600);
    g_assert(get_fd_inode(fd) > 0);
    close(fd);
    g_remove("example_file");
    g_assert(get_fd_inode(fd) == -1);
}

void test_compute_strftime_suffix()
{
    gchar *suffix = compute_strftime_suffix("example_file", ".%Y%m%d%H%M%S");
    g_assert(strlen(suffix) == 27);
    g_assert_cmpstr(suffix, >, "example_file.20191219000000"); //newer than December 19, 2019
    g_assert_cmpstr("example_file.21191219000000", >, suffix); //older than December 19, 2119
    g_free(suffix);
}

void test_create_empty()
{
    g_unlink("example_file");
    g_assert(create_empty("example_file"));
    g_assert(get_file_size("example_file") == 0);
    GError *err = NULL;
    g_file_set_contents("example_file", "hello\n", 6, &err);
    g_assert(create_empty("example_file"));
    g_assert(get_file_size("example_file") == 6);
    g_unlink("example_file");
}

//tests on control.c
void test_manage_control_file()
{
    g_unlink(".log_file.control");
    // check init control file
    g_assert(init_control_file("log_file", "start"));
    // check content
    gchar* content = get_control_file_content("log_file");
    g_assert_cmpstr(content, ==, "start");
    g_free(content);
    // lock control file (blocking)
    int fd1 = lock_control_file("log_file");
    g_assert(fd1 >= 0);
    // check inode
    int fd2 = g_open(".log_file.control", O_RDONLY);
    g_assert_cmpint(get_fd_inode(fd1), ==, get_fd_inode(fd2));
    // unlock control file
    unlock_control_file(fd1);
    // check control file can be locked again
    fd1 = lock_control_file("log_file");
    g_assert(fd1 >= 0);
    unlock_control_file(fd1);
}

#if GLIB_CHECK_VERSION(2,38,0)
void test_blocked_control_file()
{
    if (g_test_subprocess()) {
        //lock control file
        int fd1 = lock_control_file("log_file");
        g_assert(fd1 >= 0);
        //try to lock locked control file (should be blocked)
        lock_control_file("log_file");
        return;
    }
    g_test_trap_subprocess(NULL, 2000000, 0); //execute test in subprocess with 3 seconds timeout
    // check it failed (control file was blocked)
    g_test_trap_assert_failed();
}
#endif

#if GLIB_CHECK_VERSION(2,32,0)
void thread_lock_control_file()
{
    glong fd1 = -1;
    fd1 = lock_control_file("log_file");
    g_assert(fd1 >= 0);
    g_thread_exit((gpointer)fd1);
}

void test_lock_unlock_control_file()
{
    //lock control file
    int fd2 = lock_control_file("log_file");
    g_assert(fd2 >= 0);
    //run thread trying to get lock
    GThread *thread = g_thread_new("thread", (GThreadFunc)thread_lock_control_file, NULL);
    //sleep a while
    sleep(1);
    //unlock control file
    unlock_control_file(fd2);
    //check thread has obtain lock
    gpointer val = g_thread_join(thread);
    g_assert((glong)val > 0);
}
#endif

//tests on out.c
int main(int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);
    setlocale(LC_ALL, "");
    g_test_add_func("/log_proxy/test_get_current_timestamp", test_get_current_timestamp);
    g_test_add_func("/log_proxy/test_get_unique_hexa_identifier", test_get_unique_hexa_identifier);
    g_test_add_func("/log_proxy/test_get_file_size", test_get_file_size);
    g_test_add_func("/log_proxy/test_get_fd_inode", test_get_fd_inode);
    g_test_add_func("/log_proxy/test_get_file_inode", test_get_file_inode);
    g_test_add_func("/log_proxy/test_compute_strftime_suffix", test_compute_strftime_suffix);
    g_test_add_func("/log_proxy/test_create_empty", test_create_empty);
    g_test_add_func("/log_proxy/test_manage_control_file", test_manage_control_file);
#if GLIB_CHECK_VERSION(2,38,0)
    g_test_add_func("/log_proxy/test_blocked_control_file", test_blocked_control_file);
#endif
#if GLIB_CHECK_VERSION(2,32,0)
    g_test_add_func("/log_proxy/test_lock_unlock_control_file", test_lock_unlock_control_file);
#endif
    int res = g_test_run();
    return res;
}
