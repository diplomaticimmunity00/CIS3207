# CIS3207 Project 3: Networked Spell Checker 

## Overview
This is a server/client for a networked spell checker. The server listens for connections
over a given port and verifies that submitted words are spelled correctly. The server
can accept multiple simultaneous connections. The client executable provided with this program 
simulates connections from several clients. 

See [readme](https://github.com/tuh37046/CIS3207/blob/master/P3_Networked_Spell_Checker/readme.pdf) for more information<br>

## Usage

Run the server with its default dictionary and port with:<br>
`./server`<br>

Use words from `new_dict.txt`<br>
`./server new_dict.txt`<br>


Run the server on port 1738<br>
`./server 1738`<br>


Or both!<br>
`./server new_dict.txt 1738`<br>
`./server 1738 new_dict.txt`<br>



Garrett Bowser <br>
Kwatny CIS3207 Fall 2019
