# Cracker Barrel Peg Game Solver

## TLDR:
**I am bad at the cracker barrel peg game. I am ostensibly less bad at programming, and so I *definitely* over-engineered the solution to this problem by creating a lightweight, quick solver for the cracker barrel peg game that uses BFS to search through the possible states and compute all of the different combinations for winning in about ***1/5*** of a second. Wow!**
<details><summary>Spoiler: How many winning combinations are there in the original game?</summary>29760 different ways! Wow!</details>

## Skills learned:
- Improved proficiency in C/C++
- Utilized many optimizations to speedup execution time.
- Improved proficiency in bitwise operations.

## Abbreviations:
This is a section for the abbreviations that I will use throughout this document:

<a id="LUT"></a> 
LUT := LookUpTable A table that stores results of common computations. Used to save time by not recomputing results that are already known.
<a id="BFS"></a> 
BFS := Breadth First Search: A strategy for exploring a graph by exploring all adjacent nodes first before proceeding any deeper. [Wiki](https://en.wikipedia.org/wiki/Breadth-first_search)

## Abstract:


## Try it yourself
Compile the code using: <br>
Windows:<br>
In cmd run:
```
g++ src/solver.cpp -o solver.out
board.exe
```
Mac/Linux <br>
In a terminal run:
```
g++ src/solver.cpp -o solver.out
./board
```
Running the c++ file will generate a file called output.txt which represents every single state taken from start to finish that resulted in a victory.

 <figure>
  <img src="/images/catTyping.webp" alt="Cat typing on keyboard" style="width:25%">
  <figcaption>Source: <a href="https://giphy.com/gifs/cat-typing-E6jscXfv3AkWQ"> Giphy</a> </figcaption>
</figure>

# How did I do it? (Technical details ahead, Reader beware)
## Pre-Project:
Going into this project, I had 3 goals in mind.
1. Make it work.
2. Make it blazingly quick.
3. Make it memory efficient.

The first step in the thinking process was to of course, pick a programming language.

Unfortunately, because I wanted it as quick as possible and as memory efficient as possible, my favorite languages, Java and Python were out the window from the get-go as they are both too slow and too memory hungry.

 <figure>
  <img src="/images/morty.webp" alt="Me realizing that I cannot use Java/Python" title="Morty saying oh jeez" style="width:25%">
  <figcaption>Source: <a href="https://giphy.com/gifs/rickandmorty-season-1-adult-swim-rick-and-morty-QYRjw6Jz0jyr1AnPW9"> Giphy</a> </figcaption>
</figure>

This left me only one choice: to write it using C/C++ which may or may not have caused me days of agony and suffering.

## That's enough talking, lets get into the real deal. What are the implementation details?

If you look at the cracker barrel board, you will see something that looks like this:

The initial state of the board:
<a id="init-state"></a>
<figure>
  <img src="/images/pegGame.webp" alt="A picture of the cracker barrel board game" title="A puzzling game indeed." style="width:50%">
  <figcaption>Source: <a href="https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.businessinsider.com%2Fcracker-barrel-review-cozy-charming-underwhelming-photos-prices-2022-2&psig=AOvVaw2mt44u8cwZkvZud28oOCmH&ust=1723172643327000&source=images&cd=vfe&opi=89978449&ved=0CBIQjhxqFwoTCPiKhry05IcDFQAAAAAdAAAAABAW">Business Insider</a> </figcaption>
</figure>

Now if we define a coding on this board so that we are all on the same page, we get this:
<figure>
  <img src="/images/annotatedGame.webp" alt="Annotation on game: Top row is 0, Second row is 1,2. Third row is 3,4,5. Fourth row is 6,7,8,9. Fifth row is 10,11,12,13,14." style="width:50%">
  <figcaption>Source: <a href="https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.businessinsider.com%2Fcracker-barrel-review-cozy-charming-underwhelming-photos-prices-2022-2&psig=AOvVaw2mt44u8cwZkvZud28oOCmH&ust=1723172643327000&source=images&cd=vfe&opi=89978449&ved=0CBIQjhxqFwoTCPiKhry05IcDFQAAAAAdAAAAABAW">Business Insider</a> </figcaption>
</figure>


Now if you *really* think hard about the board and abstract out some of the details, you can flatten the triangle to a straight line.
<figure>
  <img src="/images/conversion.webp" alt="Annotation on game: Top row is 0, Second row is 1,2. Third row is 3,4,5. Fourth row is 6,7,8,9. Fifth row is 10,11,12,13,14." style="width:50%">
  <figcaption>Source: My bad photoshop skills</figcaption>
</figure>

**But wait, this is just a 15 bit number!  Hence, we have demonstrated that we may fit the entire state of the board in a 15 bit number!** <br>
Unfortunately, in my actual implementation I used a 16 bit (2 bytes) number as most computers are byte-addressable meaning that the smallest unit that we can access on the computer is 1 byte. This means that while *theoretically* we could have fit each state in 15 bits, we are actually going to use 16 as we are constrained by physical limitations of computers.

The upshot of all of this? The 15th bit (remember, we count starting from 0) will always be 0.

### Fair Warning: We will be using quite a bit of binary, hexadecimal, and bitwise operations from here on out. If you need a refresher on these topics, you can visit these links: [refresher-bin/hex](https://www.mathsisfun.com/binary-decimal-hexadecimal.html); [refresher-bitwise](https://en.wikipedia.org/wiki/Bitwise_operation)

### Example of a state given in binary:
The initial state would be 0x7FFE which would be 0111 1111 1111 1110 in binary.
This represents Every spot is filled except the 0th spot which is the very top of the triangle.
<a href="#init-state">Initial State</a>

Note: Remember the 15th bit will always be 0 as we only need 15 bits to represent the state.

## The Algorithms:
This is where it gets fun. (or not, it depends on your definition of fun really)

Before we hop into the main function, there are a couple of things that you should look at the details for before if you're not super well versed in C programming. (Or if you just want to see how I performed these operations quickly).
<a id="JumpLUT"></a> 
### Jump LUT
<details><summary style="font-weight:bold">Click for details</summary>
This <a href="#LUT">LUT</a> contains information for every single peg what moves are valid.<br>
The way that it does this is by storing an array of bitmask 2-tuples (expected,mask) which allow for future calculations to quickly check whether or not a jump is valid.
</details>

<a id="checkFunc"></a> 
### checkJump()
<details><summary style="font-weight:bold">Click for details</summary>
This function checks whether a jump is possible from this board state given a 2-tuple: expected and mask. (You should get these values from the Jump <a href="#LUT">LUT</a>.)<br>
This function performs this check lightning quick by doing the following operation:<br>

`return (board & mask) == expected;`<br>

How did we get to this result? Lets break it down:
A jump is only legal if:
1. There is a peg in the starting location. Let us call this peg the start peg
2. There is a peg between the start and final location. Let us call this peg the jump peg.
3. There is an empty peg after the jump and start location. Let us call this peg the empty peg.
What this boils down to in binary is that there must be two bit set and 1 bit unset. The rest of the bits don't matter.

This is where the mask and expected come into play. We can check that the above statement is true by performing a bitwise AND using the mask. This mask will AND only the 3 relevant bits with 1 and AND the rest with 0. Recall that anything AND 0 = 0 and anything AND 1 stays the same. <br>
If the result of this operation is the expected value, then we know that the jump is legal, otherwise, the jump is illegal as one of the 3 essential parts is missing!

An example of this function in play would be from the starting state using move 3->1->0 (start=3,jump=1,empty=0): <br>
We know that this move is legal as its one of the 2 opening moves, so let us confirm using our checkJump() function.
```
startState    =   0111 1111 1111 1110
mask          =   0000 0000 0000 1011
expected      =   0000 0000 0000 1010

            
startState        0111 1111 1111 1110
mask          &   0000 0000 0000 1011
--------------------------------------
startState & mask 0000 0000 0000 1010
expected          0000 0000 0000 1010 <- It's the same, hence the jump is legal.
```
</details>

<a id="performJumpFunc"></a> 
### performJump()
<details><summary style="font-weight:bold">Click for details</summary>
This function changes the state to represent a jump that happened given a 2-tuple: expected, mask.
This function does all of that in 4 cheap operations. <br>
Lets break down what happens once a jump is performed:
1. The start, jump peg is now empty, the empty peg is now full.
2. Everything else stays the same.

Notice that when this operation occurs it basically just *inverts* the 3 bits of interest(start,jump,empty). Now the name of the game is how can we do such a thing?<br>
Consider the following function:<br>
`return (state & ~mask) | (mask - expected);`

Lets break it down:
1. state & ~mask: Remember that mask is only 1 on our 3 bits of interest and so if we negate it, now we have 1 everywhere EXCEPT for our bits of interest. If we do state AND ~mask, we clear all the bits of interest and keep everything else.
2. mask - expected: Mask will ALWAYS have one extra bit 1 bit set compared to expected as mask should have the empty bit set and expected should not. Now if you do mask-expected, the result will be all zeros except for the empty bit.
3. You can probably see where this is going, but if you OR the result of step 1 with the result of step 2, the result is that you have set just the empty bit which is exactly what we wanted! Remember that 0 OR 1 = 1.

</details>

### Main Function:
Now that you have an understanding of the operations that we will perform, lets tie it all together:

First, we create a couple of data structures to help us save time and be able to store our results.
- Visited set to never backtrack on states we've already explored as if we have fully explored a state, the result will be the same the second time we explore it.
- Queue of states to explore. This is to perform the search in a <a href="#BFS">BFS</a> fashion.
- Map of ParentStates to store how we got to the current state. This is important for backtracing back our steps. If you don't need to get the path to the final state, then you may omit this.

Once these structures have been created, we are ready to start the main workhorse.

1. While we have not hit the terminal state (we can still make a move), we see which spots have pegs left (finding the positions where the bit has been set in the state).
2. For every single spot that still has a peg on it, we consult the <a href="#JumpLUT">Jump LUT</a> to get an array of (expected,mask) 2-tuples.
3. With this array, for every single 2-tuple, we call the <a href="#checkFunc">check()</a> function to see if the jump is valid.
4. If the check is valid, we perform the jump with <a href="#performJumpFunc">performJump()</a> to get the successor state. We add the (successor state,current state) key,value pair to the dictionary and then we add the successor state to the end of the queue to be evaluated later.
5. We explore the rest of the 2-tuples for all other jump options for this peg and the rest of the peg by following the same routine as specified above.
6. Once we are done, we backtrace from the final states back to the initial state to find the path that was taken by using the ParentState map that we defined earlier.

If you notice, the main work of this function are all simply bitwise operations! Bitwise operations are very cheap (in terms of number of clock cycles used). This results in this function being extremely quick even though it's a glorified brute force search!


# Postword
Full disclosure: Unfortunately, there are already others who have done similar works to what I have demonstrated here today, as linked below:<br>
https://blogs.sas.com/content/operations/2015/05/20/dinner-with-a-side-order-of-optimization/
<br>
https://adueck.github.io/peg-solitaire-solver/
<br>
https://www.reddit.com/r/learnprogramming/comments/3k8u3p/solving_the_cracker_barrel_peg_board_game_update/
<br>
**However!**
What makes mine different is the efficiency of the matter as if you look at the peg-solitaire-solver, it is written in typescript (slower than C++) and also does not optimize for space/ time. On my computer it took nearly 20 seconds to complete using this solver!

My implementation works blazingly quick: <br>
real    0m0.216s<br>
user    0m0.118s<br>
sys     0m0.099s<br>
That means that it computed all 29760 winning move combinations in 1/5 of a second. That's pretty impressive if I do say so myself.

 <figure>
  <img src="/images/thinking.webp" alt="Arrow pointing from triangle to array of 15 elements." style="width:50%" >
  <figcaption>Source: <a href="https://giphy.com/gifs/doctor-strange-WPtzThAErhBG5oXLeS"> Giphy</a> </figcaption>
</figure>

# Could we still do better?
Yes, there are a couple of ways that this could still be improved.

<ul>
    <li>You could use a <a href="https://en.wikipedia.org/wiki/Bidirectional_search">bi-directional BFS</a> to cut the search time from n^d to 2(n^(d/2)) as the latter is oftentimes cheaper. The reason I did not do this is because I did not know the ending states beforehand and so I could not start the search from the end (without cheating).</li>
    <li>You could use a heuristic and perform a best-first search to only expand upon states that look good. The reason that I did not do this is because finding a consistent, admissible heuristic is genuinely difficult as there is easy way to really say that a state is "good".s</li>
    <li>Since the number of states in this game is definitely finite as there is only 2^15 = 32768 different states. Lets suppose that each state can go to 4 different states (a reasonable estimate as some have more some have less).<br>
    If each state is 2 bytes as mentioned previously, then we will have 2^15(different states) * 2^2(4 states) * 2^1(bytes) = 2^17 = 262,144 bytes or about 262KB.<br>
    This is obviously dirt cheap and definitely in the realm of possibility for a <a href="#LUT">LUT</a>. The reason that I did not do so is because it felt like cheating. You basically already knew the answer before you even started the computation. The only work that was done was to compose the answer that you already knew.<br>
    Also it takes the fun out of this whole project if life was <i>that</i> easy.
</ul> 