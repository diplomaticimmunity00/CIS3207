import os,sys

exec_name = "server"

makestr = ""
binstr = ""

bins = [src.split(".")[0] for src in os.listdir("src/") if ".swp" not in src and "Client" not in src]

for filename in bins:
    hasheader = False
    if filename in binstr:
        continue
    binstr += "bin/{0}.o: src/{0}.cpp ".format(filename)
    if bins.count(filename) > 1:
        binstr += "src/{0}.h".format(filename)
        hasheader = True
    binstr += "\n\tg++ -fpermissive -std=c++11 -c src/{0}.cpp -lpthread -o bin/{0}.o\n\n".format(filename)

makestr = exec_name+": "

binlist = []

for filename in bins:
    if filename+".o" not in binlist:
        binlist.append(filename+".o")


makestr += "bin/"+' bin/'.join(binlist)
makestr += "\n\tg++ -std=c++11 -fpermissive "+"bin/"+' bin/'.join(binlist)+" -lpthread -o "+exec_name

f = open("makefile","w+")
f.write(makestr+"\n\n"+binstr)
f.close()
