#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <locale.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "util.h"
#include "options.h"

static gchar *stdout_path = "NULL";
static gchar *stderr_path = "STDOUT";
static gchar *fifo_tmp_dir = NULL;
static gchar *command = NULL;
static gchar **command_args = NULL;
static GOptionEntry new_entry1 = { "stdout", 'O', 0, G_OPTION_ARG_STRING, &stdout_path, "stdout file path (NULL string (default) can be used to redirect to /dev/null)", NULL };
static GOptionEntry new_entry2 = { "stderr", 'E', 0, G_OPTION_ARG_STRING, &stderr_path, "stderr file path (STDOUT string (default) can be used to redirect to the same file than stdout)", NULL };
static GOptionEntry new_entry3 = { "fifo-tmp-dir", 'F', 0, G_OPTION_ARG_STRING, &fifo_tmp_dir, "directory where to store tmp FIFO for log_proxy (default: content of environment variable TMPDIR if set, /tmp if not)", NULL };

GOptionEntry *change_options()
{
    int number_of_options = sizeof(entries) / sizeof(entries[0]);
    // we remove 2 options and we add 3 new
    GOptionEntry *res = g_malloc(sizeof(GOptionEntry) * (number_of_options + 1));
    for (int i = 0 ; i < number_of_options - 1; i++) {
        if (g_strcmp0(entries[i].long_name, "fifo") == 0) {
            continue;
        }
        if (g_strcmp0(entries[i].long_name, "rm-fifo-at-exit") == 0) {
            continue;
        }
        res[i] = entries[i];
    }
    res[number_of_options - 3] = new_entry1;
    res[number_of_options - 2] = new_entry2;
    res[number_of_options - 1] = new_entry3;
    res[number_of_options] = entries[number_of_options - 1];
    return res;
}

gchar *make_fifo(const gchar *label) {
    const gchar *tmpdir = fifo_tmp_dir;
    if ( tmpdir ==  NULL ) {
        tmpdir = g_getenv("TMPDIR");
        if (tmpdir == NULL) {
            tmpdir = "/tmp";
        }
    }
    gchar *uid = get_unique_hexa_identifier();
    gchar *path = g_strdup_printf("%s/log_proxy_%s_%s.fifo", tmpdir, label, uid);
    int res = mkfifo(path, S_IRUSR | S_IWUSR);
    if (res != 0) {
        g_critical("can't create fifo: %s", path);
        exit(1);
    }
    g_free(uid);
    return path;
}

void spawn_logproxy_async(const gchar *fifo_path, const gchar *log_path) {
    gchar *use_locks_str = "";
    if (use_locks) {
        use_locks_str = "--use-locks";
    }
    gchar *cli = g_strdup_printf("log_proxy -s %li -t %li -S \"%s\" -d \"%s\" -n %i %s -r -f \"%s\" \"%s\"", rotation_size, rotation_time, rotation_suffix, log_directory, rotated_files, use_locks_str, fifo_path, log_path);
    gboolean spawn_res = g_spawn_command_line_async(cli, NULL);
    if (spawn_res == FALSE) {
        g_critical("can't spawn %s => exit", cli);
        exit(1);
    }
    g_free(cli);
}

int main(int argc, char *argv[])
{
    GOptionContext *context;
    setlocale(LC_ALL, "");
    log_file = NULL; // not used
    context = g_option_context_new("-- COMMAND [COMMAND_ARG1] [COMMAND_ARG2] [...] - log proxy");
    GOptionEntry *new_entries = change_options();
    g_option_context_add_main_entries(context, new_entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, NULL)) {
        g_print("%s", g_option_context_get_help(context, TRUE, NULL));
        exit(1);
    }
    if (argc < 2) {
        g_print("%s", g_option_context_get_help(context, TRUE, NULL));
        exit(1);
    }
    set_default_values_from_env();
    if (g_strcmp0(stderr_path, "STDOUT") == 0) {
        stderr_path = stdout_path;
    }
    if (g_strcmp0(stdout_path, "NULL") == 0) {
        stdout_path = "/dev/null";
    }
    if (g_strcmp0(stderr_path, "NULL") == 0) {
        stderr_path = "/dev/null";
    }
    int index = 1;
    if (g_strcmp0(argv[index], "--") == 0) {
        if (argc < 3) {
            g_print("%s", g_option_context_get_help(context, TRUE, NULL));
            exit(1);
        }
        index++;
    }
    command = g_strdup(argv[index]);
    int command_args_length = argc - index - 1;
    command_args = g_malloc(sizeof(gchar*) * (command_args_length + 2));
    command_args[0] = command;
    for (int i = 0 ; i < command_args_length ; i++) {
        command_args[i + 1] = argv[index + i + 1];
    }
    command_args[command_args_length + 1] = NULL;
    gchar *stdout_fifo = g_strdup("/dev/null");
    gchar *stderr_fifo = g_strdup("/dev/null");
    if (g_strcmp0(stdout_path, "/dev/null") != 0) {
        stdout_fifo = make_fifo("stdout");
        spawn_logproxy_async(stdout_fifo, stdout_path);
    } else {
        stdout_fifo = g_strdup("/dev/null");
    }
    if (g_strcmp0(stdout_path, stderr_path) != 0) {
        if (g_strcmp0(stdout_path, "/dev/null") != 0) {
            stderr_fifo = make_fifo("stderr");
            spawn_logproxy_async(stderr_fifo, stderr_path);
        } else {
            stderr_fifo = "/dev/null";
        }
    } else {
        stderr_fifo = g_strdup(stdout_fifo);
    }
    int bak_stdout = dup(1);
    int bak_stderr = dup(2);
    int new_stdout = open(stdout_fifo, O_WRONLY);
    int new_stderr = open(stderr_fifo, O_WRONLY);
    dup2(new_stdout, 1);
    dup2(new_stderr, 2);
    close(new_stdout);
    close(new_stderr);
    execvp(command, command_args);
    // if we are here, there is an error
    fflush(stdout);
    fflush(stderr);
    dup2(bak_stdout, 1);
    dup2(bak_stderr, 2);
    close(bak_stdout);
    close(bak_stderr);
    g_critical("can't launch %s command with error: %i [%s]", command, errno, strerror(errno));
    g_option_context_free(context);
    return 1;
}
