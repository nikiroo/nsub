# Requires variable: NAME

.PHONY: man

man: README*.md
	@echo Checking for possible manual pages...
	@if [ -e README.md ]; then \
		echo Sources found for man pages; \
		if pandoc -v >/dev/null 2>&1; then \
			ls README*.md 2>/dev/null \
					| grep 'README\(-..\|\)\.md' \
					| while read man; do \
				echo "  Processing page $$lang..."; \
				lang="`echo "$$man" \
					| sed 's:README\.md:en:' \
					| sed 's:README-\(.*\)\.md:\1:'`"; \
				mkdir -p man/"$$lang"/man1; \
				( \
					echo ".TH \"${NAME}\" 1 `\
						date +%Y-%m-%d\
						` \"version `cat VERSION`\""; \
					echo; \
					UNAME="`echo "${NAME}" \
						| sed 's:\(.*\):\U\1:g'`"; \
					( \
						cat "$$man" | head -n1 \
	| sed 's:.*(README\(-fr\|\)\.md).*::g'; \
						cat "$$man" | tail -n+2; \
					) | sed 's:^#\(#.*\):\1:g' \
	| sed 's:^\(#.*\):\U\1:g;s:# *'"$$UNAME"':# NAME\n'"${NAME}"' \\- :g' \
	| sed 's:--:——:g' \
	| pandoc -f markdown -t man | sed 's:——:--:g' ; \
				) > man/"$$lang"/man1/"${NAME}.1"; \
			done; \
			mkdir -p "man/man1"; \
			cp man/en/man1/"${NAME}".1 man/man1/; \
		else \
			echo "man pages generation: pandoc required" >&2; \
			false; \
		fi; \
	fi;


