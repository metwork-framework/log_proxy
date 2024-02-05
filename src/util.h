#ifndef UTIL_H_
#define UTIL_H_

#include <glib.h>
#include <sys/types.h>

glong get_file_size(const gchar *file_path);
glong get_current_timestamp();
gchar *compute_strftime_suffix(const gchar *str, const gchar *strftime_suffix);
gchar *compute_timestamp_prefix(const gchar *strftime_prefix);
gchar *get_unique_hexa_identifier();
glong get_file_inode(const gchar *file_path);
glong get_fd_inode(int fd);
gboolean create_empty(const gchar *file_path);
gchar *compute_file_path(const gchar *directory, const gchar *file_name);
uid_t user_id_from_name(const gchar *name);
gid_t group_id_from_name(const gchar *name);

#endif /* UTIL_H_ */
