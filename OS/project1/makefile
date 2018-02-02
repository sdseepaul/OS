all:	runsc runcs clean

runsc:	system_call.c
	gcc -o sc system_call.c -lrt
	./sc

runcs:	context_switch.c
	gcc -o cs context_switch.c -lrt
	./cs

clean:
	rm -f test.txt sc cs
