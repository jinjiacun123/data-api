#-*- coding:utf-8 -*-

import struct
import binascii
import json
import sys
reload(sys)
sys.setdefaultencoding("utf-8")

def test():
    values = (1, 'abc', 2.7)
    s = struct.Struct('I3sf')
    packed_data = s.pack(*values)
    unpacked_data = s.unpack(packed_data)

    print 'Original values:', values
    print 'Format string:', s.format
    print 'Uses:', s.size,'bytes'


#make struct
def make_struct():
    pass

#deal json
def deal_json():
    pass


if __name__ == '__main__':
    filepath = "./txt/1101.txt";
    datafile = "./txt/my_data"
    with open(datafile.decode('utf-8'), "a") as my_data:
        with open(filepath.decode('utf-8'), "r") as f:
            data = json.load(f)
            for item in data['list']:
                s = struct.pack("ii6s", 
                                int(item["code_type"]), 
                                int(item["preclose"]),
                                str(item["code"]))

                my_data.write(s)
                print item,'\n'
