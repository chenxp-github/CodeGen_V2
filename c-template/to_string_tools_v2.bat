SET STRING_TOOLS_V2_PATH=..\..\StringToolsV2

SET STACK_PATH=%STRING_TOOLS_V2_PATH%\moon-tools\codegen-c\stack\

copy normal_stack.* %STACK_PATH%
copy basic_stack.* %STACK_PATH%
copy fixed_stack.* %STACK_PATH%

SET HASH_MAP_PATH=%STRING_TOOLS_V2_PATH%\moon-tools\codegen-c\hash-map\

copy hash_table.* %HASH_MAP_PATH%

SET QUEUE_PATH=%STRING_TOOLS_V2_PATH%\moon-tools\codegen-c\queue

copy normal_queue.* %QUEUE_PATH%
copy basic_queue.* %QUEUE_PATH%

SET LIST_PATH=%STRING_TOOLS_V2_PATH%\moon-tools\codegen-c\linked-list
copy linked_list.* %LIST_PATH%

