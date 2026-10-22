; PUSH IMPLEMENTATION

; push segment offset -> this for lcl and arg

@OFFSET
D=A

@SEGMENT
A=D+M
D=M

@SP
A=M
M=D

@SP
M=M+1

; push temp offset -> for temp. TEMP maps to ram slots 5-12, 0x5 is the base addr of temp
@R5
D=A
@OFFSET
A=D+A
D=M

@SP
A=M
M=D

@SP
M=M+1


; static push
; push static i:
@FILENAME.i
D=M

@SP
A=M
M=D

@SP
M=M+1

;constant push

push constant i:
@i
D=A

@SP
A=M
M=D

@SP
M=M+1

; pointer push(0 is this and 1 is that)
; pointer 0 accesses the THIS pointer
; pointer 1 accesses the THAT pointer

; push pointer 0:

@THIS
D=M

@SP
A=M
M=D

@SP
M=M+1

; push pointer 1:
@THAT
D=M

@SP
A=M
M=D

@SP
M=M+1




; POP IMPLEMENTATION:
; pop segment offset

; save top stack val
; put in RAM[segment + (offset)]

@SEGMENT
D=M
@OFFSET
D=D+A
@R13
M=D

@SP
M=M-1
A=M
D=M

@R13
A=M
M=D



; static pop implementation
; pop static 2 -> store val in Foo.2
; filename = Foo

@SP
M=M-1
A=M
D=M ; D = x

@filename.i
M=D


; pop pointer 0:
@SP
M=M-1
A=M
D=M 

@THIS
M=D

; pop pointer 1:
@SP
M=M-1
A=M
D=M 

@THAT
M=D

; pop temp offset
@R5
D=A
@i
D=D+A      
@R13
M=D        

@SP
AM=M-1     
D=M        

@R13
A=M
M=D        


; 1. Arithmetic & Logical Operations (Builtins)

; Binary operations (pop two values, compute, push result):

; The following works for add, sub, and, or -> A %s B %s = op

@SP
AM=M-1
D=M ; D=B (top of the stack)

@SP
AM=M-1
D=D%sM ; D = A %s B
M=D

@SP
M=M+1


; Unary operations (operate on top of stack):

; neg

; not

@SP
AM=M-1
M=%sM

@SP
M=M+1

; Comparison operations (pop two values, compute boolean, push result):

; eq

; gt

; lt

; comp(A, B) -> true = -1 (0xFF), false = 0 (0x0)
; add a static int n counter which increments each time a comparison op is called to prevent label clashes

@SP
AM=M-1
D=M            // D = B

@SP
AM=M-1
D=M-D          // D = A - B

@IF_TRUE.n
D;JMPCOND      // JMPCOND ∈ {JEQ, JGT, JLT}

D=0
@SP
A=M
M=D
@END.n
0;JMP

(IF_TRUE.n)
D=-1
@SP
A=M
M=D

(END.n)
@SP
M=M+1




; 2. Branching / Labels

; label <X> → define a label for jumps.

(labelName)

; goto <X> → unconditional jump.

@labelName
0;JMP

; if_goto <X> → pop top stack value, jump if non zero
@SP
M=M-1
A=M

D=M
@labelName
D;JNE

; 3. Function keyword implementations:

; call <f> nArgs
; need static int n to inc everytime
; push returnAddr ; same label (@functionName$ret.n) is used by "return" keyword
@functionName$ret.n
D=A
@SP
A=M
M=D
@SP
M=M+1
; push lcl
@LCL
D=M
@SP
A=M
M=D
@SP
M=M+1
; push arg
@ARG
D=M
@SP
A=M
M=D
@SP
M=M+1
; push this
@THIS
D=M
@SP
A=M
M=D
@SP
M=M+1
; push that
@THAT
D=M
@SP
A=M
M=D
@SP
M=M+1
; repos arg so that the function can access them when it begins executing
; ARG = SP - 5 - nArgs
@nArgs ; given by call <f> nArgs
D=A
@5
D=D+A
@SP
D=M-D
@ARG
M=D
; Set lcl = sp to push the local variables of the called function 
; LCL = SP
@SP
D=M
@LCL
M=D

; jump to called function
@className.function
0;JMP

(functionName$ret.n) ; label for return addr since after call we have other vm commands i.e. call bar.mult then neg then so and so forth 
                ; we need to remeber the return address of neg

; function <f> nLocals

(className.function) ; declare the function label for jumping into later (done via call) 
; LOOP nVars times
    ; push 0
    ; need to give labels for loops unique names since multiple functions will be looping

@nLocals
D=A

(className.function$LOOP)

@className.function$EXIT
D;JEQ

@SP
A=M
M=0

@SP
M=M+1
D=D-1
@className.function$LOOP
0;JMP

(className.function$EXIT)

; exectue the normal function commands inside the function body as per normal

; return 

; endFrame = LCL ; temp var endFrame
; JUST NEEDS TO @LCL WHEN NEEDED
; retAddr = *(endFrame - 5)
@5
D=A
@LCL
A=M-D
D=M
@R13
M=D ; store ret addr temporarily in R13 register

; *ARG = pop() ; ret val points to arg 0
@SP
AM=M-1
D=M ; store retVal 
@ARG
A=M
M=D ; arg[0] = retVal
; SP = ARG + 1 ; restore SP
@ARG
D=M+1
@SP
M=D

; THAT = *(endFrame - 1)
@LCL
A=M-1
D=M
@THAT
M=D

; THIS = *(endFrame - 2)
@2
D=A

@LCL
A=M-D
D=M
@THIS
M=D

; ARG = *(endFrame - 3)
@3
D=A

@LCL
A=M-D
D=M
@ARG
M=D

; LCL = *(endFrame - 4)
@4
D=A

@LCL
A=M-D
D=M
@LCL
M=D

; goto retAddr
@R13
D=M
0;JMP

