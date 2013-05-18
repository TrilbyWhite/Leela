# Maintainer: "Trilby" <jmcclure [at] cns [dot] umass [dot] edu>
# contributor: static DOT vortex AT gmx.com
pkgname=leela-git
pkgver=12.fe7a35a
pkgrel=1
pkgdesc="CLI frontend to poppler-glib of PDF tools"
url="http://github.com/TrilbyWhite/Leela"
arch=('any')
license=('GPLv3')
depends=('poppler-glib' 'ghostscript')
makedepends=('git')
md5sums=('SKIP')

source=("git://github.com/TrilbyWhite/Leela.git")
_gitname="Leela"

pkgver() {
	cd "$_gitname"
    # use the total number of commits + SHA-1 (first 7 characters) of the last commit
    echo $(git rev-list --count HEAD).$(git rev-parse --short HEAD)
}

build() {
    cd "$_gitname"
	make
}

package() {
	cd "$_gitname"
	make DESTDIR="$pkgdir" install
} 
