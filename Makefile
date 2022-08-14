PACKAGE=wordperfect7.0_i386
CPPFLAGS=-Iinclude
CFLAGS=-fno-stack-protector -m32

.PHONY: clean

all: $(PACKAGE).deb

$(PACKAGE).deb: $(PACKAGE)
	dpkg-deb --build --root-owner-group $(PACKAGE)
	@ls -l $@

$(PACKAGE): libcompat.so libwppatch.so
	bash build.sh $(PACKAGE)

libcompat.so: compat.o
	ld -m elf_i386 -shared -o $@ $<

libwppatch.so: patch.o
	ld -m elf_i386 -shared -o $@ $<

clean:
	rm -f *.o *.deb *.so
	rm -rf build $(PACKAGE)
	rm -rf lib