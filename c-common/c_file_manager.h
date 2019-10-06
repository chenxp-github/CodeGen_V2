#ifndef _C_FILE_MANAGER_H
#define _C_FILE_MANAGER_H

#include "cruntime.h"
#include "c_mem.h"
#include "c_closure.h"

#define C_FN_SINGLE   0x00000001
#define C_FN_EXT      0x00000002
#define C_FN_PATH     0x00000004

#define C_FN_FILENAME (C_FN_SINGLE|C_FN_EXT)
#define C_FN_FULLNAME (C_FN_PATH|C_FN_FILENAME)

#define C_EVENT_BEGIN_DIR   1
#define C_EVENT_END_DIR     2
#define C_EVENT_SINGLE_FILE 3

status_t filemanager_get_cur_dir(struct mem *dir);
bool_t filemanager_is_file_exist(const char *fn);
bool_t filemanager_is_abs_path(const char *dir);
status_t filemanager_is_dir_exist(const char *dir);
status_t filemanager_delete_file(const char* filename);
status_t filemanager_delete_dir(const char *dir);
status_t filemanager_create_dir(const char *dir);
status_t filemanager_create_dir_super(const char *_dir);
status_t filemanager_is_path_splitor(char ch);
fsize_t filemanager_get_file_size(const char *filename);
status_t filemanager_search_dir(const char *dir, bool_t recursive, struct closure *closure, int *running);
status_t filemanager_to_abs_path_with_prefix(const char *prefix,struct file_base *file);
status_t filemanager_to_abs_path(struct mem *path);

#endif

