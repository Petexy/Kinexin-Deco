# Maintainer: petexy
pkgname=kinexin-deco
pkgver=1.0.0
pkgrel=1
pkgdesc="Default Kinexin window decoration for KDE Plasma"
arch=('x86_64' 'aarch64')
depends=(
    'qt6-base'
    'kdecoration'
    'kf6-kcoreaddons'
    'kf6-kconfig'
    'kf6-kconfigwidgets'
    'kf6-kcmutils'
    'kf6-kwidgetsaddons'
)
makedepends=(
    'git'
    'cmake'
    'extra-cmake-modules'
    'qt6-base'
    'kdecoration'
    'kf6-kcoreaddons'
    'kf6-kconfig'
    'kf6-kconfigwidgets'
    'kf6-kcmutils'
    'kf6-kwidgetsaddons'
)
provides=('kinexin-deco')
conflicts=('kinexin-deco')
sha256sums=('SKIP')

build() {
    cd "$pkgname/kinexin-deco-kwin"
    cmake -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    cd "$pkgname/kinexin-deco-kwin"
    DESTDIR="$pkgdir" cmake --install build
    install -Dm644 "$srcdir/$pkgname/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
