#coding=gbk
import sys
import os
import json
import struct
import codecs
import urllib
from optparse import OptionParser
import struct
reload(sys)
sys.setdefaultencoding('gbk')
#print sys.getdefaultencoding()
#write struct file
def write_struct_file(file_name, content):
        with open(file_name, "a") as my_data:
                my_data.write(content)
        my_data.close()

#init code file
def init(file_dir):
    #get data file
    data_file = './my_data'
    fp = open(data_file, 'r')
    '''
    with codecs.open(data_file, encoding='gbk') as f:
        content = f.read()
        f.close()
    '''
    while True:
        line = fp.read(14)
        if len(line) == 0:
            break;
        #parse data
        (code_type, pre_close, code) = struct.unpack('ii6s', line)
        file_name = "%s/%s.data" % (code_type,code)
	my_dir = "%s/%s" % ("./5min", code_type)
	if not os.path.exists(my_dir):
		os.system("mkdir %s" % (my_dir))
        os.system("touch ./5min/%s"%(file_name))
       # print "code_type:%s,code:%s\n" % (code_type, code)
    fp.close()
    #create file
    print 'init'

#clean before five minit data
def clean_old_by_dir(file_dir):
    print 'clean_old_by_dir'

def clean_old_by_code(file_dir, code_type, code):
    print 'clean_old_by_code'

def add_code(file_dir,
             code_type,
             code,
             price,
             time):
    print 'add_code code_type:%s,code:%s' % (code_type,code)
    file_name = "%s%s/%s.data" % (file_dir, code_type, code)
    s = struct.pack("iii", price, time)
    write_struct_file(file_name, s)

def show_code(file_dir, code_type, code):
    print "code_type:%s, code:%s" % (code_type, code)
    file_name = "%s%s/%s.data" % (file_dir, code_type, code)
    fp = open(file_name, "r")
    while True:
        line = fp.read(8)
        if len(line) == 0:
            break
        (price, time) = struct.unpack('ii', line)
        print "price:%d,time:%d" % (price, time)

def main():
        file_dir = "./5min/"
        parser = OptionParser()
        parser.add_option("-i", "--init",
                          action = "store_true",
                          dest = "init",
                          default = False,
                          help = "init current day code and create file")
        parser.add_option("-a", "--add",
                          action = "store",
                          dest = "add",
                          default = False,
                          nargs = 4,
                          help = "add code_type code price")
        parser.add_option("-s", "--show",
                          action = "store",
                          dest = "show",
                          default = False,
                          nargs = 2,
                          help = "show code array info")
        (options, args) = parser.parse_args()
        if options.init == True:
                init(file_dir)
        if options.add != False:
                add_code(file_dir,
                         int(options.add[0]),
                         options.add[1],
                         int(options.add[2]), #price
                         int(options.add[3])  #time
                )
        if options.show != False:
                show_code(file_dir,
                          int(options.show[0], 16),
                          options.show[1])

if __name__ == '__main__':
    main()
