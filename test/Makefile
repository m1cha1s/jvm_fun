.PHONY: tests clean

JAVA_FILES := Hello.java
CLASS_FILES := $(JAVA_FILES:.java=.class)

tests: $(CLASS_FILES)

%.class: %.java
	javac -source 1.8 -target 1.8 $<

clean:
	rm -rf $(CLASS_FILES)
