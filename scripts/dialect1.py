from colorama import Fore, Back, Style

vowels = ['a','e','i','o','u']


print("Dialect 1 swaps 'e' with 'u', ex: 'whale' -> 'whalu','language' -> 'langeagu'")
message = input("what's the message?")
print(Fore.RED + "============================================================" + Fore.GREEN)
for c in message:
	if c == 'e':
		print('u',end='')
	elif c == 'E':
		print('U',end='')
	elif c == 'U':
		print('E',end='')
	elif c == 'u':
		print('e',end='')
	else:
		print(c,end='')
print(Style.RESET_ALL)
