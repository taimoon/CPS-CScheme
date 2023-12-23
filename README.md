# CPS-CScheme
I implement Scheme using C as example for me  start learning how to develop a hash table and how to incorporate garbage collector. The implementation is a tree-walk interpreter written in continuation passing style. The interpreter doesn't correctly interpret `define` construct when nested.

# The structure


# Garbage Collector
I develop the GC based on the core idea: **reachability**. The objects are garbage if not reachable from predefined rootset registers. The rootset registers are `EXP`,`VAL`,`ENV` and `CONT`. GC based on this idea can be also called as tracing GC.


The implementation is quite difficult to me without readable examples. 

One of the difficult is that graph traversal is inherently recursive. When translate to low-level program where recursion is not a thing, say assembly for example, the target program has to maintain an extra data structure storing additional contexts to be recursive. Using stack then the graph traversal is depth-first order whereas using queue you will get breath-first order. 

I didn't wrote the GC in CPS, let the compiler handle the recursion program implicitly, and then the GC is depth-first order. Yeah, I cheat sometime, and not part of the interpreter is CPS.

Mark-and-sweep is the easiest garbage collector based on reachability (traversal) idea. One of its biggest advantage is that it handles cycle structure without complicating the implementation. Disadvantage is that it doesn't defragment the heap space.

A easy change that allow mark-and-sweep GC defragments is to have heap with two parts: free and used. The GC will move the traced object from used to free heap space. This is the two-space GC. 

What if you want to manage the contexts without additional stack or queue? Yes, the used heap space also can be used as queue. This is the idea of **Cheney's Algorithm**. I quite get the context of why Cheney's Algorithm is invented, though I'm not implementing it in this repo.

# Why GC is needed?
It is not necessary but it could make the program shorter and more elegant as explained by John Mc Carthy in his [paper](http://jmc.stanford.edu/articles/lisp.html).

# Why CPS?
For tree-walk interpreter, it is not obvious that what should be in the rootset for GC to collect from. But when translated to CPS, it becomes obvious that all compuations are affected by only the 4 registers (`ENV`,`CONT`,`VAL`,`EXP`) at any time of execution.

Tree-walk interpreter is also inherently recursive that many imperative programming languages doesn't do tail-call optimization by default. This will cause stackoverflow even the written program is iterative. Trampoline technique helps here. Trampoline and CPS together allows to imperative implementations supports high-level construct like first-class functions.

When you write the program in CPS, at the same time, you automatically do tail-call optimization.

# What is control?; What is continuation?
DISCLAIMER: I'm not an expert of it.

> In computer science, a continuation is an abstract representation of the control state of a computer program. 
> \- excerpt from wikipedia

Then what is `control` even means in program?

Machine usually read instruction from memory and execute the instruction, then read the next immediate instruction and repeat. This process is sequential. If the machine decides to execute other instructions that are in other places, then this is what `control flow` means.
```
;; abstract-machine
assign c 0
assign x 0
loop:
    if c < x
    goto loop-end
    assign x + x c
    goto loop
loop-end:
    print x
```
So `for`,`while` are abstraction of simple controls that can be expressed using conditional `jmp`. `procedure` is an abstraction of controls. To implement `procedure` construct, the execution has saves the location of the next instruction, then jump to callee's instructions, execute utill callee's end, prepare return and jump back to saved location, then continuing executions. 

The act of storing additional information is the act of maintaining `continuation`.

Mathematical expression evaluation like `a*x**3+b*x**2+c*x+k` is also an abstraction of control. It is no trivial to translate the expression to asssembly. See https://craftinginterpreters.com/a-virtual-machine.html#a-value-stack-manipulator

`continuation` is capable of implementing various control structure such as including `coroutine`, `thread` and `exception`. When a programming language supports first-class continuation, these constructs can be implemented using continuation as library without requiring special mechanism.

Recursion is more expressive than iterative construct. For example, it is very hard to describe the coin-change solution using iterative construct but easier when using recursive construct. As in theory, `continuation` has same expressive power to unstructured `gotos`. 

Sometimes, people argues that when all program compiled to machine codes must be inherently iterative (only jump) and using stack to simulate recursion. All recursion can be implemented using only iterative construct and therefore programs can expressed using only iterative construct. Well, they are correct about the implementation. However, this doesn't change the essence of computations the fact that these functions are inherently recursive though implemented using iterative construct with additional data structure. 

Regardless of whether you agree or not, the reality is that you must maintain either additional states or additional data structure for some class of recursive programs.
