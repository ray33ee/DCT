
- Use a stack for math expressions
- 'ret' instruction frees locals, resores original base pointer and cleans up params AND leaves any values past the allocated locals on the stack
- 'opn' instruction pushes current base pointer, gets new frame and allocates locals
- Math functions push and pop from stack
- 'pushl' function gets a a value relative to the base pointer and pushes it onto the stack (copy a local variable or parameter onto the stack)
- 'popl' pops a value off the stack and puts it into a position relative to the base pointer (pop a value off the stack into a local variable or parameter)

- 'cpush' - push a value onto the call stack
- 'cpop' - pop a value from the call stack
- 'opushi' - push an immediate onto the operand stack
- 'opopl' - pop value off the operand stack into a local variable
- 'opushl' - Push a local variable onto the operand stack

take the example

def main():
	a = 20
	b = 40
	c = combine(a, b)
	
def combine(x, y):
	alpha = a * a
	beta = b * b
	return alpha + beta

This will look something like

main:
	cpush cbp
	cpush obp
	mov cbp, csp
	mov obp, osp
	add csp, 3
	opushi 20
	opopl 0 ; a=20
	opushi 40
	opopl 1 ; b=40
	opushl 0
	cpushl 0
	cpushl 1
	call combine
	opopl 2 ; c = combine(a,b)
	mov 

combine:
	