**Huffman Algorithm for File Compression**
=========================

https://github.com/TheNilesh/huffman/  
License: Public Domain, no warranty  
Nilesh Akhade

About
=====

Huffman Algorithm is an efficient way for file Compression and Decompression.
This program exactly follows huffman algorithm. It reads frequent characters from input file and replace it with shorter binary codeword.
The original file can be produced again without loosing any bit.

Usage
=====
Compression:
```
	./encode <file to compress>
```
Output file named <inputfile>.hzip will be produced.
Decompression:
```
	./decode <file to uncompress>
```
 
File Header
============================
| N= total number of unique characters(1 byte)
| Character 1 (1 byte) | Binary codeword(string form) for Character 1 (MAX bytes) |
... 
| Character N (1 byte) | Binary codeword for Character N(string form)(MAX bytes) |
| padding(1 byte) | 0's, p times (p bits)|
| DATA |

Algorithm
============================
[Pass1]
0. Read input file
0. Create sorted linked list of characters from file, as per character frequency
   for eah character ch from file
```
	if( ch available in linked list at node p) then 
	{
		p.freq++;
		sort Linked list as per node's freq;
	}
	else
		add new node at beginning of linked list with frequency=1;
```
0. Construct huffman tree from linked list
   .Create new node q, join two least freq nodes to its left and right
   .insert created node q into ascending list
   . repeat i & ii till only one nodes remains, i.e, ROOT of h-tree
   . Traverse tree in preorder mark each node with its codeword. simultaneously Recreate linked list of leaf nodes.

[Pass2]
0. Write Mapping Table(character to codeword) to output file.
0. Read input file.
0. Write codeword in place of each character in input file to output file
   for each character ch from input file
	write corresponding codeword into o/p file (lookup in mapping table OR linked list)
0. End

Contributing
============

Please feel free to submit issues and pull requests. I appreciate bug reports.
Testing on different platforms is especially appreciated. I only tested on Linux.

License
=======

This software is in the *Public Domain*. That means you can do whatever you like
with it. That includes being used in proprietary products without attribution or
restrictions. There are no warranties and there may be bugs. 

Formally we are using CC0 - a Creative Commons license to place this work in the
public domain. A copy of CC0 is in the LICENSE file. 

    "CC0 is a public domain dedication from Creative Commons. A work released
    under CC0 is dedicated to the public domain to the fullest extent permitted
    by law. If that is not possible for any reason, CC0 also provides a lax,
    permissive license as a fallback. Both public domain works and the lax
    license provided by CC0 are compatible with the GNU GPL."
      - http://www.gnu.org/licenses/license-list.html#CC0

Development
===========

To do:
* Binary files, like jpeg,mp3 support
* Run scan to group repeating bit patterns.
