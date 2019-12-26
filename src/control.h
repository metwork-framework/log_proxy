#ifndef CONTROL_H_
#define CONTROL_H_

#include <glib.h>

gboolean init_control_file(const gchar *path, const gchar *content);
int lock_control_file(const gchar *path, gboolean blocking, int time_max);
void unlock_control_file(int fd);
gchar *get_control_file_content(const gchar *path);

#endif /* CONTROL_H_ */
