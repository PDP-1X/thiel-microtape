This is the contents of a Microtape (DECtape) from the MIT PDP-1X
computer. See "microtape box.jpg" and "microtape with listing.jpg" for
pictures of the tape.

After the MIT PDP-1X was decomissioned by the Electrical Engineering
department, it wound up at the MIT Electronics Research Society in
room 20B-119 in Building 20. It kept the former Education Research
Center PDP-7 company. David Felsenthal was the student who had those
computers ocassionally working.

When the PDP-1X was being donated to the Computer Museum in
Marlborough run by Digital Equipment, David gave me this Microtape. He
told me it had the source to PDP-1 Spacewar on it.

I was a member of the Tech Model Railroad Club (also in Building 20),
where we had a PDP-11 with DECtape. The hope was to use "read-all"
mode on that machine to read the tape. I never managed to do that.

The tape originally belonged to David Thiel, class of 1970. I have his
permission to share the contents of the tape.

The directory listing that came with the tape (thiel2.jpg) reads (with
octal numbers):

dwt_2

p,

file        blocks

billiards
 (start
  at
   200)     13
diskpak
 insert     2
 source     14
 simulator  11
 float
  source
   4/27/70  33
  insert
   4/27/70  4
snowflake   4
mp2         10
t.s.idiot   5
dots        7
cube        17
plotter
 memo
  justified
   3/19     12
 display
  total
   20/imp   66
   11/11mpr          70
newa
 spherical
  alph      52
 score
  alph      51
 2.3
  alph      46
 auto
  alph      26
 1
  alph      21
 2
  alph      21
 3
  alph      21
 4
  alph      22
 old
  alph      30
 dec
  b         14
float
 source     23
 insert     2

11 free tape blocks
64 free directory entries

I recently sent the tape to David Gesswein to be read using his
dedicated DECtape reader, and he sent me back the SIMH 18-bit format
file that is thiel2.tu56 in this directory. He noted that the tape was
unusually short, only 512 blocks, rather than the normal 576 blocks.

It took a lot of research to crack the tape. First I just wrote a
dumper that dumped each block in octal and decoded FIODEC. There was
obvious PDP-1 source code, and some blocks with names that
corresponded with the printed directory listing that came with the
tape.

Looking online, I found that the PDP-1X used an interleave on
Microtapes. The logical blocks started at the second physical block of
the tape (number 1 for 0-based), and went every other block to the end
of the tape, and then back on the other blocks such that the last
logical block was the very first physical block of the tape. Like this
(in decimal):

511 0 510 1 509 2 ... 257 254 256 255

The document PDP-42, MICROTAPE FILE SYSTEM, was the next source of
information. This told me that the first 13 logical blocks of the tape
were the binary code of Microtape file system, and the directory.

Logical block 9 and 10 of the tape are the link table for the
non-contiguous files on the tape. There is one word for each block on
the tape. (This is why the tape was formatted for only 512 blocks.)
The first few words correspond to the blocks of Microtape filesystem,
and the first three of those are the tape name of the tape in FIODEC,
which can be up to 9 characters.

Each word of blocks 9 and 10 corresponds to a block of a file on the
tape, and has a link pointer in the low 9 bits. So if a file started in
block 017 (octal), and the next block was in block 018, the low 9 bits
of word 17 would be 017. The last block of a file has a word with a
link pointer of 000.

The high bits in each word of the link table are some metadata. The
0400 bit in the high word indicates that the block is in use. The 0100
bit in the high word indicates that the file is binary, not text.

The last few words of logical block 6, and all the words of blocks 7
and 8 are the directory of the tape. This is an array of entries, with
three words of FIODEC, and two pointer words. The three words of
FIODEC allow each component of a filename to be up to 9 characters. In
the last component of a filename, the first word after three filename
words has the logical block number of the first block of the file in
the low 9 bits. The high bits of that word are metadata, 0500
indicates a binary file, 0400 indicates a text file. The second word
after the filename words is a link pointer to the next filename
component. (I have not 100% figured out the directory chaining
structure.)

The program pdp1dump.c is not pretty, but it takes thiel2.tu56, and
builds two sub-directories, one with a binary dump of each file, and
one with a text conversion of each file. (It ignores the binary
markers in the metadata.) The commented-out code was used for earlier
dumping operations. It assumes it is running on a little-endian host.

The text decoding of the files (which are in Expensive Typewriter
format) is tricky. Sequences like center-dot-without-space followed by
comma or period are mapped to ; and :, repsectively. The state of
upper versus lower-case is tracked. Quite a few FIODEC characters
(and, or, superset, up-arrow) have no ASCII equivalent, so they are
mapped to their Unicode equivalent in UTF-8. Every Expensive
Typewriter line of text starts in a new PDP-1 word, so the last word
of a line is padded with carriage returns (077), so only the first of
those is output. Stop code (013) is tricky, always three to a
word. One word of 0131313 is equivalent to form-feed (end of
page). Two words of 0131313 in a row are end of file, characters after
that are ignored.

The "newa" files are various verions of Spacewar! source. The file
"newa old alph" appears to be a vanilla PDP-1 version. The file "newa
dec b" is presumably the Digital binary version. All of the others are
PDP-1X versions, have new PDP-1X assembly language instructions like
CXX, SX>=, etc. (The new opcodes were all upper-case.)

Quite a few of the files on this tape are from courses that Dave Thiel
was the Teaching Assistant for, such as 6.25 and 6.273, taught by
Professors Francis Lee and Sam Mason. For instance mp2 is macros for
6.25.

The dispak files were for a system that made it easy to output data on
the PDP-1X display. They were also for 6.25.

The float files were a floating point implementation, they were for
6.273.

