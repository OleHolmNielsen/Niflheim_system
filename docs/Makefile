# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build
# Configure the project's top-level folder in the web server:
WEBTOPDIR     = /var/www/wiki/Niflheim_system

ifneq ($(VIRTUAL_ENV),)
dirhtml: gitpull
else
$(error VIRTUAL_ENV not set! Please use a Python virtual environment: . venv/bin/activate)
endif

rsync: dirhtml
	@echo Rsyncing HTML pages to ${WEBTOPDIR}/
	@rsync -av --delete ./_build/dirhtml/ ${WEBTOPDIR}/

gitpull:
	git pull

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
