import os,sys

makestr = ""
binstr = ""

bins = [src.split(".")[0] for src in os.listdir("src/") if ".swp" not in src]

for filename in bins:
    hasheader = False
    if filename in binstr:
        continue
    binstr += "bin/{0}.o: src/{0}.cpp ".format(filename)
    if bins.count(filename) > 1:
        binstr += "src/{0}.h".format(filename)
        hasheader = True
    binstr += "\n\tg++ -std=c++11 -c src/{0}.cpp -o bin/{0}.o\n\n".format(filename)

makestr = "DES: "

binlist = []

for filename in bins:
    if filename+".o" not in binlist:
        binlist.append(filename+".o")


makestr += "bin/"+' bin/'.join(binlist)
makestr += "\n\tg++ -std=c++11 "+"bin/"+' bin/'.join(binlist)+" -o DES"

f = open("makefile","w+")
f.write(makestr+"\n\n"+binstr)
f.close()
