#include "c_file_base.h"
#include "c_mem.h"
#include "syslog.h"
#include "mem_tool.h"

status_t filebase_init_basic(struct file_base *self)
{
    self->user_data = NULL;
    self->split_chars = NULL;    
    self->destroy = NULL;
    self->read = NULL;
    self->write = NULL;
    self->seek = NULL;
    self->get_offset = NULL;
    self->get_size = NULL;
    self->set_size = NULL;
    self->add_block = NULL;
    self->get_max_size = NULL;
    return OK;
}

status_t filebase_init(struct file_base *self)
{
    filebase_init_basic(self);
    filebase_set_default_sp_chars(self);
    return OK;
}

status_t filebase_base_destroy(struct file_base *self)
{
    filebase_init_basic(self);
    return OK;
}

status_t filebase_destroy(struct file_base *self)
{
    if(self->destroy)
    {
        self->destroy(self);
    }
    filebase_base_destroy(self);
    return OK;
}

status_t filebase_copy(struct file_base *self,struct file_base *_p)
{
    ASSERT(_p);
    if(self == _p)return OK;

    self->user_data = _p->user_data;
    self->split_chars = _p->split_chars;
    return OK;
}

status_t filebase_comp(struct file_base *self,struct file_base *_p)
{
    ASSERT(_p);
    if(self == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t filebase_print(struct file_base *self,struct log_buffer *_buf)
{
    ASSERT(_buf);
    log_buffer_log(_buf,"user_data = 0x%p",self->user_data);
    log_buffer_log(_buf,"split_chars = 0x%p",self->split_chars);
    return OK;
}

char* filebase_get_split_chars(struct file_base *self)
{
    return self->split_chars;
}

status_t filebase_set_split_chars(struct file_base *self,char *_split_chars)
{
    self->split_chars = _split_chars;
    return OK;
}

status_t filebase_set_default_sp_chars(struct file_base *self)
{
    self->split_chars = (char*)" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";
    return OK;
}

status_t filebase_is_split_char(struct file_base *self,char ch)
{
    const char *p;
    
    if(ch == 0 || ch == '\n')
        return TRUE;
    
    p = self->split_chars;
    while(*p)
    {
        if(*p == ch)
            return TRUE;
        p++;
    }
    return FALSE;
}


int_ptr_t filebase_read(struct file_base *self,void *buf,int_ptr_t n)
{
    ASSERT(self->read);
    return self->read(self,buf,n);
}

int_ptr_t filebase_write(struct file_base *self,const void *buf,int_ptr_t n)
{
    ASSERT(self->write);
    return self->write(self,buf,n);
}

fsize_t filebase_seek(struct file_base *self,fsize_t off)
{
    ASSERT(self->seek);
    return self->seek(self,off);
}

fsize_t filebase_get_offset(struct file_base *self)
{
    ASSERT(self->get_offset);
    return self->get_offset(self);
}

fsize_t filebase_get_size(struct file_base *self)
{
    ASSERT(self->get_size);
    return self->get_size(self);
}
status_t filebase_set_size(struct file_base *self,fsize_t size)
{
    ASSERT(self->set_size);
    return self->set_size(self,size);
}

status_t filebase_add_block(struct file_base *self,fsize_t bsize)
{
    ASSERT(self->add_block);
    return self->add_block(self,bsize);
}

fsize_t filebase_get_max_size(struct file_base *self)
{
    ASSERT(self->get_max_size);
    return self->get_max_size(self);
}

char filebase_getc(struct file_base *self)
{
    char ch = 0;
    filebase_read(self,&ch,1);
    return ch;
}

fsize_t filebase_putc(struct file_base *self,char ch)
{
    return filebase_write(self,&ch,1);
}

char filebase_un_getc(struct file_base *self)
{
    char ch=0;
    fsize_t off;
    
    off = filebase_get_offset(self);
    if(off > 0)
    {
        off--;
        filebase_seek(self,off);
        off = filebase_get_offset(self);
        ch =  filebase_getc(self);
        filebase_seek(self,off);
    }
    return ch;
}


bool_t filebase_is_end(struct file_base *self)
{
    return (filebase_get_offset(self) >= filebase_get_size(self));
}

fsize_t filebase_search_bin(struct file_base *self,struct file_base *file_bin)
{    
    fsize_t old;
    char ch1,ch2;
    
    ASSERT(file_bin);
    filebase_seek(file_bin,0);
    
    while(!filebase_is_end(self))
    {
        old = filebase_get_offset(self);
        filebase_seek(file_bin,0);
        
        while (!filebase_is_end(self) && !filebase_is_end(file_bin)) 
        {
            ch2 = filebase_getc(file_bin);
            ch1 = filebase_getc(self);
            if(ch1 != ch2)
            {
                filebase_seek(file_bin,0);
                break;              
            }
        }
        
        if(filebase_is_end(file_bin))
        {
            return filebase_get_offset(self) - filebase_get_size(file_bin);
        }
        
        filebase_seek(self,old + 1);
    }
    
    return -1;
}

fsize_t filebase_write_file_with_buffer(struct file_base *self,struct file_base *file, fsize_t start, fsize_t ws, void *buf, int_ptr_t buf_size)
{    
    fsize_t block_size = buf_size;
    fsize_t left,write_size,b_size,old_off;
    int_ptr_t rs;
    
    ASSERT(file);

    old_off = filebase_get_offset(file);
    filebase_seek(file,start);
    left = ws;
    
    while(left > 0)
    {
        b_size = left;
        if(b_size > block_size)
            b_size = block_size;
        rs = filebase_read(file,buf,(int_ptr_t)b_size);
        if(rs > 0)
        {
            write_size = filebase_write(self,buf,rs);
            if(write_size > 0)
                left -= write_size;
            else
                break;
        }
        else
        {
            break;
        }
    }
    filebase_seek(file,old_off);
    return ws - left;   
}

fsize_t filebase_write_file_from(struct file_base *self,struct file_base *file, fsize_t start, fsize_t ws)
{
    char buf[FILEBASE_LBUF_SIZE];   
    return filebase_write_file_with_buffer(self,file,start,ws,buf,FILEBASE_LBUF_SIZE);
}

fsize_t filebase_write_file(struct file_base *self,struct file_base *file)
{
    ASSERT(file);
    return filebase_write_file_from(self,file,0,filebase_get_size(file));
}

fsize_t filebase_write_to_file_with_buffer(struct file_base *self,struct file_base *file, fsize_t start, fsize_t wsize, void *buf, int_ptr_t buf_size)
{
    fsize_t sum = 0;
    int_ptr_t need_read,ws,rs;
    int_ptr_t bsize = buf_size;
    
    ASSERT(file);
    
    filebase_seek(self,start);
    
    while(sum < wsize)
    {
        need_read = bsize;
        if(need_read > wsize - sum)
            need_read = (int_ptr_t)(wsize - sum);
        rs = filebase_read(self,buf,need_read);
        if(rs <= 0) break;
        
        ws = filebase_write(file,buf,rs);
        if(ws <= 0) break;
        sum += ws;
    }
    
    return sum;
}

fsize_t filebase_write_to_file_from(struct file_base *self,struct file_base *file,fsize_t start,fsize_t wsize)
{
    char buf[FILEBASE_LBUF_SIZE];   
    return filebase_write_to_file_with_buffer(self,file,start,wsize,buf,FILEBASE_LBUF_SIZE);
}

fsize_t filebase_write_to_file(struct file_base *self,struct file_base *file)
{
    return filebase_write_to_file_from(self,file,0,filebase_get_size(self));
}

fsize_t filebase_save_file_fp(struct file_base *self,FILE_HANDLE fp,fsize_t start,fsize_t wsize)
{    
    char buf[FILEBASE_LBUF_SIZE];
    int_ptr_t rs,bsize = FILEBASE_LBUF_SIZE;    
    fsize_t sum = 0,need_read;
    
    ASSERT(fp);

    filebase_seek(self,start);
    while(sum < wsize)
    {
        need_read = bsize;
        if(need_read > wsize - sum)
            need_read = wsize - sum;
        rs = filebase_read(self,buf,(int_ptr_t)need_read);
        if(rs <= 0) break;
        crt_fwrite(fp,buf,rs);
        sum += rs;
    }
    return sum;
}

fsize_t filebase_save_file_from(struct file_base *self,const char *fn,fsize_t start,fsize_t wsize)
{
    fsize_t sum = 0;
    FILE_HANDLE fp = crt_fopen(fn , "wb+");
    if(!crt_is_file_handle(fp))
    {
        return -1;
    }
    sum = filebase_save_file_fp(self,fp,start,wsize);
    crt_fclose(fp);
    return sum;
}

fsize_t filebase_save_file(struct file_base *self,const char *fn)
{
    return filebase_save_file_from(self,fn,0,filebase_get_size(self));     
}

status_t filebase_load_file_from_fp(struct file_base *self,FILE_HANDLE fp)
{
    char buf[FILEBASE_LBUF_SIZE];
    fsize_t fs,old_off;
    int_ptr_t rs,buf_size = FILEBASE_LBUF_SIZE;
 
    ASSERT(fp);

    crt_fseek(fp,0,SEEK_END);
    fs = crt_ftell(fp);
    crt_fseek(fp,0,SEEK_SET);
    
    filebase_add_block(self,fs);
    filebase_seek(self,0);
    
    do{
        old_off = crt_ftell(fp);
        crt_fread(fp,buf,buf_size);
        rs = (int_ptr_t)(crt_ftell(fp) - old_off);
        if(rs > 0)
        {
            filebase_write(self,buf,rs);
        }
    }while(rs > 0);
    
    filebase_seek(self,0);
    return OK;
}

status_t filebase_load_file(struct file_base *self,const char *fn)
{
    FILE_HANDLE fp = crt_fopen(fn,"rb");
    if(!crt_is_file_handle(fp))
    {
        return ERROR;
    }
    filebase_load_file_from_fp(self,fp);
    crt_fclose(fp);
    return OK;
}

int filebase_get_utf8(struct file_base *self,char *utf8)
{    
    uint8_t b0 = (uint8_t)filebase_getc(self);
    utf8[0] = b0;
    
    ASSERT(utf8);

    if(b0 < 0x80)
    {       
        utf8[1] = 0;
        return 1;
    }
    else
    {
        if((b0 & 0xe0)==0xc0)
        {
            utf8[1] = (uint8_t)filebase_getc(self);
            utf8[2] = 0;
            return 2;
        }
        else
        {               
            if((b0 & 0xf0)==0xe0)
            {
                utf8[1] = (uint8_t)filebase_getc(self);
                utf8[2] = (uint8_t)filebase_getc(self);
                utf8[3] = 0;
                return 3;
            }
            else
            {
                utf8[1] = (uint8_t)filebase_getc(self);
                utf8[2] = (uint8_t)filebase_getc(self);
                utf8[3] = (uint8_t)filebase_getc(self);
                utf8[4] = 0;
                return 4;
            }
        }
    }
    
    return 0;
}

int filebase_get_gb(struct file_base *self,char *letter)
{    
    char ch;
    int ret = 0;
    
    ASSERT(letter);

    ch = filebase_getc(self);
    if((uint8_t)ch < 128)
    {
        letter[0] = ch;
        letter[1] = 0;
        ret = 1;
    }
    else
    {
        letter[0] = ch;
        letter[1] = filebase_getc(self);
        ret = 2;
    }
    
    letter[2] = 0;
    return ret;
}

char filebase_char_at(struct file_base *self,fsize_t index)
{
    fsize_t old;
    char ch = 0;
    old = filebase_get_offset(self);
    filebase_seek(self,index);
    ch = filebase_getc(self);
    filebase_seek(self,old);
    return ch;
}

status_t filebase_set_char(struct file_base *self,fsize_t index,char ch)
{
    fsize_t old;
    
    old = filebase_get_offset(self);
    filebase_seek(self,index);
    filebase_putc(self,ch);
    filebase_seek(self,old);
    return OK;
}

fsize_t filebase_read_word_with_empty_char(struct file_base *self,struct file_base *file)
{
    fsize_t k = 0;
    char ch;
    
    ASSERT(file);
    
    filebase_set_size(file,0);
    
    if(filebase_is_end(self))
        return k;
    
    ch = filebase_getc(self);
    if(ch == 0)return k;
    
    if(filebase_is_split_char(self,ch))
    {
        filebase_putc(file,ch);
        k = 1;
        return k;
    }
    while(!filebase_is_split_char(self,ch))
    {
        filebase_putc(file,ch);
        k++;
        if(filebase_is_end(self))
            break;
        ch = filebase_getc(self);
    }
    if(filebase_is_split_char(self,ch))
        filebase_un_getc(self);
    return k;
}

fsize_t filebase_puts(struct file_base *self,const char *str)
{
    if(str == NULL)
        return ERROR;    
    return filebase_write(self,str,crt_strlen(str));
}

status_t filebase_read_line(struct file_base *self,struct file_base *file)
{
    char ch;
    
    ASSERT(file);

    if(filebase_is_end(self)) 
        return ERROR;
    filebase_set_size(file,0);       
    ch = filebase_getc(self);
    while(ch != '\n')
    {
        if(ch!='\r')
            filebase_putc(file,ch);
        if(filebase_is_end(self))
            break;
        ch = filebase_getc(self);
    }
    return OK;
}

status_t filebase_read_line_with_eol(struct file_base *self,struct file_base *file,struct file_base *eol)
{
    char ch;
    
    ASSERT(file && eol);
    
    if(filebase_is_end(self)) 
        return ERROR;
    
    filebase_set_size(eol,0);
    filebase_set_size(file,0);   
    
    while(!filebase_is_end(self))
    {
        ch = filebase_getc(self);
        if(ch == '\n')
        {
            filebase_putc(eol,ch);
            break;
        }
        if(ch=='\r')
        {
            filebase_putc(eol,ch);
        }
        else
        {
            filebase_putc(file,ch);
        }
    }
    return OK;
}

status_t filebase_is_empty_char(char ch)
{
    return ch == ' '|| ch == 0  ||  ch == '\r' || ch == '\n' || ch == '\t';
}

char filebase_get_last_char(struct file_base *self,bool_t empty_included)
{
    fsize_t old_off;
    char ch;
    
    old_off = filebase_get_offset(self);
    filebase_seek(self,filebase_get_size(self) - 1);
    ch =  filebase_getc(self);
    if(!empty_included)
    {
        while(filebase_is_empty_char(ch))
        {
            ch = filebase_un_getc(self);
            if(filebase_get_offset(self) <= 0)
                break;
        }
    }
    filebase_seek(self,old_off);
    return ch;
}


//if the first char is special char, the word_only is ignored
fsize_t filebase_search_str(struct file_base *self,const char *str, status_t case_sensive,status_t word_only)
{
    const char *s2;
    char ch1,ch2;
    fsize_t old,pos,len;
    fsize_t flag = 0;
    
    if(str == NULL) return -1;
    len = crt_strlen(str);
    while(!filebase_is_end(self))
    {
        old = filebase_get_offset(self);        
        ch1 = filebase_getc(self);
        s2 = str;
        ch2 = *s2;
        if(!case_sensive)
        {
            ch1 = crt_tolower(ch1);
            ch2 = crt_tolower(ch2);
        }
        while (ch2 && !(ch1 - ch2)) 
        {
            s2 ++;
            ch2 = *s2;
            
            if(filebase_is_end(self))
            {flag = 1;break;}
            
            ch1=filebase_getc(self);
            if(!case_sensive)
            {
                ch1 = crt_tolower(ch1);
                ch2 = crt_tolower(ch2);
            }
        }
        if(!*s2)
        {
            pos = filebase_get_offset(self) - len - 1;
            if(flag) pos++; //if hit end
            
            if(!word_only)
            {
                return pos;
            }
            else if(filebase_is_split_char(self,filebase_char_at(self,pos)))
            {
                return pos;
            }
            else
            {
                if((pos == 0||filebase_is_split_char(self,filebase_char_at(self,pos - 1)))
                    &&filebase_is_split_char(self,filebase_char_at(self,pos + len)))
                {
                    return pos;
                }
            }
            
        }
        filebase_seek(self,old + 1);
    }
    return -1;
}

status_t filebase_replace_str(struct file_base *self,const char *src_str, const char *des_str, status_t case_sensive, status_t word_only,struct file_base *new_file)
{
    fsize_t pos,old_pos,len1;
    
    ASSERT(new_file);
    ASSERT(new_file != self);
    
    len1 = crt_strlen(src_str);
    filebase_set_size(new_file,0);
    old_pos = filebase_get_offset(self);
    if(old_pos > 0)
    {
        filebase_write_to_file_from(self,new_file,0,old_pos);
    }
    
    filebase_seek(self,old_pos);
    while(!filebase_is_end(self))
    {
        pos = filebase_search_str(self,src_str,case_sensive,word_only);
        if(pos < 0)
        {
            filebase_write_to_file_from(self,new_file,old_pos,filebase_get_size(self) - old_pos);
            break;
        }
        else
        {
            filebase_write_to_file_from(self,new_file,old_pos,pos - old_pos);
            filebase_puts(new_file,des_str);
            old_pos = pos + len1;
            filebase_seek(self,old_pos);
        }
    }
    return OK;
}


status_t filebase_read_quote_str(struct file_base *self,char escape_char,char end_char,struct file_base *file)
{
    char ch;
    ASSERT(file);
    
    filebase_set_size(file,0);
    while(!filebase_is_end(self))
    {
        ch = filebase_getc(self);
        
        if(ch == escape_char) //escape char
        {
            filebase_putc(file,ch);
            ch = filebase_getc(self);
            filebase_putc(file,ch);
        }
        else if(ch == end_char)
        {
            break;
        }
        else
        {
            filebase_putc(file,ch);
        }
    }
    return OK;
}

status_t filebase_read_cstr(struct file_base *self,struct file_base *file)
{
    return filebase_read_quote_str(self,'\\','\"',file);
}

status_t filebase_end_with_file(struct file_base *self,struct file_base *file_str, status_t case_sensive, status_t skip_empty)
{    
    fsize_t i,len1,len2;
    unsigned char c1,c2;

    ASSERT(file_str);
    
    len1 = filebase_get_size(self);
    len2 = filebase_get_size(file_str);

    filebase_seek(self,len1);
    filebase_seek(file_str,len2);
    
    if(skip_empty)
    {
        while(len1 >= 0)
        {
            c1 = filebase_un_getc(self);
            if(!filebase_is_empty_char(c1))
            {
                filebase_getc(self);
                break;
            }
            
            len1 --;
        }
    }
    
    if(len1 <= 0 || len2 <= 0 || len1 < len2)
        return FALSE;
    
    for(i = len2 - 1; i >= 0; i--)
    {
        c1 = filebase_un_getc(self);
        c2 = filebase_un_getc(file_str);
        
        if(!case_sensive)
        {
            c1 = crt_tolower(c1);
            c2 = crt_tolower(c2);
        }
        
        if(c1 != c2)
            return FALSE;
    }
    
    return TRUE;
}

status_t filebase_end_with(struct file_base *self,const char *str, status_t case_sensive, status_t skip_empty)
{
    status_t ret;
    struct mem mem;
    mem_init(&mem);
    mem_set_str(&mem,str);
    ret = filebase_end_with_file(self,&mem.base_file_base,case_sensive,skip_empty);
    mem_destroy(&mem);
    return ret;
}

status_t filebase_skip_empty_chars(struct file_base *self)
{
    char ch;
    while(!filebase_is_end(self))
    {
        ch = filebase_getc(self);
        if(!filebase_is_empty_char(ch) )
        {
            filebase_un_getc(self);
            break;
        }
    }
    return OK;
}

status_t filebase_start_with_file(struct file_base *self,struct file_base *file_str, status_t case_sensive, status_t skip_empty)
{       
    fsize_t i,len;
    unsigned char c1,c2;

    ASSERT(file_str);
    
    filebase_seek(self,0);
    if(skip_empty)
    {
        filebase_skip_empty_chars(self);
    }
    
    len = filebase_get_size(file_str);
    if(filebase_get_size(self) < len)
    {
        return FALSE;
    }

    filebase_seek(file_str,0);
    for(i = 0; i < len; i++)
    {
        c1 = filebase_getc(self);
        c2 = filebase_getc(file_str);
        if( ! case_sensive)
        {
            c1 = crt_tolower(c1);
            c2 = crt_tolower(c2);
        }
        
        if(c1 != c2)
            return FALSE;       
    }
    
    return TRUE;
}

status_t filebase_start_with(struct file_base *self,const char *str, status_t case_sensive, status_t skip_empty)
{
    status_t ret;
    struct mem mem;
    mem_init(&mem);
    mem_set_str(&mem,str);
    ret = filebase_start_with_file(self,&mem.base_file_base,case_sensive,skip_empty);
    mem_destroy(&mem);
    return ret;
}

status_t filebase_trim(struct file_base *self)
{
    fsize_t i,l,n;
    
    l = filebase_get_size(self);
    for(i = l - 1; i >= 0; i--)
    {
        if(!filebase_is_empty_char(filebase_char_at(self,i)))
            break;
    }
    l = i + 1;
    for(i = 0; i < l; i++)
    {
        if(!filebase_is_empty_char(filebase_char_at(self,i)) )
            break;
    }
    l -= i;
    n = i;
    for(i = 0; i < l; i++)
    {
        filebase_set_char(self,i,filebase_char_at(self,i+n));
    }
    filebase_set_size(self,l);
    return OK;
}

fsize_t filebase_read_word(struct file_base *self,struct file_base *file)
{
    filebase_skip_empty_chars(self);
    return filebase_read_word_with_empty_char(self,file);
}

status_t filebase_dump(struct file_base *self)
{
    fsize_t save_off;
    C_LOCAL_MEM(mem);
               
    save_off = filebase_get_offset(self);
    filebase_seek(self,0);
    
    while(filebase_read_line(self,mem_file))
    {
        syslog_printf("%s",mem_cstr(&mem));
    }
    
    filebase_seek(self,save_off);
    return OK;
}

fsize_t filebase_printf(struct file_base *self,const char *sz_format, ...)
{
    MAKE_VARGS_BUFFER(sz_format);
    return filebase_puts(self,szBuffer);
}

fsize_t filebase_log(struct file_base *self,const char *sz_format, ...)
{   
    MAKE_VARGS_BUFFER(sz_format);    
    filebase_puts(self,szBuffer);
    filebase_puts(self,"\r\n");
    return OK;
}

status_t filebase_read_string(struct file_base *self, struct file_base *file)
{
    fsize_t k;
    k = filebase_read_word(self,file);
    while(k)
    {
        filebase_seek(file,0);
        if(!filebase_is_split_char(self,filebase_getc(file)))   
        {
            return OK;
        }
        k = filebase_read_word(self,file);
    }
    return ERROR;
}

