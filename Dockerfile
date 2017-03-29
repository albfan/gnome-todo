FROM waltervargas/jhbuild:debian
LABEL maintainer "waltervargas@linux.com"

ENV PACKAGES gnome-todo

RUN $HOME/.local/bin/jhbuild build $PACKAGES