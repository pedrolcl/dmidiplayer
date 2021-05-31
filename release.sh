#!/bin/bash
REL=1.3.1
git archive --format=zip --prefix=dmidiplayer-${REL}/ --output build/dmidiplayer-${REL}.zip master
git archive --format=tar --prefix=dmidiplayer-${REL}/ master | gzip > build/dmidiplayer-${REL}.tar.gz
git archive --format=tar --prefix=dmidiplayer-${REL}/ master | bzip2 > build/dmidiplayer-${REL}.tar.bz2


