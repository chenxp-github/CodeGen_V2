# Microsoft Developer Studio Project File - Name="dos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dos - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dos.mak" CFG="dos - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dos - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dos - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dos - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "common" /I "platform" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "dos - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "common" /I "platform" /I "template" /I "c-common" /I "c-template" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "dos - Win32 Release"
# Name "dos - Win32 Debug"
# Begin Source File

SOURCE=".\c-template\basic_queue.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\basic_queue.h"
# End Source File
# Begin Source File

SOURCE=".\c-template\basic_stack.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\basic_stack.h"
# End Source File
# Begin Source File

SOURCE=.\common\basicarray.h
# End Source File
# Begin Source File

SOURCE=.\template\basicqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\template\basicqueue.h
# End Source File
# Begin Source File

SOURCE=.\template\basicstack.cpp
# End Source File
# Begin Source File

SOURCE=.\template\basicstack.h
# End Source File
# Begin Source File

SOURCE=".\c-common\c_closure.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_closure.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_closure_c89.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_closure_list.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_closure_list.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_comb_file.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_comb_file.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_epoll.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_epoll.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file_base.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file_base.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file_manager.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_file_manager.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_log_buffer.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_log_buffer.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem_file.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem_file.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem_stk.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mem_stk.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mutex.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_mutex.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_part_file.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_part_file.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_simple_link_service.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_simple_link_service.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_socket.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_socket.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_socket_reader_writer.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_socket_reader_writer.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc_reader.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc_reader.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc_writer.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_link_rpc_writer.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_runner.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_runner.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_tcp_acceptor.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_tcp_acceptor.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_tcp_connector.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_tcp_connector.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_timer.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_task_timer.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_taskmgr.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_taskmgr.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_tcp_client.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_tcp_client.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_tcp_server.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_tcp_server.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_thread.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_thread.h"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_weak_ptr.c"
# End Source File
# Begin Source File

SOURCE=".\c-common\c_weak_ptr.h"
# End Source File
# Begin Source File

SOURCE=.\common\closure.cpp
# End Source File
# Begin Source File

SOURCE=.\common\closure.h
# End Source File
# Begin Source File

SOURCE=.\common\closurelist.cpp
# End Source File
# Begin Source File

SOURCE=.\common\closurelist.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\platform\crt_win32.h
# End Source File
# Begin Source File

SOURCE=.\common\cruntime.c
# End Source File
# Begin Source File

SOURCE=.\common\cruntime.h
# End Source File
# Begin Source File

SOURCE=.\common\dirmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dirmgr.h
# End Source File
# Begin Source File

SOURCE=.\common\encoder.cpp
# End Source File
# Begin Source File

SOURCE=.\common\encoder.h
# End Source File
# Begin Source File

SOURCE=.\common\file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\file.h
# End Source File
# Begin Source File

SOURCE=.\common\filebase.cpp
# End Source File
# Begin Source File

SOURCE=.\common\filebase.h
# End Source File
# Begin Source File

SOURCE=.\common\fileinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileinfo.h
# End Source File
# Begin Source File

SOURCE=.\common\fileinfolist.cpp
# End Source File
# Begin Source File

SOURCE=.\common\fileinfolist.h
# End Source File
# Begin Source File

SOURCE=".\c-template\hash_table.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\hash_table.h"
# End Source File
# Begin Source File

SOURCE=.\template\hashtable.cpp
# End Source File
# Begin Source File

SOURCE=.\template\hashtable.h
# End Source File
# Begin Source File

SOURCE=".\c-template\linked_list.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\linked_list.h"
# End Source File
# Begin Source File

SOURCE=.\template\linkedlist.cpp
# End Source File
# Begin Source File

SOURCE=.\template\linkedlist.h
# End Source File
# Begin Source File

SOURCE=".\c-common\linkrpccommon.h"
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mem.cpp
# End Source File
# Begin Source File

SOURCE=.\common\mem.h
# End Source File
# Begin Source File

SOURCE=.\common\mem_tool.c
# End Source File
# Begin Source File

SOURCE=.\common\mem_tool.h
# End Source File
# Begin Source File

SOURCE=.\common\memfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memfile.h
# End Source File
# Begin Source File

SOURCE=.\common\memstk.cpp
# End Source File
# Begin Source File

SOURCE=.\common\memstk.h
# End Source File
# Begin Source File

SOURCE=.\common\minibson.cpp
# End Source File
# Begin Source File

SOURCE=.\common\minibson.h
# End Source File
# Begin Source File

SOURCE=.\common\misc.c
# End Source File
# Begin Source File

SOURCE=.\common\misc.h
# End Source File
# Begin Source File

SOURCE=".\c-template\normal_queue.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\normal_queue.h"
# End Source File
# Begin Source File

SOURCE=".\c-template\normal_stack.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\normal_stack.h"
# End Source File
# Begin Source File

SOURCE=.\template\normalqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\template\normalqueue.h
# End Source File
# Begin Source File

SOURCE=.\template\normalstack.cpp
# End Source File
# Begin Source File

SOURCE=.\template\normalstack.h
# End Source File
# Begin Source File

SOURCE=.\common\raw_weak_pointer.cpp
# End Source File
# Begin Source File

SOURCE=.\common\raw_weak_pointer.h
# End Source File
# Begin Source File

SOURCE=.\common\syslog.c
# End Source File
# Begin Source File

SOURCE=.\common\syslog.h
# End Source File
# Begin Source File

SOURCE=".\c-template\task_test.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\task_test.h"
# End Source File
# Begin Source File

SOURCE=.\common\taskmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\common\taskmgr.h
# End Source File
# Begin Source File

SOURCE=.\template\tasktest.cpp
# End Source File
# Begin Source File

SOURCE=.\template\tasktest.h
# End Source File
# Begin Source File

SOURCE=.\common\tcp.cpp
# End Source File
# Begin Source File

SOURCE=.\common\tcp.h
# End Source File
# Begin Source File

SOURCE=".\c-template\test_node.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\test_node.h"
# End Source File
# Begin Source File

SOURCE=".\c-template\test_thread.c"
# End Source File
# Begin Source File

SOURCE=".\c-template\test_thread.h"
# End Source File
# Begin Source File

SOURCE=.\template\testnode.cpp
# End Source File
# Begin Source File

SOURCE=.\template\testnode.h
# End Source File
# Begin Source File

SOURCE=.\template\testtree.cpp
# End Source File
# Begin Source File

SOURCE=.\template\testtree.h
# End Source File
# Begin Source File

SOURCE=.\common\userfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\common\userfunc.h
# End Source File
# Begin Source File

SOURCE=.\common\weakptr.h
# End Source File
# Begin Source File

SOURCE=.\common\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xml.h
# End Source File
# Begin Source File

SOURCE=.\common\xmlreader.cpp
# End Source File
# Begin Source File

SOURCE=.\common\xmlreader.h
# End Source File
# End Target
# End Project
