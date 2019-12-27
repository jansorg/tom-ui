# Maintainer: Joachim Ansorg <mail@ja-dev.eu>
pkgname=tom-ui
pkgver=0.9.0
pkgrel=1
pkgdesc="Tom is a cross-platform time tracking applications."
url="https://www.tomtime.dev"
arch=('i686' 'x86_64')
license=('GPL3')
depends=(tom qt5-base qt5-declarative qt5-svg qt5-webengine)
makedepends=(git cmake extra-cmake-modules qt5-webengine)
conflicts=()
replaces=()
backup=()
install=
source=($pkgname-$pkgver.tar.gz::https://github.com/jansorg/tom-ui/archive/v${pkgver}.tar.gz)
md5sums=('a965bb12b3891e3cd03af978d3feff83')
build() {
  cd "$startdir/src/$pkgname-$pkgver"
  cmake -DCMAKE_INSTALL_PREFIX=/usr .
  make || return 1
}
package() {
  make DESTDIR="$startdir/pkg" install
}
