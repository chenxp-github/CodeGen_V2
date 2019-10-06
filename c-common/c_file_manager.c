#include "c_file_manager.h"
#include "c_mem_stk.h"
#include "syslog.h"
#include "mem_tool.h"

#define _MAX_PATH_ (LBUF_SIZE*2)

status_t filemanager_get_cur_dir(struct mem *dir)
{
    int_ptr_t max,size;
    ASSERT(dir);
    max = (int_ptr_t)(mem_get_max_size(dir)-1);
    
    size = (int_ptr_t)crt_get_cur_dir(
        mem_get_raw_buffer(dir),max
    );

    if(size > 0)
    {
        mem_set_size(dir,size);
        return OK;
    }

    return ERROR;
}

bool_t filemanager_is_file_exist(const char *fn)
{
    FILE_HANDLE fp;
    ASSERT(fn);    
    fp = crt_fopen(fn,"rb");
    if(!crt_is_file_handle(fp))
        return FALSE;
    crt_fclose(fp);
    return TRUE;
}

bool_t filemanager_is_abs_path(const char *dir)
{
    ASSERT(dir);
    
    if(dir[1] == ':')
    {
        return TRUE;
    }
    else if(dir[0] == '\\' || dir[0] =='/')
    {
        return TRUE;
    }
    
    return FALSE;
}

status_t filemanager_is_dir_exist(const char *dir)
{
    char tmp[_MAX_PATH_];
    void *p[32];
    
    ASSERT(dir);

    p[1] = tmp;
    if(crt_open_dir(dir,p))
    {
        crt_close_dir(p);
        return TRUE;
    }
    
    return FALSE;
}

status_t filemanager_delete_file(const char* filename)
{
    ASSERT(filename);
    return crt_unlink(filename);
}

status_t filemanager_delete_dir(const char *dir)
{
    ASSERT(dir);
    return crt_rmdir(dir);
}

status_t filemanager_create_dir(const char *dir)
{
    ASSERT(dir);
    return crt_mkdir(dir);
}

status_t filemanager_create_dir_super(const char *_dir)
{   
    _C_MEM_HEADER(dir);
    _C_LOCAL_MEM_HEADER(path);
    _C_LOCAL_MEM_HEADER(buf);

    ASSERT(_dir);

    _C_LOCAL_MEM_BODY(path);
    _C_LOCAL_MEM_BODY(buf);
    _C_MEM_BODY(dir);
    
    mem_set_str(&dir,_dir);
    filebase_set_split_chars(dir_file,":\\/");
    filebase_seek(dir_file,0);

    if(filebase_char_at(dir_file,1) == ':')
    {        
        filebase_putc(path_file,
            filebase_getc(dir_file));
        filebase_putc(path_file,
            filebase_getc(dir_file));
    }
    
    while(filebase_read_string(dir_file,buf_file))
    {
        char str[2];
        str[0] = crt_get_path_splitor();
        str[1] = 0;

        filebase_puts(path_file,str);
        filebase_write_file(path_file,buf_file);
        if(!filemanager_is_dir_exist(mem_cstr(&path)))
        {
            if(!filemanager_create_dir(mem_cstr(&path)))
                return ERROR;
        }
    }

    return OK;
}

status_t filemanager_is_path_splitor(char ch)
{
    return ch == '/' || ch == '\\';
}


fsize_t filemanager_get_file_size(const char *filename)
{
    FILE_HANDLE fp;
    fsize_t size;

    ASSERT(filename);
    
    fp = crt_fopen(filename,"rb");
    if(!crt_is_file_handle(fp))
        return 0;
    crt_fseek(fp,0,SEEK_END);
    size = crt_ftell(fp);
    crt_fclose(fp);
    return size;
}

status_t filemanager_search_dir(const char *dir, bool_t recursive, struct closure *closure, int *running)
{
    bool_t separator_ended;
    char filename[_MAX_PATH_];
    char old_path[_MAX_PATH_];
    void *p[32];
    bool_t is_dir;    
    int64_t size,last_write_time = 0;
    _C_LOCAL_MEM_WITH_SIZE_HEADER(fullname,_MAX_PATH_);

    ASSERT(dir);
    ASSERT(dir[0]);

    if(running && !(*running))
        return OK;
    
    _C_LOCAL_MEM_WITH_SIZE_BODY(fullname,_MAX_PATH_);

    separator_ended = FALSE;
    
     if(dir[crt_strlen(dir)-1] == crt_get_path_splitor())
         separator_ended = 1;

    crt_get_cur_dir(old_path, _MAX_PATH_);
    if(crt_need_chdir_before_open())
        crt_chdir(dir);
    
    p[1] = filename;
    if(!crt_open_dir(dir,p))
    {
        XLOG(LOG_MODULE_COMMON,LOG_LEVEL_ERROR,
            "open dir '%s' fail.",dir
        );
        return ERROR;
    }

    while(crt_read_dir(p))
    {
        if(running && !(*running))break;
        if(strcmp(filename,".") == 0)continue;
        if(strcmp(filename,"..") == 0)continue;
        
        is_dir = (p[2] != 0);
        
        mem_strcpy(&fullname,dir);
        if(!separator_ended)
            filebase_putc(fullname_file,crt_get_path_splitor());
        filebase_puts(fullname_file,filename);
        
        last_write_time = 0;
        memcpy(&size,&p[3],sizeof(size));
        memcpy(&last_write_time,&p[5],sizeof(last_write_time));
        
        closure_set_param_pointer(closure,1,filename);
        closure_set_param_pointer(closure,2,(void*)mem_cstr(&fullname));
        closure_set_param_int(closure,3,is_dir);
        closure_set_param_int64(closure,4,size);
        closure_set_param_int64(closure,5,last_write_time);
        
        if(!is_dir)
        {
            closure_run_event(closure,C_EVENT_SINGLE_FILE);
        }
        else
        {
            closure_run_event(closure,C_EVENT_BEGIN_DIR);
            
            if(recursive)
            {
                filemanager_search_dir(
                    mem_cstr(&fullname),
                    TRUE,closure,running
                );
            }
            //set again, recursive call may change closure value
            closure_set_param_pointer(closure,1,filename);
            closure_set_param_pointer(closure,2,(void*)mem_cstr(&fullname));
            closure_set_param_int(closure,3,is_dir);
            closure_set_param_int64(closure,4,size);
            closure_set_param_int64(closure,5,last_write_time);
            closure_run_event(closure,C_EVENT_END_DIR);
        }
    }
    
    crt_close_dir(p);
    if(crt_need_chdir_before_open())
        crt_chdir(old_path);
    
    return OK;
}

status_t filemanager_to_abs_path_with_prefix(const char *prefix,struct file_base *file)
{
	_C_LOCAL_MEM_HEADER(buf);
	struct mem_stk stk;    
	struct mem *pmem;
	int i;

	ASSERT(file);

	_C_LOCAL_MEM_BODY(buf);
    
    filebase_puts(buf_file,prefix);
    filebase_putc(buf_file,crt_get_path_splitor());
    filebase_write_file(buf_file,file);
    
    memstk_init(&stk,256);
    memstk_load_path(&stk,buf_file);
    
    filebase_set_size(file,0);
    for(i = 0; i < memstk_get_len(&stk); i++)
    {
        pmem = memstk_get_elem(&stk,i);
        ASSERT(pmem);
        if(filebase_char_at(&pmem->base_file_base,1) != ':')
            filebase_putc(file,crt_get_path_splitor());
        filebase_write_file(file,&pmem->base_file_base);
    }

	memstk_destroy(&stk);
    return OK;
}

status_t filemanager_to_abs_path(struct mem *path)
{
	_C_LOCAL_MEM_WITH_SIZE_HEADER(mem,_MAX_PATH_);

    ASSERT(path);		
	_C_LOCAL_MEM_WITH_SIZE_BODY(mem,_MAX_PATH_);

    if(filemanager_is_abs_path(mem_cstr(path)))
    {
        mem_strcpy(&mem,"");
    }
    else
    {
        filemanager_get_cur_dir(&mem);
    }
	
    return filemanager_to_abs_path_with_prefix(
		mem_cstr(&mem),&path->base_file_base);
}
