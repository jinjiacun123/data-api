#coding=gbk
import sys
import os
import json
import struct
import codecs
import urllib
reload(sys)
sys.setdefaultencoding('gbk')

#print sys.getdefaultencoding()

def get_content(file_name):
	with codecs.open(file_name, encoding='gbk') as f:
		value = f.read()
                f.close()
                return value

#write json to local
def set_json_to_local(file_name, json_str):
	with codecs.open(file_name,"w", encoding='gbk') as f:
		f.write(json_str)
                f.close()

#get json content
def get_json_content(file_name):
	template_str = "http://%s:%d/initinfo/stock/%s"
	host         = "dsapp.yz.zjwtj.com"
	port         = 8010
	url          = template_str %(host, port, file_name)
	f            = urllib.urlopen(url)
	s            = f.read()
        f.close()
	return s

#clean struct file
def clean_struct_file(file_name):
	if os.path.exists(file_name):
		os.remove(file_name)

#write struct file
def write_struct_file(file_name, content):
        with open(file_name, "a") as my_data:
                my_data.write(content)

#json to struct
def change_json_to_struct(json_str, data_file):
        #parse json
        json_obj = json.loads(json_str)
        code_type = int(json_obj["codetype"], 16)
        for item in json_obj["list"]:
                #change to struct
                s = struct.pack("ii6s",
                                code_type,
                                int(item["preclose"]),
                                str(item["code"]))
                write_struct_file(data_file, s)

#main
def main():
        dir_prefix = "/home/jim/source_code/data-api/"
        template_json = "%s.txt"
        template_data = dir_prefix + "proxy/sort_raise_ok/txt/%s.txt"
        file_name_list = ["1101", "1201", "1206", "120b"]
	#["1101", "1201", "1206", "120b"]
	data_file_name = dir_prefix + "proxy/sort_raise_ok/txt/my_data"

	#get source
	for file_name in file_name_list:
		content = get_json_content(template_json % file_name)
                set_json_to_local(template_data % file_name, content)
	#clean struct file
	clean_struct_file(data_file_name)

	#chang to struct
	for file_name in file_name_list:
		content = get_content(template_data % file_name)
                change_json_to_struct(content, data_file_name)

	print "success\n"

if __name__ == "__main__":
        main()
