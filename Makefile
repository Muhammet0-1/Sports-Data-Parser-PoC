all:
	gcc vulnerable_engine.c -o vulnerable_app -fno-stack-protector -z execstack
	gcc secure_patch.c -o secure_app

clean:
	rm -f vulnerable_app secure_app
