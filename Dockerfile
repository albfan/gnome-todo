FROM waltervargas/jhbuild-sysdeps-usefull-packages
LABEL maintainer "waltervargas@linux.com"

ENV PACKAGES gnome-todo

RUN $HOME/.local/bin/jhbuild buildone $PACKAGES