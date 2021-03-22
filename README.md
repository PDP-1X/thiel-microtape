This data was provided by John Shriver on March 22, 2021.  He wrote:

> I have a PDP-1X Microtape (a/k/a DECtape) which I have had for years
> which I finally got read and managed to decipher, that I’d like to
> share with the SIMH and PDP-1 community, probably by having it stored
> on Bitsavers.
> 
> It originally was David Thiel’s tape. (MIT BS 70, MS 72.) I have his
> permission to share it. This tape was living with the PDP-1X when it
> was at the MIT Electronics Research Society in Building 20, sharing a
> 10x15 foot air-conditioned room with their PDP-7. When it was leaving
> for the Computer Museum at DEC Marlborough, Dave Felsenthal gave me
> this tape, saying that it had Spacewar source code on it.
> 
> With Dave Gesswein’s help, I got the bits off the tape. With a bunch
> of detective work, I cracked the interleave, block map, and directory
> structure of the tape, and managed to get all the files out in
> coherent form.
> 
> It indeed has Spacewar! source code on it. One version is for
> PDP-1. All the others are newer versions that use the new instructions
> added to the PDP-1X, with their upper-case opcodes in assembly
> language. A snippet of the newest version:
>
> [...]
>
> It also has the binary of Microtape File System in its first 13
> logical blocks. That’s a piece of the PDP-1X software that I don’t see
> any source code listings of online.
> 
> There’s also a good bit of other PDP-1X assembly language on this
> tape, including a floating point package. It probably would be very
> useful if anyone was ambitious enough to try and add a PDP-1X
> simulator to SIMH, providing known working code.
