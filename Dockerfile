FROM waltervargas/jhbuild:debian
LABEL maintainer "waltervargas@linux.com"

RUN $HOME/.local/bin/jhbuild build gnome-todo