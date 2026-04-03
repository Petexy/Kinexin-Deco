# Maintainer: petexy
pkgname=kinexin-deco
pkgver=1.0.0
pkgrel=1
pkgdesc="Default Kinexin window decoration for KDE Plasma"
arch=('x86_64' 'aarch64')
depends=(
    'qt6-base'
    'kdecoration'
    'kcoreaddons'
    'kconfig'
    'kconfigwidgets'
    'kcmutils'
    'kwidgetsaddons'
)

makedepends=(
    'cmake'
    'extra-cmake-modules'
    'qt6-base'
    'kdecoration'
    'kcoreaddons'
    'kconfig'
    'kconfigwidgets'
    'kcmutils'
    'kwidgetsaddons'
)
provides=('kinexin-deco')
conflicts=('kinexin-deco')
source=()

build() {
    cd "$startdir/kinexin-deco-kwin"
    rm -rf build
    cmake -B build \
        -S . \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr
    cmake --build build
}

package() {
    cd "$startdir/kinexin-deco-kwin"
    DESTDIR="$pkgdir" cmake --install build
    install -Dm644 "$startdir/LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
