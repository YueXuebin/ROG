#!/usr/bin/env python
#coding=utf-8

import sys
import os

import re
import shutil
#import codecs

from BeautifulSoup import BeautifulSoup
from tornado import template

num = 0

DEBUG =True

os.chdir(os.path.dirname(os.path.abspath(__file__)))

# ==================================================
# Define output file path
# ==================================================

# Cpp files define
# --------------------------------------------------
file_in = 'message_protocol.xml'

ccfile_h_out = 'message_encoder.h'
ccfile_cpp_out = 'message_encoder.cpp'

# As files define
# --------------------------------------------------
msgdir_out = 'msgvo'

asfile_handler_out = 'IMessageHandler.as'
asfile_controller_out = 'MessageController.as'

# ==================================================
# Define output file path
# ==================================================
PREFIXES = dict (
    char    = "ch",
    byte    = "by",
    short   = "sh",
    word    = "w",
    int     = "n",
    dword   = "dw",
    int64   = "ll",
    uint64  = "qw",
    float   = "f",
    string  = "sz",
)

CPP_TYPE_MAP = dict (
    byte    = "unsigned char",
    char    = "char",
    short   = "short",
    word    = "unsigned short",
    int     = "int",
    dword   = "unsigned int",
    int64   = "long long",
    uint64  = "usigned long long",
    float   = "float",
)

CPP_CODE_MAP = dict (
    byte    = "_uint8",
    char    = "_int8",
    short   = "_int16",
    word    = "_uint16",
    int     = "_int32",
    dword   = "_uint32",
    int64   = "_int64",
    uint64  = "_uint64",
    float   = "_float",
    double  = "_double",
)

CPP_CODE_LEN_MAP = dict (
    byte    = "sizeof(uint8)",
    char    = "sizeof(int8)",
    short   = "sizeof(int16)",
    word    = "sizeof(uint16)",
    int     = "sizeof(int32)",
    dword   = "sizeof(uint32)",
    int64   = "sizeof(int64)",
    uint64  = "sizeof(uint64)",
    float   = "sizeof(float)",
    double  = "sizeof(double)",
)

AS_TYPE_MAP = dict (
    byte    = "uint",
    char    = "int",
    short   = "int",
    word    = "uint",
    int     = "int",
    dword   = "uint",
    float   = "Number",
)

AS_ENCODE_TYPE_MAP = dict (
    byte    = "Byte",
    char    = "Byte",
    short   = "Short",
    word    = "Short",
    int     = "Int",
    dword   = "UnsignedInt",
    float   = "Float",
)

AS_DECODE_TYPE_MAP = dict (
    byte    = "Byte",
    char    = "Byte",
    short   = "Short",
    word    = "UnsignedShort",
    int     = "Int",
    dword   = "UnsignedInt",
    float   = "Float",
)

if not os.access(file_in, os.F_OK):
    print 'Input file "%s" does not exist.' % file_in
    sys.exit(1)

# 解析协议xml
xml = BeautifulSoup(open(file_in, 'rb').read())



def check_valid():		# 检查xml的合法性
    # 检查id不重复
    id_list = []
    for m in xml.messages.findAll('message'):
        if id_list.count(m['id']) > 0 :
            print "id "+m['id']+" already exist"
            return False
        id_list.append(m['id'])
    return True

def output(content, filename=None):
    if filename and '--savetofiles' in sys.argv:
        open(filename, 'wb').write(content)
    else:
        print content

def nonprefix_varname(typename, name):
    return name

def prefix_varname(typename, varname):
    #assert typename in PREFIXES, 'Invalid type %s' % typename
    if PREFIXES.has_key(typename):
        return PREFIXES[typename] + varname
    else:
        return varname;

def type_item_clear(typename, varname):
    if typename == 'string':
        return '%s = "";' % (varname)

    if CPP_TYPE_MAP.has_key(typename):
        return '%s = 0;' % (varname)
    else:
        if xml.types.find(typename) == False:
            print "undefine %s", typename
        return '%s.Clear();' % (varname)

def type_def_item(typename, varname, length=None):
    #assert typename in PREFIXES, 'Invalid type %s' % typename

    prefixed_varname = prefix_varname(typename, varname)
    if typename == 'string':
        return '%s\t%s' % ('std::string', varname)

    #change
    if CPP_TYPE_MAP.has_key(typename):
        return '%s\t%s' % (CPP_TYPE_MAP[typename], varname)
    else:
        if xml.types.find(typename) == False:
            print "undefine %s", typename
        return '%s\t%s' % (typename, varname)

def type_def_array(typename, varname, length=None):
    #assert typename in PREFIXES, 'Invalid type %s' % typename

    prefixed_varname = prefix_varname(typename, varname)
    if typename == 'string':
        return 'std::vector<%s>\t%s' % ('std::string', varname)

    #change
    if CPP_TYPE_MAP.has_key(typename):
        return 'std::vector<%s>\t%s' % (CPP_TYPE_MAP[typename], varname)
    else:
        if xml.types.find(typename) == False:
            print "undefine %s", typename
        return "std::vector<%s>\t%s" % (typename, varname)

def get_as_type(name):
    if name == 'string':
        return 'String'
    if AS_TYPE_MAP.has_key(name):
        return AS_TYPE_MAP[name]
    return name

def def_parser_func():    
    code = ''    
    for i in xml.messages.findAll('message'):
        num = [0]
        code += 'int Encode%s(uint64 playerID, void* data, Net_Packet* packet)\n{\n' % i['name']
        code += '    %s* pstIn = (%s*)data;\n' % (i['type'], i['type'])
        code += '    packet->write_uint16(0x4744);\n'
        code += '    Bookmark bmLen = packet->getcurpos();\n'
        code += '    packet->write_uint32(0);\n'
        code += '    packet->write_uint64(playerID);\n'
        code += '    packet->write_uint32(%s);\n' % i['name']
        t = xml.types.find('type', attrs={'name':i['type']})
        if t != None:
            code += cc_encode_def_func(t, '', '', 1, num)
        code += '    packet->write_uint16(0x4641);\n'
        code += '    packet->write_uint32(packet->getdatalen(), bmLen);\n'
        code += '    return packet->getdatalen();\n'
        code += '}\n'

        num = [0]
        code += 'int Decode%s(uint64 playerID, uint32 sessionID, Net_Packet* packet, void* userdata, uint32 type)\n{\n' % i['name']
        code += '    %s recvmsg;\n' % (i['type'])
        t = xml.types.find('type', attrs={'name':i['type']})
        if t != None:
            code += cc_decode_def_func(t, '', '', 1, num)
        code += '    if(USERDATA_TYPE_TRANSFER == type)\n'
        code += '    {\n'
        code += '        ((TransferProxy*)userdata)->Recv(playerID, sessionID, %s,&recvmsg);\n' % (i['name'])
        code += '    }\n'
        code += '    else if(USERDATA_TYPE_FUNC == type)\n'
        code += '    {\n'
        code += '        ((EncodeCallback)userdata)(playerID, sessionID, %s, &recvmsg, 0);\n' % (i['name'])
        code += '    }\n'
        code += '    return packet->getdatalen();\n'
        code += '}\n'

    return code

# ==================================================
# C++ Code Encode
# ==================================================
def cc_encode_def_func(t, code, prename, loop, num):
    sys.stdout.write('-')
    wspace = ''
    for i in range(0, loop):
        wspace += '    '

    code += '\n'

    for i in t.findAll(['item','array']):
        if i.name == 'item':
            if i['type'] == 'string':
                code += wspace
                code += 'packet->write_string(pstIn->%s);\n' % (prename + i['name'])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + '.'
                    code = cc_encode_def_func(m, code, newprename, loop, num)
                else:
                    code += wspace
                    code += 'packet->write%s(pstIn->%s);\n' % (CPP_CODE_MAP[i['type']], prename + i['name'])
        else:
            num[0] += 1
            count = ''

            c = xml.defines.find('define', attrs={'name':i['count']})
            if(c != None):
                count = i['count']
            else:
                count = 'pstIn->' + prename + i['count']

            code += wspace + 'for( int i%s=0; i%s<%s; ++i%s)\n' % (num[0], num[0], count, num[0])
            code += wspace + '{\n'
            if i['type'] == 'string':
                code += wspace + '    '
                code += 'packet->write_string(pstIn->%s[i%s]);\n' % (prename + i['name'], num[0])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + ('[i%s].' % num[0])
                    code = cc_encode_def_func(m, code, newprename, loop+1, num)
                else:
                    code += wspace + '    '
                    n = xml.defines.find('define', attrs={'name':i['type']})
                    if(n != None):
                        code += 'packet->write%s(pstIn->%s[i%s]);\n' % (CPP_CODE_MAP[i['type']], i['name'], num[0])
                    else:
                        code += 'packet->write%s(pstIn->%s[i%s]);\n' % (CPP_CODE_MAP[i['type']], prename + i['name'], num[0])
            code += wspace + '}\n'

    return code

# ==================================================
# C++ Code Decode
# ==================================================
def cc_decode_def_func(t, code, prename, loop, num):
    sys.stdout.write('-')
    wspace = ''
    for i in range(0, loop):
        wspace +='    '

    code += '\n'

    for i in t.findAll(['item','array']):
        if i.name == 'item':
            if i['type'] == 'string':
                code += wspace
                code += 'if(packet->read_string(recvmsg.%s) == 0) return -1;\n' % (prename + i['name'])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + '.'
                    code = cc_decode_def_func(m, code, newprename, loop, num)
                else:
                    code += wspace
                    code += 'if(packet->read%s(recvmsg.%s) == 0) return -1;\n' % (CPP_CODE_MAP[i['type']], prename + i['name'])
        else:
            num[0] += 1
            count = ''

            c = xml.defines.find('define', attrs={'name':i['count']})
            if(c != None):
                count = i['count']
            else:
                count = 'recvmsg.' + prename + i['count']
            code += wspace + 'recvmsg.%s.resize(%s);\n' % (prename + i['name'], count)
            code += wspace + 'for( int i%s=0; i%s<%s; ++i%s)\n' % (num[0], num[0], count, num[0])
            code += wspace + '{\n'
            if i['type'] == 'string':
                code += wspace + '    '
                code += 'if(packet->read_string(recvmsg.%s[i%s]) == 0) return -1;\n' % (prename + i['name'], num[0])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + ('[i%s].' % num[0])
                    code = cc_decode_def_func(m, code, newprename, loop+1, num)
                else:
                    n = xml.defines.find('define', attrs={'name':i['type']})
                    if(n != None):
                        code += wspace + '    '
                        code += 'if(packet->read%s(recvmsg.%s[i%s]) == 0) return -1;\n' % (CPP_CODE_MAP[i['type']], i['name'], num[0])
                    else:
                        code += wspace + '    '
                        code += 'if(packet->read%s(recvmsg.%s[i%s]) == 0) return -1;\n' % (CPP_CODE_MAP[i['type']], prename + i['name'], num[0])
            code += wspace + '}\n'

    return code                

# ==================================================
# Cpp context
# ==================================================
cc_context = dict(xml=xml, 
            type_def_item=type_def_item, 
            type_def_array=type_def_array, 
            type_item_clear=type_item_clear,
            def_parser_func=def_parser_func,
            prefix_varname=prefix_varname, 
            nonprefix_varname=nonprefix_varname)

# ==================================================
# as type define
# ==================================================
def as_type_def(typename):
    # Must have been defined in types
    assert typename in PREFIXES, 'Invalid type %s' % typename
    # Convert type name
    if typename == 'string':
        return 'String'
    return '%s' % AS_TYPE_MAP[typename]

def as_msgvo_def(t):
    code = ''
    code += 'package net.socket.%s\n{\n' % (msgdir_out)

    for i in t.findAll(['item','array']):
        n = xml.types.find('type', attrs={'name':i['type']})
        if(n != None):
            code += '    import net.socket.%s.NET_%s;\n' % (msgdir_out, i['type'])

    code += '    public class NET_%s\n    {\n' % t['name']

    for i in t.findAll(['item','array']):
        if i.name == 'item':
            if AS_TYPE_MAP.has_key(i['type']):
                code += '        public var %s: %s;\n' % (i['name'], get_as_type(i['type']))
            else:
                if get_as_type(i['type']) == 'String':
                    code += '        public var %s: %s = new %s();\n' % (i['name'], get_as_type(i['type']), get_as_type(i['type']))
                else:
                    code += '        public var %s: NET_%s = new NET_%s();\n' % (i['name'], get_as_type(i['type']), get_as_type(i['type']))
        else:
            if (AS_TYPE_MAP.has_key(i['type']) or i['type'] == "string"):
                code += '        public var %s: Vector.<%s> = new Vector.<%s>();\n' % (i['name'], get_as_type(i['type']), get_as_type(i['type']))
            else:
                code += '        public var %s: Vector.<NET_%s> = new Vector.<NET_%s>();\n' % (i['name'], get_as_type(i['type']), get_as_type(i['type']))

    code += '    }\n}\n'

    return code;

# ==================================================
# Generate Cpp files
# ==================================================
def generate_cpp_files():
    print " Generate cpp files. "
    # Cpp template files
    H_TMPL = 'lang-tmpls/cpp/h.tmpl'
    CPP_TMPL = 'lang-tmpls/cpp/cpp.tmpl'
    
    t = template.Template(open(H_TMPL, 'rb').read())
    
    #h_out = file_in.lower().split('.')[0] + '.h'
    output(t.generate(**cc_context).replace('\n\n', '\n'), ccfile_h_out)

    t = template.Template(open(CPP_TMPL, 'rb').read())

    #cpp_out = file_in.lower().split('.')[0] + '.cpp'
    output(t.generate(**cc_context).replace('\n\n', '\n'), ccfile_cpp_out)

    return

# ==================================================
# Generate message value object
# ==================================================
def generate_as_msgvo():
    print " Generate message value object. "

    # Remove directory
    if os.path.exists(msgdir_out) == True:
        shutil.rmtree(msgdir_out)

    # Create directory
    os.mkdir(msgdir_out)

    # Create message value object file
    for t in xml.types.findAll('type'):
        filename = '%s/NET_%s.as' % (msgdir_out, t['name'])
        content = as_msgvo_def(t)
        output(content, filename)
    return

# ==================================================
# Generate as message handler interface
# ==================================================
def generate_as_message_handler():
    code = ''
    code = 'package net.socket\n{\n'

    for i in xml.messages.findAll('message'):
        if i['dir'] == 'sc':
            code += '    import net.socket.%s.NET_%s;\n' % (msgdir_out, i['type'])

    code += '    public interface IMessageHandler\n'
    code += '    {\n'
    for i in xml.messages.findAll('message'):
        if i['dir'] == 'sc':
            code += '        function on%s(vo: NET_%s): void;\n' % (i['name'], i['type'])
    code += '    }\n'
    code += '}\n'

    output(code, asfile_handler_out)
    return

# ==================================================
# Generate as message controller
# ==================================================
def generate_as_message_controller():
    CONTROLLER_TMPL = 'lang-tmpls/as/controller.tmpl'
    t = template.Template(open(CONTROLLER_TMPL, 'rb').read())
    output(t.generate(**as_context).replace('\n\n', '\n'), asfile_controller_out)

# ==================================================
# As Code Encode
# ==================================================
def as_encode_def_func(t, code, prename, loop, num):
    sys.stdout.write('-')
    wspace = ''
    for i in range(0, loop):
        wspace += '    '

    code += '\n'

    for i in t.findAll(['item','array']):
        if i.name == 'item':
            if i['type'] == 'string':
                #code += wspace + 'sendmsg.writeShort(%s.length);\n' % (prename + i['name'])
                #code += wspace + 'sendmsg.writeUTFBytes(%s);\n' % (prename + i['name'])
                code += wspace + 'sendmsg.writeUTF(%s);\n' % (prename + i['name'])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + '.'
                    code = as_encode_def_func(m, code, newprename, loop, num)
                else:
                    if (AS_ENCODE_TYPE_MAP.has_key(i['type'])):
                        code += wspace + 'sendmsg.write%s(%s);\n' % (AS_ENCODE_TYPE_MAP[i['type']], prename + i['name'])
                    else:
                        print(" !!!ERROR: undefine type "+i['type'])
        else:
            num[0] += 1
            count = ''

            n = xml.defines.find('define', attrs={'name':i['count']})
            if(n != None):
                count = i['count']
            else:
                count = prename + i['count']

            code += wspace + 'for(var i%s:int=0; i%s<%s; ++i%s)\n' % (num[0], num[0], count, num[0])

            code += wspace + '{\n'
            if i['type'] == 'string':
                #code += wspace + '    sendmsg.writeShort(%s[i%s].length);\n' % (prename + i['name'], num[0])
                #code += wspace + '    sendmsg.writeUTFBytes(%s[i%s]);\n' % (prename + i['name'], num[0])
                code += wspace + '    sendmsg.writeUTF(%s[i%s]);\n' % (prename + i['name'], num[0])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + ('[i%s].' % num[0])
                    code = as_encode_def_func(m, code, newprename, loop+1, num)
                else:
                    if (AS_ENCODE_TYPE_MAP.has_key(i['type'])):
                        code += wspace + '    sendmsg.write%s(%s[i%s]);\n' % (AS_ENCODE_TYPE_MAP[i['type']], prename + i['name'], num[0])
                    else:
                        print(" !!!ERROR: undefine type "+i['type'])
            code += wspace + '}\n'

    return code

# ==================================================
# As Code Decode
# ==================================================
def as_decode_def_func(t, code, prename, loop, num):
    sys.stdout.write('-')
    wspace = ''
    for i in range(0, loop):
        wspace += '    '

    code += '\n'

    for i in t.findAll(['item','array']):
        if i.name == 'item':
            if i['type'] == 'string':
                #code += wspace + 'strlen = recvmsg.readShort();\n'
                #code += wspace + '%s = recvmsg.readUTFBytes(strlen);\n' % (prename + i['name'])
                code += wspace + '%s = recvmsg.readUTF();\n' % (prename + i['name'])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + '.'
                    code = as_decode_def_func(m, code, newprename, loop, num)
                else:
                    if(False == AS_DECODE_TYPE_MAP.has_key(i['type'])):
                        print(" !!!ERROR: undefine type "+i['type'])
                        continue
                    code += wspace + '%s = recvmsg.read%s();\n' % (prename + i['name'], AS_DECODE_TYPE_MAP[i['type']])
        else:
            num[0] += 1
            count = ''

            n = xml.defines.find('define', attrs={'name':i['count']})
            if(n != None):
                count = i['count']
            else:
                count = prename + i['count']

            #code += wspace + '%s = new Vector.<%s>(%s);\n' % (prename + i['name'], get_as_type(i['type']), count)
            code += wspace + 'for(var i%s:int=0; i%s<%s; ++i%s)\n' % (num[0], num[0], count, num[0])

            code += wspace + '{\n'
            if i['type'] == 'string':
                #code += wspace + '    strlen = recvmsg.readShort();\n'
                #code += wspace + '    %s[i%s] = recvmsg.readUTFBytes(strlen);\n' % (prename + i['name'], num[0])
                code += wspace + '    %s[i%s] = recvmsg.readUTF();\n' % (prename + i['name'], num[0])
            else:
                m = xml.types.find('type', attrs={'name':i['type']})
                if(m != None):
                    newprename = prename + i['name'] + ('[i%s].' % num[0])
                    code += wspace + '    %s[i%s] = new NET_%s;\n' % (prename + i['name'], num[0], i['type'])
                    code = as_decode_def_func(m, code, newprename, loop+1, num)
                else:
                    if(False == AS_DECODE_TYPE_MAP.has_key(i['type'])):
                        print(" !!!ERROR: undefine type "+i['type'])
                        continue
                    code += wspace + '    %s[i%s] = recvmsg.read%s();\n' % (prename + i['name'], num[0], AS_DECODE_TYPE_MAP[i['type']])

            code += wspace + '}\n'

    return code

# ==================================================
# Generate as message parser code
# ==================================================
def generate_as_parser_code():
    code = '\n'
    for i in xml.messages.findAll('message'):
        num = [0]
        if i['dir'] == 'cs':
            t = xml.types.find('type', attrs={'name':i['type']})
            if(t == None):
                print(" !!!ERROR: undefine type "+i['type'])
                continue
            code += '        public function send%s(vo: NET_%s): void\n' % (i['name'], i['type'])
            code += '        {\n'
            code += '            if(sendhook) if(!sendhook(%s, vo)) return;\n' % i['name']
            code += '            var sendmsg: ByteArray = new ByteArray();\n'
            code += '            sendmsg.writeShort(0x4744);\n'
            code += '            sendmsg.writeUnsignedInt(0);\n'
            code += '            sendmsg.writeUnsignedInt(0);\n'
            code += '            sendmsg.writeUnsignedInt(0);\n'
            code += '            sendmsg.writeUnsignedInt(%s);\n' % i['name']
            code += as_encode_def_func(t, '', 'vo.', 3, num)
            code += '            sendmsg.writeShort(0x4641);\n'
            code += '            sendmsg.position = 2;\n'
            code += '            sendmsg.writeUnsignedInt(sendmsg.length);\n'
            code += '            if(context) context.core.send(sendmsg);\n'
            code += '        }\n'
        elif i['dir'] == 'sc':
            t = xml.types.find('type', attrs={'name':i['type']})
            if(t == None):
                print(" !!!ERROR: undefine type "+i['type'])
                continue
            code += '        private function recv%s(recvmsg: ByteArray): void\n' % (i['name'])
            code += '        {\n'
            code += '            var strlen: uint;\n'
            code += '            var vo: NET_%s = new NET_%s();\n' % (i['type'], i['type'])
            code += as_decode_def_func(t, '', 'vo.', 3, num)
            code += '            if(recvhook) if(!recvhook(%s, vo)) return;\n' % i['name']
            code += '            getHandler().on%s(vo);\n' % i['name']
            code += '        }\n'
    return code

# ==================================================
# Generate As files
# ==================================================
def generate_as_files():
    print " Generate as files. "
    # As template files
    #HANDLER_TMPL = 'lang-tmpls/as/handler.tmpl'

    generate_as_msgvo()

    generate_as_message_handler()

    #t = template.Template(open(HANDLER_TMPL, 'rb').read())
    #output(t.generate(**as_context).replace('\n\n', '\n'), asfile_controller_out)

    generate_as_message_controller()
    return

# ==================================================
# As context
# ==================================================
as_context = dict(xml=xml,
            as_type_def=as_type_def,
            generate_as_parser_code=generate_as_parser_code)

# ==================================================
# Print Logo Shit
# ==================================================
def print_logo():
    print '        .  .   .       '
    print '       (   )  (        '
    print '      ._)__(__)_.      '
    print '    _|`--------`|      '
    print '   (C|          |__    '
    print ' /` `\          /  `\  '
    print ' \    `========`    /  '
    print '  ``--------------`    '
    return

print_logo()

# 检查xml的合法性
if not check_valid():
	exit()

print " xml check ok!"
    
# ==================================================
# Generate cpp files
# ==================================================
generate_cpp_files()

# ==================================================
# Generate as files
# ==================================================
print "\n"
generate_as_files()

# ==================================================
# Formate cpp files
# ==================================================
content = re.sub(r"[\s]*\n", "\n", open(ccfile_h_out).read())
content = re.sub(r"^\n", "", content)
open(ccfile_h_out, 'w').write(content) 

content = re.sub(r"[\s]*\n", "\n", open(ccfile_cpp_out).read())
content = re.sub(r"^\n", "", content)
open(ccfile_cpp_out, 'w').write(content) 

# ==================================================
# Formate as files
# ==================================================

#encoder = codecs.lookup("utf-8")
if os.path.isfile(asfile_handler_out):
    content = re.sub(r"[\s]*\n", "\n", open(asfile_handler_out).read())
    content = re.sub(r"^\n", "", content)
    #content = encoder.encode(content)
    open(asfile_handler_out, 'w').write(content) 

if os.path.isfile(asfile_controller_out):
    content = re.sub(r"[\s]*\n", "\n", open(asfile_controller_out).read())
    content = re.sub(r"^\n", "", content)
    #content = encoder.encode(content)
    open(asfile_controller_out, 'w').write(content) 
