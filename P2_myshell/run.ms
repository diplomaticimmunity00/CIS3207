#truncating
echo ==========
echo TEST BEGIN
echo "" > test.out

#external command
echo ==========
echo Date:
date
echo Running on:
uname

#input and output redirection
echo ================================
echo Copying names.txt to test.out...
echo < names.txt > test.out

#external command
echo ==========================
echo First 4 lines of test.out:
head -n 4 test.out

#piping
echo ========================
echo Total lines in test.out:
cat notes.txt | wc -l
echo Total lines in words.txt:
wc -l words.txt

#more pipes
echo =================================
echo Current shell processing running:
ps -eaf | grep myshell

#built-in commands
echo ==================
echo Current directory:
pwd
echo ===================
echo Directory contents:
dir .
echo ==================
echo Shell environment:
environ

#background processes + appending
echo ========================
echo Background process test:
echo I & echo bet & echo these & echo printed & echo out & echo of & echo order & echo :) &

#dummy command to give the BP tests time to exit
du -shc words.txt > /dev/null
echo TEST END >> test.out

echo ========
echo TEST END
echo ========
STOP
