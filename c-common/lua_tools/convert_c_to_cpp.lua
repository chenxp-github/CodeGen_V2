--分隔符--
split_chars=" \t\r\n!@#$%^=&*()|+-\\*/{}[];.\":\',?/<>~";

--选项 1:不包括符合，2:包括符合, 3:包括符合和空字符
option = 3;


function is_camel_case_string(mem)
	local lower_case = 0;
	local upper_case = 0;

	for i=0,mem:GetSize()-1,1 do
		local c = mem:C(i);
		if c >= 97 and c <= 122 then
			lower_case = lower_case + 1;
		end

		if c >= 65 and c <= 90 then
			upper_case = upper_case + 1;
		end			
	end

	return (lower_case > 0 and upper_case > 0);
end


function print_other(mem,cur_str)
	if not FileBase.IsEmptyChar(mem:C(0)) then	
		if is_camel_case_string(mem) then
			print(to_lower_underline_case(cur_str));
		else
			print(cur_str);	
		end
	else
		print(cur_str);
	end
end

mem_text:SetSplitChars(split_chars);
for_each(mem_text, option, function(mem)
	local cur_str = mem:CStr();
	local old_off = mem_text:GetOffset();
	local hit = false;	
	
	local next_1 = mem_text:NextWord();
	local next_2 = mem_text:NextWord();

	if next_1 == "-" and next_2 == ">" then		
		
		local func_name = mem_text:NextString();
		local bracket = mem_text:NextWord();

		local comma=",";
		local old_off_1 = mem_text:GetOffset();		
		if mem_text:NextWord() == ")" then
			comma = "";
		end
		mem_text:Seek(old_off_1);

		if bracket == "(" then			
			hit = true;
			
			printf("filebase_%s(%s%s", 
				to_lower_underline_case(func_name),		
				cur_str,comma);
		end
	end
	
	
	if not hit then
		mem_text:Seek(old_off);		
		print_other(mem,cur_str);
	end

end);

