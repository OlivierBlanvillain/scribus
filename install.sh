sudo apt-get install \
  libjpeg-dev \
  libtiff5-dev \
  python-dev \
  libfreetype6-dev \
  libcairo2-dev \
  libxml2-dev \
  libpoppler-dev \
  libhunspell-dev \
  libboost-dev \
  libpoppler-dev \
  libpoppler-cpp-dev \
  libpoppler-private-dev \
  cmake  \
  libqt5webkit5-dev \
  qtquick1-5-dev \
  qtdeclarative5-dev \
  qttools5-dev-tools \
  qttools5-dev \
  liblcms2-dev

cmake . -DCMAKE_INSTALL_PREFIX:PATH=/opt/scribus15svn
make
sudo make install
