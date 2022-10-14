#!/usr/bin/python
import os
import sys
import re
import subprocess
BUILD_DIR =  os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/build"
TEST_DIR = os.path.dirname(os.path.realpath(__file__))
def run_make():
    if os.path.exists(BUILD_DIR+"/YAN"):
        os.remove(BUILD_DIR+"/YAN")
    CMD = "cd "+BUILD_DIR +" && cmake .. && make"

    make = os.popen(CMD)
    out = make.read()
    matched = re.search( r'\[100%\]', out, re.M)
    if matched is not None:    
        print("-----make build successfully-----")
    else:
        print(out)
        exit(1)

    if os.path.exists(BUILD_DIR+"/YAN"):
        cp = os.popen("cp "+BUILD_DIR+"/YAN  "+ TEST_DIR+"/.")
        cp.read();
    else:        
        print("make build fail")
        exit(1);

def compile(file):
    out = ''
    print("complie :"+file+".....begin")
    p = subprocess.Popen("./YAN "+file +" 2> tmp",shell=True)
   # p = os.popen("./YAN "+file +" 2> tmp")
    p.wait()
    print("compile:"+file+"....end")
    with open('tmp', 'r') as f:
        out = f.read()
    if re.search(u'.*Successfully generated.*',out,re.M|re.I) is not None:
        print("-----------complie successfully")
    else:
       # print(out)
       print(out)
       exit(1)
       pass
    excute(file, "a.out")
def excute(file, out):
    print("excute a.out")
    expect = []
    expectedFieldBegin = False;
    with open(file,'r') as f:
        for line in f.readlines():
            if "Expected output:" in line:
                expectedFieldBegin = True
                continue
            if expectedFieldBegin:
                if  "*/" in line:
                    break
                
                expect.append(line.strip())
    print("expected:"+str(expect))
    p = os.popen(" ./"+out)
    s = p.read()

    result = s.split("\n")
    result.pop()
    print("result: " + str(result))
    assert len(expect) == len(result) , " expect doesn't match result\n"
    
    for a,b in zip(expect, result):
        assert a == b ,"element doesn't match"

    clean_up()

    print("\033[1;32m"+file + " : --------------------Passed\033[0m")

def clean_up():
    if  os.path.exists(TEST_DIR+"/a.out"):
         os.remove(TEST_DIR+"/a.out")
    if  os.path.exists(TEST_DIR+"/a.s"):
         os.remove(TEST_DIR+"/a.s")
    if  os.path.exists(TEST_DIR+"/tmp"):
         os.remove(TEST_DIR+"/tmp")
    # if  os.path.exists(TEST_DIR+"/YAN"):
    #      os.remove(TEST_DIR+"/YAN")

def run_test():
    files = os.listdir(TEST_DIR)
    for file in files:
        if(re.match(r"^.*\.c$",file)):
            compile(file)
    # errorfiles = os.listdir(TEST_DIR+"/error_test")
    # for f in errorfiles:
    #     if(re.match(r"^.*\.c$",file)):
    #         compile(file)   




def main():
    run_make()
    run_test()
    if  os.path.exists(TEST_DIR+"/YAN"):
          os.remove(TEST_DIR+"/YAN")
    




if __name__ == '__main__':
    main()