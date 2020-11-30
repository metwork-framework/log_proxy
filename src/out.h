#ifndef OUT_H_
#define OUT_H_

#include <glib.h>

void init_output_channel(const gchar *log_file, gboolean use_locks, gboolean force_control_file, const gchar *chmod_str);
void destroy_output_channel();
gboolean write_output_channel(GString *buffer);
glong get_output_channel_age();
gboolean test_output_channel_rotated();

#endif /* OUT_H_ */
