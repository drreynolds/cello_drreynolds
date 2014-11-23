.PHONY: default
default: enzop-8

.PHONY: enzop-%
enzop-%:
	python scons.py -j$* bin/enzo-p

.PHONY: doc
doc:
	$(MAKE) -C src doc

.PHONY: diff
diff:
	./tools/org-diff.sh > diff.org
.PHONY: clean
clean:
	python scons.py -c


